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

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>
#include <chaiscript/utility/utility.hpp>
#include <fightingPit/contender/ContenderScripting.hh>

namespace fys::arena {

    ContenderScripting::ContenderScripting() : _chai(chaiscript::Std_Lib::library()), {
        registerChaiPitContender();
        registerChaiAllies();
    }

    void ContenderScripting::registerChaiAllies() {

        chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
        chaiscript::utility::add_class<fys::arena::AllyPartyTeams>(*m,
            "Utility_Test",
            { constructor<PartyTeam ()>(),
              constructor<PartyTeam (const AllyPartyTeams &)>() },
            { 
                {fun(&AllyPartyTeams::function), "function"},
            }
        );
    }

    void ContenderScripting::registerChaiPitContender() {
        
    }

    void ContenderScripting::loadContenderScript(const std::string &script) {
        // load script content (containing the class)
        _chai.eval(script);

        // instantiate the contender in chai engine
        _chai.eval(std::string("var ").append(_contenderId).append(" = ").append(_contendeName).append("()"));
    }
    

    void ContenderScripting::loadContenderScriptFromFile(const std::string &filePath) {
        
    }

    void ContenderScripting::executeAction(PitContenders &pc, AllyPartyTeams &apt) {
        _chai.eval<std::function<void (PitContenders &, AllyPartyTeams &)>> (R"(
            fun(pitContenders, allyPartyTeams) { 
                
            }
        )") (pc, apt);
    }

}
