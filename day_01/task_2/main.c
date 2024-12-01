#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Get the size of the input
int get_number_of_lines(FILE* fd) {
  int size = 0;
  char dummy[100];
  while(fgets(dummy, 100, fd)) {
    size++;
  }
  rewind(fd);
  return size;
}

// This function parses the text
void parse(FILE* fd, int* left, int* right, int size) {
  char* line_buffer = malloc(32 * sizeof(char));
  char* num         = malloc(5 * sizeof(char));
  for (int i = 0; i < size; ++i) {
    fgets(line_buffer, 32, fd);
    for (int j = 0; j < 5; ++j) {
      num[j] = line_buffer[j];
    }
    left[i] = atoi(num);
    for (int j = 0; j < 5; ++j) {
      num[j] = line_buffer[j + 8];
    }
    right[i] = atoi(num);
  }
  free(line_buffer);
  free(num);
}

// Calculates how many whats are in the where array
int how_many(int what, int* where, int size) {
  int result = 0;
  for (int i = 0; i < size; ++i) {
    if (where[i] == what) result++;
  }
  return result;
}

// The function that does the thing
int do_the_thing(int* left, int* right, int size) {
  int result = 0;
  // Gonna do some redundant calculation
  for (int i = 0; i < size; ++i)
    result += left[i] * how_many(left[i], right, size);
  return result;
}

int main(void) {
  // File for reading the input (assumes there's a list named input)
  FILE* fd = fopen("input.txt", "r");
  if (fd == NULL) {
    printf("Unable to open the file.");
    return 1;
  }

  // Get the number of lines and allocate arrays accordingly and parse numbers
  int size = get_number_of_lines(fd);
  int* left   = malloc(size * sizeof(int));
  int* right  = malloc(size * sizeof(int));
  parse(fd, left, right, size);

  // We can close the file now.
  fclose(fd);

  // Do the thing that was asked
  int result = do_the_thing(left, right, size);
  printf("The result is %d\n", result);

  // Free the arrays
  free(left);
  free(right);
  return 0;
}
