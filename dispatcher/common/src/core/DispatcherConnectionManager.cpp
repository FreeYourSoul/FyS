#include <DispatcherConnectionManager.hh>
#include <StartupDispatcherCtx.hh>

fys::network::DispatcherConnectionManager::DispatcherConnectionManager(int threadNumber, bool isLoadBalancing) :
 _zmqContext(threadNumber),
 _listener(_zmqContext, zmq::socket_type::router),
 _dipatcher(_zmqContext, (isLoadBalancing) ? zmq::socket_type::dealer : zmq::socket_type::pub),
 _clusterConnection({zmq::socket_t(_zmqContext, zmq::socket_type::sub),
                     zmq::socket_t(_zmqContext, zmq::socket_type::pub),
                     false}) {
}

void fys::network::DispatcherConnectionManager::setupConnectionManager(const fys::StartupDispatcherCtx &ctx) {
    if (ctx.isClusterAware()) {
        const std::string proxyFeConnectionString;
        const std::string proxyBeConnectionString;
        _clusterConnection.pubSocket.connect(proxyFeConnectionString);
        _clusterConnection.subSocket.connect(proxyBeConnectionString);
        subscribeToTopics(ctx.getSubscriptionTopics());
    }
    else {
        _clusterConnection.pubSocket.close();
        _clusterConnection.subSocket.close();
        _clusterConnection.closed = true;
    }
    _listener.bind("tcp://*:" + std::to_string(ctx.getBindingPort()));
}

void fys::network::DispatcherConnectionManager::subscribeToTopics(const std::vector<std::string> &topics) {
    for (const auto &topic : topics) {
        _clusterConnection.subSocket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    }
}

std::pair<bool, bool> fys::network::DispatcherConnectionManager::poll() {
    //  Initialize poll set
    zmq::pollitem_t items[] = {
            { _listener, 0, ZMQ_POLLIN, 0 },
            { _clusterConnection.subSocket, 0, ZMQ_POLLIN, 0 }
    };
    zmq::poll(&items[0], _clusterConnection.closed ? 1 : 2, -1);
    bool listenerPolling = static_cast<bool>(items[0].revents & ZMQ_POLLIN);
    bool subSocketPolling = static_cast<bool>(_clusterConnection.closed ? false : (items[1].revents & ZMQ_POLLIN));
    return {listenerPolling, subSocketPolling};
}

bool fys::network::DispatcherConnectionManager::sendMessageToDispatcherSocket(zmq::multipart_t &&msg) {
    return msg.send(_dipatcher);
}

bool fys::network::DispatcherConnectionManager::sendMessageToClusterPubSocket(zmq::multipart_t &&msg) {
    return msg.send(_clusterConnection.pubSocket);
}

