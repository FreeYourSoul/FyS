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

#include <engine/PlayersData.hh>

#include <WSResponse_generated.h>

#include <FlatbufferGenerator.hh>

namespace fys::ws {

std::pair<void*, uint>
fys::ws::FlatbufferGenerator::generateMoveNotification(const std::string& playerName, const fys::ws::PlayerInfo& info)
{
	auto moveNotification = fb::world::CreateMoveNotification(
			_fbb,
			_fbb.CreateString(playerName),
			info.velocity,
			info.angle,
			info.pos.x,
			info.pos.y
	);

	auto message = fb::world::CreateResponseFrame(
			_fbb,
			fys::fb::world::Response_MoveNotification,
			moveNotification.Union()
	);

	fb::world::FinishResponseFrameBuffer(_fbb, message);
	return std::pair(_fbb.GetBufferPointer(), _fbb.GetSize());
}

}