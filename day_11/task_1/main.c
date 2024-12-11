#include <stdio.h>
#include <stdlib.h>

#define ull unsigned long long

// A return type for splitting the number in two
typedef struct _splitret {
  ull a;
  ull b;
} splitret;

int count_initial(char* buffer) {
  int count = 1;
  int index = 0;
  while (buffer[index] != '\n') {
    if (buffer[index] == ' ') {
      count++;
    }
    index++;
  }
  return count;
}

void parse(char* buffer, ull* init_rocks, int count) {
  int index = 0;
  int rindex = 1;
  init_rocks[0] = atoi(buffer);
  while(buffer[index] != '\n') {
    if (buffer[index] == ' ') {
      init_rocks[rindex] = atoi(buffer + index);
      rindex++;
    }
    index++;
  }
}

// count the digits of a number
int count_digits(ull number) {
  ull tmp = number;
  int dg = 0;
  while (tmp != 0) {
    tmp /= 10;
    dg++;
  }
  return dg;
}

// Split the number and return the split number
splitret split_num(ull number) {
  splitret split;
  int exponent = count_digits(number) / 2;
  ull a = number;
  for (int i = 0; i < exponent; ++i) a /= 10;
  split.a = a;
  for (int i = 0; i < exponent; ++i) a *= 10;
  split.b = number - a;
  return split;
}

// A recursive solver. Doesn't care about the order
// of the rocks, as doesn't the task despite bolding that.
int solve(ull rock, int iters) {
  if (iters == 0) return 1;
  if (rock == 0) return solve(1, iters - 1);
  if (count_digits(rock) % 2 == 0) {
    splitret split = split_num(rock);
    return solve(split.a, iters - 1) +
           solve(split.b, iters - 1);
  }
  else return solve(rock * 2024, iters - 1);
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 1;
  }

  // Get the entire input to buffer string and close file
  char* buffer = malloc(256 * sizeof(char));
  fgets(buffer, 256 * sizeof(char), fd);
  fclose(fd);

  // Count initial rocks
  int init_count = count_initial(buffer);
  ull* init_rocks = malloc(init_count * sizeof(ull));
  parse(buffer, init_rocks, init_count);

  // Count the total rocks after n blinks
  int total_count = 0;
  for (int i = 0; i < init_count; ++i) {
    total_count += solve(init_rocks[i], 25);
  }
  
  printf("The total count after 25 iterations is %d\n", total_count);

  free(buffer);
  return 0;
}
