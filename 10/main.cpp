/*
* A good hiking trail starts at 0 and goes to 9. Always increases by a height of 1 at each step.
* No diagonal steps included.
*
* Trailheads start one or more trails. Always at height 0.
* A trailhead score is the number of 9 reachable from there.
*
* Sum of trailhead score is output.
*/

/*
* 1. For each trailhead DFS for all peaks.
* 2. If peak reached add trailhead to peaks set.
* 3. Calculate trailhead scores via peak sets.
*/

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

struct Position {
  size_t x;
  size_t y;

  bool operator==(const Position& other) const
  {
    return ((x == other.x) && (y == other.y));
  }

  bool operator!=(const Position& other) const
  {
    return !(*this == other);
  }

  struct HashFunction
  {
    size_t operator()(const Position& pos) const
    {
      size_t xHash = std::hash<size_t>()(pos.x);
      size_t yHash = std::hash<size_t>()(pos.y) << 1;
      return xHash ^ yHash;
    }
  };
};

int charToInt(char c)
{
  return static_cast<int>(c - '0');
}

char intToChar(int i)
{
  return static_cast<int>('0' + i);
}

class Map {
  std::vector<std::vector<int>> data{};
  std::vector<Position> trailHeads{};
  std::vector<Position> peaks{};
public:
  Map(std::string_view filePath)
  {
    std::string filePathTerminated{filePath};
    std::fstream file{filePathTerminated};

    if (!file.is_open())
    {
      throw std::runtime_error{"Unable to open file: " + filePathTerminated};
    }

    std::string currentLine{};

    Position currentPosition{
      .x = 0,
      .y = 0
    };
    while (std::getline(file, currentLine))
    {
      currentPosition.x = 0;
      std::vector<int> line{};

      for (const char c : currentLine)
      {
        int height = charToInt(c);
        line.push_back(height);

        if (height == 0)
        {
          trailHeads.push_back(currentPosition);
        }
        else if (height == 9)
        {
          peaks.push_back(currentPosition);
        }

        ++currentPosition.x;
      }

      data.push_back(line);
      ++currentPosition.y;
    }
  }

  std::vector<Position> getTrailheads()
  {
    return trailHeads;
  }

  std::vector<Position> getPeaks()
  {
    return peaks;
  }

  int get(const Position& pos)
  {
    return data[pos.y][pos.x];
  }

  void set(const Position& pos, const int value)
  {
    data[pos.y][pos.x] = value;
  }

  int getSizeX()
  {
    return data.front().size();
  }

  int getSizeY()
  {
    return data.size();
  }

  bool contains(const Position& pos)
  {
    return ((pos.x >= 0)
      && (pos.y >= 0)
      && (pos.x < getSizeX())
      && (pos.y < getSizeY()));
  }

  void print()
  {
    for (const auto& line : data)
    {
      for (const auto& height : line)
      {
        std::cout << height;
      }
        std::cout << std::endl;
    }

  }
};

// Part1 using ReachablePeaks = std::unordered_map<Position, std::unordered_set<Position, Position::HashFunction>, Position::HashFunction>;
using ReachablePeaks = std::unordered_map<Position, std::vector<Position>, Position::HashFunction>;

ReachablePeaks calculateReachablePeaks(Map& map)
{
  ReachablePeaks reachablePeaks{};
  std::vector<Position> trailHeads = map.getTrailheads();

  for (const auto& trailHead : trailHeads)
  {
    std::vector<Position> positionsToProcess{trailHead};

    while (!positionsToProcess.empty())
    {
      Position currentPosition = positionsToProcess.back();
      positionsToProcess.pop_back();

      int height = map.get(currentPosition);
      if (height == 9)
      {
        // Part1 reachablePeaks[trailHead].insert(currentPosition);
        reachablePeaks[trailHead].push_back(currentPosition);
      }
      else
      {
        std::vector<Position> moveCandidates{
            {.x = currentPosition.x, .y = currentPosition.y + 1},
            {.x = currentPosition.x, .y = currentPosition.y - 1},
            {.x = currentPosition.x - 1, .y = currentPosition.y},
            {.x = currentPosition.x + 1, .y = currentPosition.y}};

        for (const auto& moveCandidate : moveCandidates)
        {
          if (map.contains(moveCandidate) && (map.get(moveCandidate) == height + 1))
          {
            positionsToProcess.push_back(moveCandidate);
          }
        }
      }
    }
  }

  return reachablePeaks;
}

long long calculateScore(ReachablePeaks& reachablePeaks)
{
  long long score = 0;

  for (const auto& [trailHead, peaks]: reachablePeaks)
  {
    score += peaks.size();
  }

  return score;
}

int main()
{
  Map map{"input.txt"};
  map.print();
  std::cout << std::endl;

  std::cout << "Peaks: ";
  for (const auto& peak : map.getPeaks())
  {
    std::cout << "(" << peak.x << ", " << peak.y << ") ";
  }
  std::cout << std::endl;

  std::cout << "Heads: ";
  for (const auto& trailHead : map.getTrailheads())
  {
    std::cout << "(" << trailHead.x << ", " << trailHead.y << ") ";
  }
  std::cout << std::endl << std::endl;

  ReachablePeaks reachablePeaks = calculateReachablePeaks(map);
  std::cout << "Reachable Peaks: " << std::endl;
  for (const auto& [trailHead, peaks]: reachablePeaks)
  {
    std::cout << "\t[" << trailHead.x << ", " << trailHead.y << "]: ";
    for (const auto& peak : peaks)
    {
      std::cout << "(" << peak.x << ", " << peak.y << ") ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl; 

  std::cout << "Score: " << calculateScore(reachablePeaks) << std::endl;

  return 0;
}