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