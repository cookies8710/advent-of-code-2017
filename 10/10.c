#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void)
{
	unsigned char array[SIZE];
	int current = 0;
	int skip_size = 0;
	for (int i = 0; i < SIZE; i++) array[i] = i;

	int lengths[] = {63,144,180,149,1,255,167,84,125,65,188,0,2,254,229,24};
	for (int i = 0; i < sizeof(lengths) / sizeof(*lengths); i++)
		apply(array, &current, &skip_size, lengths[i]);

	printf("Part 1: %d\n", array[0] * array[1]);

	const char *p2 = "63,144,180,149,1,255,167,84,125,65,188,0,2,254,229,24";
	int tail[] = {17, 31, 73, 47, 23};
	int tail_size = sizeof(tail)/sizeof(*tail);
	int *p2lengths = malloc(strlen(p2) + tail_size);
	int nlengths = strlen(p2) + tail_size;
	int i = 0;
	for(; i < strlen(p2); i++) p2lengths[i] = p2[i];
	for(int j = 0; j < tail_size; j++, i++) p2lengths[i] = tail[j];

	current = 0;
	skip_size = 0;
	for (int i = 0; i < SIZE; i++) array[i] = i;
	for (int r = 0; r < 64; r++)
		one_round(array, &current, &skip_size, p2lengths, nlengths);

	unsigned char dense_hash[16];
	for (int block = 0; block < 16; block++)
	{
		dense_hash[block] = array[block * 16];
		for (int i = 1; i < 16; i++)
			dense_hash[block] ^= array[i + block * 16];
	}

	printf("Part 2: ");
	for (int i = 0; i < 16; i++)
		printf("%02x", dense_hash[i]);
	printf("\n");

	printf("done\n");
	free(p2lengths);
	return 0;
}


