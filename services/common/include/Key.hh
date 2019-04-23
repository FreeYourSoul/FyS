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

#include <string>
#include <algorithm>

namespace fys::util {

/**
 * Key used as token
 * At creation will create a token of 8 characters that will be initialized using the address of the Key object as base.
 * Hide the array index (index that the player data are in) inside the token.
 *
 * The encoding follow this rule:
 * <size of sizeIndex:soz> <sizeIndex:si> <index>    <random>
 *    one char                 soz          si     12 - (1+soz+si)
 */
class Key {
    constexpr static unsigned KEY_SIZE = 12;

public:
    explicit Key(unsigned index) {
        std::string fillerId = std::to_string(reinterpret_cast<unsigned long long>(this));
        std::string id = generateId(index);

        std::fill(_key, _key + KEY_SIZE, 'x');
        std::copy_n(fillerId.begin(), KEY_SIZE, _key);
        std::copy(id.begin(), id.end(), _key);
    }

    bool operator==(const Key& other) {
        return std::equal(other._key, other._key + KEY_SIZE, _key);
    }

private:
    /**
     * generate the id from the index,
     *
     * @param index will be generated into a specific id
     * @return a string formatted as follow : <sizeOfSizeOfIndex><sizeOfIndex><index>
     */
    std::string generateId(unsigned index) {
        std::string id = std::to_string(index);
        std::string idSize = std::to_string(id.size());
        id = std::to_string(idSize.size()).append(idSize).append(id);
        return id;
    }

private:
    char _key[KEY_SIZE];
};

}


#endif //FYS_KEY_HH
