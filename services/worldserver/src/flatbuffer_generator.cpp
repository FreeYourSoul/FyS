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

#include <engine/player_data.hh>

#include <WSResponse_generated.h>

#include <flatbuffer_generator.hh>

namespace fys::ws {

std::pair<void*, std::uint32_t>
flatbuffer_generator::generate_move_notif(const std::string& player_name, const character_info& info) {
  auto move_notification = fb::world::CreateMoveNotification(
      _fbb,
      _fbb.CreateString(player_name),
      info.velocity,
      info.angle,
      info.position.x,
      info.position.y);

  auto message = fb::world::CreateResponseFrame(
      _fbb,
      fb::world::Response_MoveNotification,
      move_notification.Union());

  fb::world::FinishResponseFrameBuffer(_fbb, message);
  return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

std::pair<void*, std::uint32_t> flatbuffer_generator::generate_bulk_move_notif(const std::vector<npc_action>& actions) {

  //  auto bulk_move_notifications = fb::world::CreateBulkMove(
  //  	_fbb,
  //  	);

  //  auto message = fb::world::CreateResponseFrame(
  //	  _fbb,
  //	  fb::world::BulkMove,
  //	  bulk_move_notifications.Union()
  //  );
  return std::pair<void*, std::uint32_t>();
}

}// namespace fys::ws