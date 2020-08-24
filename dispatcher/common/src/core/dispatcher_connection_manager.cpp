#include <DispatcherConnectionManager.hh>
#include <StartupDispatcherCtx.hh>

namespace fys::network {

DispatcherConnectionManager::DispatcherConnectionManager(int threadNumber, bool isLoadBalancing) noexcept :
 _isLoadBalancing(isLoadBalancing),
 _zmqContext(threadNumber),
 _listener(_zmqContext, zmq::socket_type::router),
 _dispatcher(_zmqContext, (isLoadBalancing) ? zmq::socket_type::dealer : zmq::socket_type::pub),
 _clusterConnection({zmq::socket_t(_zmqContext, zmq::socket_type::sub),
                     zmq::socket_t(_zmqContext, zmq::socket_type::pub),
                     false}) {
}

void DispatcherConnectionManager::setupConnectionManager(const fys::StartupDispatcherCtx &ctx) noexcept {
    if (ctx.isClusterAware()) {
        _clusterConnection.pubSocket.connect(ctx.getFrontendClusterProxyConnectionString());
        _clusterConnection.subSocket.connect(ctx.getBackendClusterProxyConnectionString());
        subscribeToTopics(ctx.getSubscriptionTopics());
    }
    else {
        _clusterConnection.pubSocket.close();
        _clusterConnection.subSocket.close();
        _clusterConnection.closed = true;
    }
    _dispatcher.setsockopt(ZMQ_MAXMSGSIZE, ctx.getMaxMsgSize());
    _dispatcher.bind("tcp://*:" + std::to_string(ctx.getDispatchingPort()));
    _listener.bind("tcp://*:" + std::to_string(ctx.getBindingPort()));
}

void DispatcherConnectionManager::subscribeToTopics(const std::vector<std::string> &topics) noexcept {
    for (const auto &topic : topics) {
        _clusterConnection.subSocket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    }
}

std::tuple<bool, bool, bool> DispatcherConnectionManager::poll() noexcept {
    //  Initialize poll set
    zmq::pollitem_t items[] = {
        { _listener, 0, ZMQ_POLLIN, 0 },
        { _clusterConnection.subSocket, 0, ZMQ_POLLIN, 0 },
        { _dispatcher, 0, ZMQ_POLLIN, 0 }
    };
    zmq::poll(&items[0], _clusterConnection.closed ? 1 : 2, -1);
    bool listenerPolling = static_cast<bool>(items[0].revents & ZMQ_POLLIN);
    bool subSocketPolling = static_cast<bool>(_clusterConnection.closed ? false : (items[1].revents & ZMQ_POLLIN));
    bool dispatcherRespPolling = static_cast<bool>(items[2].revents & ZMQ_POLLIN);
    return {listenerPolling, subSocketPolling, dispatcherRespPolling};
}

bool DispatcherConnectionManager::replyToListenerSocket(zmq::multipart_t && msg) noexcept {
    if (!_listener.connected())
        return false;
    return msg.send(_listener);
}

bool DispatcherConnectionManager::sendMessageToDispatcherSocket(zmq::multipart_t && msg) noexcept {
    if (!_dispatcher.connected())
        return false;
    return msg.send(_dispatcher);
}

bool DispatcherConnectionManager::sendMessageToClusterPubSocket(zmq::multipart_t && msg) noexcept {
    if (_clusterConnection.closed || !_clusterConnection.pubSocket.connected())
        return false;
    return msg.send(_clusterConnection.pubSocket);
}

}