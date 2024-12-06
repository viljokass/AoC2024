#include <stdio.h>
#include <stdlib.h>

// Enum representing grid items.
typedef enum _gridItem {
  FREE,     // .
  TAKEN,    // #
  VISITED   // where guard has been
} gridItem;

// Enum representing the direction the guard's looking at
typedef enum _guardDir {
  UP,
  DOWN,
  LEFT,
  RIGHT
} guardDir;

// Guard struct for representing the guard
typedef struct _guard {
  int x;          // X-coordinate
  int y;          // Y-coordinate
  guardDir dir;   // direction the guard's looking at
} guard;

// A return type for differences in coordinates based on guard's look dir
typedef struct _dirEncode {
  int dx;
  int dy;
} dirEncode;

// Get the direction difference encoded in the guard's direction
dirEncode getDiffs(guardDir dir) {
  dirEncode enc;
  switch (dir) {
    case UP:
      enc.dx = 0;
      enc.dy = -1;
      break;
    case DOWN:
      enc.dx = 0;
      enc.dy = 1;
      break;
    case RIGHT:
      enc.dx = 1;
      enc.dy = 0;
      break;
    case LEFT:
      enc.dx = -1;
      enc.dy = 0;
      break;
    default:
      break;
  }
  return enc;
}

// Get the direction to which the guard will turn to
guardDir next(guardDir dir) {
  if (dir == UP) return RIGHT;
  if (dir == RIGHT) return DOWN;
  if (dir == DOWN) return LEFT;
  if (dir == LEFT) return UP;
}

int scan_w(FILE* fd) {
  int w = 0;
  while(getc(fd) != '\n') w++;
  rewind(fd);
  return w;
}

int scan_h(FILE* fd) {
  int h = 0;
  for (char c = getc(fd); c != EOF; c = getc(fd))
    if (c == '\n') h++;
  rewind(fd);
  return h;
}

void parse(FILE* fd, gridItem* grid, int w, int h, guard* guard_t) {
  char* buffer = malloc(256 * sizeof(char));
  char c;
  for (int i = 0; i < h; ++i) {
    fgets(buffer, 256 * sizeof(char), fd);
    for (int j = 0; j < w; ++j) {
      c = buffer[j];
      if (c == '.') {
        grid[i * w + j] = FREE;
        continue;
      }
      if (c == '#') {
        grid[i * w + j] = TAKEN;
        continue;
      }
      if (c == '^') {
        grid[i * w + j] = VISITED;
        guard_t->x = j;
        guard_t->y = i;
        guard_t->dir = UP;
      }
    }
  }
  free(buffer);
}



// Recursive marching function
void marcher_func(gridItem* grid, int w, int h, guard* guard_t) {
  guardDir dir = guard_t->dir;
  int x = guard_t->x;
  int y = guard_t->y;
  // Check if item is out of bounds.
  if (x < 0 || x > (w - 1)) return;
  if (y < 0 || y > (h - 1)) return;

  // Get the direction differences.
  dirEncode diff = getDiffs(dir);
  // If next item is TAKEN, turn and do it again.
  if (grid[(y + diff.dy) * w + (x + diff.dx)] == TAKEN) {
    guard_t->dir = next(dir);
    marcher_func(grid, w, h, guard_t);
    return;
  } 
  // Otherwise, move on to the next item in the same direction.
  // Can be VISITED, FREE, or out of bounds.
  grid[y * w + x] = VISITED;  // Currently stood on item is visited
  guard_t->x += diff.dx;      // Set guard position
  guard_t->y += diff.dy;
  marcher_func(grid, w, h, guard_t);
  return;
}

// Let the guard march
int solve(gridItem* grid, int w, int h, guard* guard_t) {
  // March the guard
  marcher_func(grid, w, h, guard_t);

  // Count the visited places
  int visiteds = 0;
  int total_size = w * h;
  for (int i = 0; i < total_size; ++i) {
    if (grid[i] == VISITED)
      visiteds++;
  }
  return visiteds;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please refine an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open \"%s\"\n", argv[1]);
    return 1;
  }

  // Scan the input size, allocate memory accordingly and parse
  int w = scan_w(fd);
  int h = scan_h(fd);
  gridItem* grid = malloc(w * h * sizeof(gridItem));
  guard guard_t;
  parse(fd, grid, w, h, &guard_t);

  // We can close the file now.
  fclose(fd);

  // Solve
  int unique_locs = solve(grid, w, h, &guard_t);

  printf("The guard visited %d unique locations.\n", unique_locs); 
  /*
  // For debugging
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      gridItem item = grid[i * w + j];
      if (item == FREE) printf(".");
      if (item == TAKEN) printf("#");
      if (item == VISITED) printf("X");
    }
    printf("\n");
  }
  */


  // Free allocated memory
  free(grid);

  return 0;
}
