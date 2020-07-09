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
#include <inventory_server_service.hh>

namespace {

template<typename T>
[[nodiscard]] bool
verify_buffer(const void* fbBuffer, std::uint32_t size)
{
	auto v = flatbuffers::Verifier(static_cast<const uint8_t*>(fbBuffer), size);
	return v.VerifyBuffer<T>();
}

}

namespace fys::inv {

inventory_server_service::inventory_server_service(const inventory_server_context& ctx)
		:_ctx(ctx), _exchange_manager(ctx)
{
	_connection_handler.setup_connection_manager(ctx.dispatcher_connection_str());
}

void
inventory_server_service::run_server_loop()
{
	SPDLOG_INFO("[Inv : {}] : InventoryServer loop started", _ctx.get().server_code());

	while (true) {
		_connection_handler.poll_process_msg_from_dispatcher(
				[this](zmq::message_t&& player_idt, zmq::message_t&& world_server_msg) {
					if (!verify_buffer<fb::ivt::InventoryRequest>(world_server_msg.data(), world_server_msg.size())) {
						SPDLOG_ERROR("[Inv : {}] : Wrongly formatted InventoryRequest buffer", _ctx.get().server_code());
						return;
					}
					const auto* binary = fb::ivt::GetInventoryRequest(world_server_msg.data());
					zmq::multipart_t response;
					switch (binary->content_type()) {
						case fb::ivt::InventoryRequestContent_RetrievePlayerInventory:
							response.add(retrieve_player_inventory(binary->content_as_RetrievePlayerInventory()));
							break;
						case fb::ivt::InventoryRequestContent_UpdatePlayerSoulDraughtboard:
							response.add(update_player_soul_draughtboard(binary->content_as_UpdatePlayerSoulDraughtboard()));
							break;
						case fb::ivt::InventoryRequestContent_RetrievePlayerSoulDraughtboard:
							response.add(retrievePlayerSoulDraughtboard(binary->content_as_RetrievePlayerSoulDraughtboard()));
							break;
						case fb::ivt::InventoryRequestContent_InitiateExchange:
							response.add(exchange_inventory(binary->content_as_InitiateExchange(), player_idt.str()));
							break;
						default:
							SPDLOG_ERROR("[Inv : {}] : Request type '{}' not supported",
									_ctx.get().server_code(), binary->content_type());
							break;
					}
					response.add(std::move(player_idt));
					_connection_handler.send_msg_to_dispatcher(std::move(response));
				}
		);

		// direct connection with the client to handle exchanges
		_exchange_manager.pollAndProcessPlayerMessage(
				[this](zmq::message_t&& identity, zmq::message_t&& authFrame, zmq::message_t&& content) {
					if (!verify_buffer<fb::ivt::ExchangeInventory>(content.data(), content.size())) {
						SPDLOG_ERROR("[Inv : {}] : Wrongly formatted ExchangeInventory buffer", _ctx.get().server_code());
						return;
					}
					const auto* binary = fb::ivt::GetExchangeInventory(content.data());
					auto roomAccessor = room_accessor{
							binary->idExchange(),
							binary->tokenExchange()->str(),
							binary->userName()->str(),
					};

					switch (binary->content_type()) {

						case fb::ivt::ExchangeStep_JoinTransaction:
							_exchange_manager.receiver_join_room_transaction(roomAccessor, identity.str());
							break;

						case fb::ivt::ExchangeStep_AddItem:
							_exchange_manager.add_item_in_room(_item_manager, roomAccessor,
									binary->content_as_AddItem()->itemId()->str(),
									binary->content_as_AddItem()->quantity());
							break;

						case fb::ivt::ExchangeStep_RemoveItem:
							_exchange_manager.remove_item_from_room(_item_manager, roomAccessor,
									binary->content_as_AddItem()->itemId()->str(),
									binary->content_as_AddItem()->quantity());
							break;

						case fb::ivt::ExchangeStep_LockExchange:
							_exchange_manager.lock_room_transaction(roomAccessor);
							break;

						case fb::ivt::ExchangeStep_TerminateExchange:
							_exchange_manager.terminate_room_transaction(_item_manager, roomAccessor);
							break;

						default:
							SPDLOG_ERROR("[Inv : {}] : Player Request '{}' not supported",
									_ctx.get().server_code(), binary->content_type());
							break;
					}

				}
		);
	}
}

zmq::message_t
inventory_server_service::exchange_inventory(const fb::ivt::initiate_exchange_instance* exchange_request, std::string identity)
{
	const auto& exchangeRoom = _exchange_manager.makeExchangeRoom(_item_manager,
			exchange_request->userInitiator()->str(),
			exchange_request->userReceiver()->str(),
			identity);
	flatbuffer_generator fg;

	auto[data, size] = fg.generate_initiate_exchange_response(exchangeRoom);
	return zmq::message_t(data, size);
}

zmq::message_t
inventory_server_service::retrieve_player_inventory(const fb::ivt::retrieve_player_inventory* retrieve_request)
{
	const std::string player_to_retrieve = retrieve_request->playerToRetrieve()->str();
	SPDLOG_INFO("[Inv : {}] : Request of player's '{}' inventory.", _ctx.get().server_code(), player_to_retrieve);
	return {};
}

zmq::message_t
inventory_server_service::update_player_soul_draughtboard(const fb::ivt::update_player_soul_draughtboard* update_request)
{
	return {};
}

zmq::message_t
inventory_server_service::retrievePlayerSoulDraughtboard(const fb::ivt::update_player_soul_draughtboard* update_request)
{
	return {};
}

}