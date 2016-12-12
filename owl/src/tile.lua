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