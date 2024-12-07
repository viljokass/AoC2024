#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// A concatenation function that concats two numbers.
unsigned long long concat(
  unsigned long long lhs,
  unsigned long long rhs
) {
  unsigned long tmp = rhs;
  int dc = 0;
  while (tmp != 0) {
    tmp /= 10;
    dc++;
  }
  tmp = lhs;
  for (int i = 0; i < dc; ++i) {
    tmp *= 10;
  }
  return tmp + rhs;
}

// Recursive function that uses accumulation
bool solve(
  unsigned long long output, 
  unsigned long long* operands,
  int operandCount,
  int currentInd,
  unsigned long long accumulator
) {
  if (currentInd == operandCount - 1) {
    if (accumulator * operands[currentInd] == output ||
        accumulator + operands[currentInd] == output ||
        concat(accumulator, operands[currentInd]) == output) return true;
    return false;
  }
  return solve(output, operands, operandCount, currentInd + 1, accumulator + operands[currentInd]) ||
         solve(output, operands, operandCount, currentInd + 1, accumulator * operands[currentInd]) ||
         solve(output, operands, operandCount, currentInd + 1, concat(accumulator, operands[currentInd]));
}

// Parse and handle a line
unsigned long long handle_line(char* buffer) {
  unsigned long long output = atoll(buffer);
  
  // Count the operands
  int ocount = 0;
  int index = 0;

  while(buffer[index] != '\n') {
    if (buffer[index] == ' ') ocount++;
    index++;
  }

  // Allocate mem
  unsigned long long* operands = malloc(ocount * sizeof(unsigned long long));

  index = 0;
  int numindex = 0;
  while(buffer[index] != '\n') {
    if (buffer[index] == ' ') {
      operands[numindex] = atoll(buffer + index);
      numindex++;
    }
    index++;
  }

  // Handle operands. Initiate accumulator with the first operand.
  bool doable = solve(output, operands, ocount, 1, operands[0]);

  free(operands);
  if (doable) return output;
  return 0;
}

int main(int argc, const char** argv) {
  if ( argc < 2 ) {
    printf("Please sefine input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1] ,"r");
  if (fd == NULL) {
    printf("Unable to open file. \"%s\"\n", argv[1]);
    return 1;
  }

  // Read the line, handle the line and move on to the next one.
  unsigned long long result = 0;
  char* buffer = malloc(256 * sizeof(char));
  while(fgets(buffer, 256 * sizeof(char), fd)) {
    result += handle_line(buffer);
  }

  printf("The sum of all doable results is %lld\n", result);

  // Close the file
  fclose(fd);
  free(buffer);

  return 0;
}
