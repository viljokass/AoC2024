#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

#define ull unsigned long long

// a return type for splitting the number in two
typedef struct _splitret {
  ull a;
  ull b;
} splitret;

// Count the digits of a number
int count_digits(ull number) {
  ull tmp = number;
  int dg = 0;
  while (tmp != 0) {
    tmp /= 10;
    dg++;
  }
  return dg;
}

// Split the number and return the split number
splitret split_num(ull number) {
  splitret split;
  int exponent = count_digits(number) / 2;
  ull a = number;
  for (int i = 0; i < exponent; ++i) a /= 10;
  split.a = a;
  for (int i = 0; i < exponent; ++i) a *= 10;
  split.b = number - a;
  return split;
}

// Copy array to another
void copy(
  std::unordered_map<ull, ull>& target_map,
  std::unordered_map<ull, ull>& source_map
) {
  target_map.clear();
  for (auto& rock: source_map) {
    target_map.insert({rock.first, rock.second});
  }
} 

// Do one blink. For each rock in the starting map (orig_map),
// put stones to the new map according to the given rules.
void blink(std::unordered_map<ull, ull>& orig_map) {
  std::unordered_map<ull, ull> map;
  for (auto rock: orig_map) {
    if (rock.first == 0)
      map[1] += rock.second;
    else if (count_digits(rock.first) % 2 == 0) {
      splitret s = split_num(rock.first);
      map[s.a] += rock.second;
      map[s.b] += rock.second;
    }
    else
      map[rock.first * 2024] += rock.second;
  }
  copy(orig_map, map);
}

// I came across a discussion on this, which told about the use
// of hashmaps to store the stone count. That is kinda the point of
// the whole solution, and I feel a little bad for not coming up with
// it by myself. But then again we all stand on the shoulders of giants.
// Hadn't really dabbled with hashmaps before, nice to have a new tool.
int main(int argc, const char** argv) {
  if (argc < 2) {
    std::cout << "Please define an input file." << std::endl;
    return 1;
  }
  FILE* fd = fopen(argv[1], "r");
  if (fd == NULL) {
    std::cout << "Unable to open \"" << argv[1] << "\"" << std::endl;
    return 1;
  }

  // Get the input string into a buffer
  char buffer[256];
  fgets(buffer, sizeof(buffer), fd);
  fclose(fd);

  std::stringstream ss(buffer);
  
  // Initiate the base rock map with original stones
  std::unordered_map<ull, ull> rock_map;
  ull num;
  while (ss >> num)
    rock_map[num] += 1;

  // Blinking commences
  int k = 75;
  for (int i = 0; i < k; ++i)
    blink(rock_map);
  // Count the rocks
  ull count = 0;
  for (auto& rock: rock_map)
    count += rock.second;
  std::cout << "After " << k << " blinks, there's " << count << " rocks." << std::endl;
  return 0;
}
