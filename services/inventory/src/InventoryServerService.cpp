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

#include <inventory_server_context.hh>
#include <flatbuffer_generator.hh>
#include <InventoryServerService.hh>

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

InventoryServerService::InventoryServerService(const inventory_server_context& ctx)
		:_ctx(ctx), _exchangeManager(ctx)
{
	_connectionHandler.setupConnectionManager(ctx.get_dispatcher_connection_str());
}

void
InventoryServerService::runServerLoop()
{
	SPDLOG_INFO("[Inv : {}] : InventoryServer loop started", _ctx.get().getServerCode());

	while (true) {
		_connectionHandler.pollAndProcessMessageFromDispatcher(
				[this](zmq::message_t&& playerIdt, zmq::message_t&& worldServerMessage) {
					if (!verifyBuffer<fb::ivt::InventoryRequest>(worldServerMessage.data(), worldServerMessage.size())) {
						SPDLOG_ERROR("[Inv : {}] : Wrongly formatted InventoryRequest buffer", _ctx.get().getServerCode());
						return;
					}
					const auto* binary = fb::ivt::GetInventoryRequest(worldServerMessage.data());
					zmq::multipart_t response;
					switch (binary->content_type()) {
						case fb::ivt::InventoryRequestContent_RetrievePlayerInventory:
							response.add(retrievePlayerInventory(binary->content_as_RetrievePlayerInventory()));
							break;
						case fb::ivt::InventoryRequestContent_UpdatePlayerSoulDraughtboard:
							response.add(updatePlayerSoulDraughtboard(binary->content_as_UpdatePlayerSoulDraughtboard()));
							break;
						case fb::ivt::InventoryRequestContent_RetrievePlayerSoulDraughtboard:
							response.add(retrievePlayerSoulDraughtboard(binary->content_as_RetrievePlayerSoulDraughtboard()));
							break;
						case fb::ivt::InventoryRequestContent_InitiateExchange:
							response.add(exchangeInventory(binary->content_as_InitiateExchange(), playerIdt.str()));
							break;
						default:
							SPDLOG_ERROR("[Inv : {}] : Request type '{}' not supported",
									_ctx.get().getServerCode(), binary->content_type());
							break;
					}
					response.add(std::move(playerIdt));
					_connectionHandler.sendMessageToDispatcher(std::move(response));
				}
		);

		// direct connection with the client to handle exchanges
		_exchangeManager.pollAndProcessPlayerMessage(
				[this](zmq::message_t&& identity, zmq::message_t&& authFrame, zmq::message_t&& content) {
					if (!verifyBuffer<fb::ivt::ExchangeInventory>(content.data(), content.size())) {
						SPDLOG_ERROR("[Inv : {}] : Wrongly formatted ExchangeInventory buffer", _ctx.get().getServerCode());
						return;
					}
					const auto* binary = fb::ivt::GetExchangeInventory(content.data());
					auto roomAccessor = RoomAccessor{
							binary->idExchange(),
							binary->tokenExchange()->str(),
							binary->userName()->str(),
					};

					switch (binary->content_type()) {

						case fb::ivt::ExchangeStep_JoinTransaction:
							_exchangeManager.receiverJoinRoomTransaction(roomAccessor, identity.str());
							break;

						case fb::ivt::ExchangeStep_AddItem:
							_exchangeManager.addItemInRoom(_itemManager, roomAccessor,
									binary->content_as_AddItem()->itemId()->str(),
									binary->content_as_AddItem()->quantity());
							break;

						case fb::ivt::ExchangeStep_RemoveItem:
							_exchangeManager.removeItemFromRoom(_itemManager, roomAccessor,
									binary->content_as_AddItem()->itemId()->str(),
									binary->content_as_AddItem()->quantity());
							break;

						case fb::ivt::ExchangeStep_LockExchange:
							_exchangeManager.lockRoomTransaction(roomAccessor);
							break;

						case fb::ivt::ExchangeStep_TerminateExchange:
							_exchangeManager.terminateRoomTransaction(_itemManager, roomAccessor);
							break;

						default:
							SPDLOG_ERROR("[Inv : {}] : Player Request '{}' not supported",
									_ctx.get().getServerCode(), binary->content_type());
							break;
					}

				}
		);
	}
}

zmq::message_t
InventoryServerService::exchangeInventory(const fb::ivt::InitiateExchangeInstance* exchangeRq, std::string identity)
{
	const auto& exchangeRoom = _exchangeManager.makeExchangeRoom(_itemManager,
			exchangeRq->userInitiator()->str(),
			exchangeRq->userReceiver()->str(),
			identity);
	flatbuffer_generator fg;

	auto[data, size] = fg.generate_initiate_exchange_response(exchangeRoom);
	return zmq::message_t(data, size);
}

zmq::message_t
InventoryServerService::retrievePlayerInventory(const fb::ivt::RetrievePlayerInventory* retrieveRq)
{
	const std::string playerToRetrieve = retrieveRq->playerToRetrieve()->str();
	SPDLOG_INFO("[Inv : {}] : Request of player's '{}' inventory.", _ctx.get().getServerCode(), playerToRetrieve);
	return {};
}

zmq::message_t
InventoryServerService::updatePlayerSoulDraughtboard(const fb::ivt::UpdatePlayerSoulDraughtboard* updateRq)
{
	return {};
}

zmq::message_t
InventoryServerService::retrievePlayerSoulDraughtboard(const fb::ivt::UpdatePlayerSoulDraughtboard* updateRq)
{
	return {};
}

}