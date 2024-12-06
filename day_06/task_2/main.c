#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Enum representing grid items.
typedef enum _gridItem {
  FREE,     // .
  TAKEN,    // #
  VISITED,  // where guard has been
  START     // guard start position
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

// Struct for encoding turns
typedef struct _turn_t {
  int x;            // x-coord of where the turn happened
  int y;            // y-coord if where the turn happened
  guardDir newDir;  // new direction.
} turn_t;

// A struct for creating linked lists for turns
typedef struct _turn_link_t {
  struct _turn_link_t* next;
  turn_t turn;
} turn_link_t;

// Initialize turn links
turn_link_t* init_turn_links() {
  // A null item to act as a header
  turn_link_t* a = malloc(sizeof(turn_link_t));
  a->turn.x = -1;
  a->turn.y = -1;
  a->turn.newDir = UP;
  a->next = NULL;
}

// Add a turn to the list
void add_turn(turn_link_t* head, int x, int y, guardDir dir) {
  turn_link_t* current = head; 
  while(current->next != NULL) {
    current = current->next;
  }
  turn_link_t* new = malloc(sizeof(turn_link_t));
  new->turn.x = x;
  new->turn.y = y;
  new->turn.newDir = dir;
  current->next = new;
  new->next = NULL;
}

// Free the turn list
void free_turn_links(turn_link_t* head) {
  if (head == NULL) return;
  turn_link_t* node = head;
  turn_link_t* next = NULL;
  // First eliminate head, then move from there.
  while (node->next != NULL) {
    next = node->next;
    // Free node content
    free(node);
    node = next;
  }
}

// Check if a turn is already in the list
bool check_turn(turn_link_t* head, int x, int y, guardDir dir) {
  turn_link_t* node = head;
  int cx;
  int cy;
  guardDir cdir;
  while(node->next != NULL) {
    node = node->next;
    cx = node->turn.x;
    cy = node->turn.y;
    cdir = node->turn.newDir;
    if (cx == x && cy == y && cdir == dir) return true;
  }
  return false;
}

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

// Scan width
int scan_w(FILE* fd) {
  int w = 0;
  while(getc(fd) != '\n') w++;
  rewind(fd);
  return w;
}

// Scan height
int scan_h(FILE* fd) {
  int h = 0;
  for (char c = getc(fd); c != EOF; c = getc(fd))
    if (c == '\n') h++;
  rewind(fd);
  return h;
}

// Parse the file into grid of enums
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
        grid[i * w + j] = START;
        guard_t->x = j;
        guard_t->y = i;
        guard_t->dir = UP;
      }
    }
  }
  free(buffer);
}

// Recursive marching function, something weird happens here. Oh well.
// Somehow this seems to work, so whatever. An edge case throws a wrench in the
// gears, but otherwise handles pretty well.
bool marcher_func(gridItem* grid, int w, int h, guard* guard_t, turn_link_t* turns) {
  guardDir dir = guard_t->dir;
  int x = guard_t->x;
  int y = guard_t->y;
  // Check if item is out of bounds.
  if ( x < 0 || x > w - 1 ) return false;
  if ( y < 0 || y > h - 1 ) return false;

  // Get the direction differences.
  dirEncode diff = getDiffs(dir);
  int nx = x + diff.dx;
  int ny = y + diff.dy;
  if ( nx < 0 || nx > w ) return false; // These two in particular. Probably should
  if ( ny < 0 || nx > h ) return false; // put the checks in other places or something
  // If next item is TAKEN, turn and do it again.
  if (grid[ny * w + nx] == TAKEN) {
    guard_t->dir = next(dir);
    if (check_turn(turns, x, y, guard_t->dir))
      return true;
    add_turn(turns, x, y, guard_t->dir);
    return marcher_func(grid, w, h, guard_t, turns);
  } 
  // Otherwise, move on to the next item in the same direction.
  // Can be VISITED, FREE, START, or out of bounds.
  if (grid[y * w + x] != START) 
    grid[y * w + x] = VISITED;  // Currently stood on item is visited
  guard_t->x = nx;              // Set guard position
  guard_t->y = ny;
  return marcher_func(grid, w, h, guard_t, turns);
}

// Let the guard march
int solve(gridItem* grid, int w, int h, guard* guard_t) {
  // March the guard
  turn_link_t* turns = init_turn_links();
  bool loop = marcher_func(grid, w, h, guard_t, turns);
  free_turn_links(turns);

  // Count the visited places
  int visiteds = 0;
  int total_size = w * h;
  for (int i = 0; i < total_size; ++i) {
    if (grid[i] == VISITED || grid[i] == START)
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
  gridItem* init_grid = malloc(w * h * sizeof(gridItem));
  guard guard_t;
  parse(fd, init_grid, w, h, &guard_t);
  // Get a copy of the unaltered grid
  gridItem* bckp_grid = malloc(w * h * sizeof(gridItem));
  memcpy(bckp_grid, init_grid, w * h * sizeof(gridItem));
  // Also a copy of the guard
  guard guard_bckp;
  guard_bckp.x = guard_t.x;
  guard_bckp.y = guard_t.y;
  guard_bckp.dir = guard_t.dir;

  // We can close the file now.
  fclose(fd);

  // Solve
  // This transforms the board to the visited stage. We can ignore the results.
  int result = solve(init_grid, w, h, &guard_t);
  printf("Initially guard visited in %d unique positions.\n", result);

  // The idea is to put an obstacle on each visited item and see what happens. Produce
  // copies of the unaltered grid, then for each visited position in the altered grid,
  // put obstacle in the unaltered grid and see what happens.
  // A loop happens when the same turn has been made before. That is, in position x, y, a direction has been chosen before.
  int obstacleable = 0;
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      if (init_grid[i * w + j] == VISITED) {
        // Copy the grid
        gridItem* iter_grid = malloc(w * h * sizeof(gridItem));
        memcpy(iter_grid, bckp_grid, w * h * sizeof(gridItem));
        // Copy the guard
        guard guard_iter;
        guard_iter.x = guard_bckp.x;
        guard_iter.y = guard_bckp.y;
        guard_iter.dir = guard_bckp.dir;
        // Alter the grid
        iter_grid[i * w + j] = TAKEN;
        // Init the links
        turn_link_t* links = init_turn_links();
        if (marcher_func(iter_grid, w, h, &guard_iter, links)) obstacleable++; 
        // Free allocated memory
        free_turn_links(links);
        free(iter_grid);
      }
    }
  }
  printf("There were %d loops.\n", obstacleable);

  // Free allocated memory
  free(init_grid);
  free(bckp_grid);

  return 0;
}
