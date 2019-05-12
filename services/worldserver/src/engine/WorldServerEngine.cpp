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
#include <WorldServerContext.hh>
#include <WSAction_generated.h>
#include "engine/WorldServerEngine.hh"

namespace fys::ws {

    WorldServerEngine::WorldServerEngine(const fys::ws::WorldServerContext &ctx) : _map(ctx) {

    }

    void WorldServerEngine::processPlayerInputMessage(std::string &&idt, std::string &&token, const fb::WSAction *action) {
        if (const uint index = _data.getIndexAndUpdatePlayerConnection(token, std::move(idt));
             index < std::numeric_limits<uint>::max()) {

        } else {
            SPDLOG_ERROR("Player of token {} has not been registered before sending messages", token);
        }
    }

}