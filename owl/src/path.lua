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