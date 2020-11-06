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

#ifndef FYS_ONLINE_WS_FLATBUFFERGENERATOR_HH
#define FYS_ONLINE_WS_FLATBUFFERGENERATOR_HH

#include <flatbuffers/flatbuffers.h>

// forward declarations
namespace fys::ws {
struct character_info;
struct npc_action;
}// namespace fys::ws
// end forward declarations

namespace fys::ws {

class flatbuffer_generator {

public:
  std::pair<void *, std::uint32_t>
  generate_bulk_move_notif(const std::vector<npc_action> &);

  [[nodiscard]] std::pair<void *, std::uint32_t>
  generate_move_notif(const std::string &player_name, const character_info &info);

private:
  flatbuffers::FlatBufferBuilder _fbb;
};

}// namespace fys::ws

#endif//FYS_ONLINE_WS_FLATBUFFERGENERATOR_HH
