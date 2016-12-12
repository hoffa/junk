Owl = class()

function Owl:init(x, y, width, height, gc, options)
    self.x, self.y = x, y
    self.width = width
    self.height = height
    self.gc = gc or platform.gc()
    options = options or {}
    self.controls = options.controls
                    or {up = "up", down = "down", left = "left", right = "right"}
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