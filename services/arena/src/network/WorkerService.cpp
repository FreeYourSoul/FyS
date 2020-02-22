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

#include <spdlog/spdlog.h>
#include <network/WorkerService.hh>

namespace fys::arena
{

    void WorkerService::addFightingPit(std::unique_ptr<FightingPit> fp) {
        if (!fp) {
            SPDLOG_ERROR("Cannot add fighting pit in WorkerService");
            return;
        }
        if (_arenaInstances.size() >= std::numeric_limits<decltype(_currentArenaId)>::max()) {
            SPDLOG_ERROR("Cannot add fighting pit in WorkerService (worker full)");
            return;
        }
        while (++_currentArenaId != 0 && _arenaInstances.find(_currentArenaId) != _arenaInstances.end());

        fp->setArenaId(_currentArenaId);
        _arenaInstances[_currentArenaId] = std::move(fp);
    }
    
    void WorkerService::forwardMessageToFightingPit(unsigned fightingArenaId/* , FightingMessage*/) {
        if (_arenaInstances.find(fightingArenaId) == _arenaInstances.end()) {
            SPDLOG_WARN("Request received for arena id {}, but arena isn't defined", fightingArenaId);
            return;
        }

    }
    
} // namespace fys::arena
