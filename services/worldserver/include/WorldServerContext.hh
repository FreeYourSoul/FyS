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


#ifndef FYS_WORLDSERVERCONTEXT_HH
#define FYS_WORLDSERVERCONTEXT_HH

#include <ServiceContextBase.hh>

namespace fys::ws {

    struct ProximityServerAxis {

        constexpr bool isAtProximity(double axis) const noexcept {
            return (superiorTo) ? (axis > value) : (axis < value);
        }

        std::string code;
        double value;
        bool superiorTo;
    };

    class WorldServerContext : fys::common::ServiceContextBase {

    public:
        WorldServerContext(int ac, const char *const *av);

        std::string toString() const noexcept;

        std::string getDispatcherSubConnectionString() const;
        std::string getDispatcherConnectionString() const;

    private:
        std::string _serverCode;
        std::pair<double, double> _serverBoundaries;
        std::vector<ProximityServerAxis> _xAxisServerProximity;
        std::vector<ProximityServerAxis> _yAxisServerProximity;

    };

}

#endif //FYS_WORLDSERVERCONTEXT_HH
