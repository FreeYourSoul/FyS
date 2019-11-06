# FyS Online

FreeYourSoul Online, a scalable mmorpg game

## Dispatchers

### What is a FyS dispatcher

## Services

### Arena Service

FreeYourSoul Online is using an original battle system inspired by Final Fantasy turn per turn systems. It is called Side Condition Timed Battle (SCTB) and it is explained into further details [here](doc/services/Arena_BattleSystem.md#arena).

### World Service

### InventoryGrab Service

### InventoryUse Service

### Inventory Trade Service

### Chat Service

### Quest Service


## External Dependency

* [ZeroMQ](https://github.com/zeromq/libzmq)  
    Network library
* [spdlog](https://github.com/gabime/spdlog)  
    Logging framework, usage of fmt library for logging
* [FSeam](https://github.com/FreeYourSoul/FSeam)  
    Mocking framework
* [Catch2](https://github.com/catchorg/Catch2)  
    Testing framework
* [ChaiScript](https://github.com/ChaiScript/ChaiScript)  
    For scripting of monsters in the Arena (fighting mode)
* [LUA](https://github.com/lua/lua)   --> soon to be  
    For the scripting of NPC in the world map (when moving)

