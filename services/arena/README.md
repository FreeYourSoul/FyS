# Arena Service

This service is the one managing battle for FyS Online (MMORPG).  
More information on the [documentation part].

## TODO

* Create the internal (interprocess) zmq communication channel (even without threading pool) to read from the FightingPit
* Implement a first fighting version without multiple without multiple battles.
    * Make one Arena Worker by default.
    * Implement chai scripting
        * Minimum script call possible in a first place. Monster will have their script fully loaded and have a "executeAction" function that will be called from the cpp
        * Players will have to launch chai script to execute actions (no c++ hard coded action) 
        * Each chai scripts need to be tested unitarly by a cpp test case which has to be developped in a way that it will be reusable.
        * When scripting are done and tested, create a repository to store those scripts (and all the upcoming ones). And link them to the repository FyS via a submodule. 
    * Check if it is faster to return a cpp function from chaiscript and execute it compared to executing the code in chaiscript.
* Unit test the fighting mechanism with mocked scripting
* Test external authentication/implement messaging of notifications to players
* Implement thread pool into each worker to enable multiple fightingPit per service

