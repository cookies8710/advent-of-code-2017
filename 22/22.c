#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct { int x, y; } vec2;

static bool viruses_contains(vec2 **v, int s, vec2 n)
{
	for (int i = 0; i < s; i++)
		if (v[i] && v[i]->x == n.x && v[i]->y == n.y)
			return true;
	return false;
}

static void viruses_add(vec2 ***v, int *s, int *c, vec2 n)
{
	if (!viruses_contains(*v, *s, n))
	{
		vec2 **e = *v;
		bool added = false;

		for (int i = 0; !added && i < *s; i++)
			if (!e[i])
			{
				e[i] = malloc(sizeof(vec2));
				e[i]->x = n.x;
				e[i]->y = n.y;
				added = true;
			}

		if (!added)
		{
			int ns = (*s)++;
			e[ns] = malloc(sizeof(vec2));
			e[ns]->x = n.x;
			e[ns]->y = n.y;
		}

		if (*s == *c)
		{
			*v = realloc(*v, sizeof(vec2*) * ((*c) *= 2));
		}
	}
}

static void viruses_rm(vec2 **v, int s, vec2 n)
{
	for (int i = 0; i < s; i++)
		if (v[i] && v[i]->x == n.x && v[i]->y == n.y)
		{
			free(v[i]);
			v[i] = NULL;
		}
}

int main()
{
	vec2 dirs[] = {{.x=0, .y=-1}, {.x=1, .y=0}, {.x=0, .y=1}, {.x=-1, .y=0}};
	int dir = 0;
	vec2 pos = {.x=0, .y=0};

	int viruses_size = 0, viruses_capacity = 10;
	vec2** viruses = malloc(sizeof(vec2*) * viruses_capacity);

	FILE *f = fopen("input", "r");
	int len = -1;
	int row = 0;
	while (!feof(f))
	{
		char line[30];
		memset(line, 0, sizeof(char) * 30);
		fscanf(f, "%s\n", line);

		len = strlen(line);
		int l2 = len / 2;

		for (int i = 0; i < len; i++)
			if (line[i] == '#')
			{
				vec2 n = {.x = i - l2, .y = row - l2};
				viruses_add(&viruses, &viruses_size, &viruses_capacity, n);
			}
		

		row++;
	}
	fclose(f);

	int infb = 0;
	for (int i = 0; i < 10000; i++)
	{
		bool infected = viruses_contains(viruses, viruses_size, pos);
		if (infected)
		{
			viruses_rm(viruses, viruses_size, pos);
			dir = (dir + 1) % 4; // R
		}
		else
		{
			infb++;
			dir = (dir + 3) % 4; // L
			viruses_add(&viruses, &viruses_size, &viruses_capacity, pos);
		}

		pos.x += dirs[dir].x;
		pos.y += dirs[dir].y;
	}
	printf("%d viri, %d infb\n", viruses_size, infb);
	/*for (int i = 0; i < viruses_size; i++)
		if (viruses[i])
			printf("[%d, %d]\n", viruses[i]->x, viruses[i]->y);*/

	printf("done\n");
	return 0;
}
