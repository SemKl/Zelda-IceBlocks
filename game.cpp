#include <unordered_set>
#include <list>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <climits>

#include "game.h"
#include "func.h"

Game::Game(size_t x, size_t y)
{
	tree = NULL;
	board = new Board(x, y);
	this->x = x;
	this->y = y;
}

Game::Game(const std::string& filename)
{
	tree = NULL;
	board = new Board(filename);
	iceBlockCount = board->getIceBlockCount();
	this->x = board->getX();
	this->y = board->getY();
	std::wcout << "Loaded puzzle:" << std::endl;
	board->printBoard();
}

Game::~Game()
{
	delete board;
	delete tree;
}

void Game::printMove(blockmove move) const
{
	std::wcout << "Block: " << move.first << ". Move: ";
	switch (move.second)
	{
	case Direction::LEFT:
		std::wcout << "left.";
		break;
	case Direction::RIGHT:
		std::wcout << "right.";
		break;
	case Direction::UP:
		std::wcout << "up.";
		break;
	case Direction::DOWN:
		std::wcout << "down.";
		break;
	}
	std::wcout << std::endl
			   << std::endl;
}

bool Game::checkDone() const
{
	return (board->getHitTargetCount() == board->getTargetCount());
}

void Game::play() const
{
	size_t x, y;
	char c;
	coordinate co;
	Direction d;
	while (!checkDone())
	{
		do
		{
			board->printBoard();
			std::wcout << "Number of targets on which an ice block has been pushed: " << board->getHitTargetCount() << std::endl;
			std::wcout << "Coordinates of the block you want to move:" << std::endl;
			std::wcout << "X: ";
			getInput(&x);
			std::wcout << "Y: ";
			getInput(&y);
			co = {x, y};
			std::wcout << "Direction to move the block in:" << std::endl;
			std::wcout << "W - Up" << std::endl;
			std::wcout << "A - Left" << std::endl;
			std::wcout << "S - Down" << std::endl;
			std::wcout << "D - Right" << std::endl;
			std::cin >> c;
			switch (c)
			{
			case 'A':
			case 'a':
				d = Direction::LEFT;
				break;
			case 'D':
			case 'd':
				d = Direction::RIGHT;
				break;
			case 'W':
			case 'w':
				d = Direction::UP;
				break;
			case 'S':
			case 's':
				d = Direction::DOWN;
				break;
			default:
				d = (Direction)-1;
				std::wcout << "Invalid direction." << std::endl;
				break;
			}
		} while ((d != Direction::LEFT && d != Direction::RIGHT && d != Direction::UP && d != Direction::DOWN) || !board->moveBlock(co, d, true));
	}
	board->printBoard();
}

void Game::generate(size_t dif, size_t ibc, size_t tc)
{
	generateBoard(dif, ibc, tc);

	this->iceBlockCount = board->getIceBlockCount();

	board->initialize();
	board->reanalyse();

	std::wcout << "Save this puzzle? (Y/N): ";
	char c;
	coordinate co;
	std::string filename;
	do
	{
		std::cin >> c;
	} while (c != 'y' && c != 'Y' && c != 'n' && c != 'N');
	switch (c)
	{
	case 'y':
	case 'Y':
		std::wcout << "Output filename: ";
		std::cin >> filename;
		std::ofstream output;
		output.open(filename);
		output << this->x << ' ' << this->y << std::endl;
		for (size_t i = 0; i < this->y; i++)
		{
			for (size_t j = 0; j < this->x; j++)
			{
				co = {i, j};
				switch (board->getBlock(co).getElement())
				{
				case Element::EMPTY:
				case Element::PATH:
					output << '.'; // We do not want to use the PATH in the saved file
					break;
				case Element::TARGET:
					output << 'T';
					break;
				case Element::HITTARGET:
					output << 'H';
					break;
				case Element::FLOOR:
					output << 'X';
					break;
				case Element::ICEBLOCK:
					output << 'O';
					break;
				case Element::WALL:
					output << 'W';
					break;
				}
			}
			output << std::endl;
		}
	}
}



bool Game::placeRandomFloor(size_t move)
{
	size_t block = solutionmoves[move].first;
	coordinate blockco = solution[move].getIceBlock(block);
	std::vector<coordinate> candidates;
	coordinate potentialFloor;
	switch (solutionmoves[move].second)
	{
	// For all directions...
	case Direction::RIGHT:
		{
			coordinate right = {blockco.x + 1, blockco.y};
			// ... look in that direction until you encounter either a floor, wall, iceblock or target with an ice block on it.
			while (blockco.x + 1 < this->x && board->fieldIsSlidable(right))
			{
				// If there is no obsctructing block AND no ice block has slid over this space during the intended solution,
				// we can add a floor tile to prevent the block from sliding over it to prevent easier solutions.
				if (board->getBlock(right).getElement() == Element::EMPTY)
				{
					potentialFloor = right;
					// If this coordinate is not forbidden, we add it to the candidates where we can place a floor
					if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), potentialFloor) == forbiddenCoordinates.end())
					{
						candidates.push_back(potentialFloor);
					}
				}
				blockco.x++;
				right = {blockco.x + 1, blockco.y};
			}
			break;
		}
	case Direction::LEFT:
		{
			coordinate left = {blockco.x - 1, blockco.y};
			while (blockco.x > 0 && board->fieldIsSlidable(left))
			{
				if (board->getBlock(left).getElement() == Element::EMPTY)
				{
					potentialFloor = left;
					if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), potentialFloor) == forbiddenCoordinates.end())
					{
						candidates.push_back(potentialFloor);
					}
				}
				blockco.x--;
				left = {blockco.x - 1, blockco.y};
			}
			break;
		}
	case Direction::DOWN:
		{
			coordinate down = {blockco.x, blockco.y + 1};
			while (blockco.y + 1 < this->y && board->fieldIsSlidable(down))
			{
				if (board->getBlock(down).getElement() == Element::EMPTY)
				{
					potentialFloor = down;
					if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), potentialFloor) == forbiddenCoordinates.end())
					{
						candidates.push_back(potentialFloor);
					}
				}
				blockco.y++;
				down = {blockco.x, blockco.y + 1};
			}
			break;
		}
	case Direction::UP:
		{
			coordinate up = {blockco.x, blockco.y - 1};
			while (blockco.y > 0 && board->fieldIsSlidable(up))
			{
				if (board->getBlock(up).getElement() == Element::EMPTY)
				{
					potentialFloor = up;
					if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), potentialFloor) == forbiddenCoordinates.end())
					{
						candidates.push_back(potentialFloor);
					}
				}
				blockco.y--;
				up = {blockco.x, blockco.y - 1};
			}
			break;
		}		
	}
	std::wcout << "Candidates: " << std::endl;
	if (candidates.size() == 0){
		std::wcout << "No candidate tiles where floors can be placed without obstructing the actual solution were found: unable to prevent easier solution." << std::endl;
		return false;
	}
	for (size_t e = 0; e < candidates.size(); e++)
	{
		std::wcout << candidates[e].x << ", " << candidates[e].y << std::endl;
	}
	size_t chosen = std::rand() % candidates.size();
	board->setBlock(candidates[chosen], Element::FLOOR);
	std::wcout << "A floor was placed at: " << candidates[chosen].x << ", " << candidates[chosen].y << "." << std::endl;
	return true;
}


bool Game::intendedMove(size_t move)
{
	size_t block = solutionmoves[move].first;
	coordinate blockco = solution[move].getIceBlock(block);
	switch (solutionmoves[move].second)
	{
	case Direction::RIGHT:
		{
			coordinate right = {blockco.x + 1, blockco.y};
			while (blockco.x + 1 < this->x && board->fieldIsSlidable(right))
			{
				if (!board->fieldAllowedSolution(right))
				{
					return false;
				}
				blockco.x++;
			}
			break;
		}
	case Direction::LEFT:
		{
			coordinate left = {blockco.x - 1, blockco. y};
			while (blockco.x > 0 && board->fieldIsSlidable(left))
			{
				if (!board->fieldAllowedSolution(left))
				{
					return false;
				}
				blockco.x--;
			}
			break;
		}

	case Direction::DOWN:
		{
			coordinate down = {blockco.x, blockco.y + 1};
			while (blockco.y + 1 < this->y && board->fieldIsSlidable(down))
			{
				if (!board->fieldAllowedSolution(down))
				{
					return false;
				}
				blockco.y++;
			}
			break;
		}

	case Direction::UP:
		{
			coordinate up = {blockco.x, blockco.y - 1};
			while (blockco.y > 0 && board->fieldIsSlidable(up))
			{
				if (!board->fieldAllowedSolution(up))
				{
					return false;
				}
				blockco.y--;
			}
			break;
		}
	}
	return true;
}

void Game::generateBoard(size_t mov, size_t ibc, size_t tc)
{
	std::srand(std::time(NULL));

	std::wcout << "Generating using seed: " << std::time(NULL) << std::endl;

	size_t maxFields = this->x * this->y;

	size_t movecount = 0;
	size_t distance;
	coordinate coGen;
	Direction dir;

	std::vector<size_t> triedBlocks;
	std::vector<Direction> lastDirection;

	for (size_t i = 0; i < tc; i++)
	{
		do
		{ // Do not place another target on the same place
			coGen = {std::rand() % this->x, std::rand() % this->y};
		} while (board->getBlock(coGen).getElement() == Element::HITTARGET);
		board->addTarget(coGen);
		board->addIceBlock(coGen);
		board->setBlock(coGen, Element::HITTARGET);
		lastDirection.push_back((Direction)4);
	}
	std::wcout << "Placed targets with ice blocks on them. There are " << ibc - board->getIceBlockCount() << " iceblocks left to use." << std::endl;
	board->printBoard();

	// Continue generation until moves are depleted, or we tried all blocks
	while (movecount < mov && triedBlocks.size() < board->getIceBlockCount())
	{
		size_t i;
		// Attempt all blocks
		do
		{
			i = std::rand() % board->getIceBlockCount();
		} while (std::find(triedBlocks.begin(), triedBlocks.end(), i) != triedBlocks.end());
		triedBlocks.push_back(i);
		coordinate iceBlockco = board->getIceBlock(i);
		dir = (Direction)4;
		bool trydirection = true;
		// For this block, try directions (and if that does not work out, try other direction).
		// Consider that if an ice block slid left or right in the last move, it cannot slide
		// left or right again for the "previous" move, as it would either undo the "next" move,
		// or make it impossible to reach its next state.
		// Keep in mind that we generate puzzles backwards. Previous and next here refer to
		// the order of moves during solving.
		do
		{
			if (lastDirection[i] == Direction::UP || lastDirection[i] == Direction::DOWN)
			{
				if (dir == (Direction)4)
				{
					std::wcout << "Trying to move the ice block left or right" << std::endl;
					dir = (Direction)(std::rand() % 2);
				}
				else if (dir == Direction::LEFT)
				{
					std::wcout << "Tried left, now right..." << std::endl;
					dir = Direction::RIGHT;
					trydirection = false; // Tried both directions, so don't try any further.
				}
				else
				{
					std::wcout << "Tried right, now left..." << std::endl;
					dir = Direction::LEFT;
					trydirection = false; // Tried both directions, so don't try any further.
				}
			}
			else if (lastDirection[i] == Direction::LEFT || lastDirection[i] == Direction::RIGHT)
			{
				if (dir == (Direction)4)
				{
					std::wcout << "Trying to move the ice block up or down" << std::endl;
					dir = (Direction)(2 + std::rand() % 2);
				}
				else if (dir == Direction::UP)
				{
					std::wcout << "Tried up, now down..." << std::endl;
					dir = Direction::DOWN;
					trydirection = false; // Tried both directions, so don't try any further.
				}
				else
				{
					std::wcout << "Tried down, now up..." << std::endl;
					dir = Direction::UP;
					trydirection = false; // Tried both directions, so don't try any further.
				}
			}
			else
			{ // Ice block has no last direction, so it must be a new ice block and can therefore move in all four directions.
				std::wcout << "This is a new ice block, so all directions are allowed - except if we have another ice block next to this new one." << std::endl;
				bool valid = false;
				do
				{
					// This is a bit crude, as it may re-attempt directions which have been proven to be impossible.
					// But it's such a minor time loss, I'm not bothering making this more complicated than necessary.
					dir = (Direction)(std::rand() % 4);
					switch (dir)
					{
					case Direction::LEFT:
						{
							coordinate left = {iceBlockco.x - 1, iceBlockco.y};
							if (iceBlockco.x == 0)
							{
								valid = true;
							}
							else if (iceBlockco.x > 0 && board->getBlock(left).getElement() != Element::ICEBLOCK && board->getBlock(left).getElement() != Element::HITTARGET)
							{
								valid = true;
							}
							break;
						}
					case Direction::RIGHT:
						{
							coordinate right = {iceBlockco.x + 1, iceBlockco.y};
							if (iceBlockco.x == this->x - 1)
							{
								valid = true;
							}
							else if (iceBlockco.x + 1 < this->x && board->getBlock(right).getElement() != Element::ICEBLOCK && board->getBlock(right).getElement() != Element::HITTARGET)
							{
								valid = true;
							}
							break;
						}
					case Direction::UP:
						{
							coordinate up = {iceBlockco.x, iceBlockco.y - 1};
							if (iceBlockco.y == 0)
							{
								valid = true;
							}
							else if (iceBlockco.y > 0 && board->getBlock(up).getElement() != Element::ICEBLOCK && board->getBlock(up).getElement() != Element::HITTARGET)
							{
								valid = true;
							}
							break;
						}
					case Direction::DOWN:
						{
							coordinate down = {iceBlockco.x, iceBlockco.y + 1};
							if (iceBlockco.y == this->y - 1)
							{
								valid = true;
							}
							else if (iceBlockco.y + 1 < this->y && board->getBlock(down).getElement() != Element::ICEBLOCK && board->getBlock(down).getElement() != Element::HITTARGET)
							{
								valid = true;
							}
							break;
						}
					}
				} while (!valid);
			}
			distance = board->getMaxDistance(iceBlockco, dir); // Determine whether max distance is larger than 0
			std::wcout << "Max distance: " << distance << std::endl;
		} while (distance == 0 && trydirection);

		if (distance > 0) // Found a valid move (moves with a distance of 0 are invalid)
		{
			size_t temp = board->getIceBlockCount();
			if (board->placeBlockNextTo(iceBlockco, dir, ibc))
			{
				if (temp < board->getIceBlockCount())
				{
					// We have placed a new ice block i+1 to block the previous ice block i, hence we use i + 1 below to add the ice block's data.
					// The new ice block is not allowed to move left/right if the iceblock that's supposed to stop it here went left or right as a last movement.
					// The same holds for up/down.
					lastDirection.push_back(dir);
					coordinate newIceBlockco = board->getIceBlock(i + 1);
					// We also prohibit that a floor is placed on the location of the newly placed ice block, since it would be obstructed.
					if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), newIceBlockco) == forbiddenCoordinates.end())
					{
						std::wcout << "Prohibiting the placement of floors on: X:" << newIceBlockco.x << ", Y:" << newIceBlockco.y << "." << std::endl;
						forbiddenCoordinates.push_back(newIceBlockco);
					}
				}
				// Now we go back to the previous ice block so we move it.
				std::wcout << "Now moving ice block: " << i << std::endl;
				board->replaceBlock(i, dir, distance);
				lastDirection[i] = dir;
				movecount++;
				coordinate iceBlockMovedco = board->getIceBlock(i);
				if (std::find(forbiddenCoordinates.begin(), forbiddenCoordinates.end(), iceBlockMovedco) == forbiddenCoordinates.end())
				{
					std::wcout << "Prohibiting the placement of floors on: X:" << iceBlockMovedco.x << ", Y:" << iceBlockMovedco.y << "." << std::endl;
					forbiddenCoordinates.push_back(iceBlockMovedco);
				}
				triedBlocks.clear();
			}
		}
	}

	// We now check whether the puzzle has an easier solution than intended.
	// If this is the case, we try to place floors to prevent ice blocks from
	// sliding over places they were not intended to slide over.
	// It is possible this prevents an easier solution, however, another easier
	// solution may still exist, so we call this in a loop until we did not
	// place any more new floors. If this in the end results in an unsolvable
	// puzzle, it seems the algorithm has been unable to ensure the original
	// solution has to be followed. We therefore revert the puzzle to just
	// one step before it became unsolvable, as this may still result in
	// an interesting puzzle, at least more interesting than before applying
	// any prevention.

	GameState generatedBoard;
	for (size_t i = 0; i < maxFields; i++)
	{
		generatedBoard.insertBoardElement(board->getBlockIndex(i).getElement());
	}

	// We also create a backup in case the prevention goes awry and the puzzle somehow
	// becomes unsolvable. A puzzle that is too easy is better than an unsolvable one, right?
	GameState generatedBoardBackup = generatedBoard;

	std::wcout << "Puzzle has been generated! Checking if it is solvable..." << std::endl;

	uint8_t prevention_iterations = 1;
	bool finishedPrevention = false;

	this->iceBlockCount = board->getIceBlockCount();
	
	do {
		finishedPrevention = true;
		// If our attempts at making the puzzle closer to the initial solution
		// makes the puzzle unsolvable, reset the puzzle back to its previous
		// state and stop.
		if (solveBFS(0) == -1)
		{
			for (size_t i = 0; i < maxFields; i++)
			{
				board->setBlockIndex(i, generatedBoardBackup.getBoardElement(i));
			}
			break;
		}

		// Reset the game board to before solving
		for (size_t i = 0; i < maxFields; i++)
		{
			board->setBlockIndex(i, generatedBoard.getBoardElement(i));
		}

		board->initialize();
		board->analyse();
		board->reanalyse();

		// Update backup to previous version of the board
		generatedBoard.clearBoardState();
		for (size_t i = 0; i < maxFields; i++)
		{
			generatedBoardBackup.insertBoardElement(board->getBlockIndex(i).getElement());
		}

		for (size_t m = 0; m < solutionmoves.size(); m++)
		{
			if (!intendedMove(m))
			{
				std::wcout << "Move " << m << " is an unintended possibility during solving this puzzle. Placing a floor to prevent this..." << std::endl;
				if (placeRandomFloor(m)){
					finishedPrevention = false;
				}
			}
		}
		if (!finishedPrevention){
			std::wcout << "Easy solution prevention iteration: " << prevention_iterations << std::endl;
			prevention_iterations++;
			board->printBoard();
		}

		// Write changes to board so we can keep improving it
		generatedBoard.clearBoardState();
		for (size_t i = 0; i < maxFields; i++)
		{
			generatedBoard.insertBoardElement(board->getBlockIndex(i).getElement());
		}
	} while (!finishedPrevention);
	

	
	if (board->getIceBlockCount() < ibc)
	{
		std::wcout << "Note: the number of placed ice blocks is lower than was allowed." << std::endl;
		std::wcout << "The generated puzzle has " << board->getIceBlockCount() << " iceblocks instead of the given " << ibc << "." << std::endl;
	}

	std::wcout << "Here's the generated puzzle:" << std::endl;
	board->printBoard();
}

int Game::solveRandom(unsigned moves, bool print) const
{
	std::srand(std::time(NULL));
	size_t i, d;
	size_t j = 0;
	size_t trycount = 0;
	while (!checkDone() && j < moves)
	{
		if (print)
			board->printBoard();
		do
		{
			i = std::rand() % board->getIceBlockCount();
			d = std::rand() % 4;
			trycount++;
		} while (!board->moveBlock(board->getIceBlock(i), (Direction)d, false) && trycount < 50000);
		j++;
		if (trycount >= 50000)
			break;
	}
	if (print)
		board->printBoard();
	if (trycount >= 50000)
	{
		std::wcout << "No more moves are possible." << std::endl;
		return -1;
	}
	if (checkDone())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int Game::solveBFS(int blockchangefactor)
{
	size_t maxFields = this->x * this->y;

	solution.clear();
	solutionmoves.clear();
	endStates.clear();
	allStates.clear();

	GameState currentState;
	GameState nextState;
	unsigned int moveCounter = 0;
	std::vector<GameState> visitedStates;
	std::vector<blockmove> currentStatePastMoves;
	unsigned int currentDifficulty = 0;

	std::list<GameState> queue;

	for (size_t i = 0; i < maxFields; i++)
	{
		currentState.insertBoardElement(board->getBlockIndex(i).getElement());
	}

	for (size_t i = 0; i < iceBlockCount; i++)
	{
		currentState.insertIceBlockCoordinate(board->getIceBlock(i));
	}

	currentState.setMoveCount(moveCounter);
	currentState.setLastBlock(-1);
	currentState.setDifficulty(moveCounter);
	currentState.setPastMoves(currentStatePastMoves);
	currentState.setDifficulty(0);

	solutionmoves.clear();

	bool inserted;
	delete tree; // Cleanup just to be sure (and absolutely necessary when generating puzzles)
	tree = new GameNode(currentState, NULL);
	GameNode *currentStateNode;
	GameNode *temp;

	visitedStates.push_back(currentState);
	queue.push_back(currentState);

	board->analyse();
	board->initialize();

	while (!(checkDone() || queue.empty()))
	{
		currentState = queue.front();
		currentStateNode = tree->findStateExact(currentState);
		uint8_t i = 0;
		while (!checkDone() && i < iceBlockCount)
		{
			size_t d = 0;
			// Attempt all moves
			while (!checkDone() && d < 4)
			{
				// Put the current game state on the board
				for (size_t j = 0; j < maxFields; j++)
				{
					board->setBlockIndex(j, currentState.getBoardElement(j));
				}

				board->setIceBlockCoordinates(currentState.getIceBlockCoordinates());
				moveCounter = currentState.getMoveCount();

				board->initialize();
				board->reanalyse();

				// Perform this move
				if (board->moveBlock(board->getIceBlock(i), (Direction)d, false))
				{
					// Sla dit op in een nieuwe gamestate
					for (size_t j = 0; j < maxFields; j++)
					{
						nextState.insertBoardElement(board->getBlockIndex(j).getElement());
					}

					for (size_t j = 0; j < iceBlockCount; j++)
					{
						nextState.insertIceBlockCoordinate(board->getIceBlock(j));
					}
					currentDifficulty = currentState.getStateDifficulty() + 1;
					nextState.setMoveCount(moveCounter + 1);
					if (currentStateNode->getParent() != NULL && currentState.getLastBlock() != i)
					{
						currentDifficulty += blockchangefactor; // If the block of the previous move is not the same as the block we currently move, add penalty
					}
					nextState.setDifficulty(currentDifficulty);
					nextState.setLastBlock(i);

					currentStatePastMoves = currentState.getPastMoves();
					currentStatePastMoves.push_back(std::make_pair(i, (Direction)d));

					nextState.setPastMoves(currentStatePastMoves);

					// Als de nieuwe gamestate nog nooit is geweest, voeg toe aan queue
					if (std::find_if(visitedStates.begin(), visitedStates.end(), [&nextState](auto &elem)
									 { return elem.getBoardState() == nextState.getBoardState(); }) == visitedStates.end())
					{
						visitedStates.push_back(nextState);
						queue.push_back(nextState);
						inserted = false;
						tree->insertState(currentState, nextState, std::make_pair(i, (Direction)d), inserted);
						if (!inserted)
						{
							std::wcout << "Error: Could not insert state." << std::endl;
							throw std::exception();
						}
					}
					nextState.clearBoardState();
					nextState.clearIceBlockCoordinates();
					nextState.setMoveCount(0);
					nextState.setDifficulty(0);
				}
				d++;
			}
			i++;
		}
		// Verwijder de currentState, daar zijn we klaar mee
		currentState.clearBoardState();
		currentState.clearIceBlockCoordinates();
		currentState.setMoveCount(0);
		currentState.setDifficulty(0);
		queue.pop_front();
	}

	for (size_t i = 0; i < maxFields; i++)
	{
		currentState.insertBoardElement(board->getBlockIndex(i).getElement());
	}
	for (size_t i = 0; i < iceBlockCount; i++)
	{
		currentState.insertIceBlockCoordinate(board->getIceBlock(i));
	}

	temp = tree->findState(currentState);
	GameNode *temp2 = NULL;
	while (temp != NULL)
	{
		solution.insert(solution.begin(), temp->getState());
		if (temp2 != NULL)
		{
			solutionmoves.insert(solutionmoves.begin(), temp->getMove(temp2->getState()));
		}
		temp2 = temp;
		temp = temp->getParent();
	}

	for (size_t i = 0; i < solution.size(); i++)
	{
		solution[i].printState(this->x, this->y);
		if (i < solution.size() - 1)
		{
			std::wcout << "Move to next state is: ";
			printMove(solutionmoves[i]);
		}
	}

	if (board->getHitTargetCount() == board->getTargetCount())
	{
		std::wcout << "The puzzle was solved in " << solution.back().getMoveCount() << " moves." << std::endl
				   << std::endl;
		return solution.back().getMoveCount();
	}
	else
	{
		std::wcout << "No solution was found." << std::endl;
		return -1;
	}
}

int Game::solveBFSPrune(int blockchangefactor)
{
	size_t maxFields = this->x * this->y;

	solution.clear();
	solutionmoves.clear();
	endStates.clear();
	allStates.clear();

	GameState currentState;
	GameState nextState;
	int currentDifficulty = 0;

	std::list<GameState> queue;
	std::vector<blockmove> currentStatePastMoves;
	std::vector<GameNode *> foundStates;

	for (size_t i = 0; i < maxFields; i++)
	{
		currentState.insertBoardElement(board->getBlockIndex(i).getElement());
	}

	for (size_t i = 0; i < iceBlockCount; i++)
	{
		currentState.insertIceBlockCoordinate(board->getIceBlock(i));
	}

	currentState.setMoveCount(0);
	currentState.setDifficulty(currentDifficulty);
	currentState.setLastBlock(-1);
	currentState.setPastMoves(currentStatePastMoves);

	bool inserted;
	bool insertIt = false;

	delete tree;
	tree = new GameNode(currentState, NULL);
	GameNode *currentStateNode;

	queue.push_back(currentState);

	while (!queue.empty())
	{
		currentState = queue.front();
		currentStateNode = tree->findStateExact(currentState);
		// currentState.printState(this->x, this->y);

		if (currentStateNode != NULL)
		{
			uint8_t i = 0;
			while (i < iceBlockCount)
			{
				size_t d = 0;
				while (d < 4)
				{
					// Zet de huidige gamestate op het bord
					for (size_t j = 0; j < maxFields; j++)
					{
						board->setBlockIndex(j, currentState.getBoardElement(j));
					}

					board->setIceBlockCoordinates(currentState.getIceBlockCoordinates());

					board->initialize();
					board->reanalyse();
					// Maak de move die we nu bekijken

					if (board->moveBlock(board->getIceBlock(i), (Direction)d, false))
					{
						// Sla dit op in een nieuwe gamestate
						for (size_t j = 0; j < maxFields; j++)
						{
							nextState.insertBoardElement(board->getBlockIndex(j).getElement());
						}

						for (size_t j = 0; j < iceBlockCount; j++)
						{
							nextState.insertIceBlockCoordinate(board->getIceBlock(j));
						}

						nextState.setMoveCount(currentState.getMoveCount() + 1);

						currentDifficulty = currentState.getStateDifficulty() + 1;

						if (currentStateNode->getParent() != NULL && currentState.getLastBlock() != i)
						{ // If the ice block of the previous move is another than the one of the current move, we add the block change penalty
							currentDifficulty += blockchangefactor;
						}
						nextState.setDifficulty(currentDifficulty);

						currentStatePastMoves = currentState.getPastMoves();
						currentStatePastMoves.push_back(std::make_pair(i, (Direction)d));
						nextState.setPastMoves(currentStatePastMoves);
						nextState.setLastBlock(i);

						tree->findAllStateNodes(nextState, foundStates, blockchangefactor);
						if (foundStates.size() == 0) // If the new gamestate never occurred before, add to tree
						{
							queue.push_back(nextState);

							inserted = false;
							tree->insertState(currentState, nextState, std::make_pair(i, (Direction)d), inserted);
							if (!inserted)
							{
								throw std::exception();
							}
						}
						else // The gamestate occurred before, but we need to check whether it was reached with a lower difficulty than previously
						{
							insertIt = true;
							for (size_t state = 0; state < foundStates.size(); state++)
							{
								if (currentDifficulty > foundStates[state]->getState().getStateDifficulty() || (currentDifficulty == foundStates[state]->getState().getStateDifficulty() && blockchangefactor == 0))
								{
									/*if (currentDifficulty - foundStates[state]->getState().getStateDifficulty() > blockchangefactor && foundStates[state]->getState().getLastBlock() != i)
									{
									  std::wcout << "Nuttig! Want: nu bereikt met: " << currentDifficulty << " en met laatste blok: " << i << std::endl;
									  std::wcout << "Terwijl eerst berekt met: " << foundStates[state]->getState().getStateDifficulty() << " en met laatste blok: " << foundStates[state]->getState().getLastBlock() << std::endl;
									  std::wcout << "Dat is een verschil van " << currentDifficulty - foundStates[state]->getState().getStateDifficulty() << " wat meer is dan de blockchangefactor van " << blockchangefactor << std::endl;
									}*/
									// std::wcout << "Ik schijn hier wel te komen" << std::endl;
									insertIt = false;
								}
								if (currentDifficulty < foundStates[state]->getState().getStateDifficulty())
								{
									GameNode *tempParent = foundStates[state]->getParent();
									if (tempParent != NULL)
									{
										tempParent->deleteChild(foundStates[state]->getState());
									}
								}
							}
							if (insertIt)
							{
								queue.push_back(nextState);

								inserted = false;
								tree->insertState(currentState, nextState, std::make_pair(i, (Direction)d), inserted);
								if (!inserted)
								{
									throw std::exception();
								}
							}
						}
						if (inserted && checkDone())
						{
							endStates.push_back(nextState);
						}
						foundStates.clear();
						nextState.clearBoardState();
						nextState.clearIceBlockCoordinates();
						nextState.setMoveCount(0);
						nextState.setDifficulty(0);
					}
					d++;
				}
				i++;
			}
		}
		currentState.clearBoardState();
		currentState.clearIceBlockCoordinates();
		currentState.setMoveCount(0);
		currentState.setDifficulty(0);
		queue.pop_front();
	}

	int easiestSolutionDifficulty = INT_MAX;
	GameState easiestSolution;
	for (size_t i = 0; i < endStates.size(); i++)
	{
		if (endStates[i].getStateDifficulty() < easiestSolutionDifficulty)
		{
			easiestSolution = endStates[i];
			easiestSolutionDifficulty = easiestSolution.getStateDifficulty();
		}
	}

	if (endStates.size() != 0)
	{
		for (size_t i = 0; i < maxFields; i++)
		{
			board->setBlockIndex(i, easiestSolution.getBoardElement(i));
		}

		board->setIceBlockCoordinates(easiestSolution.getIceBlockCoordinates());

		board->reanalyse();
		board->initialize();

		currentStateNode = tree->findStateExact(easiestSolution);
		GameNode *temp2 = NULL;
		while (currentStateNode != NULL)
		{
			solution.insert(solution.begin(), currentStateNode->getState());
			if (temp2 != NULL)
			{
				solutionmoves.insert(solutionmoves.begin(), currentStateNode->getMove(temp2->getState()));
			}
			temp2 = currentStateNode;
			currentStateNode = currentStateNode->getParent();
		}

		for (size_t i = 0; i < solution.size(); i++)
		{
			std::wcout << "Difficulty measure of the game state printed below is: " << solution[i].getStateDifficulty() << std::endl;
			solution[i].printState(this->x, this->y);
			if (i < solution.size() - 1)
			{
				std::wcout << "Move to the next state is: ";
				printMove(solutionmoves[i]);
			}
		}
	}

	if (checkDone())
	{
		std::wcout << "The puzzle was solved in " << solution.back().getMoveCount() << " moves." << std::endl
				   << std::endl;
		return solution.back().getMoveCount();
	}
	else
	{
		std::wcout << "No solution was found." << std::endl;
		return -1;
	}
}

void Game::getAllDifficulties(const std::string& filename)
{
	if (tree != NULL)
	{
		tree->getAllStates(allStates);
	}

	for (size_t i = 0; i < allStates.size(); i++)
	{
		difficulties[allStates[i].getStateDifficulty()]++;
	}

	std::wcout << "Unique game states found: " << allStates.size() << "." << std::endl;

	std::ofstream output(filename);
	for (auto const &x : difficulties)
	{
		output << x.first << " " << x.second << std::endl;
	}
}

std::vector<GameState> Game::getAllStates() const
{
	return this->allStates;
}

void Game::solveMC(unsigned randomGames, unsigned moves, int blockchangefactor)
{
	size_t maxFields = board->getX() * board->getY();
	int best;
	int current;
	blockmove bestMove;
	blockmove currentMove;
	GameState currentState;
	GameState nextState;
	unsigned int moveCounter = 0;
	unsigned int currentDifficulty = 0;
	uint8_t lastblock = -1;

	tree = NULL;

	bool giveup = false;
	bool possiblemove = false;

	board->printBoard();

	while (!checkDone() && !giveup && moves > 0)
	{
		for (size_t i = 0; i < maxFields; i++)
		{
			currentState.insertBoardElement(board->getBlockIndex(i).getElement());
		}
		for (size_t i = 0; i < iceBlockCount; i++)
		{
			currentState.insertIceBlockCoordinate(board->getIceBlock(i));
		}

		best = 0;
		bestMove = std::make_pair(0, (Direction)4);
		possiblemove = false;

		for (uint8_t i = 0; i < iceBlockCount; i++)
		{
			for (uint8_t d = 0; d < 4; d++)
			{
				current = 0;
				for (size_t k = 0; k < maxFields; k++)
				{
					board->setBlockIndex(k, currentState.getBoardElement(k));
				}

				board->setIceBlockCoordinates(currentState.getIceBlockCoordinates());

				board->initialize();
				board->reanalyse();

				if (board->moveBlock(board->getIceBlock(i), (Direction)d, false))
				{
					possiblemove = true;
					currentMove = std::make_pair(i, (Direction)d);

					for (size_t j = 0; j < maxFields; j++)
					{
						nextState.insertBoardElement(board->getBlockIndex(j).getElement());
					}
					for (size_t j = 0; j < iceBlockCount; j++)
					{
						nextState.insertIceBlockCoordinate(board->getIceBlock(j));
					}

					for (size_t j = 0; j < randomGames; j++)
					{
						for (size_t k = 0; k < maxFields; k++)
						{
							board->setBlockIndex(k, nextState.getBoardElement(k));
						}
						board->setIceBlockCoordinates(nextState.getIceBlockCoordinates());

						board->initialize();
						board->reanalyse();

						current += solveRandom(moves, false);
						if (current < 0)
							break;
					}
				}
				if (current > best)
				{
					std::wcout << "Previous best number of wins: " << best << "." << std::endl;
					std::wcout << "Current number of wins: " << current << "." << std::endl;
					std::wcout << "Current best move: Ice Block " << currentMove.first << " to ";
					switch (currentMove.second)
					{
					case Direction::LEFT:
						std::wcout << "the left";
						break;
					case Direction::RIGHT:
						std::wcout << "the right";
						break;
					case Direction::UP:
						std::wcout << "up";
						break;
					case Direction::DOWN:
						std::wcout << "down";
						break;
					default:
						std::wcout << "undefined";
					}
					std::wcout << "." << std::endl;
					best = current;
					bestMove = currentMove;
				}
				nextState.clearBoardState();
				nextState.clearIceBlockCoordinates();
			}
		}
		if (!possiblemove)
			giveup = true;

		for (size_t k = 0; k < maxFields; k++)
		{
			board->setBlockIndex(k, currentState.getBoardElement(k));
		}
		board->setIceBlockCoordinates(currentState.getIceBlockCoordinates());

		board->initialize();
		board->reanalyse();
		if ((int)bestMove.second == 4)
		{
			std::wcout << "Did not find a best move. Performing a random move..." << std::endl;
			uint8_t i, d;
			size_t trycount = 0;
			do
			{
				i = std::rand() % board->getIceBlockCount();
				d = std::rand() % 4;
				trycount++;
			} while (!board->moveBlock(board->getIceBlock(i), (Direction)d, false) && trycount < 50000);
			bestMove.first = i;
			bestMove.second = (Direction)d;
			if (trycount >= 50000)
				giveup = true;
		}
		else
		{
			board->moveBlock(board->getIceBlock(bestMove.first), bestMove.second, false);
		}
		currentState.clearBoardState();
		currentState.clearIceBlockCoordinates();
		currentDifficulty++;
		if (moveCounter != 0 && bestMove.first != lastblock)
		{
			currentDifficulty += blockchangefactor;
		}
		std::wcout << "Difficulty measure of the game state printed below is: " << currentDifficulty << std::endl;
		board->printBoard();
		lastblock = bestMove.first;
		moveCounter++;
		moves--;
	}

	if (giveup || moves == 0)
		std::wcout << "No solution was found." << std::endl;
	else if (checkDone())
	{
		std::wcout << "The puzzle was solved in " << moveCounter << " moves." << std::endl;
		std::wcout << "Using Monte Carlo and this solution, the puzzle had a difficulty of: " << currentDifficulty << "." << std::endl;
	}
}

unsigned int Game::getDifficulty()
{
	return solution.back().getStateDifficulty();
}

void Game::getStateSpace(bool print) const
{
	size_t maxFields = this->x * this->y;

	GameState currentState;
	GameState nextState;
	std::vector<GameState> visitedStates;

	std::list<GameState> queue;

	for (size_t i = 0; i < maxFields; i++)
	{
		currentState.insertBoardElement(board->getBlockIndex(i).getElement());
	}

	for (size_t i = 0; i < iceBlockCount; i++)
	{
		currentState.insertIceBlockCoordinate(board->getIceBlock(i));
	}

	visitedStates.push_back(currentState);
	queue.push_back(currentState);

	currentState.clearBoardState();
	currentState.clearIceBlockCoordinates();

	while (!queue.empty())
	{
		currentState = queue.front();
		uint8_t i = 0;
		while (i < iceBlockCount)
		{
			size_t d = 0;
			while (d < 4)
			{
				// Zet de huidige gamestate op het bord
				for (size_t j = 0; j < maxFields; j++)
				{
					board->setBlockIndex(j, currentState.getBoardElement(j));
				}
				board->setIceBlockCoordinates(currentState.getIceBlockCoordinates());

				board->initialize();
				board->reanalyse();
				// Maak de move die we nu bekijken

				if (board->moveBlock(board->getIceBlock(i), (Direction)d, false))
				{
					// Sla dit op in een nieuwe gamestate
					for (size_t j = 0; j < maxFields; j++)
					{
						nextState.insertBoardElement(board->getBlockIndex(j).getElement());
					}

					for (size_t j = 0; j < iceBlockCount; j++)
					{
						nextState.insertIceBlockCoordinate(board->getIceBlock(j));
					}

					// Als de nieuwe gamestate nog nooit is geweest, voeg toe aan queue
					if (std::find_if(visitedStates.begin(), visitedStates.end(), [&nextState](auto &elem)
									 { return elem.getBoardState() == nextState.getBoardState(); }) == visitedStates.end())
					{
						visitedStates.push_back(nextState);
						queue.push_back(nextState);
					}
					nextState.clearBoardState();
					nextState.clearIceBlockCoordinates();
				}
				d++;
			}
			i++;
		}
		// Verwijder de currentState, daar zijn we klaar mee
		currentState.clearBoardState();
		currentState.clearIceBlockCoordinates();
		queue.pop_front();
	}

	if (print)
	{
		for (size_t i = 0; i < visitedStates.size(); i++)
		{
			for (size_t j = 0; j < maxFields; j++)
			{
				board->setBlockIndex(j, visitedStates[i].getBoardElement(j));
			}
			board->setIceBlockCoordinates(visitedStates[i].getIceBlockCoordinates());
			board->printBoard();
		}
	}

	std::wcout << "The number of possible game states is: " << visitedStates.size() << "." << std::endl;
}

size_t Game::getX() const
{
	return this->x;
}

size_t Game::getY() const
{
	return this->y;
}
