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

// Check a line
bool is_line_safe(const char* line) {
  // REDO THE LINE CHECKING
  return true;
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
