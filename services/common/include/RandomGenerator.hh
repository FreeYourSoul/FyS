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


#ifndef FYS_ONLINE_RANDOMGENERATOR_HH
#define FYS_ONLINE_RANDOMGENERATOR_HH

#include <memory>
#include <random>
#include <chrono>

namespace fys::util {

class RandomGenerator {
public:
	static fys::util::RandomGenerator&
	getInstance()
	{
		static fys::util::RandomGenerator s{};
		return s;
	}

	template<typename Type>
	static Type
	generateInRange(Type rA, Type rB)
	{
		static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type>);
		if constexpr (std::is_integral_v<Type>) {
			std::uniform_int_distribution<Type> distribution(rA, rB);
			return distribution(*getInstance().get());
		}
		else if constexpr (std::is_floating_point_v<Type>) {
			std::uniform_real_distribution<Type> distribution(rA, rB);
			return distribution(*getInstance().get());
		}
	}

	RandomGenerator(RandomGenerator const&) = delete;
	RandomGenerator&
	operator=(RandomGenerator const&) = delete;

	std::shared_ptr<std::mt19937>
	get();

private:
	RandomGenerator()
			:mt(std::make_shared<std::mt19937>())
	{
		std::random_device rd;

		if (rd.entropy() != 0) {
			mt->seed(rd());
		}
		else {
			auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			mt->seed(seed);
		}
	}

	~RandomGenerator() = default;

	std::shared_ptr<std::mt19937> mt;
};

}

#endif //FYS_ONLINE_RANDOMGENERATOR_HH
