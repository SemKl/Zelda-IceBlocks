#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <set>

#include "field.h"
#include "gamenode.h"

// Board class on which the game is played
class Board
{
public:
	// Constructor with x and y for empty puzzle
	Board(size_t x, size_t y);

	// Constructor with filename to read a puzzle from file
	Board(const std::string& filename);

	// Destructor
	~Board();

	// Return X size of the board
	size_t getX() const;

	// Return Y size of the board
	size_t getY() const;

	// Print the game board
	void printBoard() const;

	// Get the field at index i
	Field getBlockIndex(size_t i) const;

	// Get the field at coordinate (position x, y)
	Field getBlock(const coordinate& c) const;

	// Set the field at index i to element e
	void setBlockIndex(size_t i, Element e);

	// Set the field at coordinate (position x, y) to character (for reading from file)
	void setBlock(const coordinate& c, char set);

	// Set the field at coordinate (position x, y) to element e
	void setBlock(const coordinate& c, Element e);

	// Set the coordinate of ice block i to coordinate co
	void setIceBlockCoordinate(size_t i, const coordinate& co);

	// Set all ice block coordinates to given vector
	void setIceBlockCoordinates(const std::vector<coordinate>& c);

	// Add a target at coordinate
	void addTarget(const coordinate& coGen);

	// Add an ice block at coordinate
	void addIceBlock(const coordinate& coGen);

	// Check whether an ice block is able to slide over this coordinate.
	bool fieldIsSlidable(const coordinate& co) const;

	// Check whether the given field is allowed to be slid over during solution.
	 bool fieldAllowedSolution(const coordinate& co) const;

	// Function used during generation. In this function, we look for the
	// largest distance an ice block on coordinate (position x, y) could have slid
	// in direction d. So, if an ice block on coordinate (position x, y) was last sled in
	// Direction d = Left, how far could it have come from the right?
	size_t getMaxDistance(coordinate c, Direction d) const;

	// Place an obstructing tile next to an ice block that is currently
	// positioned on place x, y, such that it could have sled in
	// Direction dir to reach x, y. ibc is the ice block count, the
	// total number of ice blocks permitted during puzzle generation.
	bool placeBlockNextTo(const coordinate& c, Direction dir, size_t ibc);

	// Relocate ice block i to direction dir with a maximum distance.
	void replaceBlock(size_t i, Direction dir, size_t maxdistance);

	// Return the number of ice blocks in the puzzle
	uint8_t getIceBlockCount() const;

	// Return the number of targets in the puzzle
	uint8_t getTargetCount() const;

	// Return the number of targets on which an ice block was already
	// positioned
	uint8_t getHitTargetCount() const;

	// Return the coordinates of ice block i
	coordinate getIceBlock(uint8_t i) const;

	// Return the coordinates of target t
	coordinate getTarget(uint8_t t) const;

	// Analyse the board for the first time to find where ice blocks
	// and targets are positioned
	void analyse();

	// Analyse how many targets have an ice block on them
	void reanalyse();

	// Initialize an empty board
	void initialize();

	// Move an ice block at coordinate c in direction d
	bool moveBlock(const coordinate& c, Direction d, const bool print);

private:
	// Set the field to which an ice block on position x, y would slide
	// to if pushed in direction d
	void searchField(coordinate c, Direction d);

	// Update all the fields an ice block would slide to. All fields with
	// either x or y equal to the given coordinate (x, y) are updated.
	// Function is called after an ice block was either moved from x, y,
	// or moved to x, y, since this impacts how other ice blocks would slide
	void update(const coordinate& c);

	// Check if field is empty at x, y. Used during generation to place
	// tiles to obstruct ice blocks.
	bool checkIfFree(const coordinate& c) const;

	// Check if ice block is not obstructed to be pushed in given direction
	bool checkNewPlace(const coordinate& c, Direction d) const;

	// The game board
	Field *fieldBoard = NULL;

	// All ice blocks on the board
	std::vector<coordinate> iceblocks;

	// All targets on the board
	std::vector<coordinate> targets;

	// The number of targets that have an ice block on them
	uint8_t hitTargetCount;

	// The size of the board
	size_t x, y;

	// Conversion to print the game state
	wchar_t eltochar[7] = {'.', L'□', L'▣', L'▦', L'■', L'▩', ','};
};

#endif
