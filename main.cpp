/** @version Alpha 0.0.3
**	@anchor modelTR
**	@copyright MIT License
**	@warning THIS PROJECT IS EARLY VERSION which means it can't be used in
* actual scene *	@brief A simple program to compress *.spv to *.somlv.zst
* using zstd and smolv
** @date Last edited in 21:45 2026/09/23
*/

#include <algorithm>
#include <cstdlib>
#include <execution>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

#include "cli.hpp"
#include "compress_spv.hpp"
#include "embed.hpp"
#include "err.hpp"
#include "recorder.hpp"
#include "smolv.h"
#include "zstd_warpper.hpp"

/// @todo replace uint8_t with std::byte in C++23 through #embed as std::byte
/// @todo add benchmark
/// @todo add test

auto main(int argc, char **argv) -> int {
	try {
		util::Recorder t_record;
		spdlog::info("Current working directory: {}",
					 fs::current_path().string());

		auto cli = cli::make(argc, argv);
		auto const OUTPUT_DIR = cli.get_path();

		u8vec training_data;
		training_data.reserve(embed::total_sz);
		sizes smolvs_sz;
		smolvs_sz.reserve(embed::spvs_map.size());

		/// @note preformance better than std::unordered_map due to the parallel
		/// suport isn't compelet in this container
		vec<std::pair<str, smolv::ByteArray>> smolvs;
		smolvs.reserve(embed::spvs_map.size());

		int encoded_failed_count = 0;

		for (const auto &spv : embed::spvs_map) {
			auto res = encode_smolv(spv.second);
			if (res) {
				auto const &smolv = *res;
				smolvs.emplace_back(str{spv.first.data()}, smolv);
				smolvs_sz.push_back(smolv.size());
				training_data.insert(training_data.end(), smolv.begin(),
								   smolv.end());
			} else {
				encoded_failed_count++;
			}
		}

		if (encoded_failed_count != 0) {
			spdlog::error("Encoded to Smolv Failed {} times",
						  encoded_failed_count);
			return EXIT_FAILURE;
		}

		/// @note 16kb
		auto dict =
			fszstd::train_dict<u8, 16 * 1024>(training_data, smolvs_sz)
				.and_then([&OUTPUT_DIR](const u8vec &d) -> util::res<u8vec> {
					auto writen =
						write_file("trained_dictionary.dict", OUTPUT_DIR, d);
					if (!writen) {
						return util::err(writen.error() |
										 add_err("Write Dictionary Failed"));
					}
					return d;
				});

		if (!dict) {
			dict.error() | print();
			return EXIT_FAILURE;
		}

		std::atomic<int> failures{0};
		auto cdict_g = fszstd::make_cdict_guard(*dict, 22);
		if (!cdict_g) {
			cdict_g.error() | print();
			return EXIT_FAILURE;
		}
		auto *const cdict = cdict_g->get();

		/// @note About 20ms
		/// @note par policy almost same as par_unseq policy
		/// @note std::execution::par... need the Intel TBB as backend
		std::for_each(
			std::execution::par, std::begin(smolvs), std::end(smolvs),
			[&](const auto &smolv) {
				auto compressed_smolv =
					fszstd::dict_compress(smolv.second, cdict)
						.and_then([&](const u8vec &cs) -> util::res<void> {
							auto full_name = str{smolv.first} += ".zst";
							auto res = write_file(full_name, OUTPUT_DIR, cs);
							if (!res) {
								return util::err(
									res.error() |
									add_err("Write Smolv Failed: {}",
											smolv.first));
							}
							return {};
						});
				if (!compressed_smolv) {
					failures++;
					compressed_smolv.error() |
						add_err("Compressed Smolv Failed: {}", smolv.first) |
						print();
				}
			});

		if (failures > 0) {
			spdlog::error("{} Smolv(s) Compressed Failed", failures.load());
			return EXIT_FAILURE;
		}

		t_record.record();

		return EXIT_SUCCESS;
	} catch (const std::exception &e) {
		spdlog::critical("{}", e.what());
		return EXIT_FAILURE;
	}
}
