#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "smolv.h"
#include "zdict.h"
#include "zstd.h"
#include <spdlog/spdlog.h>

#include "err.hpp"
#include "type_alias.hpp"

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

auto encode_smolv(const cu8span<> &spv) -> util::res<smolv::ByteArray>;
