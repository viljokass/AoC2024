#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// A struct for holding necessary data
typedef struct _gridItem {
  char label;
  bool antenna;
  bool antinode;
} gridItem;

// All possible antenna labels and checker function
const char antennae[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n";
bool check_antenna(char c) {
  int ind = 0;
  char cn;
  while (antennae[ind] != '\n') {
    cn = antennae[ind];
    if (c == cn) return true;
    ind++;
  }
  return false;
}

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

// Parse the file.
void parse(FILE* fd, gridItem* grid, int w, int h) {
  char buffer[256];
  for (int i = 0; i < h; ++i) {
    fgets(buffer, sizeof(buffer), fd);
    for (int j = 0; j < w; ++j) {
      gridItem* curr = &grid[i * w + j];
      curr->label = buffer[j];
      if (check_antenna(curr->label)) {
        curr->antenna = true;
      } else {
        curr->antenna = false;
      }
      curr->antinode = false;
    }
  }
}

// Check if given coordinates are within bounds
bool check_bounds(int x, int y, int w, int h) {
  if (x < 0 || x > w - 1) return false;
  if (y < 0 || y > h - 1) return false;
  return true;
}

void set_antinodes(gridItem* grid, int w, int h) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      // The base antenna from which we look to other antennas
      gridItem* base = &grid[i * w + j];
      if (base->antenna) {
        for(int m = 0; m < h; ++m) {
          for (int n = 0; n < w; ++n) {
            // If root and head are the same, move on.
            if (m == i && n == j) continue;
            // The head antenna that we use to calculate the position of the antinode
            gridItem* head = &grid[m * w + n];
            if (head->antenna) {
              if (head->label == base->label) {
                int new_x = n + n - j;
                int new_y = m + m - i;
                // If new position is within bounds, put antinode there
                if (check_bounds(new_x, new_y, w, h)) {
                  // The new antinode grid item
                  gridItem* antinode = &grid[new_y * w + new_x];
                  if (!antinode->antenna)
                    antinode->label = '#';
                  antinode->antinode = true;
                }
              }
            }
          }
        }
      }
    }
  }
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
  parse(fd, grid, w, h);

  // We can close the file now
  fclose(fd);

  int results = 0;
  // Set the antinodes and count them
  set_antinodes(grid, w, h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      gridItem* item = &grid[i * w + j];
      if (item->antinode) results++;
    }
  }
  printf("There's %d places with unique antinodes.\n", results);

  // Free allocated memory
  free(grid);
  return 0;
}
