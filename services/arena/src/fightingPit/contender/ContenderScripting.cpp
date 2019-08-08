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

using namespace chaiscript;

namespace fys::arena {

    ContenderScripting::ContenderScripting() : _chai(chaiscript::Std_Lib::library()), {
        registerChaiPitContender();
        registerChaiAllies();
    }

    void ContenderScripting::registerChaiAllies() {

        chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
        
        chaiscript::utility::add_class<fys::arena::TeamMember>(*m,
            "TeamMember",
            { 
              constructor<TeamMember ()>(),
              constructor<TeamMember (const TeamMember &)>() 
            },
            { 
                {fun(&TeamMember::accessStatus), "accessStatus"},
            }
        );

        _chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<TeamMember>>>());

        chaiscript::utility::add_class<fys::arena::AllyPartyTeams>(*m,
            "AllyPartyTeams",
            { 
              constructor<AllyPartyTeams ()>(),
              constructor<AllyPartyTeams (const AllyPartyTeams &)>() 
            },
            { 
                {fun(&AllyPartyTeams::accessAlliesOnSide), "accessAlliesOnSide"},
            }
        );
        _chai.add(m);
    }

    void ContenderScripting::registerChaiPitContender() {
        chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
        
        chaiscript::utility::add_class<fys::arena::FightingContender>(*m,
            "FightingContender",
            { 
              constructor<FightingContender ()>(),
              constructor<FightingContender (const FightingContender &)>() 
            },
            { 
                {fun(&FightingContender::accessStatus), "accessStatus"},
            }
        );

        _chai.add(chaiscript::vector_conversion<std::vector<std::shared_ptr<FightingContender>>>());

        chaiscript::utility::add_class<fys::arena::PitContenders>(*m,
            "PitContenders",
            { 
              constructor<PitContenders ()>(),
              constructor<PitContenders (const PitContenders &)>() 
            },
            { 
                {fun(&PitContenders::getContenderOnSide), "getContenderOnSide"},
            }
        );
        _chai.add(m);
    }


    void ContenderScripting::loadContenderScript(const std::string &script) {
        if (!script.empty())
            _chai.eval(script);


        _chai.add(chaiscript::fun< std::function<void()> >([]() {
                // Create message with data :
                //     * [Optional]  Target (if any)
                //     * [Mandatory] Contender id 
                //     * [Mandatory] Spell/Action id
                //     * [Mandatory] Description String (Heal of 15, Damage of 17, Critical Damage of 55, Buff intelligence....)
                // get Allies on the contender side
            }
            , getChaiMethodName("sendContenderAction")
        );

        // _chai.add(chaiscript::fun<std::function<data::Status()>>([]() {

        // }), getChaiMethodName("getStatus"));

        // _chai.add(chaiscript::fun<std::function<int()>>([]() {

        // }), getChaiMethodName("getSide"));

        // instantiate the contender in chai engine
        _chai.eval(std::string("var ").append(getChaiContenderId()).append(" = ").append(_contendeName).append("();"));
    }
    
    void ContenderScripting::loadContenderScriptFromFile(const std::string &filePath) {
        // load script content (containing the class)
        _chai.eval_file(filePath);
        loadContenderScript("");
    }

    // returning the function object may be better ?? 
    // TODO : change the PitContender and AllyPartyTeam into loadContenderScript
    void ContenderScripting::executeAction(PitContenders &pc, AllyPartyTeams &apt) { 
        _chai.set_global(chaiscript::var(std::ref(pc), "pitContenders");
        _chai.set_global(chaiscript::var(std::ref(apt), "allyPartyTeams");

        std::string action = 
            std::string("fun(contenderId){\n").append(getChaiContenderId()).append(".runScriptedAction(contenderId);\n}\n");
        _chai.eval<std::function<void (unsigned int)>> (action) (_contenderId);
    }

}
