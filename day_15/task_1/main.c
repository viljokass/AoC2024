#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _vec2i {
  int x;
  int y;
} vec2i;

//
// Parsing functions
//

int parse_w(FILE* fd) {  
  char buffer[256];
  int index = 0;
  fgets(buffer, sizeof(buffer), fd);
  while(buffer[index] == '#')
    index++;
  rewind(fd);
  return index;
}

int parse_h(FILE* fd) {
  char buffer[256];
  int index = 0;
  while(fgets(buffer, sizeof(buffer), fd)) {
    if (buffer[0] == '\n') break;
    index++;
  }
  rewind(fd);
  return index;
}

int parse_cq_len(FILE* fd) {
  int len = 0;
  char c;
  while(1) {
    c = getc(fd);
    if (c == EOF) break;
    if (c == '<' || 
        c == '^' || 
        c == '>' || 
        c == 'v') len++;
  }
  rewind(fd);
  return len;
}

void parse_grid(
  FILE* fd,       // File descriptor
  char* grid,     // grid as char array
  vec2i* robopos, // robot position
  int w, int h    // grid boundaries
) {
  char buffer[256];
  int index, posindex;
  char c;
  for (int y = 0; y < h; ++y) {
    fgets(buffer, sizeof(buffer), fd);
    for (int x = 0; x < w; ++x) {
      posindex = y * w + x;
      c = buffer[x];
      if (c == '@') {
        robopos->x = x;
        robopos->y = y;
        grid[posindex] = '.';
      } else {
        grid[posindex] = c;
      }
    }
  }
  rewind(fd);
}

void parse_cq(FILE* fd, char* cq) {
  int index = 0;
  char c;
  while(1) {
    c = getc(fd);
    if (c == EOF) break;
    if (c == '<' ||
        c == '^' ||
        c == '>' ||
        c == 'v') {
      cq[index] = c;
      index++;
    }
  }
}

//
// Moving functions
//

// Recursive checker function to see 
// if we can move objects
bool check_free(
  char* grid, 
  int x, int y, 
  int dx, int dy,
  int rowstride
) {
  char c = grid[y * rowstride + x];
  if (c == '#') return false;
  if (c == '.') return true;
  if (c == 'O') {
    bool next_free = check_free(grid, x+dx, y+dy, dx, dy, rowstride);
    if (next_free) {
      grid[(y + dy) * rowstride + (x + dx)] = 'O';
      grid[y * rowstride + x] = '.';
    }
    return next_free;
  }
  return false;
}

// Attempt moving the bot
void move(
  char* grid,
  vec2i* robopos, 
  int rowstride, 
  char command
) {
  vec2i dir;
  int dx, dy;
  if (command == '<') {dx = -1; dy =  0;}
  if (command == '^') {dx =  0; dy = -1;}
  if (command == '>') {dx =  1; dy =  0;}
  if (command == 'v') {dx =  0; dy =  1;}

  if (check_free(grid, robopos->x + dx, 
                       robopos->y + dy, 
                       dx, dy, rowstride)) {
    robopos->x += dx;
    robopos->y += dy;
  }
}

void run_amok(
  char* grid,       // Grid represented as characters 
  vec2i* robopos,   // Robot position as vec2i
  char* cq,         // Command queue as char array
  int rowstride,    // Rowstride
  int cql           // Command queue length
) {
  // Consume the command queue
  char command;
  for (int i = 0; i < cql; ++i) {
    command = cq[i];
    move(grid, robopos, rowstride, command);
  }
}

int calculate_gps(char* grid, int w, int h) {
  int sum = 0;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (grid[y * w + x] == 'O') {
        sum += 100 * y + x;
      }
    }
  }
  return sum;
}

// This one was quite nice and easy.
int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open \"%s\"\n", argv[1]);
    return 1;
  }

  int g_w = parse_w(fd);
  int g_h = parse_h(fd);
  int cq_l = parse_cq_len(fd);
  vec2i robopos;

  // Allocate grid and command queue
  char* grid = malloc(g_w * g_h * sizeof(char));
  char* cq = malloc(cq_l * sizeof(char));

  parse_grid(fd, grid, &robopos, g_w, g_h);
  parse_cq(fd, cq);

  // We can close the file now.
  fclose(fd);

  run_amok(grid, &robopos, cq, g_w, cq_l);
  int result = calculate_gps(grid, g_w, g_h);
  printf("Sum of the gps coords is %d\n", result);
 
  // Free mem
  free(grid);
  free(cq);
  return 0;
}
