#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define mask 0b111

typedef struct _computer {
  int a;
  int b;
  int c;
  int* program;
  int progc;
  int progpoint;
} computer;

//
// Parsing
//

int count_commas(FILE* fd) {
  int commas = 0;
  char c = ' ';
  while (c != EOF) {
    c = getc(fd);
    if (c == ',') commas++;
  }
  rewind(fd);
  return commas;
}

void parse(FILE* fd, computer* comp) {
  // Get program count
  int program_count = count_commas(fd) + 1;
  comp->progc = program_count;
  comp->progpoint = 0;
  comp->program = malloc(program_count * sizeof(int));

  // Get A register
  char buffer[256];
  fgets(buffer, sizeof(buffer), fd);
  int bufferIndex = 0;
  while(buffer[bufferIndex] != 'A') {
    bufferIndex++;
  }
  bufferIndex += 2;

  // Initiate registers (seems that all except A are 0)
  comp->a = atoi(buffer + bufferIndex);
  comp->b = 0;
  comp->c = 0;

  char curr = ' ';
  char prev = ' ';
  while(!(curr == '\n' && prev == '\n')) {
    prev = curr;
    curr = getc(fd);
  }
  while(curr != ':') {
    curr = getc(fd);
  }
  // Empty char out
  getc(fd);
  for(int i = 0; i < program_count; ++i) {
    comp->program[i] = getc(fd) - '0';
    getc(fd);
  }
}

//
// Compute
//

int pow_2(int exponent) {
  int start = 1;
  for (int i = 0; i < exponent; ++i) {
    start *= 2;
  }
  return start;
}

int combo_operand(computer* comp, int operand) {
  if (0 <= operand && 3 >= operand) return operand;
  if (operand == 4) return comp->a;
  if (operand == 5) return comp->b;
  if (operand == 6) return comp->c;
  // Invalid operand!
  if (operand == 7) return -1;
}

void adv(computer* comp, int operand) {
  int numer = comp->a;
  int denom = pow_2(combo_operand(comp, operand));
  comp->a = numer/denom;
  comp->progpoint += 2;
}
void bxl(computer* comp, int operand) {
  comp->b = comp->b ^ operand;
  comp->progpoint += 2;
}
void bst(computer* comp, int operand) {
  comp->b = (combo_operand(comp, operand) % 8) & mask;
  comp->progpoint += 2;
}
int jnz(computer* comp, int operand) {
  if (comp->a == 0) {
    comp->progpoint += 2;
  } else {
    comp->progpoint = operand;
  }
}
void bxc(computer* comp, int operand) {
  comp->b = comp->b ^ comp->c;
  comp->progpoint += 2;
}
bool out(computer* comp, int operand, bool first_print) {
  if (!first_print) printf(",");
  printf("%d", (combo_operand(comp, operand) % 8) & mask);
  comp->progpoint += 2;
}
void bdv(computer* comp, int operand) {
  int numer = comp->a;
  int denom = pow_2(combo_operand(comp, operand));
  comp->b = numer/denom;
  comp->progpoint += 2;
}
void cdv(computer* comp, int operand) {
  int numer = comp->a;
  int denom = pow_2(combo_operand(comp, operand));
  comp->c = numer/denom;
  comp->progpoint += 2;
}

void compute(computer* comp) {
  bool first_print = true;
  int progp = 0;
  int progc = comp->progc;
  while(progp < progc) {
    progp = comp->progpoint;
    switch (comp->program[progp]) {
      case 0:
        adv(comp, comp->program[progp + 1]);
        break;
      case 1:
        bxl(comp, comp->program[progp + 1]);
        break;
      case 2:
        bst(comp, comp->program[progp + 1]);
        break;
      case 3:
        jnz(comp, comp->program[progp + 1]);
        break;
      case 4:
        bxc(comp, comp->program[progp + 1]);
        break;
      case 5:
        first_print = out(comp, comp->program[progp + 1], first_print);
        break;
      case 6:
        bdv(comp, comp->program[progp + 1]);
        break;
      case 7:
        cdv(comp, comp->program[progp + 1]);
        break;
    }
  }
  printf("\n");
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

  computer comp;
  parse(fd, &comp);
  // We can close the file now
  fclose(fd);

  // Do the computing
  compute(&comp);
/*
  printf("Register A: %d\n", comp.a);
  printf("Register B: %d\n", comp.b);
  printf("Register C: %d\n", comp.c);
  printf("\nProgram: %d", comp.program[0]);
  for(int i = 1; i < comp.progc; ++i) {
    printf(",%d", comp.program[i]);
  }
  printf("\n");
*/
  free(comp.program);
  return 0;
}
