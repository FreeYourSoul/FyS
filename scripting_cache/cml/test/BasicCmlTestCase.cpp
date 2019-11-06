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

#include <catch2/catch.hpp>
#define private protected
#include <Cml.hh>

class CmlBaseTest : public fys::cache::Cml {
public:
    using fys::cache::Cml::Cml;
    using fys::cache::Cml::isInLocalStorageAndUpToDate;
    using fys::cache::Cml::_inMemCache;

    bool _localStorageCalled = false;

    void createFileInLocalStorage(const fys::cache::CmlKey &cmlKey) override {
        _localStorageCalled = true;
    }
};

std::string getLocalPathStorage() {
    string file_path = __FILE__;
    auto it = file_path.rfind("\\");
    if (it == file_path.end())
        it = file_path.rfind("/");
    return file_path.substr(0, it) + "/testCacheDir";
}

TEST_CASE("Test findInCache for Basic CML", "[cml_test]") {
    CmlBaseTest cbt(getLocalPathStorage());

    SECTION("Test call of createFileInLocalStorage") {
        cbt.findInCache("inner_folder:test");
        REQUIRE_TRUE(cbt._localStorageCalled);

    } // End Section : Test call of createFileInLocalStorage

    SECTION("Test CML on filesystem") {
        REQUIRE(cbt._inMemCache.find("test1") == _inMemCache.end());
        REQUIRE(cbt._inMemCache.find("inner_folder:test2") == _inMemCache.end());
        REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test3") == _inMemCache.end());
        
        SECTION("Test file at root") {
            std::string_view cacheContent = cbt.findInCache("test1");

            REQUIRE("test1 content" == cacheContent);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("test1") != _inMemCache.end());

        } // End Section : Test file at root

        SECTION("Test file in folder") {
            std::string_view cacheTest2 = cbt.findInCache("inner_folder:test2");
            std::string_view cacheTest3 = cbt.findInCache("inner_folder:inner_folder_1:test3");

            REQUIRE("test2 content" == cacheContent2);
            REQUIRE("test3 content" == cacheContent3);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("inner_folder:test2") != _inMemCache.end());
            REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test3") != _inMemCache.end());

        } // End Section : Test file in folder

        SECTION("Test Multiple line file") {
            std::string contentToFind = R
"A thing of beauty is a joy for ever:
Its loveliness increases; it will never
Pass into nothingness; but still will keep
A bower quiet for us, and a sleep
Full of sweet dreams, and health, and quiet breathing.

– John Keats (1)";
            std::string_view cacheTest4 = cbt.findInCache("inner_folder:inner_folder_1:test4");

            REQUIRE(contentToFind == cacheContent3);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test4") != _inMemCache.end());

        }

    } // End section : Test CML on filesystem

} // End TestCase : Test Basic CML

TEST_CASE("Test isInLocalStorageAndUpToDate for Basic CML", "[cml_test]") {
    CmlBaseTest cbt(getCurrentFilePath() + "/testCacheDir");
    const CmlKey key1 = fys::cache::CmlKey{getLocalPathStorage(), "test1"};
    const CmlKey key2 = fys::cache::CmlKey{getLocalPathStorage(), "inner_folder:test2"};
    const CmlKey key3 = fys::cache::CmlKey{getLocalPathStorage(), "inner_folder:inner_folder_1:test3"};

    // Get from filesystem and store in memcache
    cbt.findInCache("test1");
    cbt.findInCache("inner_folder:test2");

    SECTION("isInLocalStorageAndUpToDate true") {
        cbt.findInCache(key1.getKey());

        if (auto it = _inMemCache.find(key1.getKey()); it != _inMemCache.end()) {
            REQUIRE(isInLocalStorageAndUpToDate(key1, it->second.timestamp));
            REQUIRE("test1 content" == it->second.content);
        }
        else
            FAIL("test1 has not been found in memcache");

        if (auto it = _inMemCache.find(key2.getKey()); it != _inMemCache.end()) {
            REQUIRE(isInLocalStorageAndUpToDate(key2, it->second.timestamp));
            REQUIRE("test2 content" == it->second.content);
        }
        else
            FAIL("test2 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate

    SECTION("isInLocalStorageAndUpToDate false: Not in memCache") {
        if (auto it = _inMemCache.find(key3.getKey()); it != _inMemCache.end()) {
            FAIL("test3 has not been found in memcache before being called");
        }
        cbt.findInCache("inner_folder:test2");
        if (auto it = _inMemCache.find(key3.getKey()); it != _inMemCache.end()) {
            REQUIRE(isInLocalStorageAndUpToDate(key3, it->second.timestamp));
            REQUIRE("test3 content" == it->second.content);
        }
        else
            FAIL("test3 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate false: Not in memCache

    SECTION("isInLocalStorageAndUpToDate false: timestamp changed") {
        if (auto it = _inMemCache.find(key3.getKey()); it != _inMemCache.end()) {
            FAIL("test3 has not been found in memcache before being called");
        }
        cbt.findInCache("inner_folder:test2");
        if (auto it = _inMemCache.find(key3.getKey()); it != _inMemCache.end()) {
            // Add 90seconds on the timestamp to represent a file update
            REQUIRE(isInLocalStorageAndUpToDate(key3, it->second.timestamp + 90)); 
            REQUIRE("test3 content" == it->second.content);
        }
        else
            FAIL("test3 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate false: Not in memCache

} // End TestCase : Test Implementation for Basic CML
