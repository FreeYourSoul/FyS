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


#ifndef FYS_ARENASERVERCONTEXT_HH
#define FYS_ARENASERVERCONTEXT_HH

#include <array>
#include <nlohmann/json.hpp>
#include <ServiceContextBase.hh>

namespace fys::arena {

    struct EncounterContext {
        struct EncounterDesc {
            std::string key;
            uint maxEncountering;
            std::array<uint, 3> chance;
        };

        std::array<std::pair<uint, uint>, 3> _rangeEncounter;
        std::vector<EncounterDesc>        _contenders;
    };

    class ArenaServerContext : fys::common::ServiceContextBase {
    public:
        ArenaServerContext(int ac, const char *const *av);

        [[nodiscard]] std::string toString() const;
        [[nodiscard]] std::string getDispatcherConnectionString() const noexcept;
        [[nodiscard]] const std::string &getPathLocalStorageCache() const { return _pathLocalStorageCache; }
        [[nodiscard]] const std::string &getPathSourceCache() const { return _pathSourceCache; }
        [[nodiscard]] const std::string &getDbHost() const { return _dbHost; }
        [[nodiscard]] uint getDbPort() const { return _dbPort; }

    private:
        void parseArenaConfigFile(const nlohmann::json &configContent);
        bool validateEncounterContext() const;

    private:
        std::string _code;
        std::string _pathLocalStorageCache;
        std::string _pathSourceCache;
        std::string _dbHost;
        uint _dbPort = 3306;

        EncounterContext _encounterContext;
    };

}

#endif // !FYS_ARENASERVERCONTEXT_HH
