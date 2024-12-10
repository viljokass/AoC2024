#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// We'll just use the same data struct from part one
// although we don't need the visited-flag this time around
typedef struct _gridItem {
  int label;
  bool visited;
} gridItem;

// Scan file width
int scan_w(FILE* fd) {
  int w = 0;
  while(getc(fd) != '\n') w++;
  rewind(fd);
  return w;
}

// Scan file height
int scan_h(FILE* fd) {
  int h = 0;
  for (char c = getc(fd); c != EOF; c = getc(fd))
    if (c == '\n') h++;
  rewind(fd);
  return h;
}

// Parse the file
void parse(FILE* fd, gridItem* grid, int w, int h) {
  char buffer[256];
  gridItem* tmp;
  for (int i = 0; i < h; ++i) {
    fgets(buffer, sizeof(buffer), fd);
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      tmp->label = buffer[j] - '0';
      tmp->visited = false;
    }
  }
}

// Check if given coordinates are within bounds
bool in_bounds(int x, int y, int w, int h) {
  if (x < 0 || x > w - 1) return false;
  if (y < 0 || y > h - 1) return false;
  return true;
}

// In goes the grid, current position,
// boundaries and what we're looking for
int recursive_marcher(
  gridItem* grid,
  int x, int y,
  int w, int h,
  int what
) {
  // x or y out of bounds
  if (!in_bounds(x, y, w, h)) return 0;

  gridItem* tmp = &grid[y * w + x];
  // If label is not what we're looking for (too steep, same or descent)
  if (tmp->label != what) return 0;

  // If we've reached the top, return 1 as an indication
  // that this is a unique route to the top
  if (tmp->label == 9) return 1;
  // Search all four non-diagonal directions for the next elevation
  int next = what + 1;
  return recursive_marcher(grid, x + 1, y, w, h, next) +
         recursive_marcher(grid, x - 1, y, w, h, next) +
         recursive_marcher(grid, x, y + 1, w, h, next) +
         recursive_marcher(grid, x, y - 1, w, h, next);
}

int solve(gridItem* grid, int w, int h) {
  int count = 0;
  gridItem* tmp;
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      if (tmp->label == 0)
        count += recursive_marcher(grid, j, i, w, h, 0);
    }
  }
  return count;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 0;
  }

  // Get width, height, allocate grid and parse the grid
  int w = scan_w(fd);
  int h = scan_h(fd);
  gridItem* grid = malloc(w * h * sizeof(gridItem));
  parse(fd, grid, w, h);

  // We can close the file now
  fclose(fd);

  // Solve the problem
  int results = solve(grid, w, h);
  printf("Sum of distinct trails is %d\n", results);

  // Free allocated memory
  free(grid);
  return 0;
}
