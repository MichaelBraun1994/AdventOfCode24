#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
#include <array>


enum class Direction {
  UP, DOWN, LEFT, RIGHT
};

Direction turnLeft(const Direction dir)
{
  switch (dir) {
    case Direction::LEFT:
      return Direction::DOWN;
    case Direction::UP:
      return Direction::LEFT;
    case Direction::RIGHT:
      return Direction::UP;
    case Direction::DOWN:
      return Direction::RIGHT;
  }
}

Direction turnRight(const Direction dir)
{
  switch (dir) {
    case Direction::LEFT:
      return Direction::UP;
    case Direction::UP:
      return Direction::RIGHT;
    case Direction::RIGHT:
      return Direction::DOWN;
    case Direction::DOWN:
      return Direction::LEFT;
  }
}

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

int countSides(const Region& region)
{
  size_t maxX = 0;
  size_t maxY = 0;
  size_t minX = std::numeric_limits<size_t>::max();
  size_t minY = std::numeric_limits<size_t>::max();

  for (const auto& pos : region.positions)
  {
    maxX = std::max(pos.x, maxX);
    maxY = std::max(pos.y, maxY);
    minX = std::min(pos.x, minX);
    minY = std::min(pos.y, minY);
  }

  Map<bool> submap{maxX - minX + 1, maxY - minY + 1};
  for (const auto& pos : region.positions)
  {
    submap.set({pos.x - minX, pos.y - minY}, true);
  }
  
  std::cout << std::endl << "//////////" << std::endl;
  submap.print();
  std::cout << "//////////" << std::endl;

  Position mostLeftLowestPosition{
    .x = std::numeric_limits<size_t>::max(),
    .y = 0
  };

  for (const auto& pos : region.positions)
  {
    if ((pos.x <= mostLeftLowestPosition.x)
      && (pos.y >= mostLeftLowestPosition.y))
    {
      mostLeftLowestPosition = pos;
    }
  }
  mostLeftLowestPosition.x -= minX;
  mostLeftLowestPosition.y -= minY;

  Position startPos{mostLeftLowestPosition};
  Direction startDirection{Direction::LEFT};

  Position currentPos{startPos};
  Direction currentDirection{startDirection};

  Map<char> edgeMap{submap.getSizeX(), submap.getSizeY()};
  for (size_t y = 0; y < edgeMap.getSizeY(); ++y)
  {
    for (size_t x = 0; x < edgeMap.getSizeX(); ++x)
    {
      edgeMap.set({x, y}, '.');
    }
  }

  do {
    switch (currentDirection) {
      case Direction::UP:
        edgeMap.set({currentPos.x, currentPos.y}, '^');
        break;
      case Direction::DOWN:
        edgeMap.set({currentPos.x, currentPos.y}, 'v');
        break;
      case Direction::LEFT:
        edgeMap.set({currentPos.x, currentPos.y}, '<');
        break;
      case Direction::RIGHT:
        edgeMap.set({currentPos.x, currentPos.y}, '>');
        break;
    }

    Position left{currentPos};
    left.advance(turnLeft(currentDirection));

    Position advanced{currentPos};
    advanced.advance(currentDirection);

    if (submap.contains(left) && submap.get(left))
    {
      currentDirection = turnLeft(currentDirection);
      currentPos = left;
    }
    else if (submap.contains(advanced) && submap.get(advanced))
    {
      currentPos = advanced;
    }
    else
    {
      currentDirection = turnRight(currentDirection);
    }

    std::cout << std::endl << "==========" << std::endl;
    edgeMap.print();
    std::cout << "==========" << std::endl;
  } while (!((currentPos == startPos) && (currentDirection == startDirection)));

  std::cout << std::endl << "==========" << std::endl;
  edgeMap.print();
  std::cout << "==========" << std::endl;

  return 0;
}

int main()
{
  PlantMap map = readPlantMap("exampleInput.txt");
  map.print();

  std::vector<Region> regions = readRegions(map);

  long long price = 0;
  constexpr int OUTPUT_WIDTH = 3;
  for (const auto& region : regions)
  {
    std::cout << "Plant: " << region.plant << std::endl;
    if (region.plant == 'M')
    {
      countSides(region);
    }
    //<< " Area: "      << std::setw(OUTPUT_WIDTH) << region.area
    //<< " Perimeter: " << std::setw(OUTPUT_WIDTH) << region.perimeter
    //<< " Sides: "     << std::setw(OUTPUT_WIDTH) << countSides(region)
    //<< " Price (1): " << std::setw(OUTPUT_WIDTH) << region.area * region.perimeter
    //<< " Price (2): " << std::setw(OUTPUT_WIDTH) << region.area * countSides(region) << std::endl;

    price += region.area * region.perimeter;
  }

  std::cout << "Price: " << price << std::endl;

  return 0;
}