#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ull unsigned long long

typedef struct _vec2i {
  int x;
  int y;
} vec2i;

typedef struct _unit {
  vec2i A;
  vec2i B;
  vec2i target;
} unit;

// Parses one problem
int parse(FILE* fd, unit* p) {
    int bindex = 0;
    char buffer[256];

    // New line in
    fgets(buffer, sizeof(buffer), fd);
    // Vector A
    while(buffer[bindex] != 'X') {
      bindex++;
    }
    p->A.x = atoi(buffer + bindex + 1);
    while(buffer[bindex] != 'Y') {
      bindex++;
    }
    p->A.y = atoi(buffer + bindex + 1);

    // New line in
    fgets(buffer, sizeof(buffer), fd);
    // Vector B
    bindex = 0;
    while(buffer[bindex] != 'X') {
      bindex++;
    }
    p->B.x = atoi(buffer + bindex + 1);
    while(buffer[bindex] != 'Y') {
      bindex++;
    }
    p->B.y = atoi(buffer + bindex + 1);

    // New line in
    fgets(buffer, sizeof(buffer), fd);
    // Target vector
    bindex = 0;
    while(buffer[bindex] != 'X') {
      bindex++;
    }
    p->target.x = atoi(buffer + bindex + 2);
    while(buffer[bindex] != 'Y') {
      bindex++;
    }
    p->target.y = atoi(buffer + bindex + 2);

    // Get empty out. If EOF, return 1.
    if (!fgets(buffer, sizeof(buffer), fd))
      return 1;

    return 0;
}

// Calculate the price of getting the prize
int solve(unit* p) {
  vec2i A = p->A;
  vec2i B = p->B;
  vec2i T = p->target;

  // How many times a and b have been pressed
  int a_count;
  int b_count;

  // accumulator for X and Y
  int x_accum;
  int y_accum;

  int price = INT_MAX;

  // Iterate over the button press amounts.
  // On the first iteration, we press A zero times,
  // then B until we reach target, go over it or reach 100 presses.
  // On the second iteration, we press A one time,
  // then -||-
  // ETC.
  for (int i = 0; i <= 100; ++i) {
    a_count = 0;
    b_count = 0;
    x_accum = 0;
    y_accum = 0;
    // Add A's
    for (int j = 0; j < i; ++j) {
      if (x_accum > T.x || y_accum > T.y) break;
      if (x_accum == T.x && y_accum == T.y) {
        int p_cand = 3 * a_count + b_count;
        if (price > p_cand) {
          price = p_cand;
          break;
        }
      }
      a_count++;
      x_accum += A.x;
      y_accum += A.y;
    }
    // Add B's
    for (int j = 0; j < 100; ++j) {
      if (x_accum > T.x || y_accum > T.y) break;
      if (x_accum == T.x && y_accum == T.y) {
        int p_cand = 3 * a_count + b_count;
        if (price > p_cand) {
          price = p_cand;
          break;
        }
      }
      b_count++;
      x_accum += B.x;
      y_accum += B.y;
    }
  }

  // We haven't found any combinations
  if (price == INT_MAX) return 0;
  // We've found the smallest price
  return price;
}

// "Stinks like Diophantine equations and
// linear combinations in here!", I thought
// to myself, but then thought of a less 
// elegant solution to this problem.
// Essentially brute-forceing, yet again.
int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unble to open \"%s\"\n", argv[1]);
    return 1;
  }
 
  ull sum = 0;
  while(1) {
    unit p;
    int res = parse(fd, &p);
    int price = solve(&p);
    sum += price;
    if (res == 1) break;
  }
  fclose(fd);

  printf("The sum of all is %lld\n", sum);

  return 0;
}
