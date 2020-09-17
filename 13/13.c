#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

bool hit(unsigned int time, unsigned int range)
{
	if (range == 1)
		return true;

	return time % (2 * (range - 1)) == 0;
}

void add(unsigned int **array, int *capacity, int *size, unsigned int n)
{
	(*array)[(*size)++] = n;
	if (*size == *capacity)
		*array = realloc(*array, sizeof(unsigned int) * ((*capacity) *=2));
}

int main()
{
	int capacity = 10;
	int size = 0;
	
	// doublets - depth: range
	unsigned int *firewall;

	firewall = malloc(capacity * sizeof(unsigned int));

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		int a, b;
		fscanf(f, "%d: %d", &a, &b);
		add(&firewall, &capacity, &size, a);
		add(&firewall, &capacity, &size, b);
	}
	fclose(f);

	int n = size / 2;

	int sum = 0;
	for (int i = 0; i < n; i++)
		if (hit(firewall[2*i], firewall[2*i + 1]))
			sum += firewall[2*i] * firewall[2*i + 1];
	printf("Trip severity (Par 1): %d\n", sum);
	
	bool caught = true;
	int delay = 0;
	for (; caught; delay++)
	{
		caught = false;
		for (int i = 0; i < n && !caught; i++)
			caught = hit(delay + firewall[2*i], firewall[2*i + 1]);
	}
	printf("Minimal delay for not to be caught (Part 2): %d\n", delay - 1);

	printf("done\n");
	return 0;
}
