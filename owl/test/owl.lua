Owl = class()

function Owl:init(x, y, width, height, gc, options)
    self.x, self.y = x, y
    self.width = width
    self.height = height
    self.gc = gc or platform.gc()
    options = options or {}
    self.controls = options.controls
                    or {up = "up",
                        down = "down",
                        left = "left",
                        right = "right"}
    self.tileSize = options.tileSize or 16
    self.scrollThreshold = options.scrollThreshold or 1
    self.canScrollOutOfBounds = options.canScrollOutOfBounds or false
    self.outOfBoundsColor = options.outOfBoundsColor or {0, 0, 0}
    self.tiles = {}
    self.paths = {}
    self.updateRegion = {0, 0, 318, 212}
    self.shouldRedrawCell = {}
    for i = 1, self.height do
        self.shouldRedrawCell[i] = {}
    end
    self:setShouldRedrawEveryCell()
    function Owl:Area(...) return Area(self, ...) end
    function Owl:Tile(...) return Tile(self, ...) end
    function Owl:Player(...) return Player(self, ...) end
    function Owl:Path(...) return Path(self, ...) end
end

function Owl:setArea(area)
    self.area = area
    self:setShouldRedrawEveryCell()
end

function Owl:processEvents(isKeyPressed)
    for _, event in ipairs(self.area.events) do
        if event.x == self.player.x and event.y == self.player.y
        and event.requireKeyPress == isKeyPressed then
            if not event.direction
            or event.direction == self.player.direction then
                event.callback()
                return true
            end
        end
    end
    return false
end

function Owl:processPaths(isKeyPressed)
    for _, path in ipairs(self.paths) do
        for i = 1, 2 do
            if path.area[i] == self.area
            and path.x[i] == self.player.x and path.y[i] == self.player.y
            and path.direction[i] == self.player.direction
            and path.requireKeyPress[i] == isKeyPressed then
                local destination = i == 1 and 2 or 1
                self:setArea(path.area[destination])
                self.player:setPosition(path.x[destination], path.y[destination])
                return true
            end
        end
    end
    return false
end  
 
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
 
Area = class()

function Area:init(owl, map, x, y)
    self.owl = owl
    self.map = {data = map,
                width = #map[1],
                height = #map,
                x = x or 0, y = y or 0}
    self.objects = {}
    self.events = {}
    function Area:Object(...) return Object(self, ...) end
    function Area:Event(...) return Event(self, ...) end
end

function Area:findObject(x, y)
    for _, object in ipairs(self.objects) do
        if object.x == x and object.y == y then
            return object
        end
    end
end

function Area:setMapTile(x, y, tile)
    self.map.data[y][x] = tile.index
end

function Area:isWalkablePosition(x, y)
    local map = self.map
    if x > 0 and x <= map.width
    and y > 0 and y <= map.height
    and self.owl.tiles[map.data[y][x]].isWalkable then
        local object = self:findObject(x, y)
        return not object or self.owl.tiles[object.index].isWalkable
    else
        return false
    end
end  
 
Tile = class()

function Tile:init(owl, sprites, color, isWalkable)
    self.owl = owl
    self.sprites = sprites
    if type(sprites) == "string" then
        self.sprites = {image.new(sprites)}
    elseif type(sprites) == "table" then
        self.sprites = {}
        for i, sprite in ipairs(sprites) do
            self.sprites[i] = image.new(sprite)
        end
    end
    self.color = color
    self.isWalkable = isWalkable
    table.insert(self.owl.tiles, self)
    self.index = #self.owl.tiles
end

function Tile:destroy()
    table.remove(self.owl.tiles, self.index)
end  
 
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
 
Object = class()

function Object:init(area, tile, x, y)
    self.area = area
    self.tile = tile
    self.x, self.y = x, y
    self.currentSprite = 1
    self.isVisible = true
    table.insert(self.area.objects, self)
    self.index = #self.area.objects
end

function Object:setSprite(spriteNumber)
    self.currentSprite = spriteNumber
end

function Object:setVisibility(isVisible)
    self.isVisible = isVisible
end

function Object:setPosition(x, y)
    self.x, self.y = x, y
end

function Object:destroy()
    table.remove(self.area.objects, self.index)
end  
 
Event = class()

function Event:init(area, x, y, direction, requireKeyPress, callback)
    self.area = area
    self.x, self.y = x, y
    self.direction = direction
    self.requireKeyPress = requireKeyPress
    self.callback = callback
    table.insert(self.area.events, self)
    self.index = #self.area.events
end

function Event:destroy()
    table.remove(self.area.events, self.index)
end  
 
Path = class()

function Path:init(owl, area1, x1, y1, direction1, requireKeyPress1,
                        area2, x2, y2, direction2, requireKeyPress2)
    self.owl = owl
    self.area = {area1, area2}
    self.x, self.y = {x1, x2}, {y1, y2}
    self.direction = {direction1, direction2}
    self.requireKeyPress = {requireKeyPress1, requireKeyPress2}
    table.insert(self.owl.paths, self)
    self.index = #self.owl.paths
end

function Path:destroy()
    table.remove(self.owl.paths, self.index)
end