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


#ifndef FYS_SERVICE_CMLKEY_HH
#define FYS_SERVICE_CMLKEY_HH

#include <algorithm>
#include <filesystem>

namespace fys::cache {
    class CmlKey {
    public:
        CmlKey(std::filesystem::path basePath, std::string key): _path(std::move(basePath)), _key(key) {
            std::replace(key.begin(), key.end(), ':', '/');
            _path /= key;
        }

        [[nodiscard]] const std::filesystem::path &getPath() const { return _path; }
        [[nodiscard]] const std::string &getKey() const { return _key; }

    private:
        std::filesystem::path _path;
        std::string _key;

    };
}

#endif //FYS_SERVICE_CMLKEY_HH
