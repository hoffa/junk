Pong = class()

function Pong:init(gc)
    self.gc = gc or platform.gc()
    self.player = {{position = 6,
                    score = 0},
                   {position = 6,
                    score = 0}}
    self.moveAmount = 10
    self.ball = {x = 50, y = 50,
                 dx = 1, dy = 1,
                 velocity = 1}
    self.invertColors = false
    self.twoPlayers = true
    self.lastPlayerHit = 1
    self.timePoint = 0
    self.timeOpponentMove = 0
    self.opponentSpeed = 0
    self.pointDelay = false
    self.p2ScoreX = 121
end

function Pong:canMove(player, up)
    if up then
        return self.player[player].position > 6
    else
        return self.player[player].position < 176
    end
end

function Pong:updateBall()
    if self.pointDelay then
        if timer.getMilliSecCounter() - self.timePoint < 1500 then
            return
        else
            if self.lastPlayerHit == 1 then
                self.ball.x = 50
                self.ball.y = 50
                self.ball.dx = 1
                self.ball.dy = 1
            else
                self.ball.x = 268
                self.ball.y = 50
                self.ball.dx = -1
                self.ball.dy = 1
            end
            self.ball.velocity = 1
            self.pointDelay = false
        end
    end
    if self.ball.x < -5 or self.ball.x > 318 then
        if self.ball.x < -5 then
            self.player[2].score = self.player[2].score + 1
        else
            self.player[1].score = self.player[1].score + 1
        end
        self.timePoint = timer.getMilliSecCounter()
        self.pointDelay = true
        if self.player[1].score >= self.player[2].score + 2
        and self.opponentSpeed <= 80 then
            self.opponentSpeed = self.opponentSpeed + 10
        elseif self.player[1].score + 2 <= self.player[2].score
        and self.opponentSpeed >= 10 then
            self.opponentSpeed = self.opponentSpeed - 10
        end
        self.p2ScoreX = 151 - (tostring(self.player[1].score):len() * 30)
        return
    end
    self.ball.x = self.ball.x + self.ball.dx
    self.ball.y = self.ball.y + self.ball.dy
    local hit = false
    if self.ball.x > 5 and self.ball.x < 10
    and self.ball.y + 5 >= self.player[1].position
    and self.ball.y <= self.player[1].position + 30
    and self.lastPlayerHit ~= 1 then
        hit = true
        self.lastPlayerHit = 1
    elseif self.ball.x > 303 and self.ball.x < 308
    and self.ball.y + 5 >= self.player[2].position
    and self.ball.y <= self.player[2].position + 30
    and self.lastPlayerHit ~= 2 then
        hit = true
        self.lastPlayerHit = 2
    elseif self.ball.y < 6 or self.ball.y > 201 then
        self.ball.dy = -self.ball.dy
    end
    if hit then
        local hitPosition = self.ball.y
                          - self.player[self.lastPlayerHit].position - 12
        self.ball.velocity = self.ball.velocity + 0.1
        self.ball.dx = (self.lastPlayerHit == 1 and 1 or -1)
                     * ((10 + (hitPosition < 0 and -hitPosition or hitPosition)) / 15)
                     * self.ball.velocity
        self.ball.dy = (hitPosition / 15)
                     * self.ball.velocity
    end
end

function Pong:moveOpponent()
    if not self.twoPlayers and timer.getMilliSecCounter() - self.timeOpponentMove > 110 - self.opponentSpeed then
        local deltaMove = 0
        if self.ball.y < self.player[2].position + 10 and self:canMove(2, true) then
            deltaMove = -self.moveAmount
        elseif self.ball.y > self.player[2].position + 30 and self:canMove(2, false) then
            deltaMove = self.moveAmount
        end
        self.player[2].position = self.player[2].position + deltaMove
        self.timeOpponentMove = timer.getMilliSecCounter()
    end
end

function Pong:drawDigit(digit, x, y)
    local segments = {{1, 1, 1, 0, 1, 1, 1},
                      {0, 0, 1, 0, 0, 1, 0},
                      {1, 0, 1, 1, 1, 0, 1},
                      {1, 0, 1, 1, 0, 1, 1},
                      {0, 1, 1, 1, 0, 1, 0},
                      {1, 1, 0, 1, 0, 1, 1},
                      {1, 1, 0, 1, 1, 1, 1},
                      {1, 0, 1, 0, 0, 1, 0},
                      {1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 0, 1, 1}}
    for i = 1, 7 do
        if segments[digit + 1][i] ~= 0 then
            if i == 1 then
                self.gc:fillRect(x, y, 25, 5)
            elseif i == 2 then
                self.gc:fillRect(x, y, 5, 25)
            elseif i == 3 then
                self.gc:fillRect(x + 20, y, 5, 25)
            elseif i == 4 then
                self.gc:fillRect(x, y + 20, 25, 5)
            elseif i == 5 then
                self.gc:fillRect(x, y + 25, 5, 25)
            elseif i == 6 then
                self.gc:fillRect(x + 20, y + 25, 5, 25)
            elseif i == 7 then
                self.gc:fillRect(x, y + 45, 25, 5)
            end
        end
    end
end

function Pong:drawNumber(number, x, y)
    local digits = tostring(number)
    for i = 1, #digits do
        self:drawDigit(tonumber(digits:sub(i, i)), (30 * (i - 1)) + x, y)
    end
end

function Pong:draw()
    if self.invertColors then
        self.gc:setColorRGB(0, 0, 0)
    else
        self.gc:setColorRGB(0, 0, 0)
        self.gc:fillRect(0, 0, 318, 212)
        self.gc:setColorRGB(255, 255, 255)
    end
    self.gc:setPen("thick", "dotted")
    self.gc:drawLine(159, 0, 159, 212)
    self.gc:fillRect(0, 0, 318, 5)
    self.gc:fillRect(0, 207, 318, 5)
    self.gc:fillRect(5, self.player[1].position, 5, 30)
    self.gc:fillRect(308, self.player[2].position, 5, 30)
    self.gc:fillRect(self.ball.x, self.ball.y, 5, 5)
    self:drawNumber(self.player[1].score, self.p2ScoreX, 16)
    self:drawNumber(self.player[2].score, 173, 16)
end

function Pong:charIn(c)
    if c == "1" and self:canMove(1, true) then
        self.player[1].position = self.player[1].position - self.moveAmount
    elseif c == "0" and self:canMove(1, false) then
        self.player[1].position = self.player[1].position + self.moveAmount
    elseif c == "3" and self:canMove(2, true) then
        self.player[2].position = self.player[2].position - self.moveAmount
    elseif c:byte() == 226 and self:canMove(2, false) then
        self.player[2].position = self.player[2].position + self.moveAmount
    elseif c == "7" then
        self.player[1].position = 6
    elseif c == "4" then
        self.player[1].position = 176
    elseif c == "9" then
        self.player[2].position = 6
    elseif c == "6" then
        self.player[2].position = 176
    elseif c == "i" then
        self.invertColors = not self.invertColors
    elseif c == "r" then
        self:init()
    elseif c == "8" then
        self.twoPlayers = not self.twoPlayers
    end
end

function on.charIn(c)
    pong:charIn(c)
end

function on.create()
    timer.start(0.01)
end

function on.timer()
    platform.window:invalidate()
end

local initialized = false
function on.paint(gc)
    if not initialized then
        pong = Pong()
        initialized = true
    end
    pong:draw()
    pong:updateBall()
    pong:moveOpponent()
end