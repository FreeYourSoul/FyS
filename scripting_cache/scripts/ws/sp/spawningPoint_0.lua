spawningPoint_0 = {
    spawned = {
        [0] = {
            idSpawningPoint = 0,
            isAlive = false,
            initial_info = { x = 10, y = 20, velocity = 1.0, angle = 0.5 },
            actions = {
                [0] = { id = 0, x = 10, y = 10 },
                [1] = { id = 0, x = 20, y = 20 },
                [2] = { id = 0, x = 30, y = 30 },
                [3] = { id = 1, x = 40, y = 40 }
            },
            backwardLoop = true,
            currentActions = 0,
            numberSteps = 4
        }

        --[1] = { isAlive = false },
        --
        --[2] = { isAlive = false },
        --
        --[3] = { isAlive = false },
        --
        --[4] = { isAlive = false }
    },
    numbers = 1,

    center_point = { x = 25, y = 25 },
    visibility_distance = 50,

    -- Inner Function can be used in order to re-arrange the script
    -- By using this function, the script is not static anymore and can change each time we spawn a monster
    spawnAtIndex = function(index)
    end
}
