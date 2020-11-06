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

#ifndef FYS_KEY_HH
#define FYS_KEY_HH

#include <algorithm>
#include <charconv>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

namespace fys::util {

/**
 * Key used as token
 * At creation will create a token of 8 characters that will be initialized using the address of the Key object as base.
 * Hide the array index (index that the player data are in) inside the token.
 *
 * The encoding follow this rule:
 * <offset until index>             <random>                 <index>
 *      one char          KEY_SIZE - (1 + index size)    (KEYSIZE - offset)
 */
class key {
  constexpr static unsigned KEY_SIZE = 8;

public:
  explicit key(unsigned index) {
	static std::random_device rd;
	static std::mt19937 g(rd());
	std::string fillerId = std::to_string(reinterpret_cast<unsigned long long>(this));
	std::shuffle(fillerId.begin(), fillerId.end(), g);
	std::string id = std::to_string(index);
	unsigned offset = KEY_SIZE - id.size();
	if (offset < 3)
	  throw std::range_error(id.append(" is too big for being an index"));
	std::string token = std::to_string(offset).append(fillerId.substr(0, offset - 1)).append(id);
	std::copy_n(token.begin(), KEY_SIZE, _key);
  }

  bool
  operator==(const key &other) {
	return std::equal(other._key, other._key + KEY_SIZE, _key);
  }

  unsigned
  getIndex() const {
	unsigned res = 0;
	unsigned offset = _key[0] - '0';
	std::from_chars(_key + offset, _key + KEY_SIZE, res);
	return res;
  }

  template<typename DisplayPolicy>
  void display(DisplayPolicy policy) { policy(_key); }

private:
  char _key[KEY_SIZE];
};

}// namespace fys::util

#endif//FYS_KEY_HH
