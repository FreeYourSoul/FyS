CurrentSpawningPointId_0 = 0;

SpawningPoint_0 = { idSpawningPoint = 0, backwardLoop = true, destinations = {}, current = 0, numberSteps = 0 }

function SpawningPoint_0:create(o)
    o = o or {}
    o.idSpawningPoint = CurrentSpawningPointId_0
    o.current = 0
    if CurrentSpawningPointId_0 == 0 then
        backwardLoop = true;
        o.destinations[0] = { x = 10, y = 10 }
        o.destinations[1] = { x = 20, y = 20 }
        o.destinations[2] = { x = 30, y = 30 }
        o.destinations[3] = { x = 40, y = 40 }
        o.numberSteps = 4
    end
    CurrentSpawningPointId_0 = CurrentSpawningPointId_0 + 1
    setmetatable(o, self)
    self.__index = self
    return o
end

function SpawningPoint_0:exec()


    -- Increment the current step
    self.current = current + 1
    if self.current == self.numberSteps then
        self.current = 0
    end
end