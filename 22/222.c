#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CLEAN 0
#define WEAKENED 1
#define INFECTED 2
#define FLAGGED 3

typedef long coord_t;
typedef struct { coord_t x, y; int state; } vec2;

static vec2** viruses_find(vec2 **v, int s, coord_t x, coord_t y)
{
	for (int i = 0; i < s; i++)
		if (v[i] && v[i]->x == x && v[i]->y == y)
			return &v[i];

	return NULL;
}

static vec2** viruses_add(vec2 ***v, int *s, int *c, vec2 n)
{
	vec2 **e = *v;

	for (int i = 0; i < *s; i++)
		if (!e[i])
		{
			e[i] = malloc(sizeof(vec2));
			if (!e[i])
				printf("malloc fail\n");
			e[i]->x = n.x;
			e[i]->y = n.y;
			e[i]->state = n.state;
			return &e[i];
		}

	int ns = (*s)++;
	e[ns] = malloc(sizeof(vec2));
	if (!e[ns]) printf("malloc fail\n");
	e[ns]->x = n.x;
	e[ns]->y = n.y;
	e[ns]->state = n.state;

	if (*s == *c)
	{
		*v = realloc(*v, sizeof(vec2*) * ((*c) *= 2));
		if (!*v)
			printf("realloc fail\n");
	}

	return (*v) + ns;
}

static void viruses_print(vec2 **v, int s, coord_t px, coord_t py)
{
	coord_t x1=v[0]->x,x2=v[0]->x;
	coord_t y1=v[0]->y,y2=v[0]->y;

	for (int i = 0; i < s; i++)
		if (v[i] && v[i]->state)
		{
			if (v[i]->x < x1) x1 = v[i]->x;
			if (v[i]->x < x1) x1 = v[i]->x;
			if (v[i]->y > y2) y2 = v[i]->y;
			if (v[i]->y > y2) y2 = v[i]->y;
		}

	if (px > x2) x2 = px;
	if (px < x1) x1 = px;
	if (py > y2) y2 = py;
	if (py < y1) y1 = py;

	coord_t sx = x2 - x1 + 1, sy = y2 - y1 + 1;
	//printf("[%d -> %d] (%d), [%d -> %d] (%d)\n", x1,x2, sx, y1,y2,sy);
	//char *p = malloc(sizeof(char) * sx * sy);
	char p[sx*sy];
	memset(p, '.', sx * sy * sizeof(char));
	
	for (int i = 0; i < s; i++)
		if (v[i] && v[i]->state)
		{
			char c;
			switch (v[i]->state)
			{
				case WEAKENED:
					c = 'W'; break;
				case INFECTED:
					c = '#'; break;
				case FLAGGED:
					c = 'F'; break;
				default:
					c = '?'; break;
			}
			p[(v[i]->y - y1) * sx + v[i]->x - x1] = c;
		}

	for (int y = 0; y < sy; y++)
	{
		for (int x = 0; x < sx; x++)
			printf((px == x + x1 && py == y + y1)?"[%c]":" %c ", p[y*sx + x]);
		printf("\n");
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
				vec2 n = {.x = i - l2, .y = row - l2, .state = INFECTED};
				viruses_add(&viruses, &viruses_size, &viruses_capacity, n);
			}

		row++;
	}
	fclose(f);

	for (int i = 0; i < viruses_size; i++)
		if (viruses[i])
			printf("[%d, %d]\n", viruses[i]->x, viruses[i]->y);
	int infb = 0, weak=0, flagg=0, cleaned=0;
	for (int i = 0; i < 10000000; i++)
	{
		if (i % 10000 == 0)
		printf("%d k (%d)\n", i / 1000, viruses_size);
		vec2 **f = viruses_find(viruses, viruses_size, pos.x, pos.y);
		if (!f || !*f)
		{
			pos.state = CLEAN;
			f = viruses_add(&viruses, &viruses_size, &viruses_capacity, pos);
		}

#ifdef DEBUG
		printf("\n\n#%d [%d, %d], state: %d, direction: (%d, %d)\n",i,
				(*f)->x, (*f)->y, (*f)->state, dirs[dir].x, dirs[dir].y);
		viruses_print(viruses, viruses_size, pos.x, pos.y);
		getchar();
#endif

		switch ((*f)->state)
		{
			case CLEAN:
				dir = (dir + 3) % 4; // L
				break;
			case WEAKENED:
				break;
			case INFECTED:
				dir = (dir + 1) % 4; // R
				break;
			case FLAGGED:
				dir = (dir + 2) % 4; // opposite direction
				break;
			default:
				printf("huh?\n");
				break;

		}

		(*f)->state = ((*f)->state + 1) % 4;
		if ((*f)->state == INFECTED)infb++;

		if ((*f)->state == CLEAN)
		{
			free(*f);
			*f = NULL;
		}

		pos.x += dirs[dir].x;
		pos.y += dirs[dir].y;
	}
	printf("%d viri, %d infb\n", viruses_size, infb);

	printf("done\n");
	return 0;
}
