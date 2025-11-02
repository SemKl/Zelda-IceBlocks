#include "gamenode.h"

#include <iostream>

GameNode::GameNode(const GameState& currentState, GameNode *parent)
{
	this->currentState = currentState;
	this->parent = parent;
}

GameNode::~GameNode()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
}

GameState GameNode::getState() const
{
	return this->currentState;
}

GameNode *GameNode::getParent() const
{
	return this->parent;
}

void GameNode::insertState(GameState& currentState, const GameState& nextState, blockmove move, bool &inserted)
{
	if (!inserted)
	{
		if (currentState == this->currentState)
		{
			GameNode *newNode = new GameNode(nextState, this);
			this->children.push_back(newNode);
			this->moves.push_back(move);
			inserted = true;
		}
		else
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				children[i]->insertState(currentState, nextState, move, inserted);
			}
		}
	}
}

void GameNode::deleteChild(const GameState& nextState)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->getState() == nextState)
		{
			delete children[i];
			children[i] = NULL;
			children.erase(children.begin() + i);
			moves.erase(moves.begin() + i);
		}
	}
}

GameNode *GameNode::findState(GameState state)
{
	if (state.getBoardState() == this->currentState.getBoardState())
	{
		return this;
	}
	else
	{
		GameNode *found = NULL;
		for (size_t i = 0; i < children.size(); i++)
		{
			found = children[i]->findState(state);
			if (found != NULL)
			{
				return found;
			}
		}
		return NULL;
	}
}

GameNode *GameNode::findStateExact(GameState state)
{
	if (state == this->currentState)
	{
		return this;
	}
	else
	{
		GameNode *found = NULL;
		for (size_t i = 0; i < children.size(); i++)
		{
			found = children[i]->findStateExact(state);
			if (found != NULL)
			{
				return found;
			}
		}
		return NULL;
	}
}

blockmove GameNode::getMove(const GameState& nextState) const
{
	blockmove nextMove = std::make_pair(-1, (Direction)-1);
	for (size_t i = 0; i < moves.size(); i++)
	{
		if (children[i]->getState() == nextState)
		{
			nextMove = moves[i];
		}
	}
	return nextMove;
}

void GameNode::findAllStateNodes(GameState state, std::vector<GameNode *> &foundStates, int blockchangefactor)
{
	if (state.getBoardState() == this->getState().getBoardState() && state.getIceBlockCoordinates() == this->getState().getIceBlockCoordinates() && (state.getLastBlock() == this->getState().getLastBlock() || (state.getStateDifficulty() - this->getState().getStateDifficulty() > blockchangefactor)))
	{
		foundStates.push_back(this);
	}
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->findAllStateNodes(state, foundStates, blockchangefactor);
	}
}

void GameNode::getAllStates(std::vector<GameState> &allStates) const
{
	GameState thisState = this->getState();
	std::vector<GameState>::iterator it = std::find_if(allStates.begin(), allStates.end(), [&thisState](auto &elem)
													   { return elem.getBoardState() == thisState.getBoardState(); });
	if (it == allStates.end())
	{
		allStates.push_back(this->getState());
	}
	else
	{
		if (thisState.getStateDifficulty() < it->getStateDifficulty())
		{
			allStates[it - allStates.begin()] = thisState;
		}
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->getAllStates(allStates);
	}
}
