// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#ifndef FYS_SERVICE_CMLSQL_HH
#define FYS_SERVICE_CMLSQL_HH

#include <fmt/format.h>

#include <cml.hh>

namespace fys::cache {
class
    cml_copy : public cml {
public:
  ~cml_copy() override {}
  cml_copy(const std::string& pathLocalStorage,
          const std::string& pathCopy)
      : cml(pathLocalStorage), _copyPathStorage(pathCopy) {
    if (!std::filesystem::exists(_copyPathStorage)) {
      fmt::print("Path copy does not exist {}", pathCopy);
    }
  }

private:
  void createUpToDateFileInLocalStorage(const cml_key& cmlKey, std::filesystem::file_time_type cacheTime) override;

protected:
  std::filesystem::path _copyPathStorage;
};
}// namespace fys::cache

#endif//FYS_SERVICE_CMLSQL_HH
