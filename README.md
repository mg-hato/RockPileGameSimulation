# RockPileGameSimulation
This project represents game simulation done via console of "rock pile" game.

## Rock pile game
This is a very simple game that can be somewhat of an example of dynamic programming. The rules are simple:
there are 2 players, and there is a pile of N rocks at start. In player's turn, 1, 2 or 3 rocks can be taken from the pile
(assuming there's enough rocks), after which the other player's turn starts. This is repeated until a player is left with empty
pile at the start of their turn, in which case, they lose.

This program is simulating that game between console-user (player in further text) and the computer.
The computer is set to play optimally(and mercilessly, so it will not miss chance to win if given one).
The player can choose who starts first, what is the number N (max value of N is set to 100) and it can interact
with the program to control Forbidden Divisibilities (FD in furhter text).

## FD?
Since the Rock pile game is simple, on some contest (I forgot when), there was a version of this game that introduced another
losing scenario. Given a list, labelled, Player's Forbidden Divisibilities (PFD), player loses if at the end of their turn,
number of left rocks on the pile is divisible by any number from PFD. Similar list is introduced for the computer.

When both of these lists are empty, the game played will be the simple rock pile game, where predicting who will win (based on N)
requires doing just the modulo by 4. If that is not the case, a neat programming solution exists (which is used in computer's
strategy).
