#include "cli.hpp"
#include "err.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <scn/scan.h>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

cli::CLI::CLI(cxxopts::Options &&opts, fs::path &&op)
	: opts_(std::move(opts)), output_path_(std::move(op)) {}

auto cli::make(int argc, char **argv) -> CLIInitializedResult {
	cxxopts::Options opts{"fresh-spv", "Compress SPIR-V file significantly"};
	fs::path op{};

	opts.add_options()("o, output", "Output Dictionary",
					   cxxopts::value<fs::path>());

	auto res = opts.parse(argc, argv);

	if (!res.count("output")) {
		spdlog::warn("{} - \"--output\" isn't Specified Initially", util::get_fn_name());
	} else {
		op = res["output"].as<fs::path>();
	}

	return {.cli = CLI{std::move(opts), std::move(op)}, .output_path = op};
}

auto cli::wait_input() -> util::res<fs::path> {
	auto fn_nm = util::get_fn_name();
	std::cout << "--output " << std::flush;
	str line;
	if (!std::getline(std::cin, line)) {
		return util::make_critical("{} - Failed to Read Any Input from stdin", fn_nm);
	}

	auto res = scn::scan<str>(line, "{}");
	if (!res) {
		return util::make_critical("{} - {}",
								   util::get_fn_name(), res.error().msg());
	}
	return fs::path{res->value()};
}