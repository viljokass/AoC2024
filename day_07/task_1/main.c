#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Recursive function that uses accumulation
bool solve(
  unsigned long long output, 
  unsigned long long* operands,
  int operandCount,
  int currentInd,
  unsigned long long accumulator
) {
  if (currentInd == operandCount) {
    return accumulator == output;
  }
  return solve(output, operands, operandCount, currentInd + 1, accumulator + operands[currentInd]) ||
         solve(output, operands, operandCount, currentInd + 1, accumulator * operands[currentInd]);
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
