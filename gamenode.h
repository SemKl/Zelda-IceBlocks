#ifndef GAMENODE_H
#define GAMENODE_H

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include "field.h"
#include "gamestate.h"

// Gamenode class to keep track of the gametree
class GameNode
{
public:
	// Constructor
	GameNode(const GameState& currentState, GameNode *parent);

	// Destructor
	~GameNode();

	// Get the game state
	GameState getState() const;

	// Get the parent of this node
	GameNode *getParent() const;

	// Insert a state from the top down as the child of the current state.
	// A block move is also added to accompany this state.
	void insertState(GameState& currentState, const GameState& nextState, blockmove move, bool &inserted);

	// Delete a child node
	void deleteChild(const GameState& nextState);

	// Find a state and return pointer to it. Here, only the board layout needs
	// to be the a match
	GameNode *findState(GameState state);

	// Find the exact state and return a pointer to it. Here, aside from the
	// board layout, the exact position of the ice block (including which one
	// is positioned where), the past moves to reach this state, and last
	// moved ice block needs to be exactly equal.
	GameNode *findStateExact(GameState state);

	// Get the move that was performed to reach the next state
	blockmove getMove(const GameState& nextState) const;

	// Find all possible nodes that have the same gamestate as the provided "nextState".
	// This function can be used to determine whether we reached the same state in an easier
	// way, for example, with less block changes. All found states are "returned" by reference.
	void findAllStateNodes(GameState nextState, std::vector<GameNode *> &foundStates, int blockchangefactor);

	// Get all possible states by reference. This is used to find the number of
	// game states that are reached with a certain difficulty.
	void getAllStates(std::vector<GameState> &allStates) const;

private:
	// Parent node to go back up the tree
	GameNode *parent;

	// The game state in this node
	GameState currentState;

	// List of children game states. The number of child states
	// differs a lot per state, so we use a vector rather than a
	// fixed number.
	std::vector<GameNode *> children;

	// List of possible moves from this state (that also make sense,
	// i.e., that result in a new state). These correspond to the child
	// states of the children vector.
	std::vector<blockmove> moves;
};

#endif
