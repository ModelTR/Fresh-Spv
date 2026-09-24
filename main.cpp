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

#include "compress_spv.hpp"
#include "embed.hpp"
#include "err.hpp"
#include "recorder.hpp"
#include "smolv.h"
#include "zstd_warpper.hpp"

/// @todo replace uint8_t with std::byte in C++23 through #embed as std::byte

auto main() -> int {
	try {
		util::Recorder t_record;

		spdlog::info("Current working directory: {}",
					 fs::current_path().string());

		u8vec smolvs_data;

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
				auto &smolv = *res;
				smolvs.emplace_back(str{spv.first.data()}, smolv);
				smolvs_sz.push_back(smolv.size());
				smolvs_data.insert(smolvs_data.end(), smolv.begin(),
								   smolv.end());
			} else {
				encoded_failed_count++;
			}
		}

		if (encoded_failed_count != 0) {
			spdlog::error("Encoded to Smolv Failed {} times",
						  encoded_failed_count);
		}

		/// @note 16kb
		auto dict =
			zstd::train_dict<u8, 16 * 1024>(smolvs_data, smolvs_sz)
				.and_then([](const u8vec &d) -> util::res<u8vec> {
					auto writen =
						write_file("trained_dictionary.dict", "../smolvs", d);
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
		auto cdict_g = zstd::make_cdict_guard(*dict, 22);
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
					zstd::dict_compress(smolv.second, cdict)
						.and_then([&](const u8vec &cs) -> util::res<void> {
							auto full_name = str{smolv.first} += ".zst";
							auto res = write_file(full_name, "../smolvs", cs);
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
		spdlog::critical("Unknown exception", e.what());
		return EXIT_FAILURE;
	}
}