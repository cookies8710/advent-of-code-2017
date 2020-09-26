#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//#define VERBOSE 
struct state;

#define NREGS 26
#define RA ('a' - 'a')
#define RB ('b' - 'a')
#define RF ('f' - 'a')
#define RI ('i' - 'a')
#define RP ('p' - 'a')

typedef long type;

typedef struct inst
{
	char name[4];
	bool xreg, yreg;
	int x, y;
	int (*func)(const struct inst *i, struct state *st);
} inst;

typedef struct state
{
	int id;
	int ip;

	int ninst;
	int instructions_capacity;
	inst *instructions;

	int rcv_queue_size;
	int rcv_queue_capacity;
	type *rcv_queue;

	bool dispatch;
	long dispatch_value;

	int dispatch_count;

	bool blocked;
	bool halted;

	type reg[NREGS];
} state;

void state_print(const state *st)
{
	printf("%d, IP: %d, RQ: %d, Registers:  a: %ld,  b: %ld,  f: %ld,  i: %ld, p: %ld\n", 
			st->id, st->ip, st->rcv_queue_size, st->reg[RA], st->reg[RB], st->reg[RF], st->reg[RI], st->reg[RP]);
	printf("RQ (%d): ",st->rcv_queue_size);
	for (int i = 0; i< st->rcv_queue_size;i++)printf("[%d]= %ld, ",i, st->rcv_queue[i]);
	printf("\n");
}

state* state_copy(const state *st)
{
	state *r = malloc(sizeof(state));

	r->ip = st->ip;
	r->ninst = st->ninst;
	r->instructions_capacity = st->instructions_capacity;
	r->instructions = malloc(sizeof(inst) * r->instructions_capacity);
	memcpy(r->instructions, st->instructions, r->ninst * sizeof(inst));

	r->rcv_queue_size = 0;
	r->rcv_queue_capacity = 10;
	r->rcv_queue = malloc(10 * sizeof(type));

	memcpy(r->reg, st->reg, sizeof(type) * NREGS);

	r->blocked = st->blocked;
	r->halted = st->halted;
	r->dispatch = st->dispatch;
	r->dispatch_count = st->dispatch_count;

	return r;
}

void regorval(const char *s, int *v, bool *reg)
{
	if (*reg = isalpha(*s))
		*v = *s - 'a';
	else
		*v = atoi(s);
}

inst snd, set, add, mul, mod, rcv, jgz;

int snd_func(const struct inst *i, struct state *st)
{
	st->dispatch = true;
	st->dispatch_value = i->xreg ? st->reg[i->x] : i->x;
	return 1;
}

int set_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("set -error, x not reg\n");
	st->reg[i->x] = i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int add_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("add - error, x not reg\n");
	st->reg[i->x] += i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int mul_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("mul- error, x not reg\n");
	st->reg[i->x] *= i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int mod_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("mod error, x not reg\n");
	st->reg[i->x] %= i->yreg ? st->reg[i->y] : i->y;
	return 1;
}

int rcv_func(const struct inst *i, struct state *st)
{
	if (!i->xreg)
		printf("rcv error, x not reg\n");
	if (st->rcv_queue_size == 0)
	{
		st->blocked = true;
		return 0;
	}
	else
	{
		st->blocked = false;
		type val = st->reg[i->x] = st->rcv_queue[0];
		st->rcv_queue_size--;
		for (int i = 0; i < st->rcv_queue_size; i++)
			st->rcv_queue[i] = st->rcv_queue[i + 1];
	}
	return 1;
}

int jgz_func(const struct inst *i, struct state *st)
{
	return (i->xreg ? st->reg[i->x] : i->x) > 0 
		? (i->yreg ? st->reg[i->y] : i->y)
		: 1;
}

void state_add_instruction(state *st, inst i)
{
	st->instructions[st->ninst++] = i;
	if (st->ninst == st->instructions_capacity)
		st->instructions = realloc(st->instructions, sizeof(inst) * (st->instructions_capacity *= 2));
}

bool state_step(state *st, state *other)
{
	if (st->halted)
		return false;
	st->ip += st->instructions[st->ip].func(st->instructions + st->ip, st); 

	if (st->dispatch)
	{
		st->dispatch = false;
		other->rcv_queue[other->rcv_queue_size++] = st->dispatch_value;

#ifdef VERBOSE
		printf("Dispatched (%d -> %d), %d's queue (%d): ", st->id, other->id, other->id, other->rcv_queue_size);
		for (int i = 0; i <other->rcv_queue_size;i++)printf("%ld, ", other->rcv_queue[i]);
		printf("\n");
#endif

		if (other->rcv_queue_size == other->rcv_queue_capacity)
			other->rcv_queue = realloc(other->rcv_queue, sizeof(type) * (other->rcv_queue_capacity *= 2));
		st->dispatch_count++;
	}

	// deadlock
	if (st->blocked && (other->blocked || other->halted))
	{
		printf("Deadlock, %d blocked and %d blocked or halted\n", st->id, other->id);
		st->halted = true;
		return false;
	}

	st->halted = st->ip < 0 || st->ip >= st->ninst;
	if (st->halted)
		printf("%d halted\n", st->id);

	return !st->halted;
}

int main()
{
	state st;
	st.ip = 0;
	st.ninst = 0;
	st.blocked = false;
	st.halted = false;
	st.instructions_capacity = 10;
	st.instructions = malloc(sizeof(inst) * 10);
	st.dispatch = false;
	st.dispatch_count = 0;
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
		strcpy(ni.name, name);
		if (0 == strcmp("snd", name))
		{
			fscanf(f, "%s", p1);
			ni.func = snd_func;
			regorval(p1, &ni.x, &ni.xreg);
		}
		else if (0 == strcmp("set", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = set_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else if (0 == strcmp("add", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = add_func;
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
		else if (0 == strcmp("mod", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = mod_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else if (0 == strcmp("rcv", name))
		{
			fscanf(f, "%s", p1);
			ni.func = rcv_func;
			regorval(p1, &ni.x, &ni.xreg);
		}
		else if (0 == strcmp("jgz", name))
		{
			fscanf(f, "%s %s", p1, p2);
			ni.func = jgz_func;
			regorval(p1, &ni.x, &ni.xreg);
			regorval(p2, &ni.y, &ni.yreg);
		}
		else 
		{
			printf("?: %s\n", name);
			exit(1);
		}

		state_add_instruction(&st, ni);
	}
	fclose(f);

	state *p0, *p1;
	p0 = state_copy(&st); 
	p0->id = 0;
	p1 = state_copy(&st); 
	p1->id = 1;
	p1->reg[RP] = 1;

	int n = 0;
	while (!p0->halted || !p1->halted) 
	{
		state_step(p0, p1);
		state_step(p1, p0);

#ifdef DEBUG
		state_print(p0);
		state_print(p1);
		getchar();
#endif
		n++;
	}

	printf("Part 2: %d\n", p1->dispatch_count);

	printf("done\n");
	return 0;
}
