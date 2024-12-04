#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define cd(a, b) (checkdir(data, i, j, rows, columns, a, b, 'M'))

// Count rows from the file
// Get lines until EOF is reached
int count_rows(FILE* fd) {
  int rows = 0;
  char buffer[1024];
  while(fgets(buffer, sizeof(buffer), fd))
    rows++;
  rewind(fd);
  return rows;
}

// Count columns from the file
// Get a line and count until \n is reached
int count_cols(FILE* fd) {
  int cols = 0;
  char buffer[1024];
  fgets(buffer, sizeof(buffer), fd);
  while(buffer[cols] != '\n')
    cols++;
  rewind(fd);
  return cols;
}

// Char by char put the file into array allocated in main
void parse_array(
  FILE* fd, 
  char* array, 
  int rows, int columns
) {
  char c;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      c = getc(fd);
      if (c == EOF ) return;
      array[i * columns + j] = c;
    }
    c = getc(fd); // Get the \n out
  }
}

// Recursive checker function that checks if given array indices are valid AND if the items in the data are what we're looking for
int checkdir(
  const char* data, 
  int row, int column, 
  int max_row, int max_column, 
  int row_dir, int column_dir, 
  char what
) {
  int new_row     = row + row_dir;
  int new_column  = column + column_dir;
  if (new_row > max_row - 1 || new_row < 0 ) return 0; // new index out of bounds
  if (new_column > max_column - 1 || new_column < 0) return 0;
  if (data[new_row * max_column + new_column] != what) return 0; // Not what we're looking for
  if (what == 'S') return 1; // If we've reached S, we've found a complete MAS
  char new_what;
  if (what == 'M') new_what = 'S';
  return checkdir(data, 
                  new_row, new_column, 
                  max_row, max_column, 
                  -2*row_dir, -2*column_dir, // Jump to the opposite side of A
                  new_what);
}

// For each X in the data, recursively check if XMASes form from going into eight directions.
int searchXMASes(const char* data, int rows, int columns) {
  int XMASes = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (data[i * columns + j] == 'A') {
        // Has some overlap but whatever.
        // IT SEEMS THAT A + IS NOT AN X!
        bool xmas = (cd(-1,-1)&&cd(1,-1)) ||
                    (cd(-1,-1)&&cd(-1,1)) ||
                    (cd(-1,1)&&cd(1,1)) ||
                     (cd(1,-1)&&cd(1,1));
        if (xmas) XMASes++;
      }
    }
  }
  return XMASes;
}

int main(int argc, const char* argv[]) {
  if ( argc < 2 ) {
    printf("Please define input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if ( fd == NULL ) {
    printf("Unable to open file.\n");
    return 1;
  }

  // Get the dimensions of the array
  int rows = count_rows(fd);
  int columns = count_cols(fd);

  // Allocate array and parse
  char* data = malloc(rows * columns * sizeof(char));
  parse_array(fd, data, rows, columns);

  // We can close the file now.
  fclose(fd);

  // Search for XMASes
  int xmases = searchXMASes(data, rows, columns);
  printf("There are %d XMASes in the input.\n", xmases);

  // Free the data array
  free(data);
  return 0;
}
