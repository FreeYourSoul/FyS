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

#ifndef FYS_SERVICECONTEXTBASE_HH
#define FYS_SERVICECONTEXTBASE_HH

#include <string>

namespace fys::common {

namespace init_beacon {
constexpr static auto SERVICE_NAME = "service.name";
constexpr static auto HOSTNAME = "service.hostname";
constexpr static auto PORT = "service.port";

constexpr static auto DISPATCHER_PORT = "dispatcher.port";
constexpr static auto DISPATCHER_SUBPORT = "dispatcher.subport";
constexpr static auto DISPATCHER_ADDR = "dispatcher.address";
}// namespace init_beacon

struct dispatcher_data {
  std::string address;
  ushort subscriber_port;
  ushort port;
};

class service_context_base {
public:
  service_context_base(int ac, const char *const *av);

  /**
   * @return the connection string required to connect to the current server
   */
  [[nodiscard]] std::string
  connection_str() const;

  [[nodiscard]] ushort
  port() const { return _port; }

  [[nodiscard]] const std::string &
  hostname() const { return _hostname; }

  [[nodiscard]] std::string
  dispatcher_connection_str() const noexcept;

private:
  void initialize_from_ini(const std::string &cfg_file_path);

protected:
  std::string _version;
  std::string _name;
  std::string _config_file;

  std::string _hostname;
  ushort _port;

  dispatcher_data _dispatcher_data;
};

}// namespace fys::common

#endif//FYS_SERVICECONTEXTBASE_HH
