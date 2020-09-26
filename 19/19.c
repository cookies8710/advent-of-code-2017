#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct 
{
	int x, y;
} vec2;

static char move(char **maze, int maze_size, vec2 *pos, vec2 *vec);

int main()
{
	int maze_size = 0;
	int maze_capacity = 10;
	char **maze;
	maze = malloc(sizeof(char*) * maze_capacity);

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *line = NULL;
		size_t size = 0;
		if (0 > getline(&line, &size, f))
			break;

		int sz = 1 + strlen(line);
		maze[maze_size] = malloc(sz * sizeof(char));
		memcpy(maze[maze_size], line, sz);

		maze_size++;
		if (maze_size == maze_capacity)
			maze = realloc(maze, sizeof(char*) * (maze_capacity *= 2));

		free(line);
		line = NULL;
	}
	fclose(f);

	for (int i = 0; i < maze_size; i++)
		printf("%s", maze[i]);

	vec2 pos = {.y = 0}, dir = {.x = 0, .y = 1};
	for (int i = 0; i < strlen(maze[0]); i++)
		if (maze[0][i] == '|') 
			pos.x = i;

	printf("Starting @ %d, %d\n", pos.x, pos.y);

	char under = maze[pos.y][pos.x];
	int step = 0;
	while(under)
	{
		under = move(maze, maze_size, &pos, &dir);
		step++;
		if (isalpha(under))
			printf("%c", under);
	}
	printf("\n");

	printf("Steps: %d\n", step);

	// cleanup
	for (int i = 0; i < maze_size; i++)
	{
		free(maze[i]);
		maze[i] = NULL;
	}
	free(maze);
	maze = NULL;
	maze_size = maze_capacity = 0;

	printf("done\n");
	return 0;
}

static char move(char **maze, int maze_size, vec2 *pos, vec2 *vec)
{
	pos->x += vec->x;
	pos->y += vec->y;

	if (pos->x < 0 || pos->y < 0 || pos->y >= maze_size || pos->x >= strlen(maze[pos->y]))
		return 0;

	char result = maze[pos->y][pos->x];

	switch (result)
	{
		case '|':
		case '-':
			break;
		case ' ':
			return 0;
		case '+':// flip to a perpendicular
			if (vec->x) // horizontal -> vertical
			{
				vec->x = 0;
				if (pos->y && maze[pos->y-1][pos->x] != ' ')
					vec->y = -1;
				else
					vec->y = 1;
			}
			else // vertical -> horizontal
			{
				vec->y = 0;
				if (pos->x && maze[pos->y][pos->x-1] != ' ')
					vec->x = -1;
				else
					vec->x = 1;
			}
			break;
		default:
			if (!isalpha(result))
				printf("? %c (%d)\n", result, result);
	}
	return result;
}
