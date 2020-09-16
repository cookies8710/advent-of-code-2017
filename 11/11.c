#include <stdio.h>
#include <string.h>

#define ABS(x) ((x<0)?(-x):(x))

int eval(int s, int n, int se, int ne, int sw, int nw)
{
	// cancel out opposite directions
	s -= n;
	sw -= ne;
	se -= nw;

	// each pair NW, NE can be substitued by N
	se -= sw;
	s += sw;

	return ABS(s) + ABS(se);
}

int main()
{
	FILE *f = fopen("input", "r");
	int s = 0, n = 0, se = 0, ne = 0, sw = 0, nw = 0;
	int max = 0;
	while (!feof(f))
	{
		char *line = 0;
		size_t sz = 0;
		if (0 > getline(&line, &sz, f))
			break;

		while (*line)
		{
			char *sta = line;
			while(*line && *line != ',' && *line != '\n')line++;
			int len = line - sta;

			if (0 == strncmp("s", sta, len))
				s++;
			else if (0 == strncmp("n", sta, len))
				n++;
			else if (0 == strncmp("sw", sta, len))
				sw++;
			else if (0 == strncmp("ne", sta, len))
				ne++;
			else if (0 == strncmp("se", sta, len))
				se++;
			else if (0 == strncmp("nw", sta, len))
				nw++;
			else 
				printf("unknown sequence\n");

			int e = eval(s, n, se, ne, sw, nw);
			if (e > max)
				max = e;

			if (*line) line++;
		}
		
	}
	fclose(f);

	printf("Part 1 (distance in the end): %d\nPart 2 (max distance at any point): %d\n", eval(s, n, se, ne, sw, nw), max);


	printf("done\n");
	return 0;
}
