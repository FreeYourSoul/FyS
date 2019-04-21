//
// Created by FyS on 3/9/19.
//

#include <zmq_addon.hpp>
#include <Dispatcher.hh>
#include "../include/WorldServerDispatcherHandler.hh"

namespace fys::ws {

void WorldServerDispatcherHandler::processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept {

}

void WorldServerDispatcherHandler::processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) noexcept {

}

} // namespace fys::ws