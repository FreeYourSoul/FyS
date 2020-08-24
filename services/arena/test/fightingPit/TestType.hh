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


#ifndef FYS_ONLINE_TESTTYPE_HH
#define FYS_ONLINE_TESTTYPE_HH

#include <fstream>

#include <random_generator.hh>

#include <FSeamMockData.hpp>
#include <fightingPit/team/party_team.hh>
#include <fightingPit/team/team_member.hh>
#include <CmlKey.hh>
#include <Cml.hh>
#include <CmlCopy.hh>

// moving action
static const std::string actionTestMoveScript = R"(
class Move_TEST {
	attr moveTo;

    def Move_TEST() {
		this.moveTo = 0;
	}

    def requireTarget() {
        NONE;
    }

    def execute(target) {
		if (this.moveTo == 0) {
			this.moveTo = this.moveTo + 1;
			pitLayout.initiateForceContenderMove(target, A_NE);
		}
		else if (this.moveTo == 1) {
			this.moveTo = this.moveTo + 1;
			pitLayout.initiateForceContenderMove(target, C_NE);
		}
		1;
    }
};
)";

// sleep action, is actually skipping turn and retrieving MP
static const std::string actionTestSleepScript = R"(
class Sleep_TEST {
    def Sleep_TEST() {}

    def requireTarget() {
        NONE;
    }

    def execute(targetStatus) {
		var new_total = targetStatus.life.current + 40;
		if (new_total > targetStatus.life.total) {
			targetStatus.life.current = targetStatus.life.total;
		}
		else {
			targetStatus.life.current = targetStatus.life.current + 40;
		}
        1;
    }
};
)";

// attack action, doing fixed damage
static const std::string actionTestAttackScript = R"(
class Attack_TEST {
    var power;

    def Attack_TEST(v) {
        this.set_explicit(true);
        this.power = v;
    }

    def requireTarget() {
        ENNEMY;
    }

    def execute(targetStatus) {
        if (targetStatus.life.current > this.power) {
            targetStatus.life.current = targetStatus.life.current - this.power;
        }
        else {
            targetStatus.life.current = 0;
        }
        1;
    }

};
)";

// monster just move arround
static const std::string MonsterTestMoveAttack = actionTestMoveScript + R"(
class TestMonsterMove {

    attr id;
    attr level;
    attr actions;

    def TestMonsterMove(contenderId, level) {
        this.set_explicit(true);
        this.level = level;
        this.id = contenderId;

        this.actions = action(

            // possibles actions/decide target vector
            [
                action(Move_TEST(), fun(action, thisContender) {
					thisContender;
				}, "test:key:1")
            ],

            // decide target function
            fun(currentContenderStatus) {
				0;
            }, ""
        );
    }

    def setupContender() {
        var &thisContender = pitContenders.getFightingContender(this.id);
        var &thisStatus = thisContender.accessStatus();
        thisStatus.speed = 8;
        thisStatus.life.total = 100;
        thisStatus.life.current = thisStatus.life.total;
        thisStatus.magicPoint.total = 100;
        thisStatus.magicPoint.current = thisStatus.magicPoint.total;
     }

    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        var &thisStatus = thisContender.accessStatus();
        var actionId = this.actions.decisionStrategy(thisStatus);
        var &action = this.actions.act[actionId];
        action.act.execute(action.decisionStrategy(action.act, thisContender));
    }

};
)";

// monster 100 life, 8 speed, only attack (90 damages)
static const std::string MonsterTestScriptAttack = actionTestAttackScript + R"(
class TestMonsterAttack {

    attr id;
    attr level;
    attr actions;

    def TestMonsterAttack(contenderId, level) {
        this.set_explicit(true);
        this.level = level;
        this.id = contenderId;

        this.actions = action(

            // possibles actions/decide target vector
            [
                action(Attack_TEST(90), fun(action, thisContender) {
                    decide_target.lower_life(action.requireTarget(), thisContender.getHexagonSideOrient()).accessStatus();
                }, "test:key:1")
            ],

            // decide target function
            fun(currentContenderStatus) {
				0;
            }, ""
        );
    }

    def setupContender() {
        var &thisContender = pitContenders.getFightingContender(this.id);
        var &thisStatus = thisContender.accessStatus();
        thisStatus.speed = 8;
        thisStatus.life.total = 100;
        thisStatus.life.current = thisStatus.life.total;
        thisStatus.magicPoint.total = 100;
        thisStatus.magicPoint.current = thisStatus.magicPoint.total;
     }

    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        var &thisStatus = thisContender.accessStatus();
        var actionId = this.actions.decisionStrategy(thisStatus);
        var &action = this.actions.act[actionId];
        action.act.execute(action.decisionStrategy(action.act, thisContender));
    }

};
)";

// attack making a one-shot poison
static const std::string actionTestPoisonScript = R"(
def alteration_InstantDeathPoison(status, lvl, turn) {
	if (turn == 0) {
		print("Poison killed");
		status.life.current = 0;
	}
	1;
}

def PoisonousAttackRetrieveAlterations() {
	[
 		"test:alterations:MortalPoison"
	]
}

class PoisonousAttack {
    attr turnToKill;
	attr poison;

    def PoisonousAttack(v) {
        this.set_explicit(true);
        this.turnToKill = v;
		this.poison = [
			Alteration("test:alterations:MortalPoison", 2, 2, alteration_InstantDeathPoison)
		];
    }

    def requireTarget() {
        ENNEMY;
    }

    def execute(targetStatus) {
		print("PoisonousAttack execute called");
        addBeforeTurnAlterations(targetStatus, this.poison, false);
		1;
    }

};)";

// monster 100 life, 8 speed, only send a one turn Mortal Poison
static const std::string MonsterTestScriptPoison = actionTestPoisonScript + R"(
class TestMonsterPoison {
    attr id;
    attr level;
    attr actions;

    def TestMonsterPoison(contenderId, level) {
        this.set_explicit(true);
        this.level = level;
        this.id = contenderId;

        this.actions = action(

            // possibles actions/decide target vector
            [
                action(PoisonousAttack(this.level), fun(action, thisContender) {
                    decide_target.lower_life(action.requireTarget(), thisContender.getHexagonSideOrient()).accessStatus();
                }, "test:key:1")
            ],

            // decide target function
            fun(currentContenderStatus) {
				0;
            }, ""
        );
    }

    def setupContender() {
        var &thisContender = pitContenders.getFightingContender(this.id);
        var &thisStatus = thisContender.accessStatus();
        thisStatus.speed = 8;
        thisStatus.life.total = 100;
        thisStatus.life.current = thisStatus.life.total;
        thisStatus.magicPoint.total = 100;
        thisStatus.magicPoint.current = thisStatus.magicPoint.total;
     }

    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        var &thisStatus = thisContender.accessStatus();
        var actionId = this.actions.decisionStrategy(thisStatus);
        var &action = this.actions.act[actionId];
        return action.act.execute(action.decisionStrategy(action.act, thisContender));
    }

};
)";

// monster 100 life, 8 speed, only sleep
static const std::string MonsterTestScriptSleep = actionTestSleepScript + R"(

class TestMonsterSleep {

    attr id;
    attr level;
    attr actions;

    def TestMonsterSleep(contenderId, level) {
        this.set_explicit(true);
        this.level = level;
        this.id = contenderId;

        this.actions = action(

            // possibles actions/decide target vector
            [
                action(Sleep_TEST(), fun(action, thisContender) {
					thisContender.accessStatus();
                }, "test:key:1")
            ],

            // decide target function
            fun(currentContenderStatus) {
				0;
            }, ""
        );
    }

    def setupContender() {
        var &thisContender = pitContenders.getFightingContender(this.id);
        var &thisStatus = thisContender.accessStatus();
        thisStatus.speed = 8;
        thisStatus.life.total = 100;
        thisStatus.life.current = thisStatus.life.total;
        thisStatus.magicPoint.total = 100;
        thisStatus.magicPoint.current = thisStatus.magicPoint.total;
     }

    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        var &thisStatus = thisContender.accessStatus();
        var actionId = this.actions.decisionStrategy(thisStatus);
        var &action = this.actions.act[actionId];
        return action.act.execute(action.decisionStrategy(action.act, thisContender));
    }

};
)";

class CmlBase final : public fys::cache::Cml {
public:
	explicit CmlBase(std::string v)
			:fys::cache::Cml(std::filesystem::path(std::move(v))) { }

	void createUpToDateFileInLocalStorage(const fys::cache::CmlKey&, std::filesystem::file_time_type) override
	{

	}

};

class DeleteFolderWhenDone {
public:
	explicit DeleteFolderWhenDone(const std::string& v)
			:
			_path(v) { }

	~DeleteFolderWhenDone()
	{
		SPDLOG_INFO("Delete folder {}", _path.string());
		std::filesystem::remove_all(_path);
	}

private:
	std::filesystem::path _path;

};

class CmlBaseCopy final : public fys::cache::CmlCopy {
public:
	explicit CmlBaseCopy(const std::string& v, const std::string& w)
			:
			fys::cache::CmlCopy(v, w) { }

protected:
	void createUpToDateFileInLocalStorage(const fys::cache::CmlKey& cmlKey, std::filesystem::file_time_type) override
	{
		std::error_code e;
		std::filesystem::create_directories(cmlKey.get_path().parent_path(), e);

		if ("testing:TestMonsterSleep.chai" == cmlKey.getKey()) {
			std::ofstream ofs(cmlKey.get_path());
			ofs << MonsterTestScriptSleep;
		}
		else if ("testing:TestMonsterAttack.chai" == cmlKey.getKey()) {
			std::ofstream ofs(cmlKey.get_path());
			ofs << MonsterTestScriptAttack;
		}
		else if ("testing:TestMonsterPoison.chai" == cmlKey.getKey()) {
			std::ofstream ofs(cmlKey.get_path());
			ofs << MonsterTestScriptPoison;
		}
		else if ("testing:TestMonsterMove.chai" == cmlKey.getKey()) {
			std::ofstream ofs(cmlKey.get_path());
			ofs << MonsterTestMoveAttack;
		}
		else {
			fys::cache::CmlKey k(_copyPathStorage, cmlKey.getKey());
			std::filesystem::copy(k.get_path(), cmlKey.get_path(), e);
		}
	}
};

[[nodiscard]] static std::unique_ptr<fys::arena::party_team>
getPartyTeam(const std::string& user)
{
	auto team = std::make_unique<fys::arena::party_team>(user);

	// Temporary hard coded party team
	auto tm1 = std::make_shared<fys::arena::team_member>(user, "Elvo");
	auto tm2 = std::make_shared<fys::arena::team_member>(user, "Mirael");
	auto tm3 = std::make_shared<fys::arena::team_member>(user, "Fyston");
	auto tm4 = std::make_shared<fys::arena::team_member>(user, "Simon");

	fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm1, fys::arena::hexagon_side::orientation::B_S);
	auto& s1 = tm1->access_status();
	s1.life_pt.total = 100;
	s1.life_pt.current = 100;
	s1.magic_pt.total = 20;
	s1.magic_pt.current = 20;
	s1.initial_speed = 3;
	fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm2, fys::arena::hexagon_side::orientation::B_S);
	auto& s2 = tm2->access_status();
	s2.life_pt.total = 200;
	s2.life_pt.current = 200;
	s2.magic_pt.total = 0;
	s2.magic_pt.current = 0;
	s2.initial_speed = 5;
	fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm3, fys::arena::hexagon_side::orientation::B_S);
	auto& s3 = tm3->access_status();
	s3.life_pt.total = 550;
	s3.life_pt.current = 550;
	s3.magic_pt.total = 10;
	s3.magic_pt.current = 10;
	s3.initial_speed = 10;
	fys::arena::fighting_pit_layout::set_ally_move_initiate_position(*tm4, fys::arena::hexagon_side::orientation::B_S);
	auto& s4 = tm4->access_status();
	s4.life_pt.total = 140;
	s4.life_pt.current = 140;
	s4.magic_pt.total = 10;
	s4.magic_pt.current = 10;
	s4.initial_speed = 20;

	team->add_team_member(std::move(tm1));
	team->add_team_member(std::move(tm2));
	team->add_team_member(std::move(tm3));
	team->add_team_member(std::move(tm4));
	return team;
}

#endif //FYS_ONLINE_TESTTYPE_HH
