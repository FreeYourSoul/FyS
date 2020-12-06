[![Build Status](https://travis-ci.com/FreeYourSoul/FyS.svg?branch=master)](https://travis-ci.com/FreeYourSoul/FyS)
[![Documentation Status](https://readthedocs.org/projects/fys/badge/?version=latest)](https://fys.readthedocs.io/en/latest/?badge=latest)
[![HitCount](http://hits.dwyl.io/FreeYourSoul/FyS.svg)](http://hits.dwyl.io/FreeYourSoul/FyS)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/2c227695c9824450a0b9bae3c6b881e0)](https://www.codacy.com/manual/FreeYourSoul/FyS?utm_source=github.com&utm_medium=referral&utm_content=FreeYourSoul/FyS&utm_campaign=Badge_Grade)
![Last commit](https://img.shields.io/github/last-commit/FreeYourSoul/FyS.svg)

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/LICENSE)
[![Scc Count Badge](https://sloc.xyz/github/FreeYourSoul/FyS/)](https://github.com/FreeYourSoul/FyS/)
[![Scc Count Badge](https://sloc.xyz/github/FreeYourSoul/FyS/?category=code)](https://github.com/FreeYourSoul/FyS/)

# FyS Online

FreeYourSoul project called Soul's Tale, a scalable MMORPG game

Global documentation (gameplay, story oriented) are in a sphinx documentation available [here](https://readthedocs.org/projects/fys/badge/?version=latest).  
Developper documentation is available [here](https://codedocs.xyz/FreeYourSoul/FyS/) as doxygen documentation.

The todo list is available [here](https://trello.com/b/lMR8LAve/fysgithubtodo)

### Dispatchers

A dispatcher is simply a proxy/reverse proxy server made to be used with zmq sockets, it has multiple "mode", in which it broadcast message to all it's connected services, or it load balance the requests.
It is used in order to be able to scale without having the client understanding the topology put in place to install Free Tails server.

### Arena Service

FreeYourSoul Online is using an original battle system inspired by Final Fantasy turn per turn systems. It is called Side Condition Timed Battle (SCTB) and it is explained into further details [here](docs/services/Arena_BattleSystem.md#arena).

The arena service is the server piece that will handle those fights (NPC handled through chaiscript), and report the experience/item gains.

### World Service

Service handling the movement in the world of the character controlled by the players and of the NPC (handled through LUA).
Each world service handle a piece of the universe (the universe of Free Tails being the complete map), a world service only handle a certain part of it (configuration defined). A transition from server to server is done discreetly when a boundary from one server to another is done in order to not hinder the user experience.
[See FyS article](https://freeyoursoul.online/fys3-the-first-architecture-part-2/) about the idea behind this. 

### Player Service

Stateless Service handling the player inventory (equipment, usage, money). Also used internally as a request service in order to retrieve information about the players (arena retrieving the team of the character).

### Quest Service

Stateless Service used in order to request validation of quest or quests advancement of players. (maybe to be combined with Player Service ?)

### Trade Service

Stateless Service handling trades of item between players. Each trade is represented as a state machine.

### Gateway Service (Chat)

Service handling connection of players to FreeTails and dispatch them over the good world service, this service is also linked the chat service (channels, messages..). The player keep being connected on the Gateway server in order to be able to communicate with any player connected on the FreeTails server.

## Installation

Two ways exist to build FyS server services (being separated in multiple binaries):

- by installing all the [external dependencies](README.md#external-dependency) and doing the cmake commands: 

```shell script
git clone https://github.com/FreeYourSoul/FyS.git
mkdir FyS/build && cd FyS/build
cmake ..
make
```

> Building can be long as the arena is using chaiscript which is header only scripting language (that unfortunately takes a long time to be compiled).

- If you don't want to install all those dependencies on your machine (understandable) especially in production environment (to avoid version conflict in case of upgrade). It has been decided to use NIX as a dependency manager. Which makes it very easy to execute a complex application (with complex dependencies) without having to install those directly on your system.

To use nix with FyS Online, you need to clone the repository and going to the root of the clone, where the default.nix file lies. You can then nix build each FyS component one per one by doing the following command <package_name> replaceable by: `arena_service`, `world_service`, `dispatcher`, `inventory_service`, `trade_service`
```shell script
git clone https://github.com/FreeYourSoul/FyS.git && cd FyS
nix-build -A <package_name>
```
Everything can be nix built with the following command:
```shell script
git clone https://github.com/FreeYourSoul/FyS.git && cd FyS
nix-build
```

Nix will compile each dependency not present in its official nix channel. For more information about how Nix works : [see their documentation](https://nixos.org/learn.html).
__It's an amazing tool that deserve attention.__

## External Dependency

-   [FiL](https://github.com/FreeYourSoul/FiL)
      General purpose library (algorithm, utilities, database...)
    -   Using [RocksDB](https://github.com/facebook/rocksdb) as base db (easy to locally test)
    -   Another DB would be used in the future following the same interface used for RocksDB for scalability (Cassandra/FoundationDB)

-   [ZeroMQ](https://github.com/zeromq/libzmq) And [cppzmq](https://github.com/zeromq/cppzmq)  
      Network library & C++ binding library

-   [spdlog](https://github.com/gabime/spdlog)  
      Logging framework, usage of fmt library for logging
      
-   [FSeam](https://github.com/FreeYourSoul/FSeam)  
      Mocking framework
      
-   [Catch2](https://github.com/catchorg/Catch2)  
      Testing framework

-   [ChaiScript](https://github.com/ChaiScript/ChaiScript)  
      For scripting of monsters in the Arena (fighting mode)

-   [LUA](https://github.com/lua/lua)
      For the scripting of NPC in the world map (when moving)
      
-   [Nix](https://nixos.org/learn.html)  
     Dependency manager for fast installation / setup.  
     [See installation paragraph](README.md#installation) to understand how to use it, this dependency is not required if you want to install fully those dependency on your machine.
