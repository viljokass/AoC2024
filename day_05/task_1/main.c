#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Update struct. Holds fields for:
//    * update page numbers
//    * amount of these page numbers
//    * whether the update is valid
typedef struct update_t {
  int* pages;
  int pageCount;
  bool valid;
} update;

// Use this to consume the rules out of the way from file.
// If two consecutive characters read from fd are \n, return.
void consume_rules(FILE* fd) {
  char curr = ' ';
  char prev = ' ';
  while(true) {
    curr = getc(fd);
    if (curr == '\n' && prev == '\n') break;
    prev = curr;
  }
}

// If two consecutive characters read from fd are \n,
// Rules have been consumed. Otherwise, if a char read from fd is \n,
// we've got ourselves a new rule.
int get_rule_count(FILE* fd) {
  int rules = 0;
  char prev = ' ';
  char current = ' ';
  while (true) {
    current = getc(fd);
    if (prev == '\n' && current == '\n') break;
    if (current == '\n') rules++;
    prev = current;
  }
  rewind(fd);
  return rules;
}

// As above, but this time we parse stuff to the allocated ruleset array.
void parse_rules(FILE* fd, int* ruleset, int ruleCount) {
  char* buffer = malloc(16 * sizeof(char));
  for (int i = 0; i < ruleCount; ++i) {
    fgets(buffer, 16 * sizeof(char), fd);
    ruleset[i * 2]      = atoi(buffer);
    ruleset[i * 2 + 1]  = atoi(buffer + 3);
  }
  free(buffer);
  rewind(fd);
}

// Get the count of updates. First, consume the rules out of the way from fd, then
// For each \n, we've got a new update. Continue until we hit EOF.
int get_update_count(FILE* fd) { 
  consume_rules(fd);
  char curr = ' ';
  int updateCount = 0;
  while(true) {
    curr = getc(fd);
    if (curr == EOF) break;
    if (curr == '\n') updateCount++;
  }
  rewind(fd);
  return updateCount;  
}

// As above, but this time we allocate updates to the list of update structs.
// We can take advantage of the fact that every update page number seems to be 2 chars long.
void parse_updates(FILE* fd, update* updates, int updateCount) {
  consume_rules(fd);
  char* buffer = malloc(256 * sizeof(char));
  for (int i = 0; i < updateCount; ++i) {
    fgets(buffer, 256 * sizeof(char), fd);
    int buffer_index = 0;
    int number_count = 1;
    while(buffer[buffer_index] != '\n') {
      if (buffer[buffer_index] == ',')
        number_count++;
      buffer_index++;
    }
    updates[i].pages = malloc(number_count * sizeof(int));
    for (int j = 0; j < number_count; ++j) {
      updates[i].pages[j] = atoi(buffer + 3 * j);
    }
    updates[i].pageCount = number_count;
    updates[i].valid = true;
  }
  free(buffer);
}

// Iterate over each update with each rule, then get the middle number
// from the valid updates and add them up. The gist of it is that
// iterating over each update with the rules control the valid-flag
// and only the valid numbers are considered for the count.
int solve(int* rules, int ruleCount, update* updates, int updateCount) {

  // Determine the valid updates
  for (int i = 0; i < updateCount; ++i) {
    update* currentUpdate = &updates[i];
    for (int j = 0; j < ruleCount; ++j) {
      // The rules and the indices they were found. If rule is not found,
      // the index stays -1.
      int rule_l = rules[j * 2];
      int rule_l_index = -1;
      int rule_r = rules[j * 2 + 1];
      int rule_r_index = -1;

      int upc = currentUpdate->pageCount;
      for (int k = 0; k < upc; ++k) {
        if (currentUpdate->pages[k] == rule_l)
          rule_l_index = k;
        if (currentUpdate->pages[k] == rule_r)
          rule_r_index = k;
      }
      if (rule_l_index != -1 && rule_r_index != -1) {
        if (rule_l_index > rule_r_index) currentUpdate->valid = false;
      }
    }
  }

  // Get the middle number form each valid update
  int result = 0;
  for (int i = 0; i < updateCount; ++i) {
    update* currentUpdate = &updates[i];
    if (currentUpdate->valid) {
      int middle = currentUpdate->pages[currentUpdate->pageCount/2];
      result += middle;
    }
  }
  return result;
}

int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define input file.\n");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"", argv[1]);
    return 1;
  }

  // Scan the file for the amount of rules, allocate array for rules and parse the rules
  int ruleCount = get_rule_count(fd);
  int* ruleset = malloc(2 * ruleCount * sizeof(int));
  parse_rules(fd, ruleset, ruleCount);

  // Scan the file for the amount of updates, allocate array of updates and parse the updates
  int updateCount = get_update_count(fd);
  update* updates = malloc(updateCount * sizeof(update));
  parse_updates(fd, updates, updateCount);

  // We can close the file now.
  fclose(fd);

  // Now that we have parsed stuff, we can finally get to the actual problem at hand.
  int result = solve(ruleset, ruleCount, updates, updateCount);
  printf("The sum of the middle numbers of the correctly ordered updates is %d.\n", result);

  // Free the allocated memory.
  for (int i = 0; i < updateCount; ++i) {
    free(updates[i].pages);
  }
  free(updates);
  free(ruleset);
  return 0;
}
