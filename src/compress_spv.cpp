#include "compress_spv.hpp"
#include "err.hpp"
#include "smolv.h"
#include <cassert>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

auto encode_smolv(const cu8span<> &spv) -> util::res<smolv::ByteArray> {
	smolv::ByteArray smolv_data;
	if (smolv::Encode(spv.data(), spv.size(), smolv_data,
					  smolv::kEncodeFlagStripDebugInfo)) {
		return smolv_data;
	}
	return util::make_err("{} - Encode Smolv Failed", util::get_fn_name());
}