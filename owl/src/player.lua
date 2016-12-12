Player = class()

function Player:init(owl, tile, x, y, direction)
    self.owl = owl
    self.tile = tile
    self.x, self.y = x, y
    self.direction = direction or 3
    self.owl.player = self
end

function Player:move(x, y)
    self.owl:setCellRedrawStatus(self.x, self.y, true)
    self.direction = (x > self.x and 2)
                     or (x < self.x and 4)
                     or (y > self.y and 3)
                     or (y < self.y and 1)
                     or self.direction
    if self.owl.area:isWalkablePosition(x, y) then
        self:setPosition(x, y)
        self.owl:setCellRedrawStatus(x, y, true)
        return true
    else
        return false
    end
end

function Player:setPosition(x, y)
    self.x, self.y = x, y
end