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

#include <fmt/format.h>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_basic.hpp>
#include <chaiscript/utility/utility.hpp>

#include <fightingPit/contender/ContenderScripting.hh>

#include <ConnectionHandler.hh>

using namespace chaiscript;

namespace fys::arena {

    void ContenderScripting::loadContenderScript(ConnectionHandler &connectionHandler, const std::string &script) {
        if (!script.empty())
            _chai.get().eval(script);

        _chai.get().add(chaiscript::fun<std::function<void(std::string, uint, uint, std::string)> >([&connectionHandler] (
                    std::string targetId,                 // [Optional]  Target (if any) TODO : Should be a pair of <bool, uint>
                    uint contenderId,                     // [Mandatory] id of the contender
                    uint spellId,                         // [Mandatory] Spell/Action id
                    const std::string &descriptionString  // [Mandatory] Description String (Heal of 15, Damage of 17, Critical Damage of 55, Buff intelligence....)
                ) {
                // instantiate a message to send
                if (!targetId.empty() && targetId.size() > 1) {
                    bool isFriendlyTarget = (targetId.at(0) == 'F');
                    uint idTargetInteger = std::stoi(targetId.substr(1));
                    // set the id value of the message to send
                }
                // continue setting values
                // send the data to connectionHandler

            }
        ), getChaiMethodName("sendContenderAction"));

        _chai.get().add(chaiscript::fun<std::function<unsigned ()> >([]() {
            return std::rand() % 100;
        }), "generateRandomNumber");

        // instantiate the contender in chai engine
        std::string createVar = fmt::format("global {}={}({});",
                getChaiContenderId(), _contenderName, std::to_string(_contenderId));
        _chai.get().eval(createVar);
    }

    void ContenderScripting::loadContenderScriptFromFile(ConnectionHandler &connectionHandler, const std::string &filePath) {
        // load script content (containing the class)
        try {
            _chai.get().eval_file(filePath);
            loadContenderScript(connectionHandler, "");
        } catch(std::exception &ex) {
            SPDLOG_ERROR("Error caught on scripting loading {}", ex.what());
        }
    }

    void ContenderScripting::executeAction() {
        std::string action = fmt::format("fun(contenderId){{ return {}.runScriptedAction(contenderId);}}", getChaiContenderId());
        auto funcAction = _chai.get().eval<std::function<int (unsigned int)>> (action);
        try {
            auto o = _chai.get().get_locals();
            if (funcAction(_contenderId))
                std::cout << "Action executed\n"; // TODO change it
        }
        catch (std::exception &ex) {
            SPDLOG_ERROR("Error caught on script execution {}", ex.what());
        }
    }
}
