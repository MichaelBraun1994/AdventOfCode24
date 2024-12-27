#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
#include <array>
#include <deque>

enum class Direction {
  UP, DOWN, LEFT, RIGHT
};

std::array<Direction, 4> DIRECT_NEIGHBOR_DIRECTIONS {
  Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT
};

struct Position {
  size_t x;
  size_t y;

  bool operator==(const Position& other) const
  {
    return ((x == other.x) && (y == other.y));
  }

  bool operator!=(const Position& other) const
  {
    return !((*this) == other);
  }

  Position& advance(Direction dir)
  {
    switch (dir) {
      case Direction::UP:
        --y;
        break;
      case Direction::DOWN:
        ++y;
        break;
      case Direction::LEFT:
        --x;
        break;
      case Direction::RIGHT:
        ++x;
        break;
    }
    return *this;
  }
};

std::array<Position, 4> getNeighbourPositions(const Position& pos)
{
  return std::array<Position, 4>{{
    { pos.x + 1, pos.y},
    { pos.x - 1, pos.y},
    { pos.x, pos.y + 1},
    { pos.x, pos.y - 1}
  }};
}

template<typename T>
class Map {
  std::vector<std::vector<T>> data{};
public:
  Map(size_t xDimension, size_t yDimension) : data(yDimension, std::vector<T>(xDimension)) {}

  T get(const Position& pos) const
  {
    return data[pos.y][pos.x];
  }

  void set(const Position& pos, T value)
  {
    data[pos.y][pos.x] = value;
  }

  size_t getSizeX() const
  {
    return data.front().size();
  }

  size_t getSizeY() const
  {
    return data.size();
  }

  bool contains(const Position& pos) const
  {
    return ((pos.x >= 0) && (pos.x < getSizeX())
      && (pos.y >= 0) && (pos.y < getSizeY()));
  }

  void print() const
  {
    for (const auto& line : data)
    {
      for (const auto& plant : line)
      {
        std::cout << plant;
      }
      std::cout << std::endl;
    }
  }
};

using PlantMap = Map<char>;

PlantMap readPlantMap(std::string_view filePath)
{
  std::string filePathTerminated{filePath};
  std::fstream file{filePathTerminated};

  if (!file.is_open())
  {
    throw std::runtime_error{"Unable to open file: " + filePathTerminated};
  }

  std::string currentLine{};
  size_t numberOfLines = 0;
  size_t lineLength = 0;

  while (std::getline(file, currentLine))
  {
    ++numberOfLines;
  }
  lineLength = currentLine.size();

  file.clear();
  file.seekg(0);

  PlantMap map{lineLength, numberOfLines};

  size_t yPos = 0;
  while (std::getline(file, currentLine))
  {
    size_t xPos = 0;

    std::stringstream plantStream{currentLine};
    std::vector<char> dataLine{};
    char plant{};

    while (plantStream >> plant)
    {
      map.set({xPos++, yPos}, plant);
    }

    ++yPos;
  }
  return map;
}

struct Region {
  long long area;
  long long perimeter;
  char plant;
  std::vector<Position> positions;

  void print() const
  {
    size_t maxX = 0;
    size_t maxY = 0;
    size_t minX = std::numeric_limits<size_t>::max();
    size_t minY = std::numeric_limits<size_t>::max();

    for (const auto& pos : positions)
    {
      maxX = std::max(pos.x, maxX);
      maxY = std::max(pos.y, maxY);
      minX = std::min(pos.x, minX);
      minY = std::min(pos.y, minY);
    }

    Map<bool> submap{maxX - minX + 1, maxY - minY + 1};
    for (const auto& pos : positions)
    {
      submap.set({pos.x - minX, pos.y - minY}, true);
    }
    
    submap.print();
  }
};

int calculateNumberOfFences(const Position& pos, const PlantMap& map, char plant)
{
  int numberOfFences = 0;

  for (const auto& testPosition : getNeighbourPositions(pos))
  {
    if (!map.contains(testPosition) || (map.get(testPosition) != plant))
    {
      ++numberOfFences;
    }
  }
  return numberOfFences;
}

void readRegionFloodFill(const PlantMap& map, Region& region, const Position& startPos, Map<bool>& positionsVisited)
{
  std::vector<Position> processingStack{startPos};
  positionsVisited.set(startPos, true);

  while (!processingStack.empty())
  {
    Position currentPosition = processingStack.back();
    processingStack.pop_back();

    ++region.area;
    region.perimeter += calculateNumberOfFences(currentPosition, map, region.plant);
    region.positions.push_back(currentPosition);

    for (const auto& testPosition : getNeighbourPositions(currentPosition))
    {
      if (map.contains(testPosition)
       && (!positionsVisited.get(testPosition))
       && (map.get(testPosition) == region.plant))
      {
        processingStack.push_back(testPosition);
        positionsVisited.set(testPosition, true);
      }
    }
  }
}

std::vector<Region> readRegions(const PlantMap& map)
{
  std::vector<Region> regions{};
  Map<bool> positionsVisited{map.getSizeX(), map.getSizeY()};

  for (size_t y = 0; y < map.getSizeY(); ++y)
  {
    for (size_t x = 0; x < map.getSizeX(); ++x)
    {
      if (positionsVisited.get({x, y}))
      {
        continue;
      }

      Region newRegion{.plant = map.get({x, y})};
      readRegionFloodFill(map, newRegion, {x, y}, positionsVisited);
      regions.push_back(newRegion);
    }
  }
  return regions;
}

bool areDirectNeighbors(const Position& posA, const Position& posB)
{
  int xA = posA.x;
  int xB = posB.x;

  int yA = posA.y;
  int yB = posB.y;

  return (((std::abs(xA - xB) == 1) && (yA == yB))
    || ((std::abs(yA - yB) == 1) && (xA == xB)));
}

void extendSide(std::vector<Position>& edgesOfType, std::deque<Position>& side)
{
  bool foundAdditionalSideEdge = true;

  while (foundAdditionalSideEdge)
  {
    foundAdditionalSideEdge = false;

    auto frontEdgeItr = std::find_if(
        edgesOfType.begin(), edgesOfType.end(), [&side](const auto &upEdge) {
          return areDirectNeighbors(side.front(), upEdge);
        });

    if (frontEdgeItr != edgesOfType.end())
    {
      side.push_front(*frontEdgeItr);
      edgesOfType.erase(frontEdgeItr);
      foundAdditionalSideEdge = true;
    }

    auto backEdgeItr = std::find_if(
        edgesOfType.begin(), edgesOfType.end(), [&side](const auto &upEdge) {
          return areDirectNeighbors(side.back(), upEdge);
        });

    if (backEdgeItr != edgesOfType.end())
    {
      side.push_back(*backEdgeItr);
      edgesOfType.erase(backEdgeItr);
      foundAdditionalSideEdge = true;
    }
  }
}

int countSidesOfType(std::vector<Position>& edgesOfType)
{
  int numberOfSides = 0;
  while (!edgesOfType.empty())
  {
    Position currentEdge = edgesOfType.back();
    edgesOfType.pop_back();

    std::deque<Position> side{currentEdge};
    extendSide(edgesOfType, side);

    ++numberOfSides;
  }
  return numberOfSides;
}

int countSides(const Region& region, const PlantMap& map)
{
  int numberOfSides = 0;

  for (const auto& direction : DIRECT_NEIGHBOR_DIRECTIONS)
  {
    std::vector<Position> directedEdges{};

    for (const auto& pos : region.positions)
    {
      Position probePos{pos};
      probePos.advance(direction);

      if (!map.contains(probePos) || (map.get(probePos) != region.plant))
      {
        directedEdges.push_back(pos);
      }
    }
    numberOfSides += countSidesOfType(directedEdges);
  }
  return numberOfSides;
}

int main()
{
  PlantMap map = readPlantMap("input.txt");
  map.print();

  std::vector<Region> regions = readRegions(map);

  long long price = 0;
  constexpr int OUTPUT_WIDTH = 3;
  for (const auto& region : regions)
  {
    int sides = countSides(region, map);
    std::cout << "Plant: " << region.plant
      << " Area: "      << std::setw(OUTPUT_WIDTH) << region.area
      << " Perimeter: " << std::setw(OUTPUT_WIDTH) << region.perimeter
      << " Sides: "     << std::setw(OUTPUT_WIDTH) << sides
      << " Price (1): " << std::setw(OUTPUT_WIDTH) << region.area * region.perimeter
      << " Price (2): " << std::setw(OUTPUT_WIDTH) << region.area * sides << std::endl;

    price += region.area * sides;
  }

  std::cout << "Price: " << price << std::endl;

  return 0;
}