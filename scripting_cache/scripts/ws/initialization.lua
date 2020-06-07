---
--- Created by FyS.
--- DateTime: 6/1/20 1:59 AM
---

function round(num)
    if num >= 0 then
        return math.floor(num + .5)
    else
        return math.ceil(num - .5)
    end
end

function getCharacterInfo(spawningPoint, luaId)
    if luaId >= spawningPoint.numbers then
        return nil
    end
    return spawningPoint.spawn[luaId].initial_info.x, spawningPoint.spawn[luaId].initial_info.y, spawningPoint.spawn[luaId].initial_info.velocity, spawningPoint.spawn[luaId].initial_info.angle
end

function spawn(spawningPoint)
    for id, spawn in pairs(spawningPoint.spawned) do
        if spawn.isAlive == false then
            spawningPoint.spawnAtIndex(id)
            spawningPoint.spawned[id].isAlive = true
            return id
        end
    end
    return nil
end

function execMovement(spawningPoint, luaId, info)
    tobe_x = round(info.pos.x)
    tobe_y = round(info.pos.y)

    if tobe_x == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentDestination].x and
            tobe_y == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentDestination].y then

        -- Increment the current step (maybe # can be used instead of numberSteps)
        spawningPoint.spawned[luaId].currentDestination = current + 1
        if spawningPoint.spawned[luaId].currentDestination == spawningPoint.spawned[luaId].numberSteps then
            spawningPoint.current = 0
        end
    end

end