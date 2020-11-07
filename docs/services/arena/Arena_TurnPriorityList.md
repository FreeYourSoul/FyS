# Turn per Turn priority

# Requirements

-   A full turn is defined as follow: The slowest pit participant played sign the completion of the current turn
-   A player's character (or a contender) can have multiple action in a single turn depending to its speed
-   The priority list has to be recalculated when :
    -   A player's character (or contender) die and is temporarily (for player's character) or definitely removed from the priority list.
    -   When a player or a contender join the fight (as for character reviving)
    -   The speed of a character has been impacted (by a spell, an item, an attack).

# Implementation

Each pit participant has a speed defined as an unsigned integer. The faster go before the slowest.    

**Generation of priorityList** : 
1. Order the list of speed from fastest to slowest.
2. The first one is the first one to have a turn (add him in the priority list as first)
3. Subtract his speed with the second one
4. Re-order the list of speed from fastest to slowest.
5. The first one in this list is added in priority list
6. Subtract his speed with the second one
7. Re-order..
8. ...

_Graphical example_, each color represent a different character (monster or player's character):

![turn1](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/doc/other/Turn1.png)

The difference of speed between the fastest and the slowest being important, the purple player can play 4 times before the end of the turn.  
For the second turn, the base maximum speed is added to the every one plus their own base speed.

![turn2](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/doc/other/Turn2.png)

This turn is way shorter than the previous turn (every character play only once), but the next one as the speed stabilized a little bit, but in the third turn, as the purple player end this turn with 19 of speed, he will play multiple times before the end of the turn.  
This algorithm give an edge to the fast player without being broken (playing multiple time every single turns).  

Here a resume of the two turns:

![resume](https://raw.githubusercontent.com/FreeYourSoul/FyS/master/doc/other/TurnResume.png)
