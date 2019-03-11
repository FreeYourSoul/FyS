#include <zmq_addon.hpp>
#include <chrono>
#include <Dispatcher.hh>

void fys::DispatcherHandlerBase::processInputMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {

}

void fys::DispatcherHandlerBase::processClusterMessage(zmq::multipart_t &&msg, network::DispatcherConnectionManager &manager) {

}
