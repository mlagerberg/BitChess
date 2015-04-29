#!/usr/bin/python
#####################################################
#
# This script plays to instances of the chess
# engine against eachother for a fixed number
# of games and moves.
# This can be used to compare the influence of
# engine changes on performance, both in strength
# and speed, by making the updated engine play
# against the previous version.
#
#####################################################

import subprocess
import time
import platform

if platform.system() == 'Windows':
    NEW_ENGINE = './chess.exe'
    OLD_ENGINE = './chess.0.1.2.exe'
else:
    NEW_ENGINE = './chess'
    OLD_ENGINE = './chess'

MAX_MOVES = 10
MAX_GAMES = 10
OLD = False
NEW = True
BLACK = False
WHITE = True


##
# Takes one argument or command to send
# to the C chess engine, and executes the chess
# process with that argument.
def chess_command(engine, command):
    exe = NEW_ENGINE if engine else OLD_ENGINE
    out = subprocess.check_output([exe, '-m', command])
    #print "`{} -m {}`:\t".format(exe, command), out
    return out


def do_move(engine):
    out = chess_command(engine, "switch")
    if "#" in out:
        print "Game has finished"
        return True
    else:
        return False


##
# Evaluates the board position using the new engine
def evaluate():
    value = 0
    try:
        out = chess_command(True, "evaluate")
        value = int(out)
    except ValueError:
        print "Unable to evaluate"
    return value


##
# A single game loop. Makes two engines fight a single game.
def loop(startEngine):
    clockOld = 0
    clockNew = 0
    count = 0
    engine = startEngine
    if engine == NEW:
        print "New engine plays white"
    else:
        print "Old engine plays white"

    # Start game with command 'black' to make the AI start
    print "1."
    chess_command(engine, "black")

    while count < MAX_MOVES * 2 - 1:
        count = count + 1
        if count % 2 == 0:
            print "{}.".format((count/2)+1)
        engine = not engine
        time_before = time.time()
        # Make the engine do a move
        result = do_move(engine)
        time_after = time.time()
        diff = time_after - time_before
        if engine:
            clockNew = clockNew + diff
        else:
            clockOld = clockOld + diff

        if result:
            print "Game has ended!"
            break

    value = evaluate()
    if value == 0:
        print "Board evaluation: it's a tie."
    else:
        # True = WHITE, False = BLACK
        winningEngine = (value > 0)
        print "Board evaluation: {}; {} engine is ahead".format(value, "new" if winningEngine == startEngine else "old")
    print "Time elapsed: {} vs. {} seconds; {} engine is faster.".format(round(clockNew, 2), round(clockOld, 2), "new" if clockNew < clockOld else "old")
    return (clockNew, clockOld, value)


##
# Tests multiple games, evenly balancing who get's to start
def test():
    totalClockNew = 0
    totalClockOld = 0
    scoreNew = 0
    gamesWonNew = 0
    gamesTie = 0
    games = 0
    # Do a number of games
    for i in xrange(0, MAX_GAMES):
        print "=== Game {} ===".format(i+1)
        startEngine = NEW if i % 2 == 0 else OLD
        try:
            clockNew, clockOld, score = loop(startEngine)
        except Exception as e:
            print e.message
            break
        totalClockNew = totalClockNew + clockNew
        totalClockOld = totalClockOld + clockOld
        if score == 0:
            gamesTie = gamesTie + 1
        if startEngine == NEW:
            scoreNew = scoreNew + score
            if score > 0:
                gamesWonNew = gamesWonNew + 1
        else:
            scoreNew = scoreNew - score
            if score < 0:
                gamesWonNew = gamesWonNew + 1
        games = games + 1
    # Calculate average
    totalClockNew = totalClockNew / games
    totalClockOld = totalClockOld / games
    scoreNew = scoreNew / games
    # Show
    print "======"
    print "Games won: {} vs {}; {} engine is best".format(gamesWonNew, games - gamesWonNew - gamesTie, "new" if scoreNew > 0 else "old")
    print "Average score: {}".format(round(scoreNew, 2))
    print "Average time: {} vs. {} seconds; {} engine is faster.".format(round(totalClockNew, 2), round(totalClockOld, 2), "new" if clockNew < clockOld else "old")


# Go!
test()
