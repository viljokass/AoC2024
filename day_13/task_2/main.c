#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ull unsigned long long
#define ll long long

typedef struct _vec2i {
  int x;
  int y;
} vec2i;

typedef struct _vec2ull {
  ull x;
  ull y;
} vec2ull;

typedef struct _unit {
  vec2i A;
  vec2i B;
  vec2ull target;
} unit;

// Parses one problem. Bunch of copy paste.
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
  p->target.x = atoi(buffer + bindex + 2) + 10000000000000;
  while(buffer[bindex] != 'Y') {
    bindex++;
  }
  p->target.y = atoi(buffer + bindex + 2) + 10000000000000;

  // Get empty out. If EOF, return 1.
  if (!fgets(buffer, sizeof(buffer), fd))
    return 1;

  return 0;
}

// Man this is so simple this way
ull solve(unit* p) {
  vec2i A = p->A;
  vec2i B = p->B;
  vec2ull T = p->target;

  ll coeff_det = A.x * B.y - B.x * A.y;
  ll x_mat_det = T.x * B.y - B.x * T.y;
  ll y_mat_det = A.x * T.y - T.x * A.y;

  // Since we're dealing with integers, this conveniently
  // takes care of non-integer solutions too.
  ll x = x_mat_det / coeff_det;
  ll y = y_mat_det / coeff_det;

  // Integer solution found
  if (A.x * x + B.x * y == T.x && 
      A.y * x + B.y * y == T.y)
    return 3 * x + y;
  // No integer solutions found
  return 0;
}

// CRAMER'S RULE TO THE RESCUE!
// My first solution was a bit embarassing... but hey it worked!
// I might want to see if there's some convenient algorithm or
// mathematical vippaskonsti for the solution in the future
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
    ull price = solve(&p);
    sum += price;
    if (res == 1) break;
  }
  fclose(fd);

  printf("The sum of all is %lld\n", sum);

  return 0;
}
