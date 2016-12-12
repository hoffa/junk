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
    if x > 0 and x <= self.map.width and y > 0 and y <= self.map.height
    and self.owl.tiles[self.map.data[y][x]].isWalkable then
        local object = self:findObject(x, y)
        return not object or self.owl.tiles[object.index].isWalkable
    else
        return false
    end
end