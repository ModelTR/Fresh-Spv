#include "compress_spv.hpp"
#include "deps_cache/spdlog-src/include/spdlog/spdlog.h"
#include "err.hpp"
#include "smolv.h"
#include <cassert>
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

auto DictCtxGuard::create(const embed::u8span<> &dict, int compression_lvl)
	-> util::res<DictCtxGuard> {

	auto *cdict = ZSTD_createCDict(dict.data(), dict.size(), compression_lvl);
	if (!cdict) {
		return util::make_err("Created CDict Failed");
	}

	auto *cctx = ZSTD_createCCtx();
	if (!cctx) {
		ZSTD_freeCDict(cdict);
		return util::make_err("Created CCtx Failed");
	}

	return DictCtxGuard(cdict, cctx);
}

DictCtxGuard::~DictCtxGuard() {
	ZSTD_freeCCtx(cctx);
	ZSTD_freeCDict(cdict);
}

auto dict_compress(const smolv::ByteArray &smolv, const embed::u8span<> &dict)
	-> util::res<u8vec> {

	if (smolv.empty() or dict.empty()) {
		return util::make_err(
			"Invalid Input for Compression -> smolv: {} | dictionary: {}",
			smolv.size(), dict.size());
	}

	auto guard = DictCtxGuard::create(dict, 22);

	if (!guard) {
		return util::err(guard.error() |
						 add_err("DictCtxGuard Created Failed"));
	}

	size_t const dst_capacity = ZSTD_compressBound(smolv.size());
	u8vec compressed_smolv(dst_capacity);

	assert(dst_capacity == compressed_smolv.size());

	size_t const compressed_size = ZSTD_compress_usingCDict(
		guard->cctx, compressed_smolv.data(), dst_capacity, smolv.data(),
		smolv.size(), guard->cdict);

	if (ZSTD_isError(compressed_size)) {
		return util::make_err("Compression Failed: {}",
							  ZSTD_getErrorName(compressed_size));
	}

	compressed_smolv.resize(compressed_size);
	return compressed_smolv;
}

auto encode_smolv(const embed::cu8span<> &spv) -> util::res<smolv::ByteArray> {
	smolv::ByteArray smolv_data;
	if (smolv::Encode(spv.data(), spv.size(), smolv_data,
					  smolv::kEncodeFlagStripDebugInfo)) {
		return smolv_data;
	}
	return util::make_err("Encode Smolv Failed");
}