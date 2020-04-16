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

#include <spdlog/spdlog.h>
#include <optional>
#include <fstream>
#include <Cml.hh>
#include <CmlKey.hh>

namespace fys::cache {

    namespace {
        std::string readFile(const std::filesystem::path& path) {
            // Open the stream to 'lock' the file.
            std::ifstream f{ path };

            // Obtain the size of the file.
            const auto sz = std::filesystem::file_size(path);

            // Create a buffer.
            std::string result(sz, ' ');

            // Read the whole file into the buffer.
            f.read(result.data(), sz);

            return result;
        }

        void writeFile(const std::filesystem::path& file, const std::string &content) {
            std::ofstream out(file.string());
            out << content << std::endl;
            out.close();
        }

    }

    Cml::Cml(std::filesystem::path pathLocalStorage) : _localPathStorage(std::move(pathLocalStorage)) {
        try {
            bool ec = std::filesystem::create_directories(_localPathStorage);
        } catch (...) { }
    }

    /**
     * @return true if the key represent a cached data in the local storage (filesystem)
     */
    bool Cml::isInLocalStorage(const CmlKey &cmlKey) const {
        return std::filesystem::is_regular_file(cmlKey.getPath());
    }

    bool Cml::isInLocalStorageAndUpToDate(const CmlKey &cmlKey, long timestamp) const {
        if (isInLocalStorage(cmlKey)) {
            return std::filesystem::last_write_time(cmlKey.getPath()).time_since_epoch().count() <= timestamp;
        }
        return false;
    }

    const std::string &Cml::findInCache(const std::string &key, bool first)  {
        static const std::string empty {};
        CmlKey cmlKey(_localPathStorage, key);

        // Check in-memory (and check if file has been updated)
        if (auto it = _inMemCache.find(key); it != _inMemCache.end()) {
            if (isInLocalStorageAndUpToDate(cmlKey, it->second.timestamp)) {
                return it->second.content;
            }
        }
        // Check in filesystem cache
        if (isInLocalStorage(cmlKey)) {
            _inMemCache[key] = InMemoryCached{ std::chrono::system_clock::now().time_since_epoch().count(),
                                               readFile(cmlKey.getPath()) };
            return _inMemCache[key].content;
        }
        if (!first) return empty;
        createFileInLocalStorage(cmlKey);
        return findInCache(key, false);
    }

    void Cml::createFile(const std::filesystem::path &pathToFile, const std::string &content) const {
        if (!std::filesystem::create_directories(pathToFile.parent_path())) {
            SPDLOG_ERROR("Couldn't create directories for path : {}", _localPathStorage.string());
            return;
        }
        writeFile(pathToFile, content);
    }

}
