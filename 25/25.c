#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st
{
	struct st* (*action[2])(char **current); 
} st;

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5

#define RIGHT (*current)++
#define LEFT (*current)--

st states[6];

st* A0(char **current)
{
	**current = 1;
	RIGHT;
	return states + B;
}

st* A1(char **current)
{
	**current = 0;
	LEFT;
	return states + F;
}

st* B0(char **current)
{
	RIGHT;
	return states + C;
}

st* B1(char **current)
{
	**current = 0;
	RIGHT;
	return states + D;
}

st* C0(char **current)
{
	**current = 1;
	LEFT;
	return states + D;
}

st* C1(char **current)
{
	RIGHT;
	return states + E;
}

st* D0(char **current)
{
	LEFT;
	return states + E;
}

st* D1(char **current)
{
	**current = 0;
	LEFT;
	return states + D;
}

st* E0(char **current)
{
	RIGHT;
	return states + A;
}

st* E1(char **current)
{
	RIGHT;
	return states + C;
}

st* F0(char **current)
{
	**current = 1;
	LEFT;
	return states + A;
}

st* F1(char **current)
{
	RIGHT;
	return states + A;
}

int main()
{
	states[A].action[0] = A0;
	states[A].action[1] = A1;
	states[B].action[0] = B0;
	states[B].action[1] = B1;
	states[C].action[0] = C0;
	states[C].action[1] = C1;
	states[D].action[0] = D0;
	states[D].action[1] = D1;
	states[E].action[0] = E0;
	states[E].action[1] = E1;
	states[F].action[0] = F0;
	states[F].action[1] = F1;

	int steps = 12994925;
	char *tape = calloc(steps * 2, sizeof(char));
	
	char *current = tape + steps;
	st *state = states;
	int step = steps;
	while(step--) state = state->action[*current](&current);

	int sum = 0;
	for (int i = 0; i < steps * 2; i++) sum += tape[i];

	printf("Part 1: %d\n", sum);

	free(tape);

	printf("done\n");
	return 0;
}
