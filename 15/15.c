#include <stdio.h>
#include <stdbool.h>

int main(void)
{
	const unsigned int fa = 16807, fb = 48271;
	//const int start_a = 65, start_b = 8921;
	const int start_a = 512, start_b = 191;

	long ga = start_a, gb = start_b;

	int match = 0;
	for (int i = 0; i < 40000000; i++)
	{
		ga = (ga * fa) % 0x7fffffff;
		gb = (gb * fb) % 0x7fffffff;
		if (((ga ^ gb) & 0xffff) == 0) 
			match++;
	}
	printf("Part 1: %d\n", match); 

	ga = start_a, gb = start_b;

	match = 0;
	bool agenerated = false, bgenerated = false;
	for (int i = 0; i < 5000000; )
	{
		if (!agenerated)
		{
			ga = (ga * fa) % 0x7fffffff;
			agenerated = (ga & 3) == 0;
		}

		if (!bgenerated)
		{
			gb = (gb * fb) % 0x7fffffff;
			bgenerated = (gb & 7) == 0;
		}

		if (agenerated && bgenerated)
		{
			i++;
			agenerated = bgenerated = false;
			if (((ga ^ gb) & 0xffff) == 0) 
				match++;
		}
	}
	printf("Part 2: %d\n", match); 

	printf("done\n");
	return 0;
}
