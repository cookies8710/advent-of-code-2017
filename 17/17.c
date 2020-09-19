#include <stdio.h>
#include <stdlib.h>

#define SKIP 324

typedef struct node
{
	struct node *next;
	int value;
} node;

void insert(node **current, int new_value, node *new_node)
{
	for (int i = 0; i < SKIP; i++)
		(*current) = (*current)->next;
	new_node->value = new_value;

	new_node->next = (*current)->next;
	(*current)->next = new_node;
	*current = new_node;
}

int main()
{
	const int pool_size = 1000000;
	int pool_used = 0;
	node *node_pool = malloc(pool_size * sizeof(node));

	int size = 0;
	node *current = malloc(sizeof(node));
	current->next = current;
	current->value = size++;

	for(;size < 50000000;) 
	{
		if (0 == (size % 1000000)) printf("%d M\n", size / 1000000);
		insert(&current, size++, node_pool + pool_used++);
		if (pool_used == pool_size)
		{
			pool_used = 0;
			node_pool = malloc(pool_size * sizeof(node));
		}
		if (size == 2018)
			printf("Part 1: %d\n", current->next->value);
	}

	printf("Inserted 50M, will look for 0\n");
	while (current->value) current = current->next;
	printf("Part 2: %d\n", current->next->value);

	printf("done\n");
	return 0;
}
