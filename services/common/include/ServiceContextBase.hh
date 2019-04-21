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
        constexpr static auto DISPATCHER_PORT = "dispatcher.port";
        constexpr static auto DISPATCHER_SUBPORT = "dispatcher.subport";
        constexpr static auto DISPATCHER_ADDR = "dispatcher.address";
    }


    struct DispatcherData {
        std::string address;
        ushort subscriberPort;
        ushort port;
    };

    class ServiceContextBase {
    public:
        ServiceContextBase(int ac, const char *const *av);

    private:
        void initializeFromIni(const std::string &configFilePath);

    protected:
        std::string _version;
        std::string _name;
        std::string _configFile;
        DispatcherData _dispatcherData;
    };

}


#endif //FYS_SERVICECONTEXTBASE_HH
