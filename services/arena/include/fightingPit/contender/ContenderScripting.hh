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


#ifndef FYS_CONTENDERSCRIPTING_HH
#define FYS_CONTENDERSCRIPTING_HH

#include <memory>
#include <string>

// forward declarations
namespace chaiscript {
    class ChaiScript;
}
namespace fys::arena {
    class ConnectionHandler;
    class PitContenders;
    class AllyPartyTeams;
}

namespace fys::arena {

    class ContenderScripting {

    public:
        explicit ContenderScripting(chaiscript::ChaiScript &chai, uint level) : _chai(chai), _level(level)
        {}

        void executeAction();

        /**
         * Method called after the contender has been added in the fighting pit, in order to initialize its stats
         */
        void setupContender();

        void loadContenderScript(const std::string& script = "");
        void loadContenderScriptFromFile(const std::string& scriptFile);

        void setContenderName(std::string contenderName) { _contenderName = std::move(contenderName); }
        void setContenderId(uint contenderId) { _contenderId = contenderId; }

    private:
        [[nodiscard]] std::string getChaiMethodName(std::string && methodName) const { return _contenderName + "_" + std::move(methodName); }
        [[nodiscard]] std::string getChaiContenderId() const { return std::string("contender_").append(_contenderName).append(std::to_string(_contenderId)); }

    private:
        std::reference_wrapper<chaiscript::ChaiScript> _chai;
        std::string _contenderName;
        uint _level;
        uint _contenderId{};

    };

    using ContenderScriptingUPtr = std::unique_ptr<ContenderScripting>;

}

#endif // !FYS_CONTENDERSCRIPTING_HH
