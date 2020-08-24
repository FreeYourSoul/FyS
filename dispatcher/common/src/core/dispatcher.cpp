#include <chrono>
#include <zmq_addon.hpp>
#include <dispatcher.hh>

namespace fys {

void DispatcherHandlerBase::process_cluster_message(zmq::multipart_t &&msg,
													network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}

void DispatcherHandlerBase::process_input_message(zmq::multipart_t &&msg,
												  network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}

void DispatcherHandlerBase::forward_message_to_frontend(zmq::multipart_t &&msg,
														network::dispatcher_connection_manager &manager) noexcept {
  manager.sendMessageToDispatcherSocket(std::move(msg));
}
}