#include <chrono>
#include <zmq_addon.hpp>
#include <Dispatcher.hh>

namespace fys {

    void DispatcherHandlerBase::processClusterMessage(zmq::multipart_t && msg,
            network::DispatcherConnectionManager &manager) noexcept
    {
        manager.sendMessageToDispatcherSocket(std::move(msg));
    }

    void DispatcherHandlerBase::processInputMessage(zmq::multipart_t && msg,
            network::DispatcherConnectionManager &manager) noexcept
    {
        manager.sendMessageToDispatcherSocket(std::move(msg));
    }

    void DispatcherHandlerBase::forwardMessageToFrontend(zmq::multipart_t && msg,
            network::DispatcherConnectionManager &manager) noexcept
    {
        manager.sendMessageToDispatcherSocket(std::move(msg));
    }
}