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


#ifndef FYS_ONLINE_PROTOCOLHELPER_HH
#define FYS_ONLINE_PROTOCOLHELPER_HH

#include <ArenaServerService.hh>
#include <ArenaServerAuth_generated.h>

namespace fys::arena::protocol::helper {

    const fys::fb::ArenaServerAuth *serializeArenaServerAuth(const AwaitingArena & awaitingArena) {
        flatbuffers::FlatBufferBuilder fbb;
        auto asa = fys::fb::CreateArenaServerAuthDirect(
                fbb,
                "name",
                "token_authentication",
                "localhost",
                "tcp://localhost:4242",
                42,
                "WS001");
        fys::fb::FinishArenaServerAuthBuffer(fbb, asa);
        return fys::fb::GetArenaServerAuth(fbb.GetBufferPointer());
    }

}

#endif //FYS_ONLINE_PROTOCOLHELPER_HH
