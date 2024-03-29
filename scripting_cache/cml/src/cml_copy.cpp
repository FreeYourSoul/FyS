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

#include "cml_copy.hh"
#include <cml_key.hh>

namespace fys::cache {

void cml_copy::createUpToDateFileInLocalStorage(const cml_key& cmlKey, std::filesystem::file_time_type cacheTime) {
  cml_key k(_copyPathStorage, cmlKey.getKey());
  std::error_code e;

  // If the file doesn't exist yet, or if it is not up to date then remake it
  if (!std::filesystem::exists(k.get_path()) || std::filesystem::last_write_time(k.get_path(), e) > cacheTime) {
    std::filesystem::create_directories(cmlKey.get_path().parent_path(), e);
    std::filesystem::copy(k.get_path(), cmlKey.get_path(), e);
  }
}

}// namespace fys::cache