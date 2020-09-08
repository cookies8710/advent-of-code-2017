#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct node 
{
	char *name;
	int weight;

	int nchildren;
	char **children_names;

	struct node *parent;
	struct node **children;
} node;

static void node_zero(node *n);
static node* node_find_by_name(node *nodes, const char *name, int nnodes);
static int node_total_weight(node *n);

// whether the node is balanced; in case it is not, the weights of majority and minority children written
static bool node_is_balanced(node *n, int *majority, int *minority, node **minority_node);

static bool node_subtree_balanced(node *n);

static void node_destroy(node *n);


int main(void)
{
	FILE *f = fopen("input", "r");

	int nnodes = 0;
	int nodes_capacity = 10;
	node *nodes = NULL;

	nodes = (node*)malloc(nodes_capacity * sizeof(node)); 

	while (!feof(f))
	{
		char *line = NULL;
		size_t sz = 0;

		int ret = getline(&line, &sz, f);
		if (ret < 0) break;

		if (nnodes == nodes_capacity)
		{
			nodes_capacity *= 2;
			nodes = realloc(nodes, nodes_capacity * sizeof(node));
		}

		node *new_node = nodes + nnodes++;
		node_zero(new_node);

		const char *p = line;
		while (*p != ' ') p++;

		new_node->name = malloc(sizeof(char) * (p - line) + 1);
		strncpy(new_node->name, line, p - line);
		new_node->name[p-line] = 0;

		// jump to the first digit with `line`
		line = p;
		while (!isdigit(*line)) line++;
		new_node->weight = 0;
		while (isdigit(*line))
		{
			new_node->weight = 10 * new_node->weight + (*line - '0');
			line++;
		}

		// children - at this point, just store the names, the tree will be built afterwards

		// jump to either first letter or end of line
		while (*line && !isalpha(*line)) line++;
		if (*line)
		{
			new_node->nchildren = 1;
			const char *comma = line;
			while (*comma) { if (*comma++ == ',') new_node->nchildren++;}

			new_node->children_names = (char**)malloc(sizeof(char*) * new_node->nchildren);
			int l = 0;
			p = line;
			while (*p)
			{
				if (isalpha(*p)) p++;
				else 
				{
					new_node->children_names[l] = malloc(sizeof(char) * (1 + p - line));
					memcpy(new_node->children_names[l], line, sizeof(char) * (p - line));
					new_node->children_names[l][p-line] = 0;
					l++;
					// move to next letter
					while (*p && !isalpha(*p)) p++;
					line = p;
				}
			}
		}
		else
			new_node->nchildren = 0;
	}
	fclose(f);

	// node information read, but tree not built (links to parent and children)
	for (int i = 0; i < nnodes; i++)
	{
		if (nodes[i].nchildren)
		{
			nodes[i].children = (node**)malloc(sizeof(node*) * nodes[i].nchildren);
			for (int l = 0; l < nodes[i].nchildren; l++)
			{
				node *child = node_find_by_name(nodes, nodes[i].children_names[l], nnodes);
				if (!child)
					printf("couldn't find node with name %s\n", nodes[i].children_names[l]);
				else
				{
					child->parent = nodes + i;
					nodes[i].children[l] = child;
				}

			}
		}
	}

	// Part 1 solution - root, a node without parent
	node *root = NULL;
	for (int i = 0; i < nnodes; i++)
		if (!nodes[i].parent)
		{
			if (root)
				printf("%s doesn't have parent, a root has already been found\n", nodes[i].name);
			else
				root = nodes + i;
		}
	printf("Part 1 - root: %s\n", root->name);

	// 
	int part2 = 0;
	for (int i = 0; i < nnodes; i++)
	{
		int ma, mi;
		node *minode;
		if (!node_is_balanced(nodes + i, &ma, &mi, &minode))
		{
			int diff = ma - mi;
			minode->weight += diff;
			if (node_subtree_balanced(root))
				printf("Part 2: setting %s's weight to %d balances the tree\n", minode->name, minode->weight);
			minode->weight -= diff;
		}
	}

	// cleanup
	for (int i = 0; i < nnodes; i++)
		node_destroy(nodes + i);
	free(nodes);
	nodes = NULL;
	
	printf("done\n");
	return 0;
}

static void node_zero(node *n)
{
	if (!n) return;

	n->name = NULL;
	n->weight = 0;
	n->nchildren = 0;
	n->children_names = NULL;
	n->parent = NULL;
	n->children = NULL;
}

static node* node_find_by_name(node *nodes, const char *name, int nnodes)
{
	if (!nodes || !name || nnodes <= 0) return NULL;

	for (int i = 0; i < nnodes; i++)
		if (strcmp(nodes[i].name, name) == 0)
			return nodes + i;
	return NULL;
}

static int node_total_weight(node *n)
{
	if (!n) return -1;

	int weight = n->weight;
	for (int i = 0; i < n->nchildren; i++)
		weight += node_total_weight(n->children[i]);
		
	return weight;
}

static bool node_is_balanced(node *n, int *majority, int *minority, node **minority_node)
{
	// nonexistent or without children is always balanced
	if (!n || !n->nchildren) return true;

	int a = 0, b = 0, a_c = 0, b_c = 0;

	a = node_total_weight(n->children[0]);
	a_c = 1;
	for (int i = 1; i < n->nchildren; i++)
	{
		int v = node_total_weight(n->children[i]);
		if (v == a)
			a_c++;
		else
		{
			b = v;
			b_c++;
		}
	}

	if (a_c && b_c)
	{
		if (majority && minority && minority_node)
		{
			if (a_c == 1)
			{
				*majority = b;
				*minority = a;
			}
			else if (b_c == 1)
			{
				*majority = a;
				*minority = b;
			}
			else
			{
				printf("warning, more than 1 imbalances\n");
			}

			for (int i = 1; i < n->nchildren; i++)
			{
				int v = node_total_weight(n->children[i]);
				if (v == *minority)
					*minority_node = n->children[i];
			}
		}

		return false;
	}

	return true;
}

static bool node_subtree_balanced(node *n)
{
	if (!node_is_balanced(n, NULL, NULL, NULL))
		return false;

	for (int i = 0; i < n->nchildren; i++)
		if (!node_subtree_balanced(n->children[i]))
			return false;


	return true;
}

static void node_destroy(node *n)
{
	free(n->name);
	n->name = NULL;

	for (int i = 0; i < n->nchildren; i++)
	{
		free(n->children_names[i]);
		n->children_names[i] = NULL;
	}
	free(n->children_names);
	n->children_names = NULL;
	
	n->parent = NULL;
	free(n->children);
	n->children = NULL;
}
