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

#include <cml.hh>
#include <cml_key.hh>
#include <fstream>
#include <optional>
#include <spdlog/spdlog.h>

namespace fys::cache {

namespace {
[[nodiscard]] std::string
readFile(const std::filesystem::path& path) {
  // Open the stream to 'lock' the file.
  std::ifstream f{path};

  // Obtain the size of the file.
  const auto sz = std::filesystem::file_size(path);

  // Create a buffer.
  std::string result(sz, ' ');

  // Read the whole file into the buffer.
  f.read(result.data(), sz);

  return result;
}

void writeFile(const std::filesystem::path& file, const std::string& content) {
  std::ofstream out(file.string());
  out << content << std::endl;
  out.close();
}

}// namespace

cml::cml(std::filesystem::path pathLocalStorage)
    : _localPathStorage(std::move(pathLocalStorage)) {
  try {
    std::filesystem::create_directories(_localPathStorage);
  } catch (...) {}
}

/**
 * @return true if the key represent a cached data in the local storage (filesystem)
 */
std::pair<bool, std::filesystem::file_time_type>
cml::localStorageInfo(const cml_key& k) const {
  std::error_code ec;
  return std::pair(std::filesystem::is_regular_file(k.get_path()), std::filesystem::last_write_time(k.get_path(), ec));
}

bool cml::isInLocalStorageAndUpToDate(const cml_key& cmlKey, std::filesystem::file_time_type cacheLastUpdate) const {
  auto [exist, lastWriteTime] = localStorageInfo(cmlKey);
  if (exist) {
    return lastWriteTime <= cacheLastUpdate;
  }
  return false;
}

const std::string&
cml::findInCache(const std::string& key) {
  static const std::string empty{};
  cml_key cmlKey(_localPathStorage, key);

  // Check in-memory (and check if file has been updated)
  if (auto it = _inMemCache.find(key); it != _inMemCache.end()) {
    if (isInLocalStorageAndUpToDate(cmlKey, it->second.lastWriteTime)) {
      return it->second.content;
    }
  }

  // If nothing found in the cache, create a up to date cache file
  createUpToDateFileInLocalStorage(cmlKey, localStorageInfo(cmlKey).second);

  // Check in filesystem cache
  auto [exist, lastWriteTime] = localStorageInfo(cmlKey);

  // Read the file and store it in the cache
  if (exist) {
    _inMemCache[key] = InMemoryCached{lastWriteTime, readFile(cmlKey.get_path())};
    return _inMemCache[key].content;
  }
  SPDLOG_ERROR("Something wrong occurred in the cache update, no file returned");
  return empty;
}

void cml::createFile(const std::filesystem::path& pathToFile, const std::string& content) const {
  if (!std::filesystem::create_directories(pathToFile.parent_path())) {
    SPDLOG_ERROR("Couldn't create directories for path : {}", _localPathStorage.string());
    return;
  }
  writeFile(pathToFile, content);
}

}// namespace fys::cache
