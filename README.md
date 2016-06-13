BitChess
===========================


BitChess is a (very simple) command-line chess engine written in C.

It is pretty much feature-complete (I think), relatively smart, and not all too slow. It's name is inspired by all the software that starts with 'bit' nowadays (Bitcoin, Bitbucket, Bit.ly, BIT.TRIP RUNNER) and should be pronounced as 'bitches'. Players of the game are to be referred to as '*playa's*'.

[![Build Status](https://travis-ci.org/mlagerberg/BitChess.svg?branch=master)](https://travis-ci.org/mlagerberg/BitChess)

## Screenshots

Starting a new game ang making a first move:

![Starting a game. Virtual coin toss decides who plays white.](http://i.imgur.com/7mLKXNN.gif)

And here is a screenshot of it running on a Raspberry Pi:
![Making a move](https://i.imgur.com/epdcOpj.png)


## The chess engine

You can run the chess engine through the command line, like so:

		usage:       chess [-s,-m,-x] <command>
		commands and options:
		 <move>       Make a move in an ongoing game. The computer player will respond
					  with a move. The move should be in a algebraic notation like so:
					  'Ndxe5+' (no quotes). If the move is a pawn promotion, an extra
					  capital letter must denote what piece to promote to, e.g: e8=Q.
					  Castling can be performed by 'O-O' or 'O-O-O' (capital 'oh', not
					  zeros).
		 new          Starts a new game, where a virtual coin toss determines who plays 
					  white. The game files are stored in ~/.Bitchess/
		 print        Shows the current board position.
		 reset        Restarts an ongoing game.
		 switch       Switches sides in an ongoing game. The computer player will make a
		 			  move.
		 backup <n>   Backs up the current game to a file with the specified number <n>.
		 			  Can be restored using the restore command.
		 restore <n>  Restores a backed-up game from the file with the given number <n>.
		 			  Requires a <n>.game file to be present.
		-s            Silences output to only critical messages, such as moves.
		-m            Uses simple move notation. When used, moves must be written like
	            	  so: 'd7-d5'. No marks for pieces, captures or check are used.
	            	  For pawn promotions, append an extra capital letter: 'e7-e4Q'.
	            	  Automatically also enables silent mode, i.e.: -s.
		-x            Performs the move without having the AI player do a counter move.
		-h --help     Shows this help message.
		-v --version  Shows the application version.



## Status of the project

I'd say 98% finished regarding game correctness. The following should be added for it to allow an actual, rules-are-rules, game of chess:

- allow the user to declare a draw,
- respect the 50-move rule,
- respect a chess clock.

For performance, I'd really like to see the following added as well:
- improve alpha-beta pruning to not only cut off the search in the current chunk, but in other chunks as well (the set of possible moves is split into a few chunks which are searched and evaluated in parallel)
- use a opening book at the start,
- use end-game tables or otherwise improve the end game (it is *terrible* at the moment),
- improve and add unit tests.


## Building

Simply run `make` to build on Linux, OS X or Windows (assuming you have the gcc build tools installed[1]). Note that that the Windows version isn't tested as often as the Linux version, and doesn't support multi-threading. Run `make test` to build an run some very basic unit tests. `make debug` for creating a verbose debug build.

[1]: On Linux: `sudo apt-get install gcc`, on Windows: http://www.mingw.org/, on Mac: install XCode.


## Why this project?

I'm aware that there are plenty of open source chess engines available that are much, much better than I'll ever be able to create. However, creating my own chess engine is something I've been wanting to do for a long time.

This project is solely meant as a hobby project for myself, and might therefore contain some questionable design choices that would not have been made if it was targeted towards the general public. This does not mean that I'm the only one to ever find use for this project, though I'm willing to take that risk.



## Licenses

This project is released under the Apache License 2.0 (included).
