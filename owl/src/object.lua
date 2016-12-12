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