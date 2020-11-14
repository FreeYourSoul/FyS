// MIT License
//
// Copyright (c) 2019 Quentin Balland
// Repository : https://github.com/FreeYourSoul/FyS
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef FYS_CONNECTIONMANAGER_HH_
#define FYS_CONNECTIONMANAGER_HH_

#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

namespace fys {
class startup_dispatcher_ctx;
}

namespace fys::network {

struct ClusterConnection {
  zmq::socket_t subSocket;// Connect on back end of the proxy
  zmq::socket_t pubSocket;// Connect on front end of the proxy
  bool closed = false;
};

class dispatcher_connection_manager {
public:
  /**
   * @param threadNumber
   *          number of thread used by the zmq::context in order to manage messages
   * @param isLoadBalancing
   *          if true, a Dealer socket is instantiated in order to dispatch messages
   *          if false, a Publisher socket is instantiated to do so
   */
  explicit dispatcher_connection_manager(int threadNumber = 1, bool isLoadBalancing = true) noexcept;

  void setupConnectionManager(const fys::startup_dispatcher_ctx& ctx) noexcept;

  /**
   * @brief Poll the reader socket (the listener one and the subscriber one if the dispatcher is cluster
   * aware
   *
   * @return a pair of bool with
   *         first  : is there anything on the listener socket to read
   *         second : is there anything on the subscriber socket to read
   */
  std::tuple<bool, bool, bool> poll() noexcept;

  /**
   * @brief Read the Listener socket and call a given handler with this message as parameter
   *
   * @tparam Handler function to call after reading message from socket (multipart_t object sent as parameter)
   * @param handler function instance to call which take zmq::multipart object as parameter
   */
  template<typename Handler>
  void dispatchMessageFromListenerSocket(Handler&& handler) {
    zmq::multipart_t msg;
    if (!msg.recv(_listener)) {
      SPDLOG_ERROR("Error while reading on the listener socket");
    } else {
      std::forward<Handler>(handler)(std::move(msg), *this);
    }
  }

  /**
   * @brief Read the Cluster subscriber socket and call a given handler with this message as parameter
   *
   * @tparam Handler function to call after reading message from socket (multipart_t object sent as parameter)
   * @param handler function instance to call which take zmq::multipart object as parameter
   */
  template<typename Handler>
  void dispatchMessageFromSubscriberSocket(Handler&& handler) noexcept {
    if (!_clusterConnection.closed) {
      zmq::multipart_t msg;
      if (!msg.recv(_clusterConnection.subSocket)) {
        SPDLOG_ERROR("Error while reading on the cluster subscriber socket");
      } else {
        std::forward<Handler>(handler)(std::move(msg), *this);
      }
    }
  }

  /**
   */
  template<typename Handler>
  void dispatchMessageFromDispatcherSocket(Handler&& handler) noexcept {
    zmq::multipart_t msg;
    if (!msg.recv(_dispatcher)) {
      SPDLOG_ERROR("Error while reading on the listener socket");
    } else {
      std::forward<Handler>(handler)(std::move(msg), *this);
    }
  }

  /**
   * @brief Send a message to the frontend socket of the dispatcher (proxy).
   * This socket is a router socket, and so the message has to contains an identity frame to be routed correctly
   * @param msg reply to the requester client of the dispatcher
   * @return true if the message has been sent correctly, false otherwise
   */
  bool replyToListenerSocket(zmq::multipart_t&& msg) noexcept;
  /**
   * @brief Send a message to the servers connected to the dispatcher (load balanced, or distributed dispatching depending
   * on the dispatcher configuration). The message has to contains the identity frame in order for the server to
   * resend it in the reply.
   * @param msg incoming message forwarded to the dispatched server
   * @return true if the message has been sent correctly, false otherwise
   */
  bool sendMessageToDispatcherSocket(zmq::multipart_t&& msg) noexcept;
  /**
   * @brief Send a message to the dispatcher cluster (if configured in the dispatcher)
   * @param msg notification to send to the dispatcher cluster
   * @return  true if the message has been sent correctly, false otherwise
   */
  bool sendMessageToClusterPubSocket(zmq::multipart_t&& msg) noexcept;

private:
  /**
   * The dispatcher connect to the proxy and subscribe to specifics channels given as parameter
   * @param topics additional topics to subscribe to (customizable through the config file)
   */
  void subscribeToTopics(const std::vector<std::string>& topics) noexcept;

private:
  bool _isLoadBalancing;
  zmq::context_t _zmqContext;
  zmq::socket_t _listener;
  zmq::socket_t _dispatcher;

  ClusterConnection _clusterConnection;
};

}// namespace fys::network

#endif// !FYS_CONNECTIONMANAGER_HH_
