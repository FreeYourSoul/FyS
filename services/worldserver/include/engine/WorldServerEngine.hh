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


#ifndef FYS_WORLDSERVERENGINE_HH
#define FYS_WORLDSERVERENGINE_HH

#include <memory>
#include <utility>
#include <engine/PlayersData.hh>
#include <engine/CollisionMap.hh>
#include <WSAction_generated.h>

namespace fys::fb {
    class WSAction;
}

namespace fys::ws {

    // forward declaration
    class WorldServerContext;
    class CollisionMap;

    class WorldServerEngine {

    public:
        explicit WorldServerEngine(const WorldServerContext& ctx);

        void processPlayerInputMessage(std::string &&idt, std::string &&token,
                const fys::fb::WSAction *actionMsg, ConnectionHandler &handler);

        void executePendingActions(ws::ConnectionHandler &conn);

    private:
        inline void movePlayerAction(const std::string &idt, uint indexPlayer, PlayerInfo &pi, ws::ConnectionHandler &conn);
        inline void notifyClientsOfMove(const std::vector<std::string_view> &ids, ws::ConnectionHandler &conn) const;

    private:
        CollisionMap _map;
        PlayersData _data;

    };

}


#endif //FYS_WORLDSERVERENGINE_HH
