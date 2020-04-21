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

#include <fightingPit/FightingPit.hh>
#include "FightingHistoryManager.hh"

namespace fys::arena {

void
FightingHistoryManager::addHistoric(unsigned int fightingPitId, HistoryAction&& ha)
{
	if (getInstance()._isManagerOn) {
		auto it = getInstance()._history.find(fightingPitId);
		if (it != getInstance()._history.end()) {
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
		instance._history[fp.getId()] = HistoryFight(std::ref(fp), seed);
	}
}

FightingHistoryManager::HistoryFight::HistoryFight(const FightingPit& pt, unsigned seed) noexcept
		:ref(pt), seed(seed) { }

FightingHistoryManager::HistoryFight::~HistoryFight()
{
	if (hasToBeSaved && seed > 0) {
// Todo Create a history file containing the logs of the fight (how it happened)
	}
}

}