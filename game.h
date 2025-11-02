#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "gamenode.h"

class Game
{
public:
	// Constructor for empty board
	Game(size_t x, size_t y);

	// Constructor for reading from file
	Game(const std::string& filename);

	// Destructor
	~Game();

	// Play game
	void play() const;

	// Check whether the game is done; all ice blocks are on targets
	bool checkDone() const;

	// Place a random floor tile to prevent easier solutions
	bool placeRandomFloor(size_t move);

	// Find whether move i was intended during generation
	bool intendedMove(size_t move);

	// Generate a new puzzle with a certain number of moves, ice blocks, and targets
	void generateBoard(size_t dif, size_t ibc, size_t tc);

	// Wrapper function that generates a new puzzle and allows the user to save the generated puzzle
	void generate(size_t mov, size_t ibc, size_t tc);

	// Solve the puzzle with random moves, and either print or do not print the performed moves
	int solveRandom(unsigned moves, bool print) const;

	// Solve the puzzle using Breadth-First Search and calculating the difficulty of the
	// found solution by the number of moves, with an additional penalty when the algorithm
	// has to switch using blocks during solving the puzzle.
	int solveBFS(int blockchangefactor);

	// Solve the puzzle using Breadth-First Search and calculating the difficulty of the
	// found solution by the number of moves, with an additional penalty when the algorithm
	// has to switch using blocks during solving the puzzle. This variation of BFS also
	// does not further investigate branches with states that occurred earlier with a lower
	// difficulty to save time.
	int solveBFSPrune(int blockchangefactor);

	// Solve the puzzle using Monte Carlo Search and calculating the difficulty of the
	// found solution by the number of moves, with an additional penalty when the algorithm
	// has to switch using blocks during solving the puzzle.
	void solveMC(unsigned randomGames, unsigned moves, int blockchangefactor);

	// Find all possible game states and find how many moves it took to reach that state.
	// Save to output file.
	void getAllDifficulties(const std::string& filename);

	// Find all possible game states.
	std::vector<GameState> getAllStates() const;

	// Get the difficulty of the puzzle after it was solved.
	unsigned int getDifficulty();

	// Find the number of possible game states.
	void getStateSpace(bool print) const;

	// Print the move that was performed.
	void printMove(blockmove move) const;

	// Get-functions for board X and Y size.
	size_t getX() const;
	size_t getY() const;

private:
	// Game board X and Y
	size_t x, y;

	// Pointer board
	Board *board;

	// Number of ice blocks
	uint8_t iceBlockCount;

	// Tree with game states
	GameNode *tree;

	// Vector with all game states
	std::vector<GameState> allStates;

	// Solution vector containing game states
	std::vector<GameState> solution;

	// Vector containing all possible game states in which the puzzle was solved
	std::vector<GameState> endStates;

	// Solution vector containing solution moves for generation
	std::vector<blockmove> solutionmoves;

	// Vector containing all coordinates where we cannot place floors to prevent
	// easier solutions during puzzle generation
	std::vector<coordinate> forbiddenCoordinates;

	// Map containing how many game states have what difficulty
	std::map<unsigned, unsigned> difficulties;
};

#endif
