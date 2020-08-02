[![Documentation Status](https://readthedocs.org/projects/fys/badge/?version=latest)](https://fys.readthedocs.io/en/latest/?badge=latest)
[![HitCount](http://hits.dwyl.io/FreeYourSoul/FyS.svg)](http://hits.dwyl.io/FreeYourSoul/FyS)
![Last commit](https://img.shields.io/github/last-commit/FreeYourSoul/FyS.svg)

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/LICENSE)
[![Scc Count Badge](https://sloc.xyz/github/FreeYourSoul/FyS/)](https://github.com/boyter/scc/)
[![Scc Count Badge](https://sloc.xyz/github/FreeYourSoul/FyS?category=code)](https://github.com/boyter/scc)
# FyS Online

FreeYourSoul project called Soul's Tale, a scalable MMORPG game

The todo list is available [here](https://trello.com/b/lMR8LAve/fysgithubtodo)

## Dispatchers

### What is a FyS dispatcher?

## Services

### Arena Service

FreeYourSoul Online is using an original battle system inspired by Final Fantasy turn per turn systems. It is called Side Condition Timed Battle (SCTB) and it is explained into further details [here](docs/services/Arena_BattleSystem.md#arena).

### World Service

Service handling the movement in the world of the character controlled by the players and of the NPC (handled through LUA).

### InventoryGrab Service

### InventoryUse Service

### Inventory Trade Service

### Chat Service

### Quest Service


## External Dependency

* [FiL](https://github.com/FreeYourSoul/FiL)
    General purpose library (algorithm, utilities...)
* [ZeroMQ](https://github.com/zeromq/libzmq) And [cppzmq](https://github.com/zeromq/cppzmq)  
    Network library & C++ binding library
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
* [Mariadbpp](https://github.com/viaduck/mariadbpp) :   
    Used as Maria DB connector

