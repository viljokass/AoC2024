#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Struct for storing items in grid. Since we are counting
// only visited nodes, not individual visits, we need to
// store that information somehow, and that is with a boolean field.
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
void recursive_marcher(
  gridItem* grid,
  int x, int y,
  int w, int h,
  int what
) {
  // x or y out of bounds
  if (!in_bounds(x, y, w, h)) return;

  gridItem* tmp = &grid[y * w + x];
  // If label is not what we're looking for (too steep, same or descent)
  if (tmp->label != what) return;

  // If we've reached the top, set visited to true and return
  if (tmp->label == 9) {
    tmp->visited = true;
    return;
  }
  // Search all four non-diagonal directions for the next elevation
  int next = what + 1;
  recursive_marcher(grid, x + 1, y, w, h, next);
  recursive_marcher(grid, x - 1, y, w, h, next);
  recursive_marcher(grid, x, y + 1, w, h, next);
  recursive_marcher(grid, x, y - 1, w, h, next);
}

// We could limit this to a subset of the grid, since max dist is +-9
int count_score(gridItem* grid, int w, int h) {
  int score = 0;
  gridItem* tmp;
  for(int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      if (tmp->visited)
        score++;
    }
  }
  return score;
}

// We could limit this to a subset of the grid, since max dist is +-9
void reset(gridItem* grid, int w, int h) {
  gridItem* tmp;
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      tmp->visited = false;
    }
  }
}

// In goes the grid, position of the trail head and boundaries
int score_trail_head(
  gridItem* grid, 
  int x, int y, 
  int w, int h
) {
  recursive_marcher(grid, x, y, w, h, 0); // Set the visited-flags
  int score = count_score(grid, w, h);    // Score the trail head
  reset(grid, w, h);                      // Reset grid (visited->false)
  return score;
}

int solve(gridItem* grid, int w, int h) {
  int count = 0;
  gridItem* tmp;
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      if (tmp->label == 0)
        count += score_trail_head(grid, j, i, w, h);
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
  printf("Sum of scores of all trailheads is %d\n", results);

  // Free allocated memory
  free(grid);
  return 0;
}
