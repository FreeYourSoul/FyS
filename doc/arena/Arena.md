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

## Reminder of how CTBB (FFX battle system)

### Exanation taken from finalfantasy.fandom.com
CTB is a turn-based system which does not operate in rounds, instead it uses an Act List that is affected through various means and thus does not guarantee that each participant in a battle will have an equal number of turns. Units with higher speed take more turns than slower ones, making speed more important than in other turn-based battle systems. Players can substitute party members mid-battle adding a new level of strategy.  
  
### Resume
It is a fighting system based on a ordering list of the player based on the speed/statistic of each player and monsters. Which means that a fast character will have the right to attack multiple times before a slow monster fight back.

## Why making a new battle system?

First because its funnier to invent an original thing instead of copying existing concept. On top of that, while I loved FFX, I think some issue in the gameplay makes it a little bit too easy to play. It is try that Final Fantasy X is not particularly famous for being hard. And I try to solve those issue while also adding some element making the gameplay maybe more enjoyable in a MMORPG context.  
I would like first to thanks [CarNage64](https://www.twitch.tv/carnage64) whom is a twitch stream that speedrun FF7. He gave me advices on how the FFX is imperfect and how it could be improved.

### What are the downsides of CTB?


