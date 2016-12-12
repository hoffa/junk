function Owl:drawTileMap()
    local x, y = self.x, self.y
    for i = self.area.map.y + 1, self.area.map.y + self.height do
        for j = self.area.map.x + 1, self.area.map.x + self.width do
            if self:getCellRedrawStatus(j, i) then
                if i > 0 and i <= self.area.map.height
                and j > 0 and j <= self.area.map.width
                and self.area.map.data[i][j] > 0 then
                    local tile = self.tiles[self.area.map.data[i][j]]
                    if tile.color then
                        self.gc:setColorRGB(unpack(tile.color))
                        self.gc:fillRect(x, y, self.tileSize, self.tileSize)
                    end
                    if tile.sprites then
                        self.gc:drawImage(tile.sprites[1], x, y)
                    end
                elseif self.outOfBoundsColor then
                    self.gc:setColorRGB(unpack(self.outOfBoundsColor))
                    self.gc:fillRect(x, y, self.tileSize, self.tileSize)
                end
                self:setCellRedrawStatus(j, i, false)
            end
            x = x + self.tileSize
        end
        x = self.x
        y = y + self.tileSize
    end
end

function Owl:drawObjects()
    for _, object in ipairs(self.area.objects) do
        if object.isVisible
        and object.x > self.area.map.x
        and object.x <= self.area.map.x + self.width
        and object.y > self.area.map.y
        and object.y <= self.area.map.y + self.height then
            self.gc:drawImage(self.tiles[object.tile.index].sprites[object.currentSprite],
                              self:tileToScreenPosition(object.x, object.y))
        end
    end
end

function Owl:drawPlayer()
    local player = self.player
    self.gc:drawImage(self.tiles[player.tile.index].sprites[player.direction],
                      self:tileToScreenPosition(player.x, player.y))
end

function Owl:drawAll()
    self:drawTileMap()
    self:drawObjects()
    self:drawPlayer()
end

function Owl:tileToScreenPosition(x, y)
    return ((x - self.area.map.x - 1) * self.tileSize) + self.x,
           ((y - self.area.map.y - 1) * self.tileSize) + self.y
end

function Owl:getCellRedrawStatus(x, y)
    return self.shouldRedrawCell[y - self.area.map.y][x - self.area.map.x]
end

function Owl:setCellRedrawStatus(x, y, shouldRedrawCell)
    self.shouldRedrawCell[y - self.area.map.y][x - self.area.map.x] = shouldRedrawCell
end

function Owl:setShouldRedrawEveryCell()
    for i = 1, self.height do
        for j = 1, self.width do
            self.shouldRedrawCell[i][j] = true
        end
    end
end

function Owl:scrollMap()
    local map = self.area.map
    local oldMapX, oldMapY = map.x, map.y
    map.x = map.x + ((self.player.x <= map.x + self.scrollThreshold and -1)
                    or (self.player.x + self.scrollThreshold > map.x + self.width and 1)
                    or 0)
    map.y = map.y + ((self.player.y <= map.y + self.scrollThreshold and -1)
                    or (self.player.y + self.scrollThreshold > map.y + self.height and 1)
                    or 0)
    if not self.canScrollOutOfBounds then
        map.x = (map.x < 0 and 0)
                or (map.x + self.width > map.width and map.width - self.width)
                or map.x
        map.y = (map.y < 0 and 0)
                or (map.y + self.height > map.height and map.height - self.height)
                or map.y
    end
    if map.x ~= oldMapX or map.y ~= oldMapY then
        self:setShouldRedrawEveryCell()
    end
end

function Owl:updateScreen()
    platform.window:invalidate(unpack(self.updateRegion))
end