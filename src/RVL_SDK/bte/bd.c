#include <revolution/bte/bd.h>

const BD_ADDR bd_addr_null = {0};

void bdcpy(BD_ADDR a, BD_ADDR const b)
{
	int i;

	for (i = BD_ADDR_LEN; i != 0; --i)
		*a++ = *b++;
}

int bdcmp(BD_ADDR const a, BD_ADDR const b)
{
	int i;

	for (i = BD_ADDR_LEN; i != 0; i--)
	{
		if (*a++ != *b++)
			return -1;
	}

	return 0;
}
