#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct 
{
   int *instr;
   int count;
   int current;
} jumps;

static void jumps_print(const jumps *j);
static jumps jumps_copy(const jumps *j);
static bool jumps_step(jumps *j, int (*inc)(int*));
static void jumps_destroy(jumps *j);

static int part1_inc(int *instr);
static int part2_inc(int *instr);

int main(int argc, const char **argv)
{
  // read jumps from file to `tape`
  FILE *input = fopen("input", "r");

  int *tape = NULL;
  int capacity = 16;
  int size = 0;

  tape = (int*) malloc(sizeof(int) * capacity);

  while (!feof(input))
  {
    char *line = NULL;
    size_t sz = 0;
    if ( 0 > getline(&line, &sz, input))
       break;
    if (size == capacity)
	    tape = realloc(tape, sizeof(int) * (capacity *= 2));
    tape[size++] = atoi(line);
  }

  fclose(input);
  
  // create jumps structure and copy it to `jmp2` for part 2 run (jumps_step mutates the structure)
  jumps jmp;
  jmp.current = 0;
  jmp.instr = malloc(sizeof(int)*size);
  memcpy(jmp.instr, tape, sizeof(int)*size);
  jmp.count = size;
  free(tape);

  jumps jmp2 = jumps_copy(&jmp);

  // count how many steps are taken until escape
  int steps_part1, steps_part2;
  for (steps_part1=1; jumps_step(&jmp, part1_inc); steps_part1++);
  for (steps_part2=1; jumps_step(&jmp2, part2_inc); steps_part2++);

  jumps_destroy(&jmp);
  jumps_destroy(&jmp2);

  printf("Part 1 steps: %d\n", steps_part1);
  printf("Part 2 steps: %d\n", steps_part2);
  return 0;
}

static void jumps_print(const jumps *j)
{
  for (int i = 0; i < j->count; i++)
    printf(i == j->current ? "(%d)\n" : "%d\n", j->instr[i]);
}

static jumps jumps_copy(const jumps *j)
{
   jumps copy;
   copy.current = j->current;
   copy.count = j->count;
   copy.instr = malloc(sizeof(int) * j->count);
   memcpy(copy.instr, j->instr, sizeof(int) * j->count);
   return copy;
}

static bool jumps_step(jumps *j, int (*inc)(int*))
{
   // jump to next instruction while modifying the current one
   j->current += inc(j->instr + j->current);

   // T iff the new index is still within maze
   return j->current >= 0 && j->current < j->count;
}

static void jumps_destroy(jumps *j)
{
   j->current = -1;
   j->count = 0;
   free(j->instr);
   j->instr = NULL;
}

static int part1_inc(int *instr)
{
   return (*instr)++;
}

static int part2_inc(int *instr)
{
   return *instr >= 3 ? (*instr)-- : (*instr)++;
}
