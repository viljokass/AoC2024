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
bool is_line_safe(char* line) {
  enum direction dir = undef;

  int prev = atoi(line);
  int curr;
  for (int i = 0; i < 100; ++i) {
    if (line[i] == '\n') break;
    if (line[i] == ' ') {
      curr = atoi(line + i);
      if ( dir == undef ) {
        if ( prev - curr < 0 ) dir = asc;
        if ( prev - curr > 0 ) dir = desc;
      }
      if ( dir == asc ) {
        if ( prev - curr > 0 )
          return false; 
      }
      if (dir == desc ) {
        if ( prev - curr < 0 )
          return false;
      }
      if ( prev - curr == 0 )
        return false; 
      if ( int_abs( prev - curr ) > 3 )
        return false;
      prev = curr;
    }
  }
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
