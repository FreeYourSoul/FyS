
## notes
* The script engine may be obsolete and not needed.
* The world could be working as a MAS (Multi Agent System). Which means that each NPC wouldn't be a script in itself, but an agent that communicate with its environment in order to choose its action. It would contact the world service certainly the same way any player would.
  * This would make it worth it to try to scale as even if no player play the game, the NPC will, and thus lot of npc would require a proper scaling
  * Playing the game would be more interesting as each player would basically be teamed up with random NPC when those NPC want to team up.


----

## Agent Properties listing:

* Hunger
* Money need
* Affinity with player (When teaming up)
* Personality traits
  * Vengeful --> more likely to get revenge (if courageous) or find other ways to avenge himself (if rich could make a contract on the head of someone ?)
  * Friendly --> more likely to team up 
  * Courageous --> more likely to take mission or options requiring fighting 

## Agent implementation

Pheromone / Environment Mediated Communication pattern ? https://www.researchgate.net/publication/2551027_Agent_Implementation_Patterns
