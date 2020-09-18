#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 256

static void print(unsigned char *a, int current)
{
	for (int i = 0; i < SIZE; i++) printf(i == current ? "[%d] " : "%d ", a[i]);
	printf("\n");
}

static void apply(unsigned char *a, int *current, int *skip_size, int length)
{
	unsigned char *copy = malloc(SIZE * sizeof(unsigned char));
	memcpy(copy, a, SIZE * sizeof(unsigned char));

	for (int i = 0; i < length; i++)
	{
		int index = (*current + length - i - 1) % SIZE;
		a[(*current + i) % SIZE] = copy[index];
	}

	free(copy);

	*current = (*current + length + (*skip_size)++) % SIZE;
}

static void one_round(unsigned char *a, int *current, int *skip_size, int *lengths, int nlengths)
{
	for (int i = 0; i < nlengths; i++)
		apply(a, current, skip_size, lengths[i]);
}

static void empty_area(char *field, int x, int y)
{
	if (x < 0 || x > 127 || y > 127 || y < 0) return;
	if (field[y*128+x] == '#')
	{
		field[y*128+x] = '.';
		empty_area(field, x-1,y);
		empty_area(field, x+1,y);
		empty_area(field, x,y-1);
		empty_area(field, x,y+1);
	}
}

static void compute_hash(const char *is, unsigned char **tgt)
{
	int tail[] = {17, 31, 73, 47, 23};
	int tail_size = sizeof(tail)/sizeof(*tail);
	int *p2lengths = malloc(sizeof(int) * (strlen(is) + tail_size));
	int nlengths = strlen(is) + tail_size;
	int i = 0;
	for(; i < strlen(is); i++) p2lengths[i] = is[i];
	for(int j = 0; j < tail_size; j++, i++) p2lengths[i] = tail[j];

	int current = 0;
	int skip_size = 0;
	unsigned char array[SIZE];
	for (int i = 0; i < SIZE; i++) array[i] = i;
	for (int r = 0; r < 64; r++)
		one_round(array, &current, &skip_size, p2lengths, nlengths);


	*tgt = malloc(16 * sizeof(unsigned char));
	for (int block = 0; block < 16; block++)
	{
		(*tgt)[block] = array[block * 16];
		for (int i = 1; i < 16; i++)
			(*tgt)[block] ^= array[i + block * 16];
	}
}

int main(void)
{
	//const char *x = "flqrgnkx";
	const char *x = "hwlqcszp";
	int l = strlen(x) + 5; // 1 dash, 3 digits, 1 null char
	char row_string[l];
	int sum = 0;

	char field[128][128];

	
	for (int i = 0; i < 128; i++)
	{
		memset(row_string, 0, sizeof(char) * l);
		sprintf(row_string, "%s-%d", x, i);

		unsigned char *dense_hash = NULL;
		compute_hash(row_string, &dense_hash);
		for (int j = 0; j < 16; j++)
		for (int k = 0, p = 128; k < 8; k++, p>>=1) 
		{
			if (p & dense_hash[j])
			{
				field[i][j*8+k] = '#';
				sum++;
			}
			else
				field[i][j*8+k] = '.';
		}
		free(dense_hash);
	}

	// Part 2
	bool empty = false;
	int areas = 0;
	while (!empty)
	{
		empty = true;
		for (int y = 0; y < 128; y++)
		for (int x = 0; x < 128; x++)
			if (field[y][x] == '#')
			{
				empty = false;
				areas++;
				empty_area(field, x, y);
			}
	}

	printf("Part 1: %d\n", sum);
	printf("Part 2: %d\n", areas);

	printf("done\n");

	return 0;
}
