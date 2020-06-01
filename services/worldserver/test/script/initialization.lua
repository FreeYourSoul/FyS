---
--- Created by FyS.
--- DateTime: 6/1/20 1:59 AM
---

function getCharacterInfo(spawningPoint, luaId)
    if luaId >= spawningPoint.numbers then return nil end
end

function spawn(spawningPoint)
    for id, spawn in pairs(spawningPoint.spawned) do
        if spawn.isAlive == false then
            spawningPoint.spawned[id].isAlive = true
            return id
        end
    end
    return nil
end

function print_test()
    print("This is a print test")
end

function execMovement(spawningPoint)
    -- Increment the current step (maybe # can be used instead of numberSteps)
    spawningPoint.current = current + 1
    if spawningPoint.current == spawningPoint.numberSteps then
        spawningPoint.current = 0
    end
end