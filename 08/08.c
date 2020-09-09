#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <stdbool.h>

#define INC 1
#define DEC 2

#define LESS 1
#define EQUALS 2
#define LESS_EQUALS 3
#define NOT_EQUALS 4
#define GREATER_EQUALS 5
#define GREATER 6

typedef struct instruction 
{
	char *regname;
	int operation;
	int value;
	char *condition_register;
	int condition_operator;
	int condition_value;
} instruction;

typedef struct reg
{
	char *regname;
	int value;
} reg;

typedef struct state
{
	int capacity;
	int nregs;
	reg *regs;
} state;

static void instruction_parse(instruction *i, const char *str);
static void instruction_print(const instruction *i);

static reg* state_find_reg_by_name(state *s, const char *regname);
static void state_execute_instruction(state *s, const instruction *i, int *total_max);

int main(int argc, const char **argv)
{
	printf("\n\nstart\n");
	instruction instr;

	state st;
	st.capacity = 10;
	st.nregs = 0;
	st.regs = malloc(10*sizeof(reg));

	int total_max = 0;

	FILE *f = fopen("input", "r");
	while (!feof(f))
	{
		char *line = NULL;
		size_t n = 0;
		int r = getline(&line, &n, f);
		if (r <= 0)
			break;
		instruction_parse(&instr, line);
		state_execute_instruction(&st, &instr, &total_max);
	}
	fclose(f);

	printf("Final state: %d registers:\n", st.nregs);
	int max = st.regs[0].value;
	for (const reg* i = st.regs; i < st.regs + st.nregs; i++)
	{
		printf("%s = %d\n", i->regname, i->value);
		if (i->value > max)
			max = i->value;
	}
	printf("Part 1 (largest value): %d\n", max);
	printf("Part 2 (max value): %d\n", total_max);
	printf("done\n");
	return 0;
}

// allocating strcpy
static void as(char **dest, const char *src)
{
	int len = strlen(src) + 1;
	*dest = malloc(len);
	memcpy(*dest, src, len);
}

static void instruction_parse(instruction *i, const char *str)
{
	char *me;
	as(&me, str);

	// register
	char *t = strtok(me, " ");
	as(&i->regname, t);

	// opration
	t = strtok(NULL, " ");
	if (strcmp(t, "inc") == 0)
		i->operation = INC;
	else if (strcmp(t, "dec") == 0)
		i->operation = DEC;
	else
		printf("Unknown operation: %s\n", t);

	// value
	t = strtok(NULL, " ");
	i->value = atoi(t);
	
	// "if"
	t = strtok(NULL, " ");
	if (strcmp(t, "if") != 0)
		printf("Unexpected string: %s\n", t);

	// condition register
	t = strtok(NULL, " ");
	as(&i->condition_register, t);

	// condition operator
	t = strtok(NULL, " ");
	if (strcmp(t, "<") == 0)
		i->condition_operator = LESS;
	else if (strcmp(t, "<=") == 0)
		i->condition_operator = LESS_EQUALS;
	else if (strcmp(t, "!=") == 0)
		i->condition_operator = NOT_EQUALS;
	else if (strcmp(t, ">") == 0)
		i->condition_operator = GREATER;
	else if (strcmp(t, ">=") == 0)
		i->condition_operator = GREATER_EQUALS;
	else if (strcmp(t, "==") == 0)
		i->condition_operator = EQUALS;
	else
		printf("Unknown condition operator: %s\n", t);

	t = strtok(NULL, " ");
	i->condition_value = atoi(t);

	free(me);
}

static void instruction_print(const instruction *i)
{
	printf("register: %s, operation: %d, value: %d, condition_register: %s, condition_operator: %d, condition_value: %d\n", i->regname, i->operation, i->value, i->condition_register, i->condition_operator, i->condition_value);
}

static reg* state_find_reg_by_name(state *s, const char *regname)
{
	for (int i = 0; i < s->nregs; i++)
		if (strcmp(s->regs[i].regname, regname) == 0)
			return s->regs + i;

	// assert there's room 
	if (++s->nregs == s->capacity)
		s->regs = realloc(s->regs, (s->capacity *= 2) * sizeof(reg));

	// register doesn't exist yet, initialize it
	reg *r = s->regs + s->nregs - 1;
	as(&r->regname, regname);
	r->value = 0;

	return r;
}

static void state_execute_instruction2(state *s, const instruction *i)
{
	reg *r = state_find_reg_by_name(s, i->condition_register);
}

static void state_execute_instruction(state *s, const instruction *i, int *total_max)
{
	instruction_print(i);
	reg *r = state_find_reg_by_name(s, i->condition_register);
	bool ch = false;
	switch (i->condition_operator)
	{
		case LESS:
			ch = r->value < i->condition_value;
			break;
		case EQUALS:
			ch = r->value == i->condition_value;
			break;
		case LESS_EQUALS:
			ch = r->value <= i->condition_value;
			break;
		case NOT_EQUALS:
			ch = r->value != i->condition_value;
			break;
		case GREATER_EQUALS:
			ch = r->value >= i->condition_value;
			break;
		case GREATER:
			ch = r->value > i->condition_value;
			break;
	}

	if (ch)
	{
		printf("condition holds, executing\n");
		reg *r = state_find_reg_by_name(s, i->regname);
		switch (i->operation)
		{
			case DEC:
				printf("%s -= %d\n", i->regname, i->value);
				r->value -= i->value;
				printf("%s = %d\n", i->regname, r->value);
				break;
			case INC:
				printf("%s += %d\n", i->regname, i->value);
				r->value += i->value;
				break;
		}

		if (r->value > *total_max)
			*total_max = r->value;
	}
}
