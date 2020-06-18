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

function changeActionsIndex(luaId, backwardLooping)
    spawningPoint.spawned[luaId].currentActions = spawningPoint.spawned[luaId].currentActions + 1
    if spawningPoint.spawned[luaId].currentActions == spawningPoint.spawned[luaId].numberSteps then
        spawningPoint.current = 0
    end
end

function execAction(spawningPoint, luaId, info)
    -- Retrieve current action
    action = spawningPoint.spawned[luaId].actions.spawningPoint.spawned[luaId].currentActions

    if (action.id > 0)
    then
        changeActionsIndex(luaId, action.backwardLoop)
        return action.id, .0, .0, 0, 0
    end

    -- todo : Check if monster is close to a player, to try to get him, changing the

    tobe_angle = retrieveAngle(info.angle)
    tobe_velocity = info.velocity
    tobe_x = info.pos.x * (info.pos.x * math.cos(tobe_angle))
    tobe_y = info.pos.y * (info.pos.y * math.sin(tobe_angle))

    if round(tobe_x) == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentActions].x and
            round(tobe_y) == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentActions].y
    then
        changeActionsIndex(luaId, action.backwardLoop)
        return 0, tobe_x, tobe_y, tobe_velocity, tobe_angle
    end

end