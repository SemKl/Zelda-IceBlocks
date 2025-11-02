#include "gamestate.h"

std::vector<Element> GameState::getBoardState() const
{
	return this->boardState;
}

Element GameState::getBoardElement(size_t i) const
{
	return this->boardState[i];
}

std::vector<coordinate> GameState::getIceBlockCoordinates() const
{
	return this->iceBlockCoordinates;
}

unsigned int GameState::getMoveCount() const
{
	return this->moveCount;
}

std::vector<std::pair<uint8_t, Direction>> GameState::getPastMoves() const
{
	return this->pastMoves;
}

int GameState::getStateDifficulty() const
{
	return this->stateDifficulty;
}

uint8_t GameState::getLastBlock() const
{
	return this->lastBlock;
}

coordinate GameState::getIceBlock(uint8_t i) const
{
	return this->iceBlockCoordinates[i];
}

void GameState::setBoardState(const std::vector<Element>& boardState)
{
	this->boardState = boardState;
}

void GameState::setIceBlockCoordinates(const std::vector<coordinate>& iceBlockCoordinates)
{
	this->iceBlockCoordinates = iceBlockCoordinates;
}

void GameState::setMoveCount(unsigned int moveCount)
{
	this->moveCount = moveCount;
}

void GameState::setDifficulty(unsigned int difficulty)
{
	this->stateDifficulty = difficulty;
}

void GameState::setPastMoves(const std::vector<blockmove>& pastMoves)
{
	this->pastMoves = pastMoves;
}

void GameState::setLastBlock(uint8_t lastBlock)
{
	this->lastBlock = lastBlock;
}

void GameState::insertBoardElement(Element e)
{
	this->boardState.push_back(e);
}

void GameState::insertIceBlockCoordinate(const coordinate& c)
{
	this->iceBlockCoordinates.push_back(c);
}

void GameState::clearBoardState()
{
	this->boardState.clear();
}

void GameState::clearIceBlockCoordinates()
{
	this->iceBlockCoordinates.clear();
}

void GameState::printState(size_t x, size_t y)
{
	std::wcout << ' ' << ' ' << ' ';
	for (size_t i = 0; i < x; i++)
	{
		if (i < 10)
		{
			std::wcout << ' ' << i << ' ';
		}
		else
		{
			std::wcout << i << ' ';
		}
	}
	std::wcout << std::endl;
	for (size_t i = 0; i < y; i++)
	{
		if (i < 10)
		{
			std::wcout << i << ' ';
		}
		else
		{
			std::wcout << i;
		}
		for (size_t j = 0; j < x; j++)
		{
			std::wcout << ' ' << eltochar[(int)boardState[i * x + j]] << ' ';
		}
		std::wcout << std::endl;
	}
	std::wcout << std::endl;

	std::wcout << "The ice blocks are on coordinates: " << std::endl;
	for (size_t i = 0; i < iceBlockCoordinates.size(); i++)
	{
		std::wcout << "Ice block " << i << " at X: " << iceBlockCoordinates[i].x << " Y: " << iceBlockCoordinates[i].y << std::endl;
	}
	std::wcout << std::endl;
	std::wcout << "Current difficulty measure: " << this->stateDifficulty << std::endl;
	// Only print this if we actually performed a move. A state cannot have a difficulty
	// of 0 if a move was performed already.
	if (this->stateDifficulty > 0){
		std::wcout << "Last used ice block: " << this->lastBlock << std::endl;
	}
}
