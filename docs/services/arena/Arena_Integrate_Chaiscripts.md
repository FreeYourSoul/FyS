# Integrate Chaiscript

* [Integrate AI](#integrate-ai-scripts) (opponent artificial intelligence)
* [Integrate attacks](#integrate-attacks-scripts) (used by opponent and team characters)

## Introduction : Chaiscript

### Existing C++/Chai Mapping

```cpp
```

## Integrate AI scripts

### Template

Chaiscript for AI are represented by a class which has to take a contenderId as parameter in constructor and implement the methods:
 * runScriptedAction(id) which will determine and apply an action.
 * defineSpawningPosition() which will return the position on the battle field for the opponent
 
```cpp
class TemplateChai {
    var contenderId;
    
    TemplateChai(contenderId) {
        this.contenderId = contenderId;
    }    

    def runScriptedAction(id) {
        // decide attack
        // decide a target (if needed)
        // apply and send message
    }

    def defineSpawningPosition() {
        // return HexagonSide
    }
}
```

## Integrate attacks scripts

### Template 

```cpp
```