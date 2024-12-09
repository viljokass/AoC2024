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

// Get previous nonempty place's index from data, starting from the end.
int get_prev_nonempty(int* data, int start_index) {
  int index = start_index;
  while(data[index] == -1) index--; 
  return index;
}

// Get file size starting from end index
int get_file_size(int* data, int end_index) {
  int what = data[end_index];
  int index = end_index;
  while(data[index] == what) {
    index--;
  }
  return end_index - index;
}

// Returns the index of leftmost free block of a given size
int get_block_index(int* data, int data_size, int block_size) {
  int block_start = 0;
  int block_end = 0;
  while(block_start < data_size) {
    if (data[block_start] == -1) {
      block_end = block_start;
      while (data[block_end] == -1) {
        block_end++;
      }
      if (block_end - block_start >= block_size) return block_start;
      block_start = block_end;
    }
    block_start++;
  }
  return -1;
}

// Swap blocks. given are the file's end index, block's start index and file size.
void swap_block(int* data, int file_end, int file_size, int block_start) {
  // If file end is before the block's start, do not swap, we've gone too far.
  if (file_end < block_start) return;
  for (int i = 0; i < file_size; ++i) {
    data[block_start + i] = data[file_end - file_size + 1 + i];
    data[file_end - file_size + 1 + i] = -1;
  }
}

// Kinda terrible work, really wasn't all that sure this would work but it did.
// The basic idea is that we start going backwards form the right, and upon
// encountering a nonempty item, we check its length. Then we look or a free block
// of that size starting from the left. If one is found, we put the file to that
// place. Otherwise we let it stay where it is an we move on.
void compress(int* data, int size) {
  // get the first item's end index and init some vars
  int file_end = get_prev_nonempty(data, size - 1);
  int file_size = -1;
  int block_ind = -1;
  // Not sure about this termination condition, I'm sure this'll
  // add some completely unneccessary iterations, but hey, it works
  while (0 < file_end) {
    // Get size of the file
    file_size = get_file_size(data, file_end);
    // Get index of a free block
    block_ind = get_block_index(data, size, file_size);
    // If there's a free block, swap. Otherwise, move on.
    if (block_ind != -1)
      swap_block(data, file_end, file_size, block_ind);
    // Get new file end index
    file_end = get_prev_nonempty(data, file_end - file_size);
  }
}

// Get the checksum from the data
unsigned long long check_sum(int* data, int size) {
  unsigned long long result = 0;
  for (int i = 0; i < size; ++i) {
    if (data[i] == -1) continue;
    //idk if this cast is necessary but here we go anyways
    result += (unsigned long long) (data[i] * i);
  }
  return result;
}

// The idea behind this task wasn't too hard to figure out. Making sense of all the
// indices regarding compression etc. was probably the hardest part, especially tired.
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
