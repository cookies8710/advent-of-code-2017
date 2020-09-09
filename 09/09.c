#include <stdio.h>
#include <stdbool.h>

static int score(const char *s);

int main(void)
{
	FILE *f = fopen("input", "r");
	char *line = 0;
	size_t size = 0;
	getline(&line, &size, f);
	fclose(f);

	printf("Part 1 - total score = %d\n", score(line));

	printf("done\n");
	return 0;
}

static int score(const char *s)
{
	bool garbage = false;
	int depth = 0;
	int score = 0;
	while(*s)
	{
		switch (*s)
		{
			case '{':
				if (!garbage) 
				{
					depth++;
					score += depth;
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

	return score;
}
