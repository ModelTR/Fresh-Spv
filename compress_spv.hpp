#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "zdict.h"
#include "zstd.h"
#include "smolv.h"
#include <spdlog/spdlog.h>

#include "embed.hpp"
#include "err.hpp"

/// @todo 使用tl::expected替换返回bool和异常处理

using u8 = uint8_t;

template <class T> using vec = std::vector<T>;

using u8vec = vec<u8>;
using cu8vec = vec<const u8>;
using sizes = vec<size_t>;

using str = std::string;
using str_view = std::string_view;

namespace fs = std::filesystem;

using paths = vec<fs::path>;

template <class T, class... Args>
auto require(const T &value, spdlog::format_string_t<Args...> fmt,
			 Args &&...args) -> util::res<T> {
	if (!value) {
		return util::make_err(fmt, std::forward<Args>(args)...);
	}
	return value;
}

struct DictCtxGuard {
	ZSTD_CDict *cdict = nullptr;
	ZSTD_CCtx *cctx = nullptr;
	DictCtxGuard(ZSTD_CDict *d, ZSTD_CCtx *c);
	DictCtxGuard() = delete;

	DictCtxGuard(const DictCtxGuard &) = delete;
	DictCtxGuard &operator=(const DictCtxGuard &) = delete;

	/// @note Default Move Constructor/Operator isn't correct
	DictCtxGuard(DictCtxGuard &&other) noexcept;
	DictCtxGuard &operator=(DictCtxGuard &&other) noexcept;

	~DictCtxGuard();

	static auto create(const embed::u8span<> &dict, int compression_lvl)
		-> util::res<DictCtxGuard>;
};

auto write_smolv(str_view file_name, const embed::cu8span<> &data,
				 str_view output_dir = "") -> util::res<void>;

auto write_dict(std::string_view file_name, const embed::cu8span<> &data)
	-> util::res<void>;

template <size_t DictSz>
auto train_dict(const u8vec &smolvs, const sizes &szs) -> util::res<u8vec> {
	if (smolvs.empty()) {
		return util::make_err("Smolvs Data is Empty");
	}
	if (szs.empty()) {
		return util::make_err("Arrary of size_t of Smolvs is Empty");
	}
	u8vec dict_buf(DictSz);

	auto dict_sz = ZDICT_trainFromBuffer(dict_buf.data(), DictSz, smolvs.data(),
										 szs.data(),
										 static_cast<unsigned int>(szs.size()));
	if (ZDICT_isError(dict_sz) or dict_sz == 0) {
		return util::make_err("Dictionary Training Failed with: {}",
							  str{ZDICT_getErrorName(dict_sz)});
	}

	if (dict_sz > dict_buf.max_size()) {
		return util::make_err(
			"Dictionary Size: {} > Dictionary Buffer Size: {}", dict_sz,
			dict_buf.max_size());
	}
	dict_buf.resize(dict_sz);
	return dict_buf;
}

auto dict_compress(const smolv::ByteArray &smolv, const embed::u8span<> &dict)
	-> util::res<u8vec>;

auto encode_smolv(const embed::cu8span<> &spv) -> util::res<smolv::ByteArray>;
