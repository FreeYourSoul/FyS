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
#include <Cml.hh>

class CmlBaseTest : public fys::cache::Cml {
public:
    using fys::cache::Cml::Cml;

    bool _localStorageCalled = false;

    void createFileInLocalStorage(const fys::cache::CmlKey &cmlKey) override {
        _localStorageCalled = true;
    }
};

TEST_CASE("Test Basic CML", "[cml_test]") {

    CmlBaseTest cbt("/home/FyS/Project/FyS/scripting_cache/cml/test/testCacheDir");

    SECTION("Test in memcache") {

    } // End section : Test in memcache

    SECTION("Test on filesystem") {

    } // End section : Test on filesystem


} // End TestCase : Test Basic CML