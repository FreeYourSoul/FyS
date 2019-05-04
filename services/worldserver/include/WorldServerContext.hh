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
#include <nlohmann/json.hpp>

namespace fys::ws {

    struct ProximityServer {
        struct ProximityServerAxis {
            double value;
            bool superiorTo;
        };

        constexpr bool isAtProximity(double axis) const noexcept {
            bool xReq = xAxisRequirement.has_value();
            bool yReq = yAxisRequirement.has_value();
            if (xReq)
                xReq = (xAxisRequirement->superiorTo) ? (axis > xAxisRequirement->value) : (axis < xAxisRequirement->value);
            if (yReq)
                yReq = (yAxisRequirement->superiorTo) ? (axis > yAxisRequirement->value) : (axis < yAxisRequirement->value);
            return xReq && yReq;
        }

        std::string code;
        std::optional<ProximityServerAxis> xAxisRequirement = std::nullopt;
        std::optional<ProximityServerAxis> yAxisRequirement = std::nullopt;
    };

    class WorldServerContext : fys::common::ServiceContextBase {

    public:
        WorldServerContext(int ac, const char *const *av);

        [[nodiscard]] std::string toString() const noexcept;
        [[nodiscard]] std::string getDispatcherSubConnectionString() const noexcept;
        [[nodiscard]] std::string getDispatcherConnectionString() const noexcept;

    private:
        void initWsContextWithJson(nlohmann::json &json);

    private:
        std::string _serverCode;
        std::pair<double, double> _serverXBoundaries;
        std::pair<double, double> _serverYBoundaries;
        std::vector<ProximityServer> _axisServerProximity;

    };

}

#endif //FYS_WORLDSERVERCONTEXT_HH
