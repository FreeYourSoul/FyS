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
        CmlKey(std::filesystem::path basePath, std::string key): _path(std::move(basePath)), _key(std::move(key)) {
            std::replace(_key.begin(), _key.end(), ':', '/');
            _path /= key;
            tokenizeKey(":");
        }

        [[nodiscard]] const std::filesystem::path &getPath() const { return _path; }
        [[nodiscard]] const std::string &getKey() const { return _key; }

    private:
        void tokenizeKey(const std::string &input, const std::string &separators) {
            std::size_t pos = 0;
            while (pos != _key.size()) {
                std::size_t from = _key.find_first_not_of(separators, pos);
                if (from == std::string::npos)
                    break;

                std::size_t to = _key.find_first_of(separators, from + 1);
                if (to == std::string::npos) 
                    to = _key.size();

                std::string temp;
                for (std::size_t i = from; i != to; i++) 
                    temp.push_back(_key[i]);

                _token.emplace_back(std::move(temp));
                pos = to;
            }
        }

    private:
        std::filesystem::path _path;
        std::string _key;
        std::vector<std::string> _token;

    };
}

#endif //FYS_SERVICE_CMLKEY_HH
