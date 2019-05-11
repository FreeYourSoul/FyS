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
#include <zmq_addon.hpp>
#include <flatbuffers/flatbuffers.h>
#include <WSAction_generated.h>
#include "WorldServerService.hh"

namespace fys::ws {

    WorldServerService::WorldServerService(const WorldServerContext &ctx) : _worldServer(ctx) {
        _connectionHandler.setupConnectionManager(ctx);

    }

    void WorldServerService::runServerLoop() noexcept {
        spdlog::get("c")->info("WorldServer loop started");

        while (true) {
            _connectionHandler.pollAndProcessSubMessage(
                [this](zmq::multipart_t &&msg) {
                    spdlog::get("c")->debug("message {}", msg.str());
                    if (msg.size() != 3) {
                        spdlog::get("c")->error("Received message is ill formatted, should contains 3 parts but has {}", msg.size());
                        return;
                    }
                    std::string idt = std::string(static_cast<char*>(msg.at(0).data()), msg.at(0).size());
                    std::string token = std::string(static_cast<char*>(msg.at(1).data()), msg.at(1).size());

                    processMessage(std::move(idt), std::move(token), fys::fb::GetWSAction(msg.at(2).data()));
                }
            );
        }
    }

    void WorldServerService::processMessage(std::string &&idt, std::string &&token, const fb::WSAction *content) noexcept {

    }

}