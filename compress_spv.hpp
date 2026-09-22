#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "smolv.h"
#include "zdict.h"
#include "zstd.h"
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

	static auto create(const u8vec &dict, int compression_lvl)
		-> util::res<DictCtxGuard>;
};

auto write_smolv(str_view file_name, const embed::cu8span<> &data,
				 str_view output_dir = "") -> util::res<void>;

auto write_dict(std::string_view file_name, const embed::cu8span<> &data)
	-> util::res<void>;

namespace zstd {
template <class T, size_t DictSz>
auto train_dict(const vec<T> &datas, const sizes &szs) -> util::res<vec<T>> {

	auto fn_name = util::get_fn_name();

	if (datas.empty()) {
		return util::make_err("{} - The Training Datas Can't be Empty",
							  fn_name);
	}

	if (szs.empty()) {
		return util::make_err("{} - The Sizes Can't be Empty", fn_name);
	}

	vec<T> dict_buf(DictSz);

	auto dict_sz =
		ZDICT_trainFromBuffer(dict_buf.data(), DictSz, datas.data(), szs.data(),
							  static_cast<unsigned int>(szs.size()));
	if (ZDICT_isError(dict_sz) or dict_sz == 0) {
		return util::make_err("{} - Dictionary Training Failed with: {}",
							  fn_name, str{ZDICT_getErrorName(dict_sz)});
	}

	if (dict_sz > dict_buf.max_size()) {
		return util::make_err(
			"{} - Dictionary Size: {} > Dictionary Buffer Size: {}", fn_name,
			dict_sz, dict_buf.max_size());
	}
	dict_buf.resize(dict_sz);
	return dict_buf;
}

template <class T>
auto dict_compress(const vec<T> &data, const vec<T> &dict, int compression_lvl = 22)
	-> util::res<vec<T>> {
	auto fn_name = util::get_fn_name();

	if (data.empty() or dict.empty()) {
		return util::make_err(
			"{} - Invalid Input for Compression -> smolv: {} | dictionary: {}",
			fn_name, data.size(), dict.size());
	}

	auto guard = DictCtxGuard::create(dict, compression_lvl);

	if (!guard) {
		return util::err(guard.error() |
						 add_err("DictCtxGuard Created Failed"));
	}

	size_t const dst_capacity = ZSTD_compressBound(data.size());
	u8vec compressed_smolv(dst_capacity);

	assert(dst_capacity == compressed_smolv.size());

	size_t const compressed_size = ZSTD_compress_usingCDict(
		guard->cctx, compressed_smolv.data(), dst_capacity, data.data(),
		data.size(), guard->cdict);

	if (ZSTD_isError(compressed_size)) {
		return util::make_err("{} - Compression Failed: {}", fn_name,
							  ZSTD_getErrorName(compressed_size));
	}

	compressed_smolv.resize(compressed_size);
	return compressed_smolv;
}

} // namespace zstd

template <class Src>
auto write_file(str_view file_name, str_view output_path, const Src &src_data)
	-> util::res<void> {

	auto fn_name = util::get_fn_name();

	auto full_path = output_path.empty() ? fs::current_path() / file_name
										 : fs::path(output_path) / file_name;

	std::ofstream data_out{full_path, std::ios::binary | std::ios::trunc};

	if (!data_out) {
		return util::make_err("{} - Open {} Failed", fn_name,
							  full_path.string());
	}

	data_out.write(reinterpret_cast<const char *>(src_data.data()),
				   std::size(src_data) * sizeof(u8));

	if (!data_out) {
		return util::make_err("{} - Write {} to {} Failed", fn_name, file_name,
							  full_path.string());
	}

	spdlog::info("{} - {} Saved Successfully", fn_name, full_path.string());

	return {};
}

auto dict_compress(const smolv::ByteArray &smolv, const embed::u8span<> &dict)
	-> util::res<u8vec>;

auto encode_smolv(const embed::cu8span<> &spv) -> util::res<smolv::ByteArray>;
