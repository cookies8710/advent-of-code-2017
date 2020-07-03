#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int part1(const char *list);
int part2(const char *list);

int main(int argc, const char **argv)
{
  size_t size;
  char *input = 0;
  FILE *f = fopen("input", "r");
  getline(&input, &size, f);
  fclose(f);

  printf("Part 1 answer is %d\n", part1(input));
  printf("Part 2 answer is %d\n", part2(input));
  printf("DONE\n");

  free(input);
  
  return 0;
}

int part1(const char *list)
{
  if (!*list) 
    return 0;

  // init with last digit if it's the same as first one
  int sum = *list == list[strlen(list) - 1] ? *list - '0' : 0;

  // add all digits which match neighbour
  for (; *list; list++)
  	if (*list == list[1])
	  sum += *list - '0';

  return sum;
}

int part2(const char *list)
{
  if (!*list) 
    return 0;

  int sum = 0;
  int len = strlen(list);
  int shift = len / 2;

  const char *start = list;

  // add all digits which match the digit half way across the list
  for (; *list; list++)
  	if (*list == start[(list - start + shift) % len])
	  sum += *list - '0';

  return sum;
}

