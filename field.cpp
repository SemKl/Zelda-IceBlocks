#include "field.h"

#include <iostream>

void Field::setSlideField(Field *slideTo, Direction d)
{
	this->slide_to[(int)d] = slideTo;
}

Field *Field::getSlideField(Direction d) const
{
	return this->slide_to[(int)d];
}

Element Field::getElement() const
{
	return this->type;
}

void Field::setElement(Element type)
{
	this->type = type;
}
