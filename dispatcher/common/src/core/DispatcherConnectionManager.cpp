#include <zmq.h>
#include <DispatcherConnectionManager.hh>
#include <StartupDispatcherCtx.hh>

fys::network::DispatcherConnectionManager::DispatcherConnectionManager(int threadNumber, bool isLoadBalancing) :
 _zmqContext(threadNumber),
 _listener(_zmqContext, zmq::socket_type::router),
 _dipatcher(_zmqContext, (isLoadBalancing) ? zmq::socket_type::dealer : zmq::socket_type::pub),
 _clusterConnection({zmq::socket_t(_zmqContext, zmq::socket_type::sub),
                     zmq::socket_t(_zmqContext, zmq::socket_type::pub)}) {
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
    }
    _listener.bind("tcp://*:" + std::to_string(ctx.getBindingPort()));
}

void fys::network::DispatcherConnectionManager::subscribeToTopics(const std::vector<std::string> &topics) {
    for (const auto &topic : topics) {
        _clusterConnection.subSocket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
    }
}
