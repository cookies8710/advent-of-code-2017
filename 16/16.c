#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NPROGRAMS 16

void spin(char *programs, int size)
{
	char copy[NPROGRAMS];
	memcpy(copy, programs, NPROGRAMS * sizeof(char));

	memcpy(programs, copy + NPROGRAMS - size, size * sizeof(char));
	memcpy(programs + size, copy, (NPROGRAMS - size) * sizeof(char));
}

void exchange(char *programs, int a, int b)
{
	char tmp = programs[a];
	programs[a] = programs[b];
	programs[b] = tmp;
}

void partner(char *programs, char a, char b)
{
	int ai = 0, bi = 0;
	for (int i = 0; i < NPROGRAMS; i++)
	{
		if (programs[i] == a) ai = i;
		if (programs[i] == b) bi = i;
	}
	exchange(programs, ai, bi);
}

int runs_find(char **runs, int runs_size, char *program)
{
	for (int i = 0; i < runs_size; i++)
		if (0 == strcmp(runs[i] , program))
			return i;

	return -1;
}

int main(void)
{
	printf("\n\nstart\n");
	char programs[NPROGRAMS +1];
	for (int i = 0; i < NPROGRAMS; i++) programs[i] = 'a' + i;
	programs[NPROGRAMS] = 0;

	int cycle = -1;

	int runs_size = 0, runs_capacity = 10;
	char **runs = malloc(runs_capacity * sizeof(char*));


	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *line = 0;
		size_t sz = 0;
		if (0 > getline(&line, &sz, f))
			break;
	
		char *copy = malloc(strlen(line) + 1);
		strcpy(copy, line);
		
		while (cycle < 0)
		{
			strcpy(line, copy);

			for(char *p = strtok(line, ","); p; p = strtok(NULL, ","))
			{
				int ss, a, b;
				char ca, cb;
				switch (p[0])
				{
					case 's':
						sscanf(p, "s%d", &ss);
						spin(programs, ss);
						break;
					case 'x':
						sscanf(p, "x%d/%d", &a, &b);
						exchange(programs, a, b);
						break;
					case 'p':
						sscanf(p, "p%c/%c", &ca, &cb);
						partner(programs, ca, cb);
						break;
						
				}
			}

			cycle = runs_find(runs, runs_size, programs);
			if (cycle < 0)
			{
				int idx = runs_size++;
				runs[idx] = malloc(sizeof(char) * (NPROGRAMS + 1));
				strcpy(runs[idx], programs);

				if (runs_size == runs_capacity)
					runs = realloc(runs, sizeof(char*) * (runs_capacity *= 2));
			}
			else
			{
				cycle = runs_size - cycle;
			}
		}

		free(line);
	}
	fclose(f);

	printf("Part 1: %s\n", runs[0]);
	printf("Part 2: %s\n", runs[(1000000000 % cycle) - 1]);

	for (int i = 0; i < runs_size; i++)
	{
		free(runs[i]);
		runs[i] = NULL;
	}
	free(runs);
	runs = NULL;
	runs_capacity = runs_size = 0;

	printf("done\n");
	return 0;
}
