#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FREE(x) {free(x); x = NULL;}

typedef struct
{
	int side;
	// 8 * side * side 
	char ***patterns;

	// (side + 1)**2
	char **output;
} rule;

int rot2[2][2][2] = {
	{{1,0}, {1,1}},
	{{0,0}, {0,1}}
};

int rot3[3][3][2] = {
	{{2,0}, {2,1}, {2,2}},
	{{1,0}, {1,1}, {1,2}},
	{{0,0}, {0,1}, {0,2}}
};

typedef struct
{
	char **plane;
	int side; 
} plane;

/**
 * For passed rule `r` will generate all input patterns
 */
static rule* rule_create(char *input, char *output);

static char** to_pattern(char *line, int side);
static void pattern_print(char **p, int side)
{
	for (int i = 0; i < side; i++)
		printf("%s\n", p[i]);
}

static char** flip(char **pattern, int side);
static char** rotate(char **pattern, int side);

static void rule_print(const rule* r);
static void rule_destroy(rule *r);

static plane* plane_apply(plane *p, rule **rules, int rules_size);
static void plane_print(const char *str, plane *p);
static void plane_destroy(plane *p);

static int plane_count_on(plane *p);

int main()
{
	int rules_capacity = 10, rules_size = 0;
	rule **rules = malloc(sizeof(rule*) * rules_capacity);

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char a[128], b[128];
		memset(a, 0, 128);
		memset(b, 0, 128);
		fscanf(f, "%s => %s\n", a, b);
		rules[rules_size++] = rule_create(a, b);
		if (rules_size == rules_capacity)
			rules = realloc(rules, sizeof(rule*) * (rules_capacity *= 2));
	}
	fclose(f);

	plane *pl = malloc(sizeof(plane));
	pl->side = 3;
	pl->plane = malloc(pl->side * sizeof(char*));
	for (int i = 0; i < pl->side; i++)
	{
		pl->plane[i] = calloc(pl->side + 1, sizeof(char));
	}
	strcpy(pl->plane[0], ".#.");
	strcpy(pl->plane[1], "..#");
	strcpy(pl->plane[2], "###");

	plane_print("Initial", pl);

	for (int i = 0; i < 18; i++)
	{
		plane *old = pl;
		pl = plane_apply(old, rules, rules_size);
		plane_destroy(old);

		printf("After %d steps, %d pixels are ON\n", i+1,plane_count_on(pl));
	}

	// cleanup
	plane_destroy(pl);
	for (int i = 0; i < rules_size; i++)
	{
		rule_destroy(rules[i]);
		free(rules[i]);
		rules[i] = NULL;
	}
	free(rules);
	

	printf("done\n");
	return 0;
}

static char** to_pattern(char *line, int side)
{
	char **r;
	r = malloc(side * sizeof(char *));
	for (char *c = strtok(line, "/"), i=0;c;i++,c = strtok(NULL, "/"))
	{
		r[i] = malloc((side + 1) * sizeof(char));
		strcpy(r[i], c);
	}
	return r;
}

static char** flip(char **pattern, int side)
{
	char **r = malloc(side * sizeof(char*));
	for (int i = 0; i < side; i++)
	{
		r[i] = malloc((side + 1) * sizeof(char));
		strcpy(r[i], pattern[side - i - 1]);
	}

	return r;
}

static char** rotate(char **pattern, int side)
{
	// Allocate and init ending chars
	char **r = malloc(side * sizeof(char*));
	for (int i = 0; i < side; i++)
	{
		r[i] = malloc((side + 1) * sizeof(char));
		r[i][side] = 0;
	}

	if (side == 2)
	{
		for (int i = 0; i < side; i++)
		for (int j = 0; j < side; j++)
		{
			int x = rot2[i][j][0];
			int y = rot2[i][j][1];
			r[y][x] = pattern[i][j];
		}
	}
	else
	{
		for (int i = 0; i < side; i++)
		for (int j = 0; j < side; j++)
		{
			int x = rot3[i][j][0];
			int y = rot3[i][j][1];
			r[y][x] = pattern[i][j];
		}
	}

	return r;
}

static rule* rule_create(char *input, char *output)
{
	rule *r = malloc(sizeof(rule));

	// Count side - number of char before the first '/'
	int side = 0;
	for (char *i = input; *i && *i != '/'; i++, side++);
	r->side = side;

	// Output pattern
	r->output = to_pattern(output, side + 1);	

	// Allocate memory for input and output patterns
	r->patterns = malloc(8 * sizeof(char**));

	r->patterns[0] = to_pattern(input, side);
	
	// Group of symmetries of square has 8 members and 
	// generators a rotate and flip
	for (int i = 1; i < 4; i++)
		r->patterns[i] = rotate(r->patterns[i-1], side);
	r->patterns[4] = flip(r->patterns[0], side);
	for (int i = 5; i < 8; i++)
		r->patterns[i] = rotate(r->patterns[i-1], side);
	
	return r;
}

static void rule_print(const rule* r)
{
	printf("Side: %d\n", r->side);
	printf("Inputs:\n");
	for (int i = 0; i < 8; i++)
	{
		printf("%d:\n", i);
		pattern_print(r->patterns[i], r->side);
	}
	printf("Output:\n");
	pattern_print(r->output, r->side);
	printf("\n");
}

static void rule_destroy(rule *r)
{
	for (int i = 0; i < 8; i++)
	for (int j = 0; j < r->side; j++)
	{
		free(r->patterns[i][j]);
		r->patterns[i][j] = NULL;
	}

	for (int i = 0; i < r->side; i++)
	{
		free(r->output[i]);
		r->output[i] = NULL;
	}
}

// checks whether given sector matches rule
static int rule_match(const rule *rule, char **src, int sector_x, int sector_y)
{
	for (int n = 0; n < 8; n++)
	{
		bool matches = true;

		for (int i = 0; matches && i < rule->side; i++)
		for (int j = 0; matches && j < rule->side; j++)
			matches = rule->patterns[n][i][j] == src[sector_y * rule->side + i][sector_x * rule->side + j];

		if (matches) return n;
	}

	return -1;
}

// outputs rule to given sector in output
static void rule_apply(const rule *rule, int sector_x, int sector_y, char **dst)
{
	int side = rule->side + 1;
	for (int i = 0; i < side; i++)
	for (int j = 0; j < side; j++)
	{
		//printf("Writing %c to [%d, %d]\n",rule->output[i][j],sector_y * side + i,sector_x * side + j);
		dst[sector_y * side + i][sector_x * side + j] = rule->output[i][j];
		//printf("\tWritten\n");
	}
}

static plane* plane_apply(plane *p, rule **rules, int rules_size)
{
	plane *r = malloc(sizeof(plane));
	r->side = p->side % 2 ? 4 * p->side / 3 : 3 * p->side / 2;

	// allocate plane
	r->plane = malloc(r->side * sizeof(char*));
	for (int i = 0; i < r->side; i++)
	{
		r->plane[i] = calloc(r->side + 1, sizeof(char));
	}
	
	// determine which rules to use
	int rule_side = p->side % 2 ? 3 : 2;
	int nsectors = p->side / rule_side;

	// traverse each sector, find rule, apply it and copy to result
	for (int i = 0; i < nsectors; i++)
	for (int j = 0; j < nsectors; j++)
	{
		for (int rule = 0; rule < rules_size; rule++)
		{
			int n;
			if (rules[rule]->side == rule_side
					&& 0 <= (n = rule_match(rules[rule], p->plane, i, j)))
				rule_apply(rules[rule], i, j, r->plane);
		}
	}




	return r;
}

static void plane_print(const char *str, plane *p)
{
	printf("[%s] Plane %d x %d:\n", str, p->side, p->side);
	pattern_print(p->plane, p->side);
}

static void plane_destroy(plane *p)
{
	for (int i = 0; i < p->side; i++)
		FREE(p->plane[i]);

	FREE(p->plane);
}

static int plane_count_on(plane *p)
{
	int r = 0;
	for (int i = 0; i < p->side; i++)
	for (int j = 0; j < p->side; j++)
		if (p->plane[i][j] == '#')
			r++;
	return r;
}
