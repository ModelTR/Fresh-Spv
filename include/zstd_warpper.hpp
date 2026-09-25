#pragma once

#include <cstddef>

#include "zstd.h"

#include "err.hpp"
#include "type_alias.hpp"

namespace details {
class CDictGuard {
public:
	CDictGuard() = delete;

	CDictGuard(const CDictGuard &) = delete;
	auto operator=(const CDictGuard &) -> CDictGuard & = delete;

	CDictGuard(CDictGuard &&other) noexcept;
	auto operator=(CDictGuard &&) noexcept -> CDictGuard &;

	~CDictGuard();

	static auto create(const u8vec &dict, int compression_lvl)
		-> util::res<CDictGuard>;

	[[nodiscard]] auto get() const noexcept -> ZSTD_CDict * { return cdict_; }

private:
	CDictGuard(ZSTD_CDict *d);
	ZSTD_CDict *cdict_ = nullptr;
};

class CCtxGuard {
public:
	CCtxGuard() = delete;

	CCtxGuard(const CCtxGuard &) = delete;
	auto operator=(const CCtxGuard &) -> CCtxGuard & = delete;

	CCtxGuard(CCtxGuard &&other) noexcept;
	auto operator=(CCtxGuard &&other) noexcept -> CCtxGuard &;

	~CCtxGuard();

	static auto create() -> util::res<CCtxGuard>;

	[[nodiscard]] auto get() const noexcept -> ZSTD_CCtx * { return cctx_; }

private:
	CCtxGuard(ZSTD_CCtx *cctx);
	ZSTD_CCtx *cctx_;
};

} // namespace details

namespace fszstd {

auto make_cdict_guard(const u8vec &dict, int compression_lvl)
	-> util::res<details::CDictGuard>;

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

	if (dict_sz > DictSz) {
		return util::make_err(
			"{} - Dictionary Size: {} > Dictionary Buffer Size: {}", fn_name,
			dict_sz, dict_buf.max_size());
	}
	dict_buf.resize(dict_sz);
	return dict_buf;
}

template <class T>
auto dict_compress(const vec<T> &data, const ZSTD_CDict *cdict)
	-> util::res<vec<T>> {
	auto fn_name = util::get_fn_name();

	if (data.empty()) {
		return util::make_err("{} - Datas to be Compressed Can't be Empty",
							  fn_name);
	}

	auto cctx_g = details::CCtxGuard::create();
	if (!cctx_g) {
		return util::err(cctx_g.error());
	}
	auto *const cctx = cctx_g->get();

	size_t const dst_capacity = ZSTD_compressBound(data.size());
	u8vec compressed_smolv(dst_capacity);

	size_t const compressed_size =
		ZSTD_compress_usingCDict(cctx, compressed_smolv.data(), dst_capacity,
								 data.data(), data.size(), cdict);

	if (ZSTD_isError(compressed_size)) {
		return util::make_err("{} - Compression Failed: {}", fn_name,
							  ZSTD_getErrorName(compressed_size));
	}

	compressed_smolv.resize(compressed_size);
	return compressed_smolv;
}

} // namespace zstd