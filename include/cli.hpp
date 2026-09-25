#pragma once

#include "cxxopts.hpp"

#include "err.hpp"
#include "type_alias.hpp"
#include <filesystem>

namespace cli {
class CLI {
public:
	CLI(int argc, char **argv) {
		opts_.add_options()("o,output", "Output Dictionary",
							cxxopts::value<fs::path>());
		auto res = opts_.parse(argc, argv);

		if (!res.count("output")) {
			spdlog::warn("No --output Given, Falling Back to CWD: {}",
						 fs::current_path().string());

		} else {
			output_path_ = res["output"].as<fs::path>();
		}
	}
	CLI() = delete;
	CLI(const CLI &) = delete;
	auto operator=(const CLI &) -> CLI & = delete;
	CLI(CLI &&) = delete;
	auto operator=(CLI &&) -> CLI & = delete;

	auto get_path() const -> fs::path { return output_path_; }

private:
	cxxopts::Options opts_{"fresh-spv", "Compress SPIR-V file drastically"};
	fs::path output_path_;
};

auto make(int argc, char **argv) -> CLI;

} // namespace cli