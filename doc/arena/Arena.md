# Arena 


The Arena is the name for the service that is managing FyS Online fighting phase. An instance of a fight is called a **Fighting Pit**.  
In further explanation, the service will be called Arena and Fighting Pit will be called FP (for faster writing, because as any developer, I am lazy to write down long names).

## Arena specification

The arena will have multiple requirement to fulfill:
* First it will be required for the Arena to be a service able to host multiple Fighting Pits (FP).
* Any Arena instance can host a fight from any player (no character restriction depending on its position for example).
* A fight hosted by one Arena will be managed from begin to end on the same arena (which will make the service stateful).

# Fighting Pit

The fighting pit (FP) is an instance of a fight in the world of FyS Online. FyS Online has a turn per turn battle system called SRTb (**S**ide **R**eal **T**ime **B**attle).  
This is strongly inspired from the Final Fantasy X battle (CTBB : Conditional Turn Based Battle) but is quite different in pratice.

## Reminder of how CTB (FFX battle system)

### Exanation taken from finalfantasy.fandom.com
CTB is a turn-based system which does not operate in rounds, instead it uses an Act List that is affected through various means and thus does not guarantee that each participant in a battle will have an equal number of turns. Units with higher speed take more turns than slower ones, making speed more important than in other turn-based battle systems. Players can substitute party members mid-battle adding a new level of strategy.  
  
### Resume
It is a fighting system based on a ordering list of the player based on the speed/statistic of each player and monsters. Which means that a fast character will have the right to attack multiple times before a slow monster fight back.

## Why making a new battle system?

First because its funnier to invent an original thing instead of copying existing concept. On top of that, while I loved FFX, I think some issue in the gameplay makes it a little bit too easy to play. Final Fantasy X is not particularly famous for being hard. And I try to solve those issue while also adding some elements making the gameplay maybe more enjoyable in a MMORPG context (multiplayer).  
I would like first to thanks [CarNage64](https://www.twitch.tv/carnage64) whom is a twitch stream that speedrun FF7. He gave me advices on how the FFX is imperfect and how it could be improved.

![ffx-fight](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/doc/other/FFX-Fight.png)

### What are the downsides of CTB?

The way CTB is designed present several issues:
* Infinite time turn: Turns are potentially infinite, everything is based on the priority list displayed on the UI which is calculated based on the speed of your characters (and enemies). Which gives you all the time you want to elaborate a strategy, to heal your hurt characters at the last second (knowing perfectly that the opponent won't attack before you heal). This last point being important as the "stress of potential death" is almost not present. This point is certainly the biggest flaw in CTB.
* Too easy to change characters: In CTB, you have 4 characters fighting and many more are waiting behind and can replace your character currently in play. This is a nice thing as it increase the number of playable character making the game more interesting. But the issue is that there is no penalty changing character in-fight (the turn of the switched character not being lost). This also makes the game easier as you can retreat your hurt character before the enemy attack, and makes him come back afterwards basically for free.
* Not adapted for multiplayer

# A new battle system SCTB (Side Condition Timed Battle)

The name of the 
