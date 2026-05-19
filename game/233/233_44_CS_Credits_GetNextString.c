#include <common.h>

// ASM-verified: 0x800b8810-0x800b885c
char *CS_Credits_GetNextString(char *str)
{
	char c = *str;
	while (c != '\0')
	{
		if (c == '\r')
			return str + 1;
		str++;
		c = *str;
	}
	if (*str != '\r')
		return 0;
	return str + 1;
}
