function Owl:handleMovementKey(key)
    local direction = (key == self.controls.up and 1)
                      or (key == self.controls.right and 2)
                      or (key == self.controls.down and 3)
                      or (key == self.controls.left and 4)
    if direction == self.player.direction then
        if self:processPaths(false) then
            return
        end
        local x = self.player.x + ((direction == 2 and 1) or (direction == 4 and -1) or 0)
        local y = self.player.y + ((direction == 3 and 1) or (direction == 1 and -1) or 0)
        if self.player:move(x, y) then
            self:processEvents(false)
            if self.area.map.width > self.width
            and self.area.map.height > self.height then
                self:scrollMap()
            end
        end
    else
        self.player.direction = direction
    end
end

function Owl:handleEventKey()
    self:processEvents(true)
    self:processPaths(true)
end