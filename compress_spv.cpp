#include "compress_spv.hpp"
#include "deps_cache/spdlog-src/include/spdlog/spdlog.h"
#include "err.hpp"
#include "smolv.h"
#include <cassert>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

DictCtxGuard::DictCtxGuard(ZSTD_CDict *d, ZSTD_CCtx *c) : cdict(d), cctx(c) {}

DictCtxGuard::DictCtxGuard(DictCtxGuard &&other) noexcept
	: cdict(other.cdict), cctx(other.cctx) {
	other.cdict = nullptr;
	other.cctx = nullptr;
}

auto DictCtxGuard::operator=(DictCtxGuard &&other) noexcept -> DictCtxGuard & {
	if (this != &other) {
		ZSTD_freeCCtx(cctx);
		ZSTD_freeCDict(cdict);
		cdict = other.cdict;
		cctx = other.cctx;
		other.cdict = nullptr;
		other.cctx = nullptr;
	}
	return *this;
}

auto DictCtxGuard::create(const u8vec &dict, int compression_lvl)
	-> util::res<DictCtxGuard> {

	auto fn_name = util::get_fn_name();

	auto *cdict = ZSTD_createCDict(dict.data(), dict.size(), compression_lvl);
	if (!cdict) {
		return util::make_err("{} - Created CDict Failed", fn_name);
	}

	auto *cctx = ZSTD_createCCtx();
	if (!cctx) {
		ZSTD_freeCDict(cdict);
		return util::make_err("{} - Created CCtx Failed", fn_name);
	}

	return DictCtxGuard(cdict, cctx);
}

DictCtxGuard::~DictCtxGuard() {
	ZSTD_freeCCtx(cctx);
	ZSTD_freeCDict(cdict);
}

auto encode_smolv(const embed::cu8span<> &spv) -> util::res<smolv::ByteArray> {
	smolv::ByteArray smolv_data;
	if (smolv::Encode(spv.data(), spv.size(), smolv_data,
					  smolv::kEncodeFlagStripDebugInfo)) {
		return smolv_data;
	}
	return util::make_err("{} - Encode Smolv Failed", util::get_fn_name());
}