#include <chrono>
#include <zmq_addon.hpp>
#include <Dispatcher.hh>

namespace fys {

    void DispatcherHandlerBase::processClusterMessage(zmq::multipart_t &&msg,
            network::DispatcherConnectionManager &manager) noexcept
    {
        manager.sendMessageToDispatcherSocket(std::move(msg));
    }

    void DispatcherHandlerBase::processInputMessage(zmq::multipart_t &&msg,
            network::DispatcherConnectionManager &manager) noexcept
    {
        if (checkAuthentication(msg, manager))
            manager.sendMessageToDispatcherSocket(std::move(msg));
    }

    bool DispatcherHandlerBase::checkAuthentication(const zmq::multipart_t &msg,
            fys::network::DispatcherConnectionManager &manager) noexcept
    {
        std::string token(static_cast<const char *>(msg.at(0).data()), msg.size());
        if (_authenticated.find(token) != _authenticated.end())
            return true;
        // TODO should check authentication directly in redis
        bool checkAuth = true;
        if (checkAuth)
            _authenticated.emplace(token);
        return checkAuth;
    }

}