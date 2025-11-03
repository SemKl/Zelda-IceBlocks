# The Legend of Zelda: Ice Block Puzzles
My Bachelor Thesis: a program which allows users to play Ice Block puzzles from The Legend of Zelda series (very similar to PushPush). This program also implements some basic algorithms to solve these puzzles, as well as an algorithm that can generate new puzzles.

## Introduction
This project is a further development of a program I wrote for my Bachelor's thesis in 2019-2020, where I researched the complexity and solvability of Ice Block Puzzles (which are similar to PushPush). I revised this code with my renewed knowledge and improved skills in 2024, and by the end of 2025 I have finally decided to release it. This code is still by no means perfect, but it works well enough. If you are interested in my research, I refer you to the PDF which is also available in this repo.

## Rules
Ice block puzzles as seen in The Legend of Zelda have a simple set of rules. Of course, in the original game, the player takes control of Link, who has to push the ice blocks around. These puzzles can, however, be simplified to a grid of m by n tiles. Each tile can be one of several elements:

1. Ice blocks. These can be pushed around, and will slide over ice floors, not stopping until they hit an obstacle, such as walls, "normal" floors and other ice blocks. To push ice blocks, Link would need to stand in the tile opposite to the direction he wishes to push the ice block to. I account for this limitation, even though Link is not in this program.
2. Ice Floors. Ice blocks continue to slide over ice floors until they hit an obstacle. Link can stand on ice floors. Note that Link does not slide!
3. Walls. One of the obstacles. Our hypothetical Link cannot stand here, hence he cannot push ice blocks from this location.
4. Normal floors. Also one of the obstacles. Functionally the same as walls, but Link can stand here, so he can push ice blocks from this location.
5. Targets. These are the tiles that Ice Blocks need to be pushed on. Note that Ice Blocks will slide over targets unless an obstacle next to the target obstructs the sliding path!

Note that I assume that every puzzle is surrounded by normal floors. A puzzle with explicit normal floors around it is therefore somewhat redundant, although maybe a bit more clear.

It should also be noted that one element is not present, even though it occurs in some of the games: bottomless pits. In some Zelda titles, there are bottomless pits in which ice blocks could be pushed, which would either reset the puzzle or make it unsolvable (until you re-enter the room). I have not implemented bottomless pits, but in the games they are usually functionally similar to walls, which also have the ability to make puzzles unsolvable.

Because Ice Blocks can also act as obstacles, they are used to set up the board to reach the target with other ice blocks in more complex puzzles. This aspect is one of the most interesting properties of this puzzle.

I've been told they are also similar to puzzles found in some Ice-themed Gyms in Pokémon, with the main difference being that there, the main character is the sliding element, whereas here, Link is not the sliding object.

The rules are explained in further detail with visual examples in my thesis, as well as in the program.

## Features
This program has various features, some of them are left in from my research.

1. Playing puzzles. You can load in puzzles from a txt file and solve them yourself.
2. Solving puzzles using simple "algorithms". You can also load in puzzles and have a simple algorithm solve the puzzle.
3. Generating puzzles. This program can also generate new puzzles and write them as txt files to disk.
4. Determining state space. You can load in puzzles and determine the number of reachable game states.
5. Determining difficulty. Difficulty is a measure that I defined as m number of moves and n number of times the player needs to switch using ice blocks. The intuition is that pushing one single ice block 10 times is easier than needing to set up ice blocks in a specific way to reach the target. For more details, I refer to my paper.
6. Performing experiments. These functions were mostly useful for my thesis, to determine reachable state diffulty and the likes. I've left them in, in case they are needed.
7. A seperate program is also included, which is implemented in latexgen.cc. This program allows users to generate Tikzpicture LaTeX images using txt files.

## The txt files
This repo includes, as far as I know, every Ice Block Puzzle from the Legend of Zelda series so far. At least, the ones that are meaningful for this program and that can be solved with this program. I am aware there is a small one in Echoes of Wisdom, but that one seems more timing-based than puzzle-based.

You can of course also create your own puzzles manually. Txt files represent tiles from the puzzles using simple characters. The first line of the txt file is reserved for the dimensions of the board: X Y

- `.` = ice floor
- `O` = ice block
- `T` = target
- `H` = target with ice block on top
- `W` = wall
- `X` = normal floor
- `,` = PATH element - only used for puzzle generation algorithm

## The implemented algorithms
The algorithms I implemented back in the day are Breadth-First Search, Pruned Breadth-First Searched which will take game difficulty in consideration, and Monte Carlo. Of course, these algorithms are very basic and do not solve the puzzles in a strategic way. However, as you may find for yourself, it is quite difficult to find an algorithm that solves these puzzles in a smarter way. In short, there does not seem to be a heuristic denoting whether an algorithm is on track to solve a puzzle. Even if there are strategies available to solve some puzzles, those do not apply to all puzzles. The paper dives somewhat deeper into this.

I invite you to think of a smarter algorithm. If you do, please let me know, or create a pull request.

## Generating puzzles
Puzzle generation is done backwards. In other words, the puzzle generation starts in a potential end state with an ice block on a target, and then the puzzle is "undone" in random ways, placing obstacles where needed. As you may suspect, it is very likely that the final puzzle has a solution that is much easier than anticipated with this method, which is why I keep track of how the puzzle was "undone". Afterwards, the puzzle is solved using BFS. This way, the puzzle generation algorithm can determine whether any unintended moves were performed, and it will then try to place obstacles in such a way that unintended solutions become impossible. Of course, the algorithm will (try to) ensure that the puzzle remains solvable at all times, even if it is easier than intended.

## Running the program
You can download this repo or clone it. You can compile all files and programs using `make`. By default, everything is compiled using `-O3`. You can execute `./iceblocks` to run the main program, and `./latexgen` to run the utility program to generate Tikzpictures for LaTeX. To clean up (to recompile), you can simply use `make clean`.

This program uses wchars in order to show more diverse characters as a representation for the puzzles. It is possible you need to change your terminal's configuration in order to display these characters.

## Special thanks
I would like to give my special thanks to my supervisors and especially to Walter Kosters, with whom I have had a lot of friendly discussions about these puzzles (and other important things in life).

## Legal Disclaimer
This project is a non-commercial work based on The Legend of Zelda series by Nintendo.
All trademarks, characters, story elements, and other intellectual property related to The Legend of Zelda are the property of Nintendo Co., Ltd.