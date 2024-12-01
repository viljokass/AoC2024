#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Get the size of the input
int get_number_of_lines(FILE* fd) {
  int size = 0;
  char* dummy = malloc(25 * sizeof(char));
  while(fgets(dummy, 25, fd)) {
    size++;
  }
  rewind(fd);
  free(dummy);
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

// Returns the index of the smallest unused number
int get_smallest_unused(int* data, bool* usage, int size) {
  int smallest_index  = -1;
  int smallest_number = 10e7;
  for (int i = 0; i < size; ++i) {
    if (!usage[i]) {
      if (data[i] < smallest_number) {
        smallest_index = i;
        smallest_number = data[i];
      }
    }
  }
  // Set the usage of the smallest to true
  usage[smallest_index] = true;
  return data[smallest_index];
}

int int_abs(int lhs, int rhs) {
  int x = lhs - rhs;
  if (x < 0) return -x;
  return x;
}

// The function to do the thing
int do_the_thing(int* left, int* right, bool* left_usage, bool* right_usage, int size) {
  int distance = 0;
  int current_left;
  int current_right;
  for (int i = 0; i < size; ++i) {
    current_left = get_smallest_unused(left, left_usage, size);
    current_right = get_smallest_unused(right, right_usage, size);
    distance += int_abs(current_left, current_right);
  }
  return distance;
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

  // boolean lists to keep track of which numbers have been used. Format to false.
  bool* left_usage  = malloc(size * sizeof(bool));
  bool* right_usage = malloc(size * sizeof(bool));
  for (int i = 0; i < size; ++i) {
    left_usage[i]   = false;
    right_usage[i]  = false;
  }

  // Do the thing that was asked
  int result = do_the_thing(left, right, left_usage, right_usage, size);
  printf("The total distance is %d\n", result);

  // Free the arrays
  free(left);
  free(right);
  free(left_usage);
  free(right_usage);
  return 0;
}
