# Integrate Chaiscript

The goal of this documentation page is to explain how to integrate new scripts (Contender and/or Actions) in ArenaService and make it usable to the players.

## Introduction : Chaiscript

ChaiScript is a scripting language made by Jason Turner designed to be used with C++. Its main focus is in its usability and interfacing with C++. While keeping good performances (less than LUA but still acceptable) it makes it possible to do an almost transparent interface between C++ and Chai, making it a very suitable scripting language for complicated game logic.

### Existing C++/Chai Mapping

Before explaining how to make new script, it is important to note that helper functions have been registered and exposed to the ChaiScript engine to give access to information for the script. Those ChaiScript function are registered in the file [ChaiRegister.cpp](https://github.com/FreeYourSoul/FyS/blob/master/services/arena/src/ChaiRegister.cpp).

Below are the C++ signatures of those functions, they are accessible and callable from chai scripts:

```cpp
/**
 * Generate a random number between the double range_InBound and range_OutBound
 * @return a randomly generated number (double)
 */
double generateRandomNumber(double range_InBound, double range_OutBound);
//! Overload with int
int generateRandomNumber(int range_InBound, int range_OutBound);

/**
 * @param isContender true if the Character to check is a contender, false if it is an Ally
 * @param id id of the Character to check
 * @param orient Side to check
 * @return true if the character is on the given side, false otherwise
 */
bool isCharacterOnSide(bool isContender, unsigned id, HexagonSide::Orientation orient);

/**
 * Check if a character is on an adjacent side (adjacent side beeing a side you can move to)
 *
 * @param isContender true if the Character to check is a contender, false if it is an Ally
 * @param id id of the Character to check
 * @param orient Side to check
 * @return true if the character is adjacent to the given side, false otherwise
 */
bool isCharacterOnAdjacentSide(bool isContender, unsigned id, HexagonSide::Orientation side);

/**
 * @param lhs a side to check 
 * @param rhs a side to check 
 * @return true if the two given sides are adjacent, false otherwise
 */
bool isSideAdjacentSide(HexagonSide::Orientation lhs, HexagonSide::Orientation rhs);

/**
 * @param alterations alterations to add 
 * @param replaceIfExist true if the alteration override existing ones, false otherwise
 * @return 
 */
void addOnTurnAlterations(data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist);
//! Same function, but the alteration will be applied before the turn occurs
void addBeforeTurnAlterations(data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist);
//! Same function, but the alteration will be applied after the turn occurs
void addAfterTurnAlterations(data::Status& status, std::vector<data::Alteration> alterations, bool replaceIfExist);

```

On top of the above functions registered reference to types present in the C++ worlds exist in order to access to the status of the characters

```cpp

//! name of the reference to use in Chai scripts of the instance of PitContenders
pitContenders;

//==== PitContenders chai exposed member methods ====

/**
 * @param comparator against which the contenders will be checked against
 * @return the contender following the comparator the most
 */
FightingContender PitContenders::selectSuitableContender(HexagonSide::Orientation side);
//! Same but retrieve an alive character
FightingContender PitContenders::selectSuitableContenderAlive();
//! Same but on a specific side
FightingContender PitContenders::selectSuitableContenderOnSide(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp);
//! Same but only retrieve an alive character on a specific side
FightingContender PitContenders::selectSuitableContenderOnSideAlive();

//! Retrieve a randomly chosen alive character on a specific side 
FightingContender PitContenders::selectRandomContenderOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<FightingContender> comp);

//! Retrieve a contender with a specific id
FightingContender PitContenders::getFightingContender(unsigned id);

//! Retrieve all the contender on a specific side
vector<FightingContender> PitContenders::getContenderOnSide(HexagonSide::Orientation side);

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//! name of the reference to use in Chai scripts of the instance of AllyPartyTeams
allyPartyTeams;

//==== AllyPartyTeams chai exposed member methods ====
/**
 * @param comp against which the allies will be checked against
 * @return the ally following the comparator the most
 */
TeamMember AllyPartyTeams::selectSuitableMember(ComparatorSelection<TeamMember> comp);
//! same but retrieve an alive character
TeamMember AllyPartyTeams::selectSuitableMemberAlive(ComparatorSelection<TeamMember> comp);
//! same but retrieve a character on a specific side
TeamMember AllyPartyTeams::selectSuitableMemberOnSide(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp);
//! same but retrieve an alive character on a specific side
TeamMember AllyPartyTeams::selectSuitableMemberOnSideAlive(HexagonSide::Orientation side, ComparatorSelection<TeamMember> comp);

//! Retrieve a randomly chosen alive character on a specific side 
TeamMember AllyPartyTeams::selectRandomMemberOnSideAlive();

``` 

## Integrate AI scripts

### Template

Chaiscript for AI are represented by a class which has to take a contenderId and a level as parameter in constructor and follow this implementation:  

| to follow	| descriptions 	|  	|  	|  	|
|-----------------------	|------------------------------------------	|---	|---	|---	|
| Constructor(id, level)	| Constructor taking the level of the contender and its id, this constructor has to set the actions attribute	|  	|  	|  	|
| runScriptedAction(id) 	| which will determine and apply an action which will be following the next one	|  	|  	|  	|
| setupContender() 	| which will set the spawning position 	|  	|  	|  	|
| contains actions	| The contender class has an action attributes, which will be a map of Action chai object	|  	|  	|  	|
| 	|  	|  	|  	|  	|

```cpp
class Sampy {

    attr id;
    attr level;
    attr actions;

    def Sampy(contenderId, level) {
        this.id = contenderId;
        this.level = level;
        this.actions = action(

            // possibles actions/decide target vector
            [
                action(ACTION_SCRIPT(50), fun(action, thisContender) {
                    // decision strategy
                }, "test:key:1"),

                action(ACTION_SCRIPT(80), fun(action, thisContender) {
                    // decision strategy
                }, "test:key:2"),

                action(ACTION_SCRIPT(0), fun(action, thisContender) {
                    // decision strategy
                }, "test:key:3")
            ],

            // decide target function
            fun(currentContenderStatus) {
                // return index of the action to do
            }, ""
        );
    }

    def setupContender() {
        var &thisContender = pitContenders.getFightingContender(this.id);
        var &thisStatus = thisContender.accessStatus();
        // Set the status of this contender
     }

    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        var &thisStatus = thisContender.accessStatus();
        var actionId = this.actions.decisionStrategy(thisStatus);
        var &action = this.actions.act[actionId];
        return action.act.execute(action.decisionStrategy(action.act, thisContender));
    }

```
### Action attribute

You noticed above the attribute action, which is a core part of the artificial intelligence as it is the place where the decision making is done concerning the target, and which spell is going to be cast.

```cpp
class action {
    attr act;
    attr decisionStrategy;
    attr actionKey;

    def action(a, decisionStrategy, actionKey)
    {
        this.set_explicit(true);
        this.act = a;
        this.decisionStrategy = decisionStrategy;
        this.actionKey = actionKey;
    }

}
```

The action object is a simple chai object defined as above, containing an attack object (see below how to create such chaiscript object), a lambda function that return the status of the target for the given attack, and the key of the attack. 

This action attribute is firstly set with a vector of action and a function to decide which action to do. This is the decide action part. And each action object in this vector contains an actual action and a function that select a target for it.

```cpp
    def runScriptedAction(id) {
        var &thisContender = pitContenders.getFightingContender(id);
        // retrieve status of the current contender
        var &thisStatus = thisContender.accessStatus();

        // decide action to do thanks to the action attribute containing the vector of actions. We call the decisionStrategy that decide and return the index in the vecotr of the action to do.
        var actionId = this.actions.decisionStrategy(thisStatus);
        // retrieve the action object thanks to this index
        var &action = this.actions.act[actionId];
        // execute this action on the target retrieved thanks to the decisionStrategy of the action
        return action.act.execute(action.decisionStrategy(action.act, thisContender));
    }
```

## Integrate attacks scripts

### Template 

```cpp
```

## Integrate alterations script

### Template

## Include actions 
