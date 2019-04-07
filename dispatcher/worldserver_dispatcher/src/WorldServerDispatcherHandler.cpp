//
// Created by FyS on 3/9/19.
//

#include <zmq_addon.hpp>
#include <Dispatcher.hh>
#include "../include/WorldServerDispatcherHandler.hh"

namespace fys::worldserver {

void WorldServerDispatcherHandler::processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {

}

void WorldServerDispatcherHandler::processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {

}

} // namespace fys::worldserver