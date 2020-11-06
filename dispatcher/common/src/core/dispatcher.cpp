#include <chrono>
#include <dispatcher.hh>
#include <zmq_addon.hpp>

namespace fys {

void dispatcher_handler_base::process_cluster_message(zmq::multipart_t &&msg,
													  network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}

void dispatcher_handler_base::process_input_message(zmq::multipart_t &&msg,
													network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}

void dispatcher_handler_base::forward_message_to_frontend(zmq::multipart_t &&msg,
														  network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}
}// namespace fys