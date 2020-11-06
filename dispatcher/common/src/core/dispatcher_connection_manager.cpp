#include <dispatcher_connection_manager.hh>
#include <startup_dispatcher_ctx.hh>

namespace fys::network {

dispatcher_connection_manager::dispatcher_connection_manager(int threadNumber, bool isLoadBalancing) noexcept : _isLoadBalancing(isLoadBalancing),
																												_zmqContext(threadNumber),
																												_listener(_zmqContext, zmq::socket_type::router),
																												_dispatcher(_zmqContext, (isLoadBalancing) ? zmq::socket_type::dealer : zmq::socket_type::pub),
																												_clusterConnection({zmq::socket_t(_zmqContext, zmq::socket_type::sub),
																																	zmq::socket_t(_zmqContext, zmq::socket_type::pub),
																																	false}) {
}

void dispatcher_connection_manager::setupConnectionManager(const fys::startup_dispatcher_ctx &ctx) noexcept {
  if (ctx.isClusterAware()) {
	_clusterConnection.pubSocket.connect(ctx.frontend_cluster_proxy_connection_str());
	_clusterConnection.subSocket.connect(ctx.backend_cluster_proxy_connection_str());
	subscribeToTopics(ctx.getSubscriptionTopics());
  } else {
	_clusterConnection.pubSocket.close();
	_clusterConnection.subSocket.close();
	_clusterConnection.closed = true;
  }
  _dispatcher.setsockopt(ZMQ_MAXMSGSIZE, ctx.getMaxMsgSize());
  _dispatcher.bind("tcp://*:" + std::to_string(ctx.getDispatchingPort()));
  _listener.bind("tcp://*:" + std::to_string(ctx.getBindingPort()));
}

void dispatcher_connection_manager::subscribeToTopics(const std::vector<std::string> &topics) noexcept {
  for (const auto &topic : topics) {
	_clusterConnection.subSocket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());
  }
}

std::tuple<bool, bool, bool> dispatcher_connection_manager::poll() noexcept {
  //  Initialize poll set
  zmq::pollitem_t items[] = {
	  {_listener, 0, ZMQ_POLLIN, 0},
	  {_clusterConnection.subSocket, 0, ZMQ_POLLIN, 0},
	  {_dispatcher, 0, ZMQ_POLLIN, 0}};
  zmq::poll(&items[0], _clusterConnection.closed ? 1 : 2, -1);
  bool listenerPolling = static_cast<bool>(items[0].revents & ZMQ_POLLIN);
  bool subSocketPolling = static_cast<bool>(_clusterConnection.closed ? false : (items[1].revents & ZMQ_POLLIN));
  bool dispatcherRespPolling = static_cast<bool>(items[2].revents & ZMQ_POLLIN);
  return {listenerPolling, subSocketPolling, dispatcherRespPolling};
}

bool dispatcher_connection_manager::replyToListenerSocket(zmq::multipart_t &&msg) noexcept {
  if (!_listener.connected())
	return false;
  return msg.send(_listener);
}

bool dispatcher_connection_manager::sendMessageToDispatcherSocket(zmq::multipart_t &&msg) noexcept {
  if (!_dispatcher.connected())
	return false;
  return msg.send(_dispatcher);
}

bool dispatcher_connection_manager::sendMessageToClusterPubSocket(zmq::multipart_t &&msg) noexcept {
  if (_clusterConnection.closed || !_clusterConnection.pubSocket.connected())
	return false;
  return msg.send(_clusterConnection.pubSocket);
}

}// namespace fys::network