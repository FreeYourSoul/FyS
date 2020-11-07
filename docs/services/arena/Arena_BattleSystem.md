# Arena

The Arena is the name for the service that is managing FyS Online fighting phase. An instance of a fight is called a **Fighting Pit**.  
In further explanation, the service will be called Arena and Fighting Pit will be called FP (for faster writing, because as any developer, I am lazy to write down long names).

## Arena specification

The arena will have multiple requirement to fulfill:

-   First it will be required for the Arena to be a service able to host multiple Fighting Pits (FP).
-   Any Arena instance can host a fight from any player (no character restriction depending on its position for example).
-   A fight hosted by one Arena will be managed from begin to end on the same arena (which will make the service stateful).

# Fighting Pit

The fighting pit (FP) is an instance of a fight in the world of FyS Online. FyS Online has a turn per turn battle system called SCTB (**S**ide **C**onditional **T**imed **B**attle).  
This is strongly inspired from the Final Fantasy X battle (CTB : Conditional Turn Based Battle) but is quite different in pratice.

## Reminder of how CTB (FFX battle system)

### Exanation taken from finalfantasy.fandom.com

CTB is a turn-based system which does not operate in rounds, instead it uses an Act List that is affected through various means and thus does not guarantee that each participant in a battle will have an equal number of turns. Units with higher speed take more turns than slower ones, making speed more important than in other turn-based battle systems. Players can substitute party members mid-battle adding a new level of strategy.  

### Resume

It is a fighting system based on a ordering list of the player based on the speed/statistic of each player and monsters. Which means that a fast character will have the right to attack multiple times before a slow monster fight back.

## Why making a new battle system?

First because its funnier to invent an original thing instead of copying existing concept. On top of that, while I loved FFX, I think some issue in the gameplay makes it a little bit too easy to play. Final Fantasy X is not particularly famous for being hard. And I try to solve those issue while also adding some elements making the gameplay maybe more enjoyable in a MMORPG context (multiplayer).  
I would like first to thanks [CarNage64](https://www.twitch.tv/carnage64) whom is a twitch stream that speedrun FF7. He gave me advices on how the FFX is imperfect and how it could be improved.

![ffx-fight](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/docs/assets/FFX-Fight.png)

### What are the downsides of CTB?

The way CTB is designed present several issues:

-   Infinite time turn: Turns are potentially infinite, everything is based on the priority list displayed on the UI which is calculated based on the speed of your characters (and enemies). Which gives you all the time you want to elaborate a strategy, to heal your hurt characters at the last second (knowing perfectly that the opponent won't attack before you heal). This last point being important as the "stress of potential death" is almost not present. This point is certainly the biggest flaw in CTB.
-   Too easy to change characters: In CTB, you have 4 characters fighting and many more are waiting behind and can replace your character currently in play. This is a nice thing as it increase the number of playable character making the game more interesting. But the issue is that there is no penalty changing character in-fight (the turn of the switched character not being lost). This also makes the game easier as you can retreat your hurt character before the enemy attack, and makes him come back afterwards basically for free.
-   Not adapted for multiplayer

# A new battle system SCTB (Side Condition Timed Battle)

Side Condition Timed Battle is a mix between CTB (FF-X battle system) and more classical battle system based on timers.  
Let's peel the SCTB acronyme in order to understand the basics of this battle system.

## Condition Timed

Even if FF-X condition list, which make you able to know the ordering of the characters/opponnents turn, made the game easy. It was a pretty good idea as it gave an interesting strategic aspect to the battle, making you able to better play with the "limit" of your characters ressources (nearly dying, nearly out of magic power and so on...).  
SCTB has the same condition list (based on characters/monsters speed), but in order to remove the easiness of FF-X battle system, each characters turn are timed. Plus a configurable difficulty mode makes you able to change this timer to be shorter (easy) or longer (hard). At the end of the timer, the character turn is skipped and the next character in the condition list takes it place.  

This "Condition Timed" part of SCTB remove the Infinite time turn problematic explained above.

## Side

Here is where SCTB makes a difference compared to other turn per turn battle system.  
The field in which you fight your opponnent(s) is not flat like in pokemon (or Final Fantasy 1 to 10). But is composed of a total of 18 possible angles. The gaming board could be represented as 3 hexagons stuck together (see ascii art below).  
Your team of characters (4 characters) will be on a SIDE of the below hexagon, same for the monster(s). Your characters can move independently from one side to another (adjacent side) to fight monsters.

                             _____             
                            /     \          
                      _____/  B    \        18 possible playable sides  
                     /     \       /        6  possible ambush sides [A_NE, B_SW, A_SE, C_NW, C_N, B_S]  
                    /  A    \_____/           
                    \       /     \  
                     \_____/  C    \  
                           \       /   
                            \_____/  

Little monster (the size of your character) are going to be able to be on only one side of one of the three hexagon (the same as you character). Which means that if a character change sides, if the monster doesn't follow you, he won't be able to reach this monster from the new side.  
You can thing of each sides as a separate battle happenning at the same time. There can be 18 battle front in total, and your characters can use a turn to change battle front (side) instead of doing another action (attacking for example).  
This aspect add some strategy as some monster could have very powerfull area of effect spells that would touch ever.  

You can find bellow a very sketchy draw of example:

![SCTB-MoveExample](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/docs/assets/ExampleFySArena.png)

In this state the player is in the at the south west of the hexagon B, if the player decide to move all his characters to the left, the characters will be on the North west of Polygon B (as shown below). In this other side of the polygon B, another monster will be present.

![SCTB-MoveExampleAfter](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/docs/assets/ExampleFySArenaAfter.png)

### Large sized monster

A Monsters can be big enough to be attacked from multiple sides. For example a big Hydra Boss monster could be on the 6 faces of the Hexagon A at the same time. Some characters of your team would be on the border north-east of the Hexagon A to fight one of the Head, others could be on the north border of the hexagon to fight another head. Or even at the back of the hydra (on the border south / south-east) where the characters would fight the tail of the Hydra.    

### Multiplayer adapted

18 battle front possible is really a lot, way more than all the normal encounters that will be usually made in the game (1 to (rarely) 3 battle front in random encounter). So why having that many of them ? The answer is simple, in MMORPG, two things are really appreciated by the players, the PvP (player vs player) to show off the individual skill of each players. And the PvE (Player vs Environment) to play in team and make succesful strategy against the game's traps.
FyS Online is focusing on PvE for now, and the SCTB is particularly interesting for it as it would be the first (as far as I know) turn per turn game that would allow multiple player to play at the same time in a very good pace (not having to wait long before playing).  

**Multiple player that has to wait their turn?** : Yeah, sounds horrible right? if there is a maximum of 18 battle field with, let's say 2 player by battle field. Then there is 36 players. You should wait for 36 player + the monsters turn before playing. It is impossible. But, what if each side, each battle field, have their own turns? Then it means for each battle field (each sides of the hexagon), you just have to wait for 1 player and the monster present on this specific side to play before your turn.
And this is how SCTB works, there is one timer and one priority list by side, which makes it playable even with a lot of players playing at the same time.  
The fact that you can move from one side to another is not inconsequential as most of the spells you can cast are limited to the current side your character is in, so strategy can be put in place in order to have healers coming to heal and protect themselves on a side that is less dangerous (for example).  
Of course if every players are present on the same side, it will have the waiting issue. But it shouldn't be the case as the only reason to have lot of player fighting together is to beat a boss that will be a "larged sized monster" as explained above. Whom will have multiple sides to fight one and adds (monster that could spawn during the different phase of the boss), also on other sides.

**But you said each player can move their character from side to side independently:** Yes, making the characters able to move independently is the goal. It would increase the complexity of the game exponentially for the players (as they will need to handle multiple turn at the same time from different sides). But it can have the issue of making too many players on one specific sides and thus slowing down the pace of the game greatly. Additional work on this part is required to make the gameplay fun. 
