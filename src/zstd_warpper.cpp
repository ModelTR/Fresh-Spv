#include "zstd_warpper.hpp"
#include "err.hpp"
#include "util.hpp"
#include <spdlog/spdlog.h>
#include <utility>

details::CDictGuard::CDictGuard(ZSTD_CDict *d) : cdict_(d) {}

details::CDictGuard::CDictGuard(CDictGuard &&other) noexcept
	: cdict_(std::exchange(other.cdict_, nullptr)) {}

auto details::CDictGuard::operator=(CDictGuard &&other) noexcept
	-> CDictGuard & {
	if (this != &other) {
		ZSTD_freeCDict(cdict_);
		cdict_ = std::exchange(other.cdict_, nullptr);
		return *this;
	}
	return *this;
}

details::CDictGuard::~CDictGuard() { ZSTD_freeCDict(cdict_); }

auto details::CDictGuard::create(const u8vec &dict, int compression_lvl)
	-> util::res<CDictGuard> {

	if (dict.empty()) {
		return util::make_err("{} - Trained Dict Can't be empty",
							  util::get_fn_name());
	}

	auto cdict = util::require(
		ZSTD_createCDict(dict.data(), dict.size(), compression_lvl),
		"{} - CDict Created Failed", util::get_fn_name());

	if (!cdict) {
		return util::err(cdict.error());
	}

	return CDictGuard{*cdict};
}

details::CCtxGuard::CCtxGuard(ZSTD_CCtx *c) : cctx_(c) {}

details::CCtxGuard::CCtxGuard(details::CCtxGuard &&other) noexcept
	: cctx_(std::exchange(other.cctx_, nullptr)) {}

auto details::CCtxGuard::operator=(details::CCtxGuard &&other) noexcept
	-> CCtxGuard & {
	if (this != &other) {
		ZSTD_freeCCtx(cctx_);
		cctx_ = std::exchange(other.cctx_, nullptr);
		return *this;
	}

	return *this;
}

details::CCtxGuard::~CCtxGuard() { ZSTD_freeCCtx(cctx_); }

auto details::CCtxGuard::create() -> util::res<CCtxGuard> {
	auto cctx = util::require(ZSTD_createCCtx(), "{} - CCtx Created Failed",
							  util::get_fn_name());
	if (!cctx) {
		return util::err(cctx.error());
	}

	return CCtxGuard{*cctx};
}

auto zstd::make_cdict_guard(const u8vec &dict, int compression_lvl)
	-> util::res<details::CDictGuard> {
	return details::CDictGuard::create(dict, compression_lvl);
}