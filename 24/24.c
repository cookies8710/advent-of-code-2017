#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct comp
{
	unsigned char a, b;
} comp;

static void filter(comp **components, int n, unsigned char ending, comp ***filtered, int *matching);
static int eval(comp **branch, int branch_size, unsigned char ending, comp **remaining, int rem_size, int *max_length, int *max);
static void comp_print(const char *str, comp **c, int n);

int main()
{
	int capacity = 10, size = 0;
	comp** components = malloc(sizeof(comp*) * capacity);

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *c=0;
		size_t s=0;
		int ret = getline(&c, &s, f);
		if (ret>0)
		{
			int a = 0, b = 0;
			while (*c && *c != '/')
			{
				a = 10 * a + *c - '0';
				c++;
			}
			c++;
			while (isdigit(*c))
			{
				b = 10 * b + *c - '0';
				c++;
			}

			components[size] = malloc(sizeof(comp));
			components[size]->a = a;
			components[size]->b = b;
			size++;
			if (size == capacity)
				components = realloc(components, sizeof(comp*) * (capacity *= 2));
		}
	}
	fclose(f);

	comp_print("Components read", components, size);

	int longest_size = 0, longest_strength = 0;
	int max_strength = eval(NULL, 0, 0, components, size, &longest_size, &longest_strength);
	printf("max_strength (Part 1 answer) = %d, longest_length: %d, longest_strength (Part 2 answer): %d\n", max_strength, longest_size, longest_strength);


	free(components);
	components = NULL;

	printf("done\n");
	return 0;
}

static void filter(comp **components, int n, unsigned char ending, comp ***filtered, int *matching)
{
	// count how many components have a matching ending
	*matching = 0;
	for (int i = 0; i < n; i++) if (components[i]->a == ending || components[i]->b == ending) (*matching)++;

	// allocated target memory and copy ptrs to matching components
	if (*matching)
		*filtered = malloc(sizeof(comp*) * (*matching));
	else
		*filtered = NULL;

	for (int i = 0, k = 0; i < n; i++) 
	{
		if (components[i]->a == ending || components[i]->b == ending) 
			(*filtered)[k++] = components[i];
	}
}

static int eval(comp **branch, int branch_size, unsigned char ending, comp **remaining, int rem_size, int *longest_length, int *longest_strength)
{
	comp **filtered = NULL; 
	int nf = 0;

	filter(remaining, rem_size, ending, &filtered, &nf);
	if (nf == 0) 
	{
		int sum = 0;
		for (int i = 0; i < branch_size; i++)
			sum += branch[i]->a + branch[i]->b;

		if (branch_size > *longest_length)
		{
			*longest_length = branch_size;
			*longest_strength = sum;
		}
		else if (branch_size == *longest_length && sum > *longest_strength)
		{
			*longest_strength = sum;
		}

		return sum;
	}

	int max_strength = 0;
	for (int i = 0; i < nf; i++)
	{
		// enlarge branch by the component filtered[i]
		comp **new_branch = malloc(sizeof(comp*) * branch_size + 1);
		memcpy(new_branch, branch, sizeof(comp*) * branch_size);
		new_branch[branch_size] = filtered[i];

		// remove from remaining
		comp **new_rem = NULL;
		if (rem_size - 1)
		{
			new_rem = malloc(sizeof(comp*) * (rem_size - 1));
			int idx = 0;
			for (int j = 0; j < rem_size; j++)
				if (remaining[j] != filtered[i])
					new_rem[idx++] = remaining[j]; 
		}

		int tmp = eval(new_branch, branch_size + 1, 
				filtered[i]->a == ending ? filtered[i]->b : filtered[i]->a,
				new_rem, 
				rem_size - 1,
				longest_length, longest_strength);
		free(new_branch);
		new_branch = NULL;
		if (new_rem)
		{
			free(new_rem);
			new_rem = NULL;
		}
		if (tmp > max_strength)
			max_strength = tmp;
	}

	free(filtered);
	filtered = NULL;

	return max_strength;
}

static void comp_print(const char *str, comp **c, int n)
{
	printf("[%s] %d:\n", str, n);
	if (c)
		for (int i = 0; i < n; i++)
			printf("\t%d/%d\n", c[i]->a, c[i]->b);
}
