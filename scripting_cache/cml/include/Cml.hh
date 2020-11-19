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

#ifndef FYS_SERVICE_CML_HH
#define FYS_SERVICE_CML_HH

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace fys::cache {

class CmlKey;

struct InMemoryCached {
  std::filesystem::file_time_type lastWriteTime;
  std::string content;
};

class Cml {

public:
  virtual ~Cml() = default;
  explicit Cml(std::filesystem::path pathLocalStorage);

  const std::string& findInCache(const std::string& key);

  virtual void createUpToDateFileInLocalStorage(const CmlKey& cmlKey, std::filesystem::file_time_type cacheTime) = 0;

  void createFile(const std::filesystem::path& pathToFile, const std::string& content) const;

private:
  [[nodiscard]] inline std::pair<bool, std::filesystem::file_time_type>
  localStorageInfo(const CmlKey& k) const;

  [[nodiscard]] bool
  isInLocalStorageAndUpToDate(const CmlKey& cmlKey, std::filesystem::file_time_type cacheLastUpdate) const;

private:
  std::filesystem::path _localPathStorage;
  std::unordered_map<std::string, InMemoryCached> _inMemCache;
};

}// namespace fys::cache

#endif//FYS_SERVICE_CML_HH
