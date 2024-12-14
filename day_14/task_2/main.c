#include <stdio.h>
#include <stdlib.h>

// 2-dimensional integer vector struct
typedef struct _vec2i{
  int x;
  int y;
} vec2i;

// Robot struct that holds robot's position and speed
typedef struct _robot {
  vec2i position;
  vec2i speed;
} robot;

// Scan file height (robot count)
int scan_h(FILE* fd) {
  int h = 0;
  for (char c = getc(fd); c != EOF; c = getc(fd))
    if (c == '\n') h++;
  rewind(fd);
  return h;
}

// Parse file into robot struct
void parse(FILE* fd, robot* robots) {
  char buffer[256];
  int bot_index = 0;
  int buffer_index;
  int px;
  int py;
  int sx;
  int sy;
  while(fgets(buffer, sizeof(buffer), fd)) {
    buffer_index = 0;
    px = atoi(buffer + 2);
    while(buffer[buffer_index] != ',') buffer_index++;
    py = atoi(buffer + buffer_index + 1);
    while(buffer[buffer_index] != 'v') buffer_index++;
    sx = atoi(buffer + buffer_index + 2);
    while(buffer[buffer_index] != ',') buffer_index++;
    sy = atoi(buffer + buffer_index + 1);
    robots[bot_index].position.x = px;
    robots[bot_index].position.y = py;
    robots[bot_index].speed.x = sx;
    robots[bot_index].speed.y = sy;
    bot_index++;
  }
}

void print_bots(
  robot* robots,
  int robot_count, 
  int w, int h,
  int second
) {
  printf("\e[1;1H\e[2J");
  printf("%d\n", second);
  int gbc = 0;
  robot* tmp;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      gbc = 0;
      for (int i = 0; i < robot_count; ++i) {
        tmp = &robots[i];
        if (tmp->position.x == x && tmp->position.y == y) gbc++;
      }
      if (gbc > 0) printf("%d", gbc);
      else printf(".");
    }
    printf("\n");
  }
}

int move_bots(robot* robots, int robot_count, int w, int h) {
  robot* tmp;
  int tx;
  int ty;
  for (int i = 0; i < robot_count; ++i) {
    tmp = &robots[i];

    // X-coordinates
    tx = tmp->position.x + tmp->speed.x;
    tmp->position.x = tx % w + (tx % w < 0 ? w : 0);

    // Y-coordinates
    ty = tmp->position.y + tmp->speed.y;
    tmp->position.y = ty % h + (ty % h < 0 ? h : 0);
  }

  int sum = 0;
  for (int i = 0; i < robot_count; ++i) {
    tmp = &robots[i];
    if (tmp->position.y == 35) sum++;
  }
  if (sum > 30) return 1;
  return 0;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 1;
  }

  int h = scan_h(fd);
  robot* robots = malloc(h * sizeof(robot));
  parse(fd, robots);

  // We can close the file now.
  fclose(fd);
 
  print_bots(robots, h, mx, my, 0);
  int i = 0;
  int mx = 101;
  int my = 103;
  // If row 35 has more than thirty items
  int mtt = 0;
  while(1) {
    while(1) {
      mtt = move_bots(robots, h, mx, my);
      ++i;
      if (mtt > 0) break;
    }
    print_bots(robots, h, mx, my, i);
    // Smack enter until you see a tree
    while(getchar() != '\n');
  }

  // Free allocated memory
  free(robots);
  return 0;
}
