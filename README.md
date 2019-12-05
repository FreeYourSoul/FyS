[![CodeGrade](https://www.code-inspector.com/project/1982/status/svg)](https://www.code-inspector.com/project/dashboard/1982)
[![BCH compliance](https://bettercodehub.com/edge/badge/FreeYourSoul/FyS?branch=master)](https://bettercodehub.com/)
[![Documentation Status](https://readthedocs.org/projects/fys/badge/?version=latest)](https://fys.readthedocs.io/en/latest/?badge=latest)
[![HitCount](http://hits.dwyl.io/FreeYourSoul/FyS.svg)](http://hits.dwyl.io/FreeYourSoul/FyS)
![Last commit](https://img.shields.io/github/last-commit/FreeYourSoul/FyS.svg)

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/LICENSE)
[![](https://tokei.rs/b1/github/FreeYourSoul/FyS?category=files)](https://github.com/FreeYourSoul/FyS)
[![](https://tokei.rs/b1/github/FreeYourSoul/FyS?category=lines)](https://github.com/FreeYourSoul/FyS)
[![](https://tokei.rs/b1/github/FreeYourSoul/FyS?category=code)](https://github.com/FreeYourSoul/FyS)
# FyS Online

FreeYourSoul Online, a scalable mmorpg game

## Dispatchers

### What is a FyS dispatcher

## Services

### Arena Service

FreeYourSoul Online is using an original battle system inspired by Final Fantasy turn per turn systems. It is called Side Condition Timed Battle (SCTB) and it is explained into further details [here](docs/services/Arena_BattleSystem.md#arena).

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
* [Mariadbpp](https://github.com/viaduck/mariadbpp) :   
    Used as Maria DB connector

