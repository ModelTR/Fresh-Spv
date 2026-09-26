#pragma once

#include "cxxopts.hpp"
#include "err.hpp"

#include "type_alias.hpp"
#include <filesystem>

namespace cli {

class CLI {
public:
	CLI(cxxopts::Options &&opts, fs::path &&op);
	CLI() = delete;
	CLI(const CLI &) = delete;
	auto operator=(const CLI &) -> CLI & = delete;
	CLI(CLI &&) = delete;
	auto operator=(CLI &&) -> CLI & = delete;

private:
	cxxopts::Options opts_{"fresh-spv", "Compress SPIR-V file drastically"};
	fs::path output_path_;
};

struct CLIInitializedResult {
	CLI cli;
	fs::path output_path;
};

auto make(int argc, char **argv) -> CLIInitializedResult;

auto wait_input() -> util::res<fs::path>;

} // namespace cli