// MIT License
//
// Copyright (c) 2021 Quentin Balland
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

#include <spdlog/spdlog.h>

#include <boost/property_tree/ini_parser.hpp>

#include <fil/cli/command_line_interface.hh>

#include "service_context_base.hh"

namespace fys::common {

service_context_base::service_context_base(int ac, const char* const* av) try {
  _version = "0.0.1";

  fil::command_line_interface cli([] {}, "Context setup");
  std::string config_path;

  cli.add_option(fil::option(
      "-c", [&v = config_path](std::string str) { v = std::move(str); }, "Path of the config file"));
  cli.add_option(fil::option(
      "-s", [&v = _config_file](std::string str) { v = std::move(str); }, "Path of specific config file"));
  cli.add_option(fil::option(
      "-a", [&v = _dispatcher_data.address](std::string str) { v = std::move(str); }, "Hostname of the service dispatcher"));
  cli.add_option(fil::option(
      "-p", [&v = _dispatcher_data.subscriber_port](std::uint64_t value) { v = value; }, "Port number of the dispatcher to subscribe to"));
  cli.add_option(fil::option(
      "-d", [&v = _dispatcher_data.port](std::uint64_t value) { v = value; }, "Port number of the service dispatcher"));

  cli.parse_command_line(ac, const_cast<char**>(av));

  this->initialize_from_ini(config_path);
} catch (std::exception& e) {
  SPDLOG_ERROR("Context of the service not initialized caused by : {}", e.what());
}

void service_context_base::initialize_from_ini(const std::string& cfg_file_path) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_ini(cfg_file_path, pt);

  _name = pt.get<std::string>(fys::common::init_beacon::SERVICE_NAME);
  _hostname = pt.get<std::string>(fys::common::init_beacon::HOSTNAME);
  _port = pt.get<ushort>(fys::common::init_beacon::PORT);
  _dispatcher_data.subscriber_port = pt.get<ushort>(fys::common::init_beacon::DISPATCHER_SUBPORT);
  _dispatcher_data.port = pt.get<ushort>(fys::common::init_beacon::DISPATCHER_PORT);
  _dispatcher_data.address = pt.get<std::string>(fys::common::init_beacon::DISPATCHER_ADDR);
}

std::string
service_context_base::dispatcher_connection_str() const noexcept {
  return std::string("tcp://").append(_dispatcher_data.address).append(":").append(std::to_string(_dispatcher_data.port));
}

std::string
service_context_base::connection_str() const {
  return std::string("tcp://").append(_hostname).append(":").append(std::to_string(_port));
}

}// namespace fys::common