#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
#include <ctime>

#include "board.h"
#include "func.h"

Board::Board(size_t x, size_t y)
{
	this->x = x;
	this->y = y;
	fieldBoard = new Field[x * y];
	for (size_t i = 0; i < x * y; i++)
	{
		fieldBoard[i].setElement(Element::EMPTY);
	}
}

Board::Board(const std::string& filename)
{
	std::wcout << "File: " << std::wstring(filename.begin(), filename.end()) << std::endl;
	std::string line;
	std::ifstream input(filename);
	if (input.is_open())
	{
		getline(input, line, ' ');
		this->x = atoi(line.c_str());
		std::wcout << "X: " << std::wstring(line.begin(), line.end()) << std::endl;
		getline(input, line);
		this->y = atoi(line.c_str());
		std::wcout << "Y: " << std::wstring(line.begin(), line.end()) << std::endl;

		fieldBoard = new Field[x * y];

		// First initialize an empty board
		for (size_t i = 0; i < x * y; i++)
		{
			fieldBoard[i].setElement(Element::EMPTY);
		}

		size_t j = 0;
		hitTargetCount = 0;
		while (getline(input, line) && j < y)
		{
			for (size_t i = 0; i < std::min(x, line.length()); i++)
			{
				coordinate co = {i, j};
				if (line[i] == 'H' || line[i] == 'h')
				{
					hitTargetCount++;
				}
				setBlock(co, line[i]);
			}
			j++;
		}
		initialize();
		analyse();
	}
	else
	{
		throw std::invalid_argument("Input file was not opened!");
	}
}

Board::~Board()
{
	delete[] fieldBoard;
}

bool Board::checkIfFree(const coordinate& c) const
{
	return (getBlock(c).getElement() == Element::EMPTY);
}

bool Board::placeBlockNextTo(const coordinate& c, Direction dir, size_t ibc)
{
	uint8_t blockType;
	Element el = Element::EMPTY;
	if (ibc - iceblocks.size() > 0) // There are iceblocks left to use during generation of this puzzle
	{
		blockType = std::rand() % 2;
	}
	else // There are no iceblocks left to use during generation
	{
		blockType = 0;
	}
	switch (blockType)
	{
	case 0:
		el = Element::FLOOR;
		break;
	case 1:
		el = Element::ICEBLOCK;
		break;
	}

	coordinate p;

	switch (dir)
	{
	case Direction::LEFT:
		if (c.x != 0)
		{
			p = {c.x - 1, c.y};
			if (getBlock(p).getElement() == Element::PATH && el != Element::ICEBLOCK)
			{
				return false;
			}
			else if (checkIfFree(p))
			{
				setBlock(p, el);
				if (el == Element::ICEBLOCK)
				{
					iceblocks.push_back(p);
				}
			}
		}
		break;
	case Direction::RIGHT:
		if (c.x + 1 < this->x)
		{
			p = {c.x + 1, c.y};
			if (getBlock(p).getElement() == Element::PATH && el != Element::ICEBLOCK)
			{
				return false;
			}
			if (checkIfFree(p))
			{
				setBlock(p, el);
				if (el == Element::ICEBLOCK)
				{
					iceblocks.push_back(p);
				}
			}
		}
		break;
	case Direction::UP:
		if (c.y != 0)
		{
			p = {c.x, c.y - 1};
			if (getBlock(p).getElement() == Element::PATH && el != Element::ICEBLOCK)
			{
				return false;
			}
			if (checkIfFree(p))
			{
				setBlock(p, el);
				if (el == Element::ICEBLOCK)
				{
					iceblocks.push_back(p);
				}
			}
		}
		break;
	case Direction::DOWN:
		if (c.y + 1 < this->y)
		{
			p = {c.x, c.y + 1};
			if (getBlock(p).getElement() == Element::PATH && el != Element::ICEBLOCK)
			{
				return false;
			}
			if (checkIfFree(p))
			{
				setBlock(p, el);
				if (el == Element::ICEBLOCK)
				{
					iceblocks.push_back(p);
				}
			}
		}
		break;
	}
	printBoard();
	return true;
}

bool Board::fieldIsSlidable(const coordinate& co) const
{
	Field block = this->getBlock(co);
	Element elem = block.getElement();
	return elem != Element::WALL && elem != Element::FLOOR && elem != Element::ICEBLOCK && elem != Element::HITTARGET;
}

bool Board::fieldAllowedSolution(const coordinate& co) const
{
	Field block = this->getBlock(co);
	Element elem = block.getElement();
	return elem == Element::PATH || elem == Element::ICEBLOCK || elem == Element::TARGET || elem == Element::HITTARGET;
}

size_t Board::getMaxDistance(coordinate c, Direction d) const
{
	size_t distance = 0;
	size_t encounteredIceblocks = 0;
	switch (d)
	{
	case Direction::LEFT:
		std::wcout << "Searching to the right to slide ice block from right to left." << std::endl;
		while (c.x + 1 < this->x && getBlock({c.x+1, c.y}).getElement() != Element::FLOOR && getBlock({c.x+1, c.y}).getElement() != Element::WALL)
		{
			distance++;
			c.x++;
			// We have to keep track how many ice blocks we encounter.
			// In very rare cases, it is possible that we attempt to move an
			// ice block in the direction where all blocks are ice blocks.
			// In this case, we would report the distance as if nothing would
			// be in the way - as desired, because ice blocks present during
			// generation may be somewhere else during puzzle solving.
			// However, in the rare instances where all encountered fields
			// contain an ice block and we get a regular distance returned,
			// we would end up in a softlock-loop, because the algorithm will
			// keep trying various distances, but all spaces are occupied by
			// ice blocks. Hence, if encounteredIceblocks == distance, we
			// return distance as 0.
			if (getBlock({c.x+1, c.y}).getElement() == Element::ICEBLOCK || getBlock({c.x+1, c.y}).getElement() == Element::HITTARGET)
			{
				encounteredIceblocks++;
			}
		}
		break;
	case Direction::RIGHT:
		std::wcout << "Searching to the left to slide ice block from left to right." << std::endl;		
		while (c.x > 0 && getBlock({c.x-1, c.y}).getElement() != Element::FLOOR && getBlock({c.x-1, c.y}).getElement() != Element::WALL)
		{
			distance++;
			c.x--;
			if (getBlock({c.x-1, c.y}).getElement() == Element::ICEBLOCK || getBlock({c.x-1, c.y}).getElement() == Element::HITTARGET)
			{
				encounteredIceblocks++;
			}
		}
		break;

	case Direction::UP:
		std::wcout << "Searching down to slide ice block from down to up." << std::endl;
		while (c.y + 1 < this->y && getBlock({c.x, c.y+1}).getElement() != Element::FLOOR && getBlock({c.x, c.y+1}).getElement() != Element::WALL)
		{
			distance++;
			c.y++;
			if (getBlock({c.x, c.y+1}).getElement() == Element::ICEBLOCK || getBlock({c.x, c.y+1}).getElement() == Element::HITTARGET)
			{
				encounteredIceblocks++;
			}
		}
		break;

	case Direction::DOWN:
		std::wcout << "Searching up to slide ice block from up to down." << std::endl;
		while (c.y > 0 && getBlock({c.x, c.y-1}).getElement() != Element::FLOOR && getBlock({c.x, c.y-1}).getElement() != Element::WALL)
		{
			distance++;
			c.y--;
			if (getBlock({c.x, c.y-1}).getElement() == Element::ICEBLOCK || getBlock({c.x, c.y-1}).getElement() == Element::HITTARGET)
			{
				encounteredIceblocks++;
			}
		}
		break;
	}

	// If the distance is 0, we cannot actually move the ice block.
	if (distance == 0)
	{
		std::wcout << "Invalid direction: ditance equal to 0." << std::endl;
		return 0;
	}

	// Prevention: if the distance is equal to the number of encountered ice blocks
	// on the way, we return a distance of 0, which will be seen as invalid.
	if (distance == encounteredIceblocks)
	{
		std::wcout << "Ice blocks occupy each field in this direction. This is blocking!" << std::endl;
		return 0;
	}
	std::wcout << "Found position: X:" << c.x << ", Y:" << c.y << " with distance: " << distance << "." << std::endl;
	return distance;
}

bool Board::checkNewPlace(const coordinate& c, Direction dir) const
{
	if (getBlock(c).getElement() == Element::ICEBLOCK || getBlock(c).getElement() == Element::HITTARGET)
		return false;

	Element elem = Element::EMPTY;
	coordinate p;
	switch (dir)
	{
	case Direction::LEFT:
		if (c.x == this->x - 1) // Ice block is at right border of the puzzle board
			return true; // Moves from the border of the puzzle board are always assumed as possible, since we assume boards are surrounded by floors.
		else
		{
			// Get element next to hypothetical space the ice block originated from.
			// The ice block could not have moved in this direction if another ice block or a wall was next to it.
			p = {c.x + 1, c.y};
			elem = getBlock(p).getElement();
			return !(elem == Element::ICEBLOCK || elem == Element::HITTARGET || elem == Element::WALL);
		}
		break;

	case Direction::RIGHT:
		if (c.x == 0) // Ice block is at left border of the puzzle board
			return true;
		else
		{
			p = {c.x - 1, c.y};
			elem = getBlock(p).getElement();
			return !(elem == Element::ICEBLOCK || elem == Element::HITTARGET || elem == Element::WALL);
		}
		break;

	case Direction::UP:
		if (c.y == this->y - 1) // Ice block is at top border of the puzzle board
			return true;
		else
		{
			p = {c.x, c.y + 1};
			elem = getBlock(p).getElement();
			return !(elem == Element::ICEBLOCK || elem == Element::HITTARGET || elem == Element::WALL);
		}
		break;

	case Direction::DOWN:
		if (c.y == 0) // Ice block is at bottom border of the puzzle board
			return true;
		else
		{
			p = {c.x, c.y - 1};
			elem = getBlock(p).getElement();
			return !(elem == Element::ICEBLOCK || elem == Element::HITTARGET || elem == Element::WALL);
		}
		break;
	default:
  		__builtin_unreachable();
	}
	// We should never get here, but something went wrong so we assume the move was invalid.
	return false;
}

void Board::replaceBlock(size_t i, Direction dir, size_t maxdistance)
{
	coordinate c; // x, y coordinate to find a potential place from which the iceblock originated
	coordinate p; // x, y coordinate to mark all empty fields the ice block slid over as a path used in solution
	size_t distance = -1;
	switch (dir)
	{
	case Direction::LEFT: // The ice block was slid from right to left
		std::wcout << "From right to left." << std::endl;
		do
		{
			distance = 1 + (std::rand() % maxdistance);
			c = iceblocks[i];
			c.x += distance;
		} while (!checkNewPlace(c, dir)); // Find out if the ice block could have originated from here
		for (size_t j = c.x - 1; j > iceblocks[i].x; j--)
		{
			// Then mark all empty fields between originating location and current location as path, to
			// denote that the ice block has slid here. This is to keep track our final puzzle also uses
			// The intended solution without shortcuts. If a shortcut is made, we are placing blocks to
			// prevent said shortcuts. However, blocks can never obstruct a path, so we need to keep track
			// of the intended solution.
			p = {j, iceblocks[i].y};
			if (getBlock(p).getElement() == Element::EMPTY)
			{
				setBlock(p, Element::PATH);
			}
		}
		break;
	case Direction::RIGHT: // The ice block was slid from left to right
		std::wcout << "From left to right." << std::endl;
		do
		{
			distance = 1 + (std::rand() % maxdistance);
			c = iceblocks[i];
			c.x -= distance;
		} while (!checkNewPlace(c, dir));
		for (size_t j = c.x + 1; j < iceblocks[i].x; j++)
		{
			p = {j, iceblocks[i].y};
			if (getBlock(p).getElement() == Element::EMPTY)
			{
				setBlock(p, Element::PATH);
			}
		}
		break;
	case Direction::UP: // The ice block was slid from down to up
		std::wcout << "From down to up." << std::endl;
		do
		{
			distance = 1 + (std::rand() % maxdistance);
			c = iceblocks[i];
			c.y += distance;
		} while (!checkNewPlace(c, dir));
		for (size_t j = c.y - 1; j > iceblocks[i].y; j--)
		{
			p = {iceblocks[i].x, j};
			if (getBlock(p).getElement() == Element::EMPTY)
			{
				setBlock(p, Element::PATH);
			}
		}
		break;
	case Direction::DOWN: // The ice block was slid from up to down
		std::wcout << "From up to down." << std::endl;
		do
		{
			distance = 1 + (std::rand() % maxdistance);
			// std::wcout << "distance:" << distance << std::endl;
			c = iceblocks[i];
			c.y -= distance;
		} while (!checkNewPlace(c, dir));
		for (size_t j = c.y + 1; j < iceblocks[i].y; j++)
		{
			p = {iceblocks[i].x, j};
			if (getBlock(p).getElement() == Element::EMPTY)
			{
				setBlock(p, Element::PATH);
			}
		}
		break;
	default:
  		__builtin_unreachable();
	}

	std::wcout << "Chosen distance: " << distance << "." << std::endl;
	// We now have a coordinate from where this ice block was slid

	// Now remove the ice block from its position and replace it to the
	// coordinates where we determined it came from
	if (getBlock(iceblocks[i]).getElement() == Element::HITTARGET)
	{
		setBlock(iceblocks[i], Element::TARGET);
	}
	else
	{
		setBlock(iceblocks[i], Element::PATH);
	}

	iceblocks[i] = c;
	if (getBlock(iceblocks[i]).getElement() == Element::TARGET)
	{
		setBlock(iceblocks[i], Element::HITTARGET);
	}
	else
	{
		setBlock(iceblocks[i], Element::ICEBLOCK);
	}
	std::wcout << "Ice block was moved." << std::endl;
	printBoard();
	std::wcout << "--------------------------------------" << std::endl;
}

void Board::searchField(coordinate c, Direction d)
{
	Field *temp = NULL;
	switch (d)
	{
	case Direction::LEFT:
		temp = &fieldBoard[c.y * this->x + (c.x - 1)];
		break;
	case Direction::UP:
		temp = &fieldBoard[(c.y - 1) * this->x + c.x];
		break;
	case Direction::RIGHT:
		temp = &fieldBoard[c.y * this->x + (c.x + 1)];
		break;
	case Direction::DOWN:
		temp = &fieldBoard[(c.y + 1) * this->x + c.x];
		break;
	}
	// Ice block is obstructed, so it stops here
	if (temp->getElement() == Element::ICEBLOCK || temp->getElement() == Element::HITTARGET || temp->getElement() == Element::FLOOR || temp->getElement() == Element::WALL)
	{
		fieldBoard[c.y * this->x + c.x].setSlideField(&fieldBoard[c.y * this->x + c.x], d);
	}
	// Ice block is unobstructed, so it continues to slide in the same direction
	else
	{
		fieldBoard[c.y * this->x + c.x].setSlideField(temp->getSlideField(d), d);
	}
}

void Board::initialize()
{
	coordinate c;
	// Initialize the upper row to itself: we cannot slide further up
	for (size_t i = 0; i < this->x; i++)
	{
		fieldBoard[i].setSlideField(&fieldBoard[i], Direction::UP);
	}

	// Initialize the leftmost column to itself: we cannot slide further left
	for (size_t i = 0; i < this->y; i++)
	{
		fieldBoard[i * this->x].setSlideField(&fieldBoard[i * this->x], Direction::LEFT);
	}

	// Initialize the upper row for sliding to the left
	for (size_t i = 1; i < this->x; i++)
	{
		c = {i, 0};
		searchField(c, Direction::LEFT);
	}

	// Initialize the leftmost column for sliding up
	for (size_t i = 1; i < this->y; i++)
	{
		c = {0, i};
		searchField(c, Direction::UP);
	}

	// Now fix the rest: all fields (in an empty board) would slide
	// to the upper row of leftmost column when sliding up or left.
	for (size_t i = 1; i < this->x; i++)
	{
		for (size_t j = 1; j < this->y; j++)
		{
			c = {i, j};
			searchField(c, Direction::UP);
			searchField(c, Direction::LEFT);
		}
	}

	// Initialize the bottom row to itself: we cannot slide further down
	for (size_t i = 0; i < this->x; i++)
	{
		fieldBoard[(this->y - 1) * this->x + i].setSlideField(&fieldBoard[(this->y - 1) * this->x + i], Direction::DOWN);
	}

	// Initialize the rightmost row to itself: we cannot slide further right
	for (size_t i = 0; i < this->y; i++)
	{
		fieldBoard[i * this->x + (this->x - 1)].setSlideField(&fieldBoard[i * this->x + (this->x - 1)], Direction::RIGHT);
	}

	// Initialize the bottom row for sliding to the right
	for (size_t i = this->x - 1; i > 0; i--)
	{
		c = {i - 1, this->y - 1};
		searchField(c, Direction::RIGHT);
	}

	// Initialize the rightmost column for sliding down
	for (size_t i = this->y - 1; i > 0; i--)
	{
		c = {this->x - 1, i - 1};
		searchField(c, Direction::DOWN);
	}

	// Now fix the rest: all fields (in an empty board) would slide
	// to the bottom row of rightmost column when sliding down or right.
	for (size_t i = this->x - 1; i > 0; i--)
	{
		for (size_t j = this->y - 1; j > 0; j--)
		{
			c = {i - 1, j - 1};
			searchField(c, Direction::DOWN);
			searchField(c, Direction::RIGHT);
		}
	}
}

void Board::analyse()
{
	hitTargetCount = 0;
	iceblocks.clear();
	targets.clear();
	coordinate currentBlock;
	for (size_t i = 0; i < this->y; i++)
	{
		for (size_t j = 0; j < this->x; j++)
		{
			currentBlock = {j, i};
			if (fieldBoard[i * this->x + j].getElement() == Element::ICEBLOCK)
			{
				iceblocks.push_back(currentBlock);
			}
			else if (fieldBoard[i * this->x + j].getElement() == Element::TARGET)
			{
				targets.push_back(currentBlock);
			}
			else if (fieldBoard[i * this->x + j].getElement() == Element::HITTARGET)
			{
				iceblocks.push_back(currentBlock);
				targets.push_back(currentBlock);
				hitTargetCount++;
			}
		}
	}
}

void Board::reanalyse()
{
	hitTargetCount = 0;
	for (uint8_t i = 0; i < targets.size(); i++)
	{
		for (uint8_t j = 0; j < iceblocks.size(); j++)
		{
			if (getTarget(i) == getIceBlock(j))
			{
				hitTargetCount++;
			}
		}
	}
}

size_t Board::getX() const
{
	return x;
}

size_t Board::getY() const
{
	return y;
}

void Board::printBoard() const
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
			std::wcout << ' ' << eltochar[(int)fieldBoard[i * this->x + j].getElement()] << ' ';
		}
		std::wcout << std::endl;
	}
	std::wcout << std::endl;

	std::wcout << "Ice Blocks are positioned at: " << std::endl;
	for (size_t i = 0; i < iceblocks.size(); i++)
	{
		std::wcout << "Ice Block " << i << " at X: " << iceblocks[i].x << " Y: " << iceblocks[i].y << std::endl;
	}
	std::wcout << std::endl;
}

Field Board::getBlockIndex(size_t i) const
{
	return fieldBoard[i];
}

Field Board::getBlock(const coordinate& c) const
{
	return fieldBoard[c.y * this->x + c.x];
}

void Board::setBlockIndex(size_t i, Element e)
{
	this->fieldBoard[i].setElement(e);
}

void Board::setBlock(const coordinate& c, Element e)
{
	this->fieldBoard[c.y * this->x + c.x].setElement(e);
}

void Board::setBlock(const coordinate& c, char set)
{
	Element temp;
	switch (set)
	{
	case '.':
		temp = Element::EMPTY;
		break;
	case 'T':
	case 't':
		temp = Element::TARGET;
		break;
	case 'H':
	case 'h':
		temp = Element::HITTARGET;
		break;
	case 'X':
	case 'x':
		temp = Element::FLOOR;
		break;
	case 'O':
	case 'o':
		temp = Element::ICEBLOCK;
		break;
	case 'W':
	case 'w':
		temp = Element::WALL;
		break;
	default:
		std::wcout << "Invalid character read: " << set << ". Handling as an empty space." << std::endl;
		temp = Element::EMPTY;
		break;
	}

	this->fieldBoard[c.y * this->x + c.x].setElement(temp);
}

void Board::setIceBlockCoordinate(size_t i, const coordinate& co)
{
	this->iceblocks[i] = co;
}

void Board::setIceBlockCoordinates(const std::vector<coordinate>& c)
{
	this->iceblocks = c;
}

void Board::addTarget(const coordinate& coGen)
{
	this->targets.push_back(coGen);
}

void Board::addIceBlock(const coordinate& coGen)
{
	this->iceblocks.push_back(coGen);
}

void Board::update(const coordinate& c)
{
	coordinate u; // Coordinate to update column x and row y
	for (size_t i = c.x + 1; i < this->x; i++)
	{
		u = {i, c.y};
		searchField(u, Direction::LEFT);
	}

	for (size_t i = c.x; i > 0; i--)
	{
		u = {i - 1, c.y};
		searchField(u, Direction::RIGHT);
	}

	for (size_t i = c.y + 1; i < (this->y); i++)
	{
		u = {c.x, i};
		searchField(u, Direction::UP);
	}

	for (size_t i = c.y; i > 0; i--)
	{
		u = {c.x, i - 1};
		searchField(u, Direction::DOWN);
	}
}

uint8_t Board::getIceBlockCount() const
{
	return iceblocks.size();
}

uint8_t Board::getTargetCount() const
{
	return targets.size();
}

uint8_t Board::getHitTargetCount() const
{
	return hitTargetCount;
}

coordinate Board::getIceBlock(uint8_t i) const
{
	return iceblocks[i];
}

coordinate Board::getTarget(uint8_t t) const
{
	return targets[t];
}

bool Board::moveBlock(const coordinate& c, Direction d, const bool print)
{
	if (print)
	{
		printBoard();
	}

	if (c.x >= this->x || c.y >= this->y)
	{
		if (print)
		{
			std::wcout << "Invalid coordinates chosen: X: " << c.x << ", Y: " << c.y << "." << std::endl;
		}
		return false;
	}

	if (d != Direction::LEFT && d != Direction::UP && d != Direction::RIGHT && d != Direction::DOWN)
	{
		if (print)
		{
			std::wcout << "Invalid direction chosen." << std::endl;
		}
		return false;
	}

	Field currentBlock = getBlock(c);

	if (currentBlock.getElement() != Element::ICEBLOCK && currentBlock.getElement() != Element::HITTARGET)
	{
		if (print)
		{
			std::wcout << "There is no iceblock positioned on given coordinates: X:" << c.x << ", Y:" << c.y << "." << std::endl;
		}
		return false;
	}

	// Check if block can be moved
	coordinate p;
	switch (d)
	{
	case Direction::LEFT:
		p = {c.x+1, c.y};
		if (c.x < this->x - 1 && (getBlock(p).getElement() == Element::ICEBLOCK || getBlock(p).getElement() == Element::HITTARGET || getBlock(p).getElement() == Element::WALL))
		{
			if (print)
			{
				std::wcout << "Invalid move: this ice block cannot be pushed left." << std::endl;
			}
			return false;
		}
		break;
	case Direction::RIGHT:
		p = {c.x-1, c.y};
		if (c.x > 0 && (getBlock(p).getElement() == Element::ICEBLOCK || getBlock(p).getElement() == Element::HITTARGET || getBlock(p).getElement() == Element::WALL))
		{
			if (print)
			{
				std::wcout << "Invalid move: this ice block cannot be pushed right." << std::endl;
			}
			return false;
		}
		break;
	case Direction::UP:
		p = {c.x, c.y+1};
		if (c.y < this->y - 1 && (getBlock(p).getElement() == Element::ICEBLOCK || getBlock(p).getElement() == Element::HITTARGET || getBlock(p).getElement() == Element::WALL))
		{
			if (print)
			{
				std::wcout << "Invalid move: this ice block cannot be pushed up." << std::endl;
			}
			return false;
		}
		break;
	case Direction::DOWN:
		p = {c.x, c.y-1};
		if (c.y > 0 && (getBlock(p).getElement() == Element::ICEBLOCK || getBlock(p).getElement() == Element::HITTARGET || getBlock(p).getElement() == Element::WALL))
		{
			if (print)
			{
				std::wcout << "Invalid move: this ice block cannot be pushed down." << std::endl;
			}
			return false;
		}
		break;
	}

	Field *slideToField = currentBlock.getSlideField(d);
	coordinate newC;

	// Determine new block location
	switch (d)
	{
	case Direction::LEFT:
	case Direction::RIGHT:
		newC = {(slideToField - &fieldBoard[0]) - (c.y * this->x), c.y};
		break;
	case Direction::UP:
	case Direction::DOWN:
		newC = {c.x, ((slideToField - &fieldBoard[0]) - c.x) / this->x};
		break;
	default:
  		__builtin_unreachable();
	}

	// If new coordinates if the same as the old coordinates, the ice block has not moved
	if (newC == c)
	{
		if (print)
			std::wcout << "This move did not change the board." << std::endl;
		return false;
	}

	if (currentBlock.getElement() == Element::ICEBLOCK)
	{
		setBlock(c, Element::EMPTY);
	}
	else if (currentBlock.getElement() == Element::HITTARGET)
	{
		setBlock(c, Element::TARGET);
		hitTargetCount--;
	}

	if (slideToField->getElement() == Element::TARGET)
	{
		slideToField->setElement(Element::HITTARGET);
	}
	else
	{
		slideToField->setElement(Element::ICEBLOCK);
	}

	// Update both the slide fields for where the ice block was removed,
	// as well as where the ice block came to
	update(c);
	update(newC);

	// Determine whether the ice block hit a target
	for (uint8_t i = 0; i < targets.size(); i++)
	{
		if (targets[i] == newC)
		{
			hitTargetCount++;
		}
	}

	// Update the position of the ice block
	for (uint8_t i = 0; i < iceblocks.size(); i++)
	{
		if (iceblocks[i] == c)
		{
			iceblocks[i] = newC;
		}
	}

	return true;
}
