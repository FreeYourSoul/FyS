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

#ifndef FYS_ONLINE_CMLSCRIPTDOWNLOADER_HH
#define FYS_ONLINE_CMLSCRIPTDOWNLOADER_HH

#include <cml.hh>
#include <functional>

namespace fys::cache {

class cml_script_downloader final : cache::cml {

public:
  virtual ~cml_script_downloader() = default;

  template<typename DownloaderFunc>
  cml_script_downloader(std::filesystem::path pathLocalStorage, DownloaderFunc&& downloader)
      : cache::cml(std::move(pathLocalStorage)), _downloader(std::forward<DownloaderFunc>(downloader)) {}

private:
  void createUpToDateFileInLocalStorage(const cml_key& cml_key, std::filesystem::file_time_type cache_time) override;

private:
  std::function<void(const std::string&, const std::string&)> _downloader;
};

}// namespace fys::cache

#endif//FYS_ONLINE_CMLSCRIPTDOWNLOADER_HH
