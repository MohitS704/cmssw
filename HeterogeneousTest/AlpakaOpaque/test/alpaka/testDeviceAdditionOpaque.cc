#include <cstdint>
#include <random>
#include <vector>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/devices.h"
#include "HeterogeneousTest/AlpakaOpaque/interface/alpaka/DeviceAdditionOpaque.h"

using namespace ALPAKA_ACCELERATOR_NAMESPACE;

TEST_CASE("HeterogeneousTest/AlpakaOpaque test", "[alpakaTestOpaqueAdditionOpaque]") {
  auto const& devices = cms::alpakatools::devices<Platform>();
  if (devices.empty()) {
    FAIL("No devices available for the " EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE) " backend, "
        "the test will be skipped.");
  }

  // random number generator with a gaussian distribution
  std::random_device rd{};
  std::default_random_engine rand{rd()};
  std::normal_distribution<float> dist{0., 1.};

  // tolerance
  constexpr float epsilon = 0.000001;

  // buffer size
  constexpr uint32_t size = 1024 * 1024;

  // allocate input and output host buffers
  std::vector<float> in1(size);
  std::vector<float> in2(size);
  std::vector<float> out(size);

  // fill the input buffers with random data, and the output buffer with zeros
  for (uint32_t i = 0; i < size; ++i) {
    in1[i] = dist(rand);
    in2[i] = dist(rand);
    out[i] = 0.;
  }

  SECTION("Test add_vectors_f on " EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE) " backend") {
    // launch the 1-dimensional kernel for vector addition
    REQUIRE_NOTHROW(test::opaque_add_vectors_f(in1.data(), in2.data(), out.data(), size));

    // check the results
    for (uint32_t i = 0; i < size; ++i) {
      float sum = in1[i] + in2[i];
      CHECK_THAT(out[i], Catch::Matchers::WithinAbs(sum, epsilon));
    }
  }
}
