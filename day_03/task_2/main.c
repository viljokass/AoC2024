#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define get_c

enum enable_mul {
  YES,
  NO,
  IDK,
  ENDOF
};

// List of numbers
const char nums[] = "1234567890";

bool isnum(char c) {
  bool isnum = false;
  for ( int i = 0; i < 10; ++i)
    isnum |= c == nums[i];
  return isnum;
}

// Carefully defined sequence of checks starting from m.
// If each one passes, return the product of mul, otherwise
// return either 0, or if EOF is reached -1.
int handle_m(FILE* fd) {

  int l_num_index = 1;
  int r_num_index = 1;
  char num_l[] = "   "; // format the strings
  char num_r[] = "   ";
  char c;

  // Check u
  c = getc(fd);
  if ( c == EOF ) return -1;
  if ( c != 'u' ) return 0;

  // Check l
  c = getc(fd);
  if ( c == EOF ) return -1;
  if ( c != 'l' ) return 0;

  // Check (
  c = getc(fd);
  if ( c == EOF  ) return -1;
  if ( c != '(' ) return 0;

  // Check that there's at least 1 number
  c = getc(fd);
  if ( c == EOF ) return -1;
  if ( !isnum(c) ) return 0;
  num_l[0] = c;
  c = getc(fd);
  
  // Check numbers up to 3 digits
  while (isnum(c) && (l_num_index < 3)) {
    num_l[l_num_index] = c;
    l_num_index++;
    c = getc(fd);
    // Get next char
    if ( c == EOF ) return -1;
  }

  // Either coming out of the loop or straight after checking the first digit, check if next is comma.
  if ( c != ',' ) return 0;

  // The same thing as above but for the other number
  c = getc(fd);
  if ( c == EOF ) return -1;
  if ( !isnum(c) ) return 0;
  num_r[0] = c;
  c = getc(fd);
  
  // The same
  while (isnum(c) && (r_num_index < 3)) {
    num_r[r_num_index] = c;
    r_num_index++;
    c = getc(fd);
    if ( c == EOF ) return -1;
  }

  // And as above, check if the last one is ).
  if ( c != ')' ) return 0;

  // If all the checks have passed, return the product of the numbers
  return atoi(num_l) * atoi(num_r);
}

// Check if )
enum enable_mul getdo(FILE* fd) {
  char c;
  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != ')' ) return IDK;

  return YES;
}

// Check if 't()
enum enable_mul getdont(FILE* fd) {
  char c;
  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != 39 ) return IDK;
 
  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != 't' ) return IDK;

  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != '(' ) return IDK;

  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != ')' ) return IDK;

  return NO;
}

// if d happens, then do this
enum enable_mul get_enable(FILE* fd) {
  char c;
  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c != 'o' ) return IDK;

  // two ways: do() and don't().
  c = getc(fd);
  if ( c == EOF ) return ENDOF;
  if ( c == 'n' ) {
    return getdont(fd);
  }
  if ( c == '(' ) {
    return getdo(fd);
  }
  return IDK;
}

int main(int argc, const char* argv[]) {
  
  if (argc < 2) {
    printf("Define input file, please.\n");
    return 1;
  }

  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 1;
  }
 
  // Handle the file.
  // You know, I probably just could have used regex :D
  // Doing things the unneccessarily complicated way...
  int result = 0;
  char c;
  bool enablemul_bool = true;
  while (true) {
    c = getc(fd);
    if ( c == EOF ) break;
    if ( c == 'd') {
      enum enable_mul enable = get_enable(fd);
      if (enable == YES) enablemul_bool = true;
      if (enable == NO ) enablemul_bool = false;
      if (enable == ENDOF) break;
    }
    if (enablemul_bool &&  c == 'm') {
      int res = handle_m(fd);
      if (res == -1) break;
      result += res;
    }
  }

  printf("The result is %d\n", result);
  
  fclose(fd);
  return 0;
}
