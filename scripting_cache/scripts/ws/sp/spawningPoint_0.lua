---
--- Created by FyS.
--- DateTime: 19/11/20 1:59 AM
---
----- Testing File

spawningPoint_0 = {
    idSpawningPoint = 0,
    spawned = {
        [0] = {
            isAlive = false,

            -- MANDATORY PUBLIC
            shouldSpawn = function(this)
                return this.isAlive
            end,

            getNextAction = function(this)
                action_result = this.actions[this.currentActions]
                if (action.id > 0)
                then
                    this.currentActions = this.currentActions + 1
                    if this.currentActions == this.numberSteps then
                        this.currentActions = 0
                    end
                    action_result = this.action[this.currentActions]
                end
                return action_result
            end,

            -- PRIVATE

            initial_info = { x = 10, y = 20, velocity = 1.0, angle = 0.5 },
            actions = {
                [0] = { id = 1, x = 10, y = 10 },
                [1] = { id = 1, x = 20, y = 20 },
                [2] = { id = 1, x = 30, y = 30 },
                [3] = { id = 1, x = 40, y = 40 }
            },
            backwardLoop = true,
            currentActions = 0,
            numberSteps = 4
        }

    },
    numbers = 1,

    center_point = { x = 25, y = 25 },
    visibility_distance = 50,

    --- Inner Function can be used in order to re-arrange the script
    --- By using this function, the script is not static anymore and can change each time we spawn a monster
    spawnAtIndex = function(index)
    end
}

