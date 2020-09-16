#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef struct
{
	int capacity;
	int size;
	int *p;
} set;

typedef struct 
{
	int n;
	set ships;

} relation;

static relation* relation_find(relation* rs, int rs_size, int n);
static void set_add(set *s, int elem);
static bool set_contains(set *s, int elem);
static set set_new();

int main()
{
	int rc = 10;
	int nr = 0;
	relation* rs = malloc(sizeof(relation) * rc);

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *line = NULL;
		size_t sz = 0;
		if (0 > getline(&line, &sz, f))
			break;

		int process = 0;
		while (isdigit(*line)) process = 10 * process + *line++ - '0';

		rs[nr].n = process;
		rs[nr].ships = set_new();
		
		while (!isdigit(*line)) line++;

		while (*line)
		{
			int other = 0;
			while (isdigit(*line)) other = 10 * other + *line++ - '0';
			set_add(&rs[nr].ships, other);

			while (*line && !isdigit(*line)) line++;
		}
		nr++;
		
		if (nr == rc)
		{
			rs = realloc(rs, (rc *= 2) * sizeof(relation));
		}
	}
	fclose(f);

	printf("Detecting groups...\n");
	bool added = true;
	while (added)
	{
		added = false;
		for (int r = 0; r < nr; r++)
		{
			relation *zero = rs + r;
			for (int i = 0; i < zero->ships.size; i++)
			{
				int other = zero->ships.p[i];
				for (int j = 0; j < rs[other].ships.size; j++)
				{
					if (!set_contains(&zero->ships, rs[other].ships.p[j]))
					{
						set_add(&zero->ships, rs[other].ships.p[j]);
						added = true;
					}
				}
			}
		}

	}


	printf("Process zero size (Part 1): %d\n", rs[0].ships.size);

	bool *discovered = calloc(nr, sizeof(bool));
	int ngroups=0;
	for (int r = 0; r < nr; r++)
	{
		if (discovered[r])
			continue;
		ngroups++;
		discovered[r] = true;
		const set *other = &rs[r].ships;
		for (int i = 0; i <other->size; i++) 
			discovered[other->p[i]] = true;

	}

	printf("Number of equivalence classes (Part 2): %d\n", ngroups);

	printf("done\n");
	return 0;
}

static relation* relation_find(relation* rs, int rs_size, int n)
{
	for (int i = 0; i < rs_size; i++)
		if (rs[i].n == n)
			return rs + i;
	return NULL;
}

static void set_add(set *s, int elem)
{
	s->p[s->size++] = elem;

	if (s->size == s->capacity)
		s->p = realloc(s->p, (s->capacity *= 2) * sizeof(int));
}

static bool set_contains(set *s, int elem)
{
	for (int i = 0; i < s->size; i++)
		if (s->p[i] == elem)
			return true;
	return false;
}

static set set_new()
{
	set result;

	result.capacity = 1;
	result.size = 0;
	result.p = malloc(result.capacity * sizeof(int));

	return result;
}
