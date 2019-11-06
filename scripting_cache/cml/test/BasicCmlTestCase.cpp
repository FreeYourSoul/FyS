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
#include <CmlKey.hh>
#define private protected
#include <Cml.hh>

class CmlBaseTest : public fys::cache::Cml {
public:
    CmlBaseTest(std::string v) : fys::cache::Cml(std::filesystem::path(std::move(v))) {}
    using fys::cache::Cml::isInLocalStorageAndUpToDate;
    using fys::cache::Cml::_inMemCache;

    bool _localStorageCalled = false;

    void createFileInLocalStorage(const fys::cache::CmlKey &cmlKey) override {
        _localStorageCalled = true;
    }
};

std::string getLocalPathStorage() {
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind('\\'));
    if (dir_path.size() == file_path.size())
        dir_path = file_path.substr(0, file_path.rfind('/'));
    return dir_path + "/testCacheDir";
}

TEST_CASE("Test findInCache for Basic CML", "[cml_test]") {
    CmlBaseTest cbt(getLocalPathStorage());

    SECTION("Test call of createFileInLocalStorage") {
        cbt.findInCache("inner_folder:testKO");
        REQUIRE(cbt._localStorageCalled);

    } // End Section : Test call of createFileInLocalStorage

    SECTION("Test CML on filesystem") {
        REQUIRE(cbt._inMemCache.find("test1") == cbt._inMemCache.end());
        REQUIRE(cbt._inMemCache.find("inner_folder:test2") == cbt._inMemCache.end());
        REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test3") == cbt._inMemCache.end());
        
        SECTION("Test file at root") {
            std::string_view cacheContent = cbt.findInCache("test1");

            REQUIRE("test1 content" == cacheContent);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("test1") != cbt._inMemCache.end());

        } // End Section : Test file at root

        SECTION("Test file in folder") {
            std::string_view cacheTest2 = cbt.findInCache("inner_folder:test2");
            std::string_view cacheTest3 = cbt.findInCache("inner_folder:inner_folder_1:test3");

            REQUIRE("test2 content" == cacheTest2);
            REQUIRE("test3 content" == cacheTest3);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("inner_folder:test2") != cbt._inMemCache.end());
            REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test3") != cbt._inMemCache.end());

        } // End Section : Test file in folder

        SECTION("Test Multiple line file") {
            std::string contentToFind =
R"d(A thing of beauty is a joy for ever:
Its loveliness increases; it will never
Pass into nothingness; but still will keep
A bower quiet for us, and a sleep
Full of sweet dreams, and health, and quiet breathing.

John Keats)d";
            std::string_view cacheTest4 = cbt.findInCache("inner_folder:inner_folder_1:test4");

            REQUIRE(contentToFind == cacheTest4);
            REQUIRE_FALSE(cbt._localStorageCalled);
            REQUIRE(cbt._inMemCache.find("inner_folder:inner_folder_1:test4") != cbt._inMemCache.end());

        }

    } // End section : Test CML on filesystem

} // End TestCase : Test Basic CML

TEST_CASE("Test isInLocalStorageAndUpToDate for Basic CML", "[cml_test]") {
    CmlBaseTest cbt(getLocalPathStorage());
    const fys::cache::CmlKey key1 = fys::cache::CmlKey{getLocalPathStorage(), "test1"};
    const fys::cache::CmlKey key2 = fys::cache::CmlKey{getLocalPathStorage(), "inner_folder:test2"};
    const fys::cache::CmlKey key3 = fys::cache::CmlKey{getLocalPathStorage(), "inner_folder:inner_folder_1:test3"};

    // Get from filesystem and store in memcache
    cbt.findInCache("test1");
    cbt.findInCache("inner_folder:test2");

    SECTION("isInLocalStorageAndUpToDate true") {
        cbt.findInCache(key1.getKey());

        if (auto it = cbt._inMemCache.find(key1.getKey()); it != cbt._inMemCache.end()) {
            REQUIRE(cbt.isInLocalStorageAndUpToDate(key1, it->second.timestamp));
            REQUIRE("test1 content" == it->second.content);
        }
        else
            FAIL("test1 has not been found in memcache");

        if (auto it = cbt._inMemCache.find(key2.getKey()); it != cbt._inMemCache.end()) {
            REQUIRE(cbt.isInLocalStorageAndUpToDate(key2, it->second.timestamp));
            REQUIRE("test2 content" == it->second.content);
        }
        else
            FAIL("test2 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate

    SECTION("isInLocalStorageAndUpToDate false: Not in memCache") {
        if (auto it = cbt._inMemCache.find(key3.getKey()); it != cbt._inMemCache.end()) {
            FAIL("test3 has not been found in memcache before being called");
        }
        auto content = cbt.findInCache("inner_folder:inner_folder_1:test3");
        REQUIRE("test3 content" == content);
        if (auto it = cbt._inMemCache.find(key3.getKey()); it != cbt._inMemCache.end()) {
            REQUIRE(cbt.isInLocalStorageAndUpToDate(key3, it->second.timestamp));
            REQUIRE("test3 content" == it->second.content);
        }
        else
            FAIL("test3 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate false: Not in memCache

    SECTION("isInLocalStorageAndUpToDate false: timestamp changed") {
        if (auto it = cbt._inMemCache.find(key3.getKey()); it != cbt._inMemCache.end()) {
            FAIL("test3 has not been found in memcache before being called");
        }
        auto content = cbt.findInCache("inner_folder:inner_folder_1:test3");
        REQUIRE("test3 content" == content);
        if (auto it = cbt._inMemCache.find(key3.getKey()); it != cbt._inMemCache.end()) {
            // Add 90seconds on the timestamp to represent a file update
            REQUIRE(cbt.isInLocalStorageAndUpToDate(key3, it->second.timestamp + 90)); 
            REQUIRE("test3 content" == it->second.content);
        }
        else
            FAIL("test3 has not been found in memcache");

    } // End Section : isInLocalStorageAndUpToDate false: Not in memCache

} // End TestCase : Test Implementation for Basic CML
