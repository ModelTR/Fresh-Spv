#include "cli.hpp"

auto cli::make(int argc, char **argv) -> CLI { return CLI(argc, argv); }