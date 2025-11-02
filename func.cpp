#include <iostream>
#include <string.h>

#include "func.h"

void getInput(size_t *input)
{
	char buf[8];
	bool valid;
	do
	{
		if (!scanf("%s", buf)){
      return;
    }
		for (size_t i = 0; i < strlen(buf); i++)
		{
			if (isdigit(buf[i]))
			{
				*input = atoi(buf);
				valid = true;
			}
			else
			{
				valid = false;
				break;
			}
		}
	} while (!valid);
}
