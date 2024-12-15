#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

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
      posindex = 2 * y * w + 2 * x;
      c = buffer[x];
      if (c == '#') {
        grid[posindex] = '#';
        grid[posindex + 1] = '#';
      }
      if (c == 'O') {
        grid[posindex] = '[';
        grid[posindex + 1] = ']';
      }
      if (c == '.') {
        grid[posindex] = '.';
        grid[posindex + 1] = '.';
      }
      if (c == '@') {
        grid[posindex] = '.';
        grid[posindex + 1] = '.';
        robopos->x = 2 * x;
        robopos->y = y;
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
  if (c == '.') return true;
  if (c == '#') return false;
  if (dx == 0) {
    int ny = y + dy;
    // We movin up or down.
    if (c == '[') { 
      return check_free(grid, x, ny, 0, dy, rowstride) &&
             check_free(grid, x + 1, ny, 0, dy, rowstride); 
    }
    if (c == ']') {
      return check_free(grid, x, ny, 0, dy, rowstride) &&
             check_free(grid, x - 1, ny, 0, dy, rowstride); 
    }
  }
  else {
    // We movin left or right, recursion jumps twice as far
    return check_free(grid, x + 2 * dx, y, dx, 0, rowstride);
  }
  return false;
}


// Recursive function to move objects
bool push(
  char* grid,
  int x, int y,
  int dx, int dy,
  int rowstride
) {
  char c = grid[y * rowstride + x];
  if (c == '#') return false;
  if (c == '.') return true;
  if (dx == 0) {
    int ny = y + dy;
    bool next_free;
    if (c == '[') {
      next_free = push(grid, x, ny, 0, dy, rowstride) &&
                  push(grid, x + 1, ny, 0, dy, rowstride);
      if (next_free) {
        grid[ny * rowstride + x] = '[';
        grid[ny * rowstride + x + 1] = ']';
        grid[y * rowstride + x] = '.';
        grid[y * rowstride + x + 1] = '.';
      }
    }
    if (c == ']') {
      next_free = push(grid, x, ny, 0, dy, rowstride) &&
                  push(grid, x - 1, ny, 0, dy, rowstride);
      if (next_free) {
        grid[ny * rowstride + x] = ']';
        grid[ny * rowstride + x - 1] = '[';
        grid[y * rowstride + x] = '.';
        grid[y * rowstride + x - 1] = '.';
      }
    }
    return next_free;
  }
  else {
    bool next_free = push(grid, x + 2 * dx, y, dx, 0, rowstride);
    if (next_free) {
      grid[y * rowstride + (x + 2 * dx)] = grid[y * rowstride + (x + dx)];
      grid[y * rowstride + x + dx] = grid[y * rowstride + x];
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
    push(grid, robopos->x + dx,
               robopos->y + dy,
               dx, dy, rowstride);
    robopos->x += dx;
    robopos->y += dy;
  }
}

// Visualize
void print_board(
  char* grid, 
  vec2i* robopos, 
  int w, int h, 
  char command, 
  int cnum, int cql,
  bool free_play
) {
  printf("\e[1;1H\e[2J");
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (y == robopos->y && x == robopos->x) {
        printf("@");
      }
      else printf("%c", grid[y * w + x]);
    }
    printf("\n");
  }
  if (free_play) printf("Control robot by writing a string made of\n'w' (^), 'a' (<), 's' (v) or 'd' (>) and hit enter.\n");
  printf("Command: %c ---- Command number: %d/%d\n", command, cnum, cql);
}

void run_amok(
  char* grid,       // Grid represented as characters 
  vec2i* robopos,   // Robot position as vec2i
  char* cq,         // Command queue as char array
  int w, int h,     // Table bounds
  int cql,          // Command queue length
  bool viz,         // Do we visualize?
  bool free_play    // Free mode (good for debugging)
) {
  // Consume the command queue
  char command = '-';
  int iter = cql;
  if (free_play) iter = 1 << 31 - 1;
  for (int i = 0; i < iter; ++i) {
    if (viz) print_board(grid, robopos, w, h, command, i, iter, free_play);
    if (free_play) while (1) {
      char c = getchar();
      if (c == 'a') {
        command = '<';
        break;
      }
      if (c == 'w') {
        command = '^';
        break;
      }
      if (c == 'd') {
        command = '>';
        break;
      }
      if (c == 's') {
        command = 'v';
        break;
      }
    }
    if (viz) usleep(125 * 1000);
    if (!free_play) command = cq[i];
    move(grid, robopos, w, command);
  }
  if (viz) {
    do {
      print_board(grid, robopos, w, h, command, iter, iter, false);
      printf("PRESS ENTER\n");
    } while (getchar() != '\n');
  }
}

int calculate_gps(char* grid, int w, int h) {
  int sum = 0;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (grid[y * w + x] == '[') {
        sum += 100 * y + x;
      }
    }
  }
  return sum;
}

// This one wasn't that nice and easy.
// Took me a while to figure out that because recursion
// splits, I can't update the positions, since all in all
// it can be false. Then I implemented a check before that and
// Now it all works.
// Then I implemented a visualization and free mode for debugging
// One can push around boxes for fun.
int main(int argc, const char** argv) {
  bool viz = false;
  bool free_play = false;
  if (argc < 2) {
    printf("Please define an input file.\n");
    printf("You can also enable visualization by entering '-v' handle\nand free play by entering '-f' handle.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open \"%s\"\n", argv[1]);
    return 1;
  }
  for (int i = 2; i < argc; ++i) {
    if ( !strcmp(argv[i], "-v"))
      viz = true; 
    if ( !strcmp(argv[i], "-f")) {
      viz = true;
      free_play = true;
    }
  }

  int g_w = parse_w(fd);
  int g_h = parse_h(fd);
  int cq_l = parse_cq_len(fd);
  vec2i robopos;

  // Allocate grid and command queue
  char* grid = malloc(2 * g_w * g_h * sizeof(char));
  char* cq = malloc(cq_l * sizeof(char));

  parse_grid(fd, grid, &robopos, g_w, g_h);
  parse_cq(fd, cq);

  // Update grid width
  g_w *= 2;
  // We can close the file now.
  fclose(fd);

  run_amok(grid, &robopos, cq, g_w, g_h, cq_l, viz, free_play);
  int result = calculate_gps(grid, g_w, g_h);
  printf("Sum of the gps coords is %d\n", result); 
 
  // Free mem
  free(grid);
  free(cq);
  return 0;
}
