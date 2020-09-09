#include <stdio.h>
#include <stdbool.h>

static void evaluate(const char *s, int *score, int *garbage_size);

int main(void)
{
	FILE *f = fopen("input", "r");
	char *line = 0;
	size_t size = 0;
	getline(&line, &size, f);
	fclose(f);

	int score = 0, garbage_size = 0;
	evaluate(line, &score, &garbage_size);
	printf("Part 1 - total score = %d\n", score);
	printf("Part 2 - garbage size = %d\n", garbage_size);

	printf("done\n");
	return 0;
}

static void evaluate(const char *s, int *score, int *garbage_size)
{
	bool garbage = false;
	int depth = 0;
	*score = 0;
	*garbage_size = 0;
	while(*s)
	{
		if (garbage && *s != '>' && *s != '!') (*garbage_size)++;
		switch (*s)
		{
			case '{':
				if (!garbage) 
				{
					depth++;
					*score += depth;
				}
				break;
			case '}':
				if (!garbage) depth--;
				break;
			case '<':
				garbage = true;
				break;
			case '>':
				garbage = false;
				break;
			case '!':
				s++;
				break;
		}
		s++;
	}
}
