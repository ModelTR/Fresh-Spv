#include "cli.hpp"
#include "err.hpp"
#include <cxxopts.hpp>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <utility>

cli::CLI::CLI(cxxopts::Options &&opts, fs::path &&op)
	: opts_(std::move(opts)), output_path_(std::move(op)) {}

auto cli::make(int argc, char **argv) -> CLI {
	cxxopts::Options opts{"fresh-spv", "Compress SPIR-V file significantly"};
	fs::path op{};

	opts.add_options()("o, output", "Output Dictionary",
					   cxxopts::value<fs::path>());

	auto res = opts.parse(argc, argv);

	if (!res.count("output")) {
		spdlog::warn("{} - \"output\" isn't specified", util::get_fn_name());
	} else {
		op = res["output"].as<fs::path>();
	}

	return CLI{std::move(opts), std::move(op)};
}