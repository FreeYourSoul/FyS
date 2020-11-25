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

#include <CmlCopy.hh>
#include <catch2/catch.hpp>
#include <filesystem>

namespace {
std::string
local_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/testCacheDir";
}

std::string
copy_path_storage() {
  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
  if (dir_path.size() == file_path.size())
    dir_path = file_path.substr(0, file_path.rfind('/'));
  return dir_path + "/testCopyFromCacheDir";
}
}// namespace

TEST_CASE("CmlCopy", "[common][cml_test]") {
  fys::cache::CmlCopy ccpy(local_path_storage(), copy_path_storage());
  std::filesystem::path baseCache = local_path_storage() + "/copy_folder";
  std::error_code ec;
  std::filesystem::remove_all(baseCache, ec);
  REQUIRE_FALSE(std::filesystem::exists(baseCache));

  auto content = ccpy.findInCache("copy_folder:inner_copy_folder:testingCopyFile.txt");

  REQUIRE(std::filesystem::exists(baseCache));
  REQUIRE(std::filesystem::exists(baseCache / "inner_copy_folder" / "testingCopyFile.txt"));
  REQUIRE("This is a copied file cache" == content);

  std::filesystem::remove_all(baseCache, ec);
}// End TestCase : CmlCopy