#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>

struct Position
{
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

using Positions = std::unordered_set<Position, Position::HashFunction>;
using TowerPositions = std::map<char, Positions>;

class Map {
  std::vector<std::vector<char>> data{};
  TowerPositions towerPositions{};

  const char EMPTY_MARK = '.';

  void load(const std::string& filePath)
  {
    std::fstream file{filePath};

    if (!file.is_open())
    {
      throw std::runtime_error{"Unable to open file: " + filePath};
    }

    std::string currentLine{};
    while (std::getline(file, currentLine))
    {
      std::vector<char> lineData{};
      std::stringstream ss{currentLine};

      char currentCharacter;
      while (ss >> currentCharacter)
      {
        lineData.push_back(currentCharacter);

        if (currentCharacter != EMPTY_MARK)
        {
          Position currentPosition{lineData.size() - 1, data.size()};
          towerPositions[currentCharacter].insert(currentPosition);
        }
      }
      data.push_back(lineData);
    }
  }

public:
  Map(const std::string& filePath)
  {
    load(filePath);
  }

  size_t getSizeX() const
  {
    return data.front().size();
  }

  size_t getSizeY() const
  {
    return data.front().size();
  }

  bool isInMap(const Position& pos) const
  {
    return ((pos.x >= 0)
      && (pos.y >= 0)
      && (pos.x < getSizeX())
      && (pos.y < getSizeY()));
  }

  char get(const Position& pos) const
  {
    if (!isInMap(pos))
    {
      throw std::invalid_argument{"Position not in map!"};
    }
    return data[pos.y][pos.x];
  }

  void set(const Position& pos, char mark)
  {
    if (!isInMap(pos))
    {
      throw std::invalid_argument{"Position not in map!"};
    }
    data[pos.y][pos.x] = mark;
  }

  TowerPositions getTowerPositions() const
  {
    return towerPositions;
  }

  void print()
  {
    for (size_t y = 0; y < getSizeY(); ++y)
    {
      for (size_t x = 0; x < getSizeX(); ++x)
      {
        std::cout << get({x, y});
      }
      std::cout << std::endl;
    }
  }

  void printTowerPositions()
  {
    for (const auto& [frequency, positions] : towerPositions)
    {
      std::cout << "frequency " << frequency << ": ";

      for (const auto& position : positions)
      {
        std::cout << "(" << position.x << ", " << position.y << ") ";
      }
      std::cout << std::endl;
    }
  }
};

Positions calculateAntiNodePositionsOfTowerPair(const Map& map, const Position& posTowerA, const Position& posTowerB)
{
  Positions antiNodePositionsOfTowerPair{};

  antiNodePositionsOfTowerPair.insert(posTowerA);
  antiNodePositionsOfTowerPair.insert(posTowerB);

  size_t distanceX = posTowerB.x - posTowerA.x;
  size_t distanceY = posTowerB.y - posTowerA.y;

  int stepFactorA = 1;
  int stepFactorB = 1;

  Position antiNodePosA{posTowerA.x - stepFactorA * distanceX, posTowerA.y - stepFactorA * distanceY};
  Position antiNodePosB{posTowerB.x + stepFactorB * distanceX, posTowerB.y + stepFactorB * distanceY};

  while (map.isInMap(antiNodePosA))
  {
    antiNodePositionsOfTowerPair.insert(antiNodePosA);

    ++stepFactorA;
    antiNodePosA = {posTowerA.x - stepFactorA * distanceX, posTowerA.y - stepFactorA * distanceY};
  }
  while (map.isInMap(antiNodePosB))
  {
    antiNodePositionsOfTowerPair.insert(antiNodePosB);

    ++stepFactorB;
    antiNodePosB = {posTowerB.x + stepFactorB * distanceX, posTowerB.y + stepFactorB * distanceY};
  }

  return antiNodePositionsOfTowerPair;
}

Positions calculateAntiNodePositionsOfFrequency(const Map& map, const Positions& towerPositions)
{
  Positions antiNodePositionsOfFrequency{};

  for (auto itrA = towerPositions.begin(); itrA != towerPositions.end(); ++itrA)
  {
    const Position& posA = *itrA;

    for (auto itrB = std::next(itrA); itrB != towerPositions.end(); ++itrB)
    {
      const Position& posB = *itrB;
      Positions antiNodePositionsOfTowerPair = calculateAntiNodePositionsOfTowerPair(map, posA, posB);
      antiNodePositionsOfFrequency.insert(antiNodePositionsOfTowerPair.begin(), antiNodePositionsOfTowerPair.end());
    }
  }

  return antiNodePositionsOfFrequency;
}

int countDistinctAntiNodePositions(Map& map)
{
  std::unordered_set<Position, Position::HashFunction> antiNodePositions{};

  for (const auto& [frequency, towerPositions] : map.getTowerPositions())
  {
    Positions antiNodePositionOfFrequency = calculateAntiNodePositionsOfFrequency(map, towerPositions);
    antiNodePositions.insert(antiNodePositionOfFrequency.begin(), antiNodePositionOfFrequency.end());
  }

  return antiNodePositions.size();
}

int main()
{
  Map map{"input.txt"};
  map.print();
  map.printTowerPositions();

  std::cout << "Number of distinct anti nodes: " << countDistinctAntiNodePositions(map) << std::endl;

  return 0;
}