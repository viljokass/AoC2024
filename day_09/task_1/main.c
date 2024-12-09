#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Get the size of memory to be allocated
int get_mem_size(FILE* fd) {
  int size = 0;
  char c = getc(fd);
  while(c != '\n') {
    size += c - '0';
    c = getc(fd);
  }
  rewind(fd);
  return size;
}

// Parse the file to the int array
void parse(FILE* fd, int* data) {
  int index = 0;
  int data_index = 0;
  bool set_data = true;
  char c = getc(fd);
  while(c != '\n') {
    // Use character arithmetic to get the number from character
    int lim = c - '0';
    for (int i = 0; i < lim; ++i) {
      if (set_data) {
        data[data_index] = index;
      } else {
        data[data_index] = -1;
      }
      data_index++;
    }
    if (set_data) index++;
    // Alternate the data setting
    set_data = !set_data;
    c = getc(fd);
  }
}

// Get next empty place's index from data starting from the beginning.
int get_next_empty(int* data, int start_index) {
  int index = start_index;
  while(data[index] != -1) index++;
  return index;
}

// Get previous nonempty place's index from data, starting from the end.
int get_prev_nonempty(int* data, int start_index) {
  int index = start_index;
  while(data[index] == -1) index--; 
  return index;
}

// Compress the data -> swap the rightmost 
// nonempty item to leftmost empty place
void compress(int* data, int size) {
  int swap_index_1 = get_next_empty(data, 0);
  int swap_index_2 = get_prev_nonempty(data, size - 1);
  while (swap_index_1 < swap_index_2) {
    data[swap_index_1] = data[swap_index_2];
    data[swap_index_2] = -1;
    swap_index_1 = get_next_empty(data, swap_index_1);
    swap_index_2 = get_prev_nonempty(data, swap_index_2);
  }
}

// Get the checksum from the data
unsigned long long check_sum(int* data, int size) {
  unsigned long long result = 0;
  for (int i = 0; i < size; ++i) {
    if (data[i] == -1) break;
    //idk if this cast is necessary but here we go anyways
    result += (unsigned long long) (data[i] * i);
  }
  return result;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 1;
  }
  
  // Get the size of the data to be allocated, then allocate and parse.
  int allocates = get_mem_size(fd);
  int* data = malloc(allocates * sizeof(int)); 
  parse(fd, data);

  // We can close the file now.
  fclose(fd);

  // Compress the data as per instructions and calculate the checksum.
  compress(data, allocates);
  unsigned long long checksum = check_sum(data, allocates);

  // Output the results
  printf("The result is %lld\n", checksum);

  // Free allocated mem
  free(data);
  return 0;
}
