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
#include <fightingPit/FightingPit.hh>
#include "FightingHistoryManager.hh"

namespace {

[[nodiscard]] std::string
targetStr(const std::vector<uint>& targets)
{
	std::string ret;
	for (uint t : targets) {
		ret.append(std::to_string(t) + " ");
	}
	return ret;
}

}

namespace fys::arena {

void
FightingHistoryManager::addHistoric(unsigned fightingPitId, HistoryAction&& ha)
{
	if (getInstance()._isManagerOn) {
		auto it = getInstance()._history.find(fightingPitId);
		if (it != getInstance()._history.end()) {

			SPDLOG_DEBUG("[fp:{}] <HistoryAction> : {} of id {} named '{}' is executing action '{}' "
						 "with contender target [ {}] and ally target [ {}]", fightingPitId,
					ha.isContender ? "Contender" : "Ally",
					ha.idCharacter, ha.name, ha.actionKey,
					targetStr(ha.idContenderTarget),
					targetStr(ha.idAllyTarget));

			it->second.hist.emplace_back(std::move(ha));
		}
	}
}

void
FightingHistoryManager::setHistoricManagerOn(bool on)
{
	getInstance()._isManagerOn = on;
}

void
FightingHistoryManager::setToBeSaved(unsigned int fightingPitId, bool toBeSaved)
{
	auto& instance = getInstance();
	if (instance._isManagerOn) {
		auto it = instance._history.find(fightingPitId);
		if (it != instance._history.end()) {
			it->second.hasToBeSaved = toBeSaved;
		}
	}
}

void
FightingHistoryManager::createHistoric(const FightingPit& fp, unsigned seed)
{
	auto& instance = getInstance();
	if (instance._isManagerOn) {
		instance._history.insert(std::pair(fp.getId(), HistoryFight(std::ref(fp), seed)));
	}
}

void
FightingHistoryManager::save(unsigned int fightingPitId)
{
	auto& instance = getInstance();
	if (!instance._isManagerOn) return;

	auto historyIt = instance._history.find(fightingPitId);
	if (historyIt == instance._history.end()) return;

	if (historyIt->second.hasToBeSaved && historyIt->second.seed > 0) {
		// Todo Create a history file containing the logs of the fight (how it happened)
	}
	getInstance()._history.erase(historyIt);
	}
}




}