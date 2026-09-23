#pragma once

#include "zstd.h"

#include "err.hpp"
#include "type_alias.hpp"
#include "util.hpp"


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
auto dict_compress(const vec<T> &data, const vec<T> &dict,
				   int compression_lvl = 22) -> util::res<vec<T>> {
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