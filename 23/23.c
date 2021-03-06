#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct state;

typedef struct inst
{
	char name[4];
	bool xreg, yreg;
	int x, y;
	int (*func)(const struct inst *i, struct state *st);
} inst;

typedef struct state
{
	int ip;

	int ninst;
	int instructions_capacity;
	inst *instructions;

	int mul_exec;

	long reg[26];
} state;

void regorval(const char *s, int *v, bool *reg)
{
	if (*reg = isalpha(*s))
		*v = *s - 'a';
	else
		*v = atoi(s);
}

inst set, sub, mul,jnz;

int set_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("set error, x not reg\n");
	st->reg[i->x] = i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int sub_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("sub error, x not reg\n");
	st->reg[i->x] -= i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int mul_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("mul error, x not reg\n");
	st->reg[i->x] *= i->yreg ? st->reg[i->y] : i->y;
	st->mul_exec++;
	return 1;
}

int jnz_func(const struct inst *i, struct state *st)
{
	if (i->xreg)
		return st->reg[i->x] ? i->y : 1;
	else
		return i->x ? i->y : 1;
}

void state_add_instruction(state *st, inst i)
{
	st->instructions[st->ninst++] = i;
	if (st->ninst == st->instructions_capacity)
		st->instructions = realloc(st->instructions, sizeof(inst) * (st->instructions_capacity *= 2));
}

bool state_step(state *st)
{
	int a, b;
	a = st->ip;
	st->ip += st->instructions[st->ip].func(st->instructions + st->ip, st); 
	b = st->ip;
	//printf("IP: %d -> %d\n", a, b);
	return st->ip >= 0 && st->ip < st->ninst;
}

int main()
{
	state st;
	st.ip = 0;
	st.mul_exec = 0;
	st.ninst = 0;
	st.instructions_capacity = 10;
	st.instructions = malloc(sizeof(inst) * 10);
	memset(st.reg, 0, sizeof(int) * 26);

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *line = NULL;
		size_t sz = 0;

		char name[4], p1[10], p2[10];
		inst ni;
		memset(name, 0, sizeof(char) * 4);
		memset(p1, 0, sizeof(char) * 10);
		memset(p2, 0, sizeof(char) * 10);
		if (EOF == fscanf(f, "%s", name))
			break;
		printf("%s\n", name);
		strcpy(ni.name, name);
		if (0 == strcmp("set", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = set_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else if (0 == strcmp("sub", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = sub_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else if (0 == strcmp("mul", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = mul_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else if (0 == strcmp("jnz", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = jnz_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else 
		{
			printf("?: %s\n", name);
			break;
		}

		state_add_instruction(&st, ni);

		printf("%s %s %s\n", name, p1, p2);
	}
	fclose(f);

	printf("Program loaded, %d instructions\n", st.ninst);
	for (int i = 0; i < st.ninst; i++)
	{
		printf("%d: %s\n", i, st.instructions[i].name);
	}

	int n = 0;

	while (state_step(&st)) n++;
	printf("Part 1: %d\n", st.mul_exec);


	printf("done\n");
	return 0;
}
