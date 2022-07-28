// MIT License
//
// Copyright (c) 2021-2022 Quentin Balland
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

#ifndef FYS_ONLINE_CHAIUTILITY_HH
#define FYS_ONLINE_CHAIUTILITY_HH

#include <string>

#include <chaiscript/chaiscript.hpp>
#include <fmt/format.h>


namespace chaiscript {
class ChaiScript;
}

namespace fys::arena::chai::util {

[[maybe_unused]] [[nodiscard]] static std::string
get_ally_action_retriever(const std::string& user, const std::string& member, const std::string& action) {
  return fmt::format(R"(ally_actions["{}_{}"]["{}"])", user, member, action);
}

[[maybe_unused]] [[nodiscard]] static bool
member_has_action_registered(chaiscript::ChaiScript& chai,
                             const std::string& userName, const std::string& memberName, const std::string& actionName) {
  return chai.eval<bool>(
      fmt::format(R"(ally_actions.count("{}_{}") > 0 && ally_actions["{}_{}"].count("{}") > 0;)",
                  userName, memberName, userName, memberName, actionName));
}

}// namespace fys::arena::chai::util

#endif//FYS_ONLINE_CHAIUTILITY_HH
