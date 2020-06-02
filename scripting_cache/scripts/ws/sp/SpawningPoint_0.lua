spawningPoint_0 = {
    current = 0,
    spawned = {
        [0] = {
            idSpawningPoint = 0,
            isAlive = false,
            info = { x = 10, y = 10, velocity = 1, angle = 0.0 },
            destinations = {
                [0] = { x = 10, y = 10 },
                [1] = { x = 20, y = 20 },
                [2] = { x = 30, y = 30 },
                [3] = { x = 40, y = 40 }
            },
            backwardLoop = true,
            numberSteps = 4
        },

        [1] = { isAlive = false },

        [2] = { isAlive = false },

        [3] = { isAlive = false },

        [4] = { isAlive = false }
    },
    numbers = 5,

    -- Inner Function can be used in order to re-arrange the script
    -- By using this function, the script is not static anymore and can change each time we spawn a monster
    spawnAtIndex = function(index) end
}

