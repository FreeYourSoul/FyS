---
--- Created by FyS.
--- DateTime: 6/1/20 1:59 AM
---

function getCharacterInfo(spawningPoint, luaId)
    if luaId >= spawningPoint.numbers then
        return nil
    end
    return spawningPoint.spawn[luaId].initial_info.x, spawningPoint.spawn[luaId].initial_info.y, spawningPoint.spawn[luaId].initial_info.velocity, spawningPoint.spawn[luaId].initial_info.angle
end

function spawn(spawningPoint)
    for id, spawn in pairs(spawningPoint.spawned) do
        if spawn.shouldSpawn(spawn) == false then
            spawningPoint.spawnAtIndex(id)
            spawningPoint.spawned[id].isAlive = true
            return id
        end
    end
    return nil
end

local function _round(num)
    if num >= 0 then
        return math.floor(num + .5)
    else
        return math.ceil(num - .5)
    end
end

--- Lua Function that will execute the next action scripted for the spawn
--- @spawningPoint spawningpoint : Used to retrieve the proper spawn with the luaId
--- @luaId id of the spawn entity : this id is Unique by spawningPoint
--- @info information on the spawn : Position, and orientation of the spawn
--- @return : a tuple of 5 elements
---         1) action_id : Type of action made by the spawn (0: IDLE, 1: MOVE, 2: STOP)
---         2) new x position
---         3) new y position
---         4) new velocity
---         5) new orientation angle
function execAction(spawningPoint, luaId, info)
    -- Retrieve current action
    action = spawningPoint.spawned[luaId].getNextAction(spawningPoint.spawned[luaId])


    -- todo : Check if monster is close to a player, to try to get him, changing the

    tobe_angle = retrieveAngle(info.angle)
    tobe_velocity = info.velocity
    tobe_x = info.pos.x * (info.pos.x * math.cos(tobe_angle))
    tobe_y = info.pos.y * (info.pos.y * math.sin(tobe_angle))

    if _round(tobe_x) == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentActions].x and
            _round(tobe_y) == spawningPoint.spawned[luaId].destination[spawningPoint.spawned[luaId].currentActions].y
    then
        _changeActionsIndex(luaId, action.backwardLoop)
        return 0, tobe_x, tobe_y, tobe_velocity, tobe_angle
    end

end