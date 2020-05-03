# Soul Draughtboard

One of the particularity of Soul Tales (FyS Online MMORPG) is the draughtboard system. It is greatly inspired with the Final Fantasy mecanism. Character in the game doesn't have level in the classical way of the term. The level of a character will determine how many Primordial Souls a character can use. But has a soul draughtboard, in which they can embed souls that are going to unlock skills and improve your characters in general.

## Souls Type

| Type Soul	| Slot | descriptions 	| 
|-----------------------	|------------------------------------------	|------------------------------------------	|
| Primordial Soul	| Primordial	| This soul has to be embed firstly in a Primordial Slot in order to extends the draughtboard	|
| Skill Soul	| Skill	| Unlock an ability	|
| Enhance Skill Soul	| Skill/Standard+	| Improved an unlocked ability to the next level	|
| Minor Soul 	| Standard	| Basic souls to improve stats : life amount, magic point amount, speed	|
| Normal Soul 	| Standard 	| Basic souls to improve stats : life amount, magic point amount, speed	|
| Major Soul	| Standard+	| Basic souls to improve stats : life amount, magic point amount, speed |

More information about how speed impact the game in the Arena battle system documentation part.

## Draughtboard Souls Slot

| Soul Slot Name	| descriptions 	| 
|-----------------------	|------------------------------------------	|
| Primordial	| Can only be used by Primordial souls, any soul replacing one in-place remove the soul already embedded	|
| Standard	| Can be used by Minor/Normal soul. Those souls can be retrieved	|
| Standard+	| Can be used by Major soul. Those souls can be retrieved	|
| Skill	| Can be used by Skill/Enhance Skill Souls in order to learn new skills or improve them to the next level, can also be used as a Standard+ slot if needed. Those souls can be retrieved	|

## Skill souls

Those souls makes it possible to unlock abilities, it is interesting to note that some ability are "Linked ability", which means that they can't be learned except all the required ability are learned.  

For example: the ability __Infernal Fire__ is dependent on __Fire Ball__ and __Hell Gate__, plus __Hell Gate__ is dependent on __Curse Touch__, it means that to unlock __Infernal Fire__ with a Skill soul, you need to used 4 Skill Souls in total (one for each of the dependencies and one for the skill to obtain).

Skills can be cast in battle, but skills could also be alterations on your character (for example, an alteration that is set at the start of the battle by default making your character recover life point every turn) or even on character from your complete team, like an aura that improve your whole team at the start of the battle.



## Soul Draughtboard

__note__: Those are examples that are still under work and may very well be changed in the future :

![Example](../assets/Soul%20Draughtboard.png)

As visible in this simplified diagram, like in FFX, slots are linked with between each other, forcing the player to fill a path before attaining specific slots that the user may want to use.


What is interesting is that some draughtboard could not have access to (or very few) Primordial slots. Which would be the reason for a player to carefully chose his companions, and/or, to change as their character could not improve anymore.

It is interesting to note that main characters way more primordial slot that standard characters (number to be defined).

## Todo (To Think)

* How many primordial slots do we make for main characters? 
* Maybe some slot could give special advantages? like giving a Skill if filled, making each draughtboard a little bit more unique.
* How are we dropping draughtboard ? It has to be a little bit challenging without being chinese farming. I assume the best would be to deliver some in the main quest line. But maybe having random drops in certain conditions..