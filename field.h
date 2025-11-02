#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <cstddef>

// Struct to save coordinates
struct coordinate
{
	size_t x;
	size_t y;

	// Operator overloading to compare coordinates
	inline bool operator==(coordinate const &a) const
	{
		return (this->x == a.x && this->y == a.y);
	}
};

// All types of elements that can be placed on the board.
// Empty:     This is ice where the ice blocks can slide over unobstructed.
// Target:    This is where ice blocks need to be slid on to complete the puzzle.
// Hittarget: This is a target where an ice block was already slid on. This target is satisfied.
// Iceblock:  These are the movable blocks that need to be pushed onto the targets to complete the puzzle. Ice blocks will
//            continue to slide until they are stopped by a floor, wall, or another ice block (which can be on a target).
// Floor:     This is a tile where the ice blocks do not slide over, but the hypothetical player is able to stand on.
//            If an ice block is positioned next to a floor tile, the block can be pushed to the opposite direction,
//            i.e., if a floor tile is positioned to the right of an ice block, the ice block can be pushed to the left.
// Wall:      This is a tile where the ice blocks do not slide over, but the hypothetical player is unable to stand on.
//            If an ice block is positioned next to a wall tile, the block cannot be pushed to the opposite direction,
//            i.e., if a wall tile is positioned to the right of an ice block, the ice block cannot be pushed to the left.
// Path:      This is equal to an empty element. These are only used during generation of new puzzles to keep track of
//            how the ice blocks moved during the backwards generation. This is done to ensure these paths are unobstructed,
//            but also that the intended solution is the only possible one and that the puzzle cannot be easily solved.
enum class Element
{
	EMPTY,
	TARGET,
	HITTARGET,
	FLOOR,
	ICEBLOCK,
	WALL,
	PATH
};

// The four directions
enum class Direction
{
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// A field in the board.
class Field
{
public:
	// Set the field an ice block would slide to when pushed in a certain direction
	void setSlideField(Field *slideTo, Direction d);

	// Get the field an ice block slides to when pushed in a certain direction
	Field *getSlideField(Direction d) const;

	// Return the element of this field
	Element getElement() const;

	// Set the element of this field
	void setElement(Element type);

private:
	// The four fields an ice block would slide to, in the four directions
	Field *slide_to[4];

	// The type of this field
	Element type;
};

#endif
