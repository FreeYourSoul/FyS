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

#include <flatbuffers/flatbuffers.h>
#include <InventoryRequest_generated.h>
#include <ExchangeInventory_generated.h>
#include "InventoryServerService.hh"

namespace {

template<typename T>
[[nodiscard]] bool
verifyBuffer(const void* fbBuffer, uint size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}

}

namespace fys::inv {

InventoryServerService::InventoryServerService(const InventoryServerContext& ctx)
{
	_connectionHandler.setupConnectionManager(ctx.getDispatcherConnectionString());
}

void
InventoryServerService::runServerLoop()
{
	SPDLOG_INFO("InventoryServer loop started");

	while (true) {
		_connectionHandler.pollAndProcessMessageFromDispatcher(
				[this](zmq::message_t&& identity, zmq::message_t&& worldServerMessage) {
					if (!verifyBuffer<fb::InventoryRequest>(worldServerMessage.data(), worldServerMessage.size())) {
						SPDLOG_ERROR("Wrongly formatted InventoryRequest buffer");
						return;
					}
					const auto* binary = fys::fb::GetInventoryRequest(worldServerMessage.data());
					switch (binary->content_type()) {
						case fb::InventoryRequestContent_RetrievePlayerInventory:
							retrievePlayerInventory(binary->content_as_RetrievePlayerInventory());
							break;
						case fb::InventoryRequestContent_UpdatePlayerSoulDraughtboard:
							updatePlayerSoulDraughtboard(binary->content_as_UpdatePlayerSoulDraughtboard());
							break;
						case fb::InventoryRequestContent_RetrievePlayerSoulDraughtboard:
							retrievePlayerSoulDraughtboard(binary->content_as_RetrievePlayerSoulDraughtboard());
							break;
						case fb::InventoryRequestContent_ExchangeInventory:
							exchangeInventory(binary->content_as_ExchangeInventory());
							break;
						default:
							SPDLOG_ERROR("Request type '{}' not handled by Inventory Server Service", binary->content_type());
							break;
					}
				});
	}
}

void
InventoryServerService::exchangeInventory(const fb::ExchangeInventory* exchangeRq) {

}

void
InventoryServerService::retrievePlayerInventory(const fb::RetrievePlayerInventory* retrieveRq)
{

}

void
InventoryServerService::updatePlayerSoulDraughtboard(const fb::UpdatePlayerSoulDraughtboard* updateRq)
{

}

void
InventoryServerService::retrievePlayerSoulDraughtboard(const fb::UpdatePlayerSoulDraughtboard* updateRq)
{

}

}