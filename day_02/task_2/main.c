#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Enumeration for direction
enum direction {
  undef,
  desc,
  asc
};

int int_abs(int x) {
  if ( x < 0 ) return -x;
  return x;
}

// Counts the nums in line
int count_nums(const char* line) {
  int count = 0;
  for ( int i = 0; i < 100; ++i ) {
    if ( line[i] == '\n') {
      count++;
      break;
    }
    if ( line[i] == ' ' ) count++;
  }
  return count;
}

// Parse numbers from the text
void parsenums(const char* line, int* nums) {
  int index = 1;
  nums[0] = atoi(line);
  for (int i = 0; i < 100; ++i) {
    if ( line[i] == '\n') {
      break;
    }
    if ( line[i] == ' ' ) {
      nums[index] = atoi(line + i + 1);
      index++;
    }
  }
}

// Check if the line is safe if one is removed
bool check_line_with_pardon(int* nums, int size, int pardon_index) {
  enum direction dir = undef;

  int start_index = 0;
  if ( pardon_index == 0 ) start_index = 1;
  int prev = nums[start_index];
  int curr;

  for ( int i = start_index + 1; i < size; ++i ) {
    if ( i == pardon_index ) continue;
    curr = nums[i];
    if ( dir == undef ) {
      if ( prev - curr < 0 ) dir = asc;
      if ( prev - curr > 0 ) dir = desc;
    }
    if ( dir == asc ) {
      if ( prev - curr > 0 ) return false;
    }
    if ( dir == desc ) {
      if ( prev - curr < 0 )  return false;
    }
    if ( prev == curr ) return false;
    if ( int_abs(prev - curr) > 3 ) return false;
    prev = curr;
  }
  return true;
}

// Check a line
bool is_line_safe(const char* line) {
  int numcount = count_nums(line);
  int* nums = malloc(numcount * sizeof(int));
  parsenums(line, nums);

  bool safe = check_line_with_pardon(nums, numcount, -1);
  for (int i = 0; i < numcount; ++i)
    safe |= check_line_with_pardon(nums, numcount, i);
  free(nums);
  return safe;
}

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    printf("Define input, please.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file.\n");
    return 1;
  }

  // Safe lines count
  int safes = 0;

  // Buffer for line reading
  char* buffer = malloc(100 * sizeof(char));
  while(fgets(buffer, 100, fd)) {
    // Handle a row
    if (is_line_safe(buffer)) safes++;
  }
  
  // output the result
  printf("Safe reports: %d\n", safes);

  // Free buffer and close the file.
  free(buffer);
  fclose(fd);
  return 0;
}
