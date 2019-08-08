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

#include <chaiscript/chaiscript.hpp>

namespace fys::arena {

    // TODO:  Class should inherit from a generic class for scripting (as Ally are also going to use a lot of the same config)
    class ContenderScripting {

    public:
        ContenderScripting();

        void executeAction();
        
        void loadContenderScript(const std::string& script);
        void loadContenderScriptFromFile(const std::string& scriptFile);

        void setContenderName(std::string contenderName) { _contenderName = std::move(contenderName); }
        void setContenderId(uint contenderId) { _contenderId = contenderId; }

    private:
        void registerChaiAllies();
        void registerChaiPitContender();

        std::string getChaiMethodName(std::string && methodName) const { return _contenderName.append("::").append(std::move(methodName)); }
        std::string getChaiContenderId() const { return std::string("contender_").append(std::to_string(cotnenderId)); }

    private:
        chaiscript::ChaiScript _chai;
        std::string _contenderName;
        uint _contenderId;

    };

}

#endif // !FYS_CONTENDERSCRIPTING_HH
