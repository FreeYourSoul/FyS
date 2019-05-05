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

#ifndef FYS_WORLDSERVERSERVICE_HH
#define FYS_WORLDSERVERSERVICE_HH

#include <engine/WorldServerEngine.hh>
#include <ConnectionHandler.hh>

namespace fys::ws {

    class WorldServerService {

    public:
        explicit WorldServerService(const WorldServerContext &ctx);
        void runServerLoop() noexcept;

    private:
        void processMessage(zmq::multipart_t &&msg) noexcept;
//        constexpr bool filterInvalidMsg() const;

    private:
        fys::ws::WorldServerEngine _worldServer;
        fys::ws::ConnectionHandler _connectionHandler;
    };

}


#endif //FYS_WORLDSERVERSERVICE_HH
