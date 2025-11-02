#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "func.h"

#include <locale>

int main()
{
	std::locale::global(std::locale("en_US.UTF-8"));

	char c;
	size_t x, y, ibc, tc, mov;
	Game *game = NULL;
	std::string filename;

  std::wcout << " _____                                                                             _____ " << std::endl;
  std::wcout << "( ___ )                                                                           ( ___ )" << std::endl;
  std::wcout << " |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   | " << std::endl;
  std::wcout << " |   |                                                                             |   | " << std::endl;
  std::wcout << " |   |   ___           ____  _            _      ____                _             |   | " << std::endl;
  std::wcout << " |   |  |_ _|___ ___  | __ )| | ___   ___| | __ |  _ \\ _   _ _______| | ___  ___   |   | " << std::endl;
  std::wcout << " |   |   | |/ __/ _ \\ |  _ \\| |/ _ \\ / __| |/ / | |_) | | | |_  /_  / |/ _ \\/ __|  |   | " << std::endl;
  std::wcout << " |   |   | | (_|  __/ | |_) | | (_) | (__|   <  |  __/| |_| |/ / / /| |  __/\\__ \\  |   | " << std::endl;
  std::wcout << " |   |  |___\\___\\___| |____/|_|\\___/ \\___|_|\\_\\ |_|    \\__,_/___/___|_|\\___||___/  |   | " << std::endl;
  std::wcout << " |   |                                                               version 2.0   |   | " << std::endl;
  std::wcout << " |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___| " << std::endl;
  std::wcout << "(_____)                                                                           (_____)" << std::endl;
  std::wcout << std::endl;

  std::wcout << "This program allows you to solve Ice Block Puzzles from The Legend of Zelda series." << std::endl;
  std::wcout << "These puzzles are very similar to PushPush, although there may be some differences." << std::endl;
  std::wcout << "You can read an existing puzzle from a file. You can then either solve it yourself," << std::endl;
  std::wcout << "or let the computer solve it using various algorithms. It is also possible to gene-" << std::endl;
  std::wcout << "rate new puzzles. These will of course not be of a quality as high as the original"  << std::endl;
  std::wcout << "puzzles, but sometimes a nice one may come out. There's also various options to per-" << std::endl;
  std::wcout << "form various experiments, as a leftover from the original purpose of this program:"  << std::endl;
  std::wcout << "Gaining insights in these puzzles for my Bachelor's thesis. Have fun and please be"  << std::endl;
  std::wcout << "kind for my not-perfect programming. Feel free to report bugs on Github." << std::endl;
  std::wcout << "Happy puzzling!" << std::endl;
  std::wcout << "Sem" << std::endl;
  std::wcout << std::endl;

	do
	{
		std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Menu ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

		std::wcout << "1. Generate a puzzle" << std::endl;
		std::wcout << "2. Read puzzle from file and play" << std::endl;
		std::wcout << "3. Read puzzle from file and solve using alogrithms" << std::endl;
		std::wcout << "4. Determine difficulty measure of a puzzle" << std::endl;
		std::wcout << "5. Determine the number of game states of a puzzle" << std::endl;
		std::wcout << "6. Perform experiments" << std::endl;
    std::wcout << "7. Explanation & help" << std::endl;
    std::wcout << "8. Known bugs, nice to have's, etc." << std::endl;
		std::wcout << "q. Quit" << std::endl;
		std::cin >> c;

		switch (c)
		{
		case '1': // Generate a puzzle.
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Generating puzzles ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "This mode allows you to generate new puzzles automaticcaly and to save them on disk" << std::endl;
      std::wcout << "as a txt file. Note that making a 'crowded' board may result in the program becoming" << std::endl;
      std::wcout << "stuck! So please refrain from making puzzles with tiny boards and large numbers of" << std::endl;
      std::wcout << "ice blocks. This algorithm will attempt to generate puzzles that are not easier" << std::endl;
      std::wcout << "to solve than requested (meaning: requiring less moves and ice blocks than you request)." << std::endl;
			std::wcout << "Give the desired size:" << std::endl;
			std::wcout << "X: ";
			getInput(&x);
			std::wcout << "Y: ";
			getInput(&y);
			std::wcout << "Give the maximum number of ice blocks that may be used: ";
			getInput(&ibc);
			std::wcout << "Give the number of targets to be used: ";
			getInput(&tc);
			std::wcout << "Give the maximum number of moves that may be performed to solve the puzzle: ";
			getInput(&mov);
			if (ibc < tc || ibc == 0)
			{
				std::wcout << "Error: " << ibc << " ice blocks is not enough to solve a puzzle with " << tc << " targets." << std::endl;
			}
			else
			{
				game = new Game(x, y);
				game->generate(mov, ibc, tc);
			}
			delete game;
      std::wcout << std::endl;
			break;

		case '2': // Read puzzle form file and play
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Solving puzzles manually ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "This mode allows you to solve the puzzles yourself!" << std::endl;
			std::wcout << "Filename to load puzzle: ";
			std::cin >> filename;
			try
			{
				game = new Game(filename);
				game->play();
				delete game;
			}
			catch (std::exception &e)
			{
				std::wcout << "Error while loading puzzle. Check the filename and/or file format." << std::endl;
			}
      std::wcout << std::endl;
			break;

		case '3': // Read puzzle from file and solve using algorithms
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~ Solving puzzles using algorithms ~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "This mode allows you to solve the puzzles using various (basic) algorithms." << std::endl;
      std::wcout << "Please note that these algorithms are very basic and not very smart, so solving may" << std::endl;
      std::wcout << "take a long time, or may take forever for very complex puzzles." << std::endl;
			std::wcout << "Filename to load puzzle: ";
			std::cin >> filename;
			try
			{
				game = new Game(filename);
				std::wcout << "Choose an alogrithm to solve the puzzle:" << std::endl;
				std::wcout << "1. Random" << std::endl;
				std::wcout << "2. Brute Force" << std::endl;
				std::wcout << "3. Pruned Brute Force" << std::endl;
				std::wcout << "4. Monte Carlo" << std::endl;
				getInput(&x);
				size_t moves;
				switch (x)
				{
				case 1:
					std::wcout << "Maximum number of moves allowed before giving up: ";
					getInput(&moves);
					game->solveRandom(moves, true);
					break;
				case 2:
					std::wcout << "Give the difficulty penalty for switching ice blocks. Note that BFS does not account for this when solving the puzzle." << std::endl;
					std::wcout << "However, this penalty will be used to calculate the difficulty of the puzzle: ";
					getInput(&x);
					game->solveBFS(x);
					break;
				case 3:
					std::wcout << "Give the difficulty penalty for switching ice blocks: ";
					getInput(&x);
					game->solveBFSPrune(x);
					break;
				case 4:
					size_t randomGames;
					std::wcout << "Number of random games Monte Carlo plays per move: ";
					getInput(&randomGames);
					std::wcout << "Maximum number of moves the random playout of Monte Carlo may perform: ";
					getInput(&moves);
					std::wcout << "Give the difficulty penalty for switching ice blocks. Note that Monte Carlo does not account for this when solving the puzzle." << std::endl;
					std::wcout << "However, this penalty will be used to calculate the difficulty of the puzzle: ";
					getInput(&x);
					game->solveMC(randomGames, moves, x);
					break;
				default:
					std::wcout << "Error: Invalid input." << std::endl;
					break;
				}
				delete game;
			}
			catch (std::exception &e)
			{
				std::wcout << "Error while loading puzzle. Check the filename and/or file format." << std::endl;
			}
      std::wcout << std::endl;
			break;

		case '4': // Determine difficulty measure of a puzzle
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Determining difficulty ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "In this mode, you can calculate the difficulty of a puzzle. The difficulty is calcu-" << std::endl;
      std::wcout << "lated using: D = n_moves + (n_times_moved_different_block * f) where f is a user-" << std::endl;
      std::wcout << "definable factor. The intuition behind this difficulty measure is that a puzzle be-" << std::endl;
      std::wcout << "comes harder if more moves need to be performed, but that having to change which ice" << std::endl;
      std::wcout << "block you 'currently work on' by moving a different one than in your previous move" << std::endl;
      std::wcout << "is what makes a puzzle truly hard. Consider that a puzzle where you need to move" << std::endl;
      std::wcout << "just one ice block multiple times, is usually easier than a puzzle where you need" << std::endl;
      std::wcout << "to use multiple ice blocks to set up the board such that a target can be reached." << std::endl;
      std::wcout << "The default penalty I would recommend is 5." << std::endl;
			std::wcout << "Filename to load puzzle: ";
			std::cin >> filename;
			try
			{
				std::wcout << "Give the difficulty penalty for switching ice blocks: ";
				getInput(&x);
				game = new Game(filename);
				game->solveBFSPrune(x);
				std::wcout << "The difficulty measure of this puzzle is: " << game->getDifficulty() << "." << std::endl;
				delete game;
			}
			catch (std::exception &e)
			{
				std::wcout << "Error while loading puzzle. Check the filename and/or file format." << std::endl;
			}
      std::wcout << std::endl;
			break;

		case '5': // Determine the number of game states of a puzzle
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Determining gamestates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "In this mode, you can calculate the total number of gamestates for a puzzle." << std::endl;
      std::wcout << "Please note that this will take a very long time and may take infinitely long for" << std::endl;
      std::wcout << "larger boards (6x6 being very large already) or boards with multiple ice blocks." << std::endl;
			std::wcout << "Filename to load puzzle: ";
			std::cin >> filename;
			std::wcout << "Print all game states? (Y/N) This will significantly slow down runtime!";
			std::cin >> c;
			try
			{
				game = new Game(filename);
				if (c == 'y' || c == 'Y')
					game->getStateSpace(true);
				else
					game->getStateSpace(false);
				delete game;
			}
			catch (std::exception &e)
			{
				std::wcout << "Error while loading puzzle. Check the filename and/or file format." << std::endl;
			}
      std::wcout << std::endl;
			break;

		case '6': // Perform experiments
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Performing experiments ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "In this mode, you can calculate the difficulty of each reachable game board state." << std::endl;
      std::wcout << "Note that calculating this may take a long time, especially on larger boards with" << std::endl;
      std::wcout << "a lot of ice blocks. For an explanation about difficulty, please read the infor-" << std::endl;
      std::wcout << "mation in the 'Determining difficulty' mode." << std::endl;
			std::wcout << "Filename to load puzzle: ";
			std::cin >> filename;
			try
			{
				std::wcout << "Give the difficulty penalty for switching ice blocks: ";
				getInput(&x);
				game = new Game(filename);
				std::wcout << "Experiments output filename: ";
				std::cin >> filename;
				game->solveBFSPrune(x);
				game->getAllDifficulties(filename);
				delete game;
			}
			catch (std::exception &e)
			{
				std::wcout << "Error while loading puzzle. Check the filename and/or file format." << std::endl;
			}
      std::wcout << std::endl;
			break;

    case '7': // Explanation & Help
      std::wcout << std::endl;
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Explanation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "Ice Block Puzzles are puzzles similar to PushPush, where the purpose is to place an"   << std::endl;
      std::wcout << "ice block on each target square in the game board. You can move ice blocks by sliding" << std::endl;
      std::wcout << "them in a direction. The catch is that ice blocks continue to move until obstructed"   << std::endl;
      std::wcout << "by some other element: either another ice block, a normal floor tile or a wall."       << std::endl;
      std::wcout << "Note that if you push an ice block and it slides against another ice block, it will"   << std::endl;
      std::wcout << "stop, and it will not give its momentum to the ice block it slid against."             << std::endl;
      std::wcout << std::endl;
      std::wcout << "In the original puzzles, Link would have to be able to stand next to the ice block"    << std::endl;
      std::wcout << "opposite from the direction the player wishes to move the ice block to. Link cannot"   << std::endl;
      std::wcout << "stand where an ice block or a wall is. For simplicity, I omitted the player character" << std::endl;
      std::wcout << "from this implementation, however, the game plays as if there would be a character."   << std::endl;
      std::wcout << "For simplicity's sake, we assume puzzles are surrounded by normal floors Link would"   << std::endl;
      std::wcout << "be able to stand on, unless the puzzle is explicitly surrounded by walls." << std::endl;
      std::wcout << "So, in short:" << std::endl;
      std::wcout << "1. Ice blocks slide only over ice floors and will slide over and past targets if not" << std::endl;
      std::wcout << "obstructed by another block." << std::endl;
      std::wcout << "2. Ice blocks are obstructed by either another ice block, a normal floor, or a wall." << std::endl;
      std::wcout << "3. Ice blocks do not slide over normal floors or over walls." << std::endl;
      std::wcout << "4. Ice blocks can only be pushed in a direction, if there is no wall or ice block" << std::endl;
      std::wcout << "directly next to it in the opposite direction." << std::endl;
      std::wcout << "5. You win when every target has an ice block on it." << std::endl;
      std::wcout << std::endl;
      std::wcout << "This game has the following symbols to represent puzzles:" << std::endl;
      std::wcout << " _____                                             _____" << std::endl;
      std::wcout << "( ___ )-------------------------------------------( ___ )" << std::endl;
      std::wcout << " |   |                                             |   | " << std::endl;
      std::wcout << " |   |    .   -   Ice floor                        |   | " << std::endl;
      std::wcout << L" |   |    ◼   -   Ice block                        |   | " << std::endl;
      std::wcout << L" |   |    ◻   -   Target                           |   | " << std::endl;
      std::wcout << L" |   |    ▣   -   Target with ice block on it      |   | " << std::endl;
      std::wcout << L" |   |    ▦   -   Normal floor                     |   | " << std::endl;
      std::wcout << L" |   |    ▩   -   Wall                             |   | " << std::endl;
      std::wcout << " |   |    ,   -   Path                             |   | " << std::endl;
      std::wcout << " |   |                                             |   | " << std::endl;
      std::wcout << " |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___| " << std::endl;
      std::wcout << "(_____)                                           (_____)" << std::endl;
      std::wcout << std::endl;
      std::wcout << "Note that the , for Path is only used during the generation of puzzles and is a" << std::endl;
      std::wcout << "technical solution. You should not have to worry about it during general use." << std::endl;
      std::wcout << "I am aware that the normal floor and wall character are very similar, but wchar_t's" << std::endl;
      std::wcout << "are kind of limited in terms of visual expression. I try to remember that normal floors" << std::endl;
      std::wcout << "have a grid pattern similar to a tile floor, and walls have this somewhat brick-like texture." << std::endl;
      std::wcout << "These are a bit hard to see since terminals default to quite small fonts, but if you zoom in," <<std::endl;
      std::wcout << "you should be able to tell the difference." << std::endl;
      std::wcout << std::endl;
      std::wcout << "Here's a few examples to illustrate various (im)possible moves." << std::endl;
      std::wcout << "Ice block slides past target unless obsctructed:" << std::endl;
      std::wcout << L". ■⇒. □ . will result in: . . . □ ■" << std::endl;
      std::wcout << "Because the ice block was not obstructed. If we place a wall or normal floor:" << std::endl;
      std::wcout << L". ■⇒. □ ▩ will result in: . . . ▣ ▩" << std::endl;
      std::wcout << L". ■⇒. □ ▦ will result in: . . . ▣ ▦" << std::endl;
      std::wcout << L". ■⇒. □ ◼ will result in: . . . ▣ ◼" << std::endl;
      std::wcout << L". ■⇒. □ ▣ will result in: . . . ▣ ▣" << std::endl;
      std::wcout << "These moves are impossble because the tile opposing the direction we want to slide in" << std::endl;
      std::wcout << "is obstructed:" << std::endl;
      std::wcout << L"■ ■⇏. □ ▩ and ▣ ■⇏. □ ▩ and ▩ ■⇏. □ ▩ and " << std::endl;
      std::wcout << "However, if there were a normal floor, this move would be possible:" << std::endl;
      std::wcout << L"▦ ■⇒. □ ▩ will result in: ▦ . . ▣ ▩" << std::endl;
      std::wcout << "I hope this explains everything. Good luck on your puzzle adventure!" << std::endl;
      std::wcout << std::endl;
      break;

    case '8':
      std::wcout << std::endl;
      std::wcout << "~~~~~~~~~~~~~~~~~~~~~~~~~ Known bugs, nice to have's, etc. ~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      std::wcout << "This program is not perfect. I wrote it quite some time ago and there's still a few" << std::endl;
      std::wcout << "minor bugs present. None of the are too drastic. I think this program should be" << std::endl;
      std::wcout << "free of memory leaks. Even still, there's a few things that I hope to add some time" << std::endl;
      std::wcout << "in the future. Here's a list:" << std::endl;
      std::wcout << "1. Adding smarter algorotihms for solving puzzles, as current implementations are" << std::endl;
      std::wcout << "   simplistic at best. I could look into Monte Carlo Tree Search, or solving the" << std::endl;
      std::wcout << "   puzzles backwards, similar to how I generate puzzles." << std::endl;
      std::wcout << "2. There's probably some questionable design decisions, but I don't really feel like" << std::endl;
      std::wcout << "   rewriting lots of this program. It's just for fun." << std::endl;
      std::wcout << "3. In some instances, puzzle generation becomes stuck in an infinite loop. This is" << std::endl;
      std::wcout << "   more likely to happen on crowded board, for example, if you instruct the algorithm" << std::endl;
      std::wcout << "   to create a puzzle on a 5x5 board with 10 ice blocks." << std::endl;
      std::wcout << "4. There are no bottomless pits as they are present in the Legend of Zelda series." << std::endl;
      std::wcout << "   In these games, the bottomless pit would either make the puzzle unsolvable, or" << std::endl;
      std::wcout << "   make the puzzle unsolvable. For simplicity, I chose not to implement them." << std::endl;
      std::wcout << "   walls at the edge of the board are functionally somewhat similar to botomless pits," << std::endl;
      std::wcout <<"    because the ice block cannot be pushed into the field anymore if the entire board" << std::endl;
      std::wcout <<"    is surrounded by walls, making the puzzle (probably) unsolvable." << std::endl;
      std::wcout << "5. Monte Carlo is very bad at solving puzzles, because it can get stuck in a loop" << std::endl;
      std::wcout << "   due to randomness."
      std::wcout << "If you find any bugs or have some suggestions, please create an issue on Github." << std::endl;
      std::wcout << "Or implement it yourself and create a pull request!" << std::endl;
      std::wcout << std::endl;
      break;
		case 'q':
			break;

		default:
			std::wcout << c << std::endl;
			std::wcout << "Error: Invalid input." << std::endl;
      std::wcout << std::endl;
			break;
		}
	} while (c != 'q');
	return 0;
}
