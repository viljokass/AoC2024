#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Struct to hold the following data:
//  * what plant is it            (character)
//  * id to hold the identification of a plant 
//  * how many of a different group does
//    it have next to itself      (fences, non-diagonal)
//  * if the item has been visited during this crawl
typedef struct _gridItem {
  char plant;
  int id;
  int strangers;
  bool crawl_visit;
} gridItem;

//
// File utilities
//

int scan_w(FILE* fd) {
  int w = 0;
  while(getc(fd) != '\n') w++;
  rewind(fd);
  return w;
}

int scan_h(FILE* fd) {
  int h = 0;
  for (char c = getc(fd); c != EOF; c = getc(fd))
    if (c == '\n') h++;
  rewind(fd);
  return h;
}

void parse(
  FILE* fd, 
  gridItem* grid, 
  int w, int h
) {
  char buffer[256];
  gridItem* tmp;
  for (int i = 0; i < h; ++i) {
    fgets(buffer, sizeof(buffer), fd);
    for (int j = 0; j < w; ++j) {
      tmp = &grid[i * w + j];
      tmp->plant = buffer[j];
      tmp->id = -1;
      tmp->strangers = 0;
      tmp->crawl_visit = false;
    }
  }
}

//
// ID-tagging functions
//

// A recursive function to crawl around in search for an id
int crawl(
  gridItem* grid, 
  int x, int y,
  int w, int h,
  char what
) {
  // Out of bounds
  if (x < 0 || x > w - 1) return -1;
  if (y < 0 || y > h - 1) return -1;
  gridItem* tmp = &grid[y * w + x];
  // Hey, we visited this one already!
  if (tmp->crawl_visit) return -1;
  // Leave a mark
  tmp->crawl_visit = true;
  char plant = tmp->plant;
  int id = tmp->id;
  // This is a wrong plant...
  if (plant != what) return -1;
  // Right plant with an id found!
  if (id != -1) return id;
  int crawl_res;
  // The plant is right, but there's no id. 
  // Continue search to all four directions.
  crawl_res = crawl(grid, x + 1, y, w, h, what);
  if (crawl_res != -1) return crawl_res;
  crawl_res = crawl(grid, x - 1, y, w, h, what);
  if (crawl_res != -1) return crawl_res;
  crawl_res = crawl(grid, x, y + 1, w, h, what);
  if (crawl_res != -1) return crawl_res;
  crawl_res = crawl(grid, x, y - 1, w, h, what);
  if (crawl_res != -1) return crawl_res;
  return -1;
}

// Tag all the individual groups of plants with
// with an unique ID
int id_tag(gridItem* grid, int w, int h) {
  int roll_id = 0;
  int found_id;
  gridItem* tmp;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      tmp = &grid[y * w + x];
      found_id = crawl(grid, x, y, w, h, tmp->plant);
      // Reset visit status
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          grid[i * w + j].crawl_visit = false;
        }
      }
      // A proper ID!
      if (found_id != -1) {
        tmp->id = found_id;
      }
      // No ID found -> new ID.
      else {
        tmp->id = roll_id;
        roll_id++;
      }
    }
  }
  // We'll be using this number later.
  return roll_id;
}

//
// Fencing functions
//

// See if an item is a friend
int is_same(gridItem* grid, int x, int y, int w, int h, char what) {
  if (x < 0 || x > w - 1) return 0;
  if (y < 0 || y > h - 1) return 0;
  if (grid[y * w + x].plant == what) return 1;
  return 0;
}

// Calculate all the strangers next to this one.
void calc_strangers(gridItem* grid, int x, int y, int w, int h) {
  gridItem* tmp = &grid[y * w + x];
  char what = tmp->plant;
  int friends = is_same(grid, x + 1, y, w, h, what) +
                is_same(grid, x - 1, y, w, h, what) +
                is_same(grid, x, y + 1, w, h, what) +
                is_same(grid, x, y - 1, w, h, what);

  tmp->strangers = 4 - friends;
}

// Calculate the stranger count, which is the amount of fences.
void set_strangers(gridItem* grid, int w, int h) {
  gridItem* tmp;
  for(int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      calc_strangers(grid, x, y, w, h);
    }
  }
}

//
// Monetary functions
//

// Calculate the price of the fencing.
int calc_price(gridItem* grid, int w, int h, int groups) {
  int sum = 0;
  int area;
  int fences;
  gridItem* tmp;
  // Iterate over each distinct group
  for (int i = 0; i < groups; ++i) {
    area = 0;
    fences = 0;
    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        tmp = &grid[y * w + x];
        if (tmp->id == i) {
          area++;
          fences += tmp->strangers;
        }
      }
    }
    // printf("%d * %d\n", area, fences);
    sum += area * fences;
  }
  return sum;
}

//
// Main function
//

// The gist of this solution is to tag all the separate areas with an ID.
// Since same plants can grow in different groups, simply using the plant won't do
// with this solution. Then for each plant, set number of not-the-same-plant next to
// it, which corresponds to the amount of fences needed for that plant.
// Then iterate over the grid unique plant-groups times and calc all the plants (area)
// and all the fences of that area (fences) and multiply. Simple enough.
int main(int argc, const char** argv) {
  if (argc < 2) {
    printf("Please define an input file.");
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    printf("Unable to open file \"%s\"\n", argv[1]);
    return 1;
  }

  // Parse the file to data structure
  int w = scan_w(fd);
  int h = scan_h(fd);
  gridItem* grid = malloc(w * h * sizeof(gridItem));
  parse(fd, grid, w, h);

  // We can close the file now
  fclose(fd);

  // Tag every item and get the number of groups
  // Set the strangers, i.e. fence counts
  int group_c = id_tag(grid, w, h);
  set_strangers(grid, w, h);

  // Calculate the price using the id's
  int price = calc_price(grid, w, h, group_c);
  printf("The price of the fencing is %d\n", price);

  // Free mallocated memory
  free(grid);
  return 0;
}
