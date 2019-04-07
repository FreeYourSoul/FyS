#include <zmq_addon.hpp>
#include <chrono>
#include <Dispatcher.hh>

namespace fys {

void DispatcherHandlerBase::processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {
    manager.sendMessageToDispatcherSocket(std::move(msg));
}

void DispatcherHandlerBase::processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {
    manager.sendMessageToDispatcherSocket(std::move(msg));
}

}