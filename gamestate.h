#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <iostream>
#include "field.h"

// A blockmove contains both an ice block (uint8_t) and
// the direction this block was pushed in.
#define blockmove std::pair<uint8_t, Direction>

// Gamestate class to save game states. These keep track
// of what elements are positioned where on the board, but
// also which ice block is where. This is done to discern
// game similar game states where ice blocks are on the same
// positions as another, but, for example, two ice blocks are
// swapped. This state might be functionally identical, but
// is different due to the difficulty and block change penalty.
class GameState
{
public:
	// Return the board state as vector
	std::vector<Element> getBoardState() const;

	// Get the board element at position i in the gamestate vector
	Element getBoardElement(size_t i) const;

	// Return all ice block coordinates of this current gamestate
	std::vector<coordinate> getIceBlockCoordinates() const;

	// Return the number of moves to reach this game state
	unsigned int getMoveCount() const;

	// Return the difficulty of the game state
	int getStateDifficulty() const ;

	// Return the vector of blockmoves that were used to reach the
	// current game state
	std::vector<blockmove> getPastMoves() const;

	// Return the last iceblock that was moved to reach this game state
	uint8_t getLastBlock() const;

	// Return the coordinates of ice block i
	coordinate getIceBlock(uint8_t i) const;

	// Set the board state
	void setBoardState(const std::vector<Element>& boardState);

	// Set the ice block coordinates
	void setIceBlockCoordinates(const std::vector<coordinate>& iceBlockCoordinates);

	// Set the move count
	void setMoveCount(unsigned int moveCount);

	// Set the past moves
	void setPastMoves(const std::vector<blockmove>& pastMoves);

	// Set the game state difficulty
	void setDifficulty(unsigned int difficulty);

	// Set the last ice block that was moved
	void setLastBlock(uint8_t lastBlock);

	// Insert element on the game board
	void insertBoardElement(Element e);

	// Insert ice block coordinates in vector
	void insertIceBlockCoordinate(const coordinate& c);

	// Clear board state vector
	void clearBoardState();

	// Clear ice block coordinates vector
	void clearIceBlockCoordinates();

	// Print the game state. Pass through the x and y of the game state.
	// x and y are only saved in the Game class and therefore need to
	// be passed through manually.
	void printState(size_t x, size_t y);

	// Operator overloading to compare game states.
	// Everything needs to be exactly equal in order to
	// have an equal game state. A game state is not necesserily
	// equal to another, even if the ice blocks are on the same
	// coordinates.
	bool operator==(GameState const &gs)
	{
		return (boardState == gs.boardState && iceBlockCoordinates == gs.iceBlockCoordinates && pastMoves == gs.pastMoves && lastBlock == gs.lastBlock);
	}

private:
	// We save the board state as a vector of elements without x and y information
	// as this would be more efficient than saving a whole Game instance
	std::vector<Element> boardState;

	// The coordinates of the ice blocks are saved separately to ensure we keep track
	// which ice blocks are where, rather than only where the ice blocks are
	std::vector<coordinate> iceBlockCoordinates;

	// The sequence of moves performed to get to this state is also saved separately
	std::vector<blockmove> pastMoves;

	// The last ice block that was moved is also saved to ensure this state is exactly
	// equal
	uint8_t lastBlock;

	// The number of moves performed to reach this state
	unsigned int moveCount;

	// The difficulty of the current state
	int stateDifficulty;

	// Conversion to print the game state
  // Empty: .
  // Target: ◻
  // Target with ice block on it: ▣
  // Floor: ▦
  // Ice block: ◼
  // Wall: ▩
  // Path: ,
	wchar_t eltochar[7] = {'.', L'◻', L'▣', L'▦', L'◼', L'▩', ','};
};

#endif