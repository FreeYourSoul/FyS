// MIT License
//
// Copyright (c) 2019 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FyS
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef FYS_ONLINE_RANDOM_GENERATOR_HH
#define FYS_ONLINE_RANDOM_GENERATOR_HH

#include <chrono>
#include <memory>
#include <random>

namespace fys::util {

class random_generator {
public:
  [[nodiscard]] static fys::util::random_generator&
  instance() {
    static fys::util::random_generator s{};
    return s;
  }

  template<typename Type>
  [[nodiscard]] static Type
  generate_in_range(Type rA, Type rB) {
    static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type>);
    if constexpr (std::is_integral_v<Type>) {
      std::uniform_int_distribution<Type> distribution(rA, rB);
      return distribution(*instance().get());
    } else if constexpr (std::is_floating_point_v<Type>) {
      std::uniform_real_distribution<Type> distribution(rA, rB);
      return distribution(*instance().get());
    }
  }

  random_generator(random_generator const&) = delete;
  random_generator& operator=(random_generator const&) = delete;

  std::shared_ptr<std::mt19937> get() const;

  unsigned get_current_seed() const { return _seed; }

private:
  random_generator()
      : mt(std::make_shared<std::mt19937>()) {
    std::random_device rd;

    if (rd.entropy() != 0) {
      _seed = rd();
    } else {
      _seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    mt->seed(_seed);
  }

  ~random_generator() = default;

  std::shared_ptr<std::mt19937> mt;
  unsigned _seed;
};

}// namespace fys::util

#endif//FYS_ONLINE_RANDOM_GENERATOR_HH
