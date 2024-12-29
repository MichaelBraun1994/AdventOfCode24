#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

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

  Position operator+(const Position& other) const
  {
    return {x + other.x, y + other.y};
  }

  Position operator-(const Position& other) const
  {
    return {x - other.x, y - other.y};
  }
};

Position operator*(const Position& position, size_t value)
{
  return {position.x * value, position.y * value};
}

Position operator*(size_t value, const Position& position)
{
  return position * value;
}

enum class Direction {
  UP, DOWN, LEFT, RIGHT
};

enum class Tile {
  WALL, BOX, EMPTY 
};

class Map{
  std::vector<std::vector<Tile>> data{};
  Position startPosition{};
  std::vector<Direction> moveInstructions{};

  Direction parseDirectionCharacter(const char directionCharacter)
  {
    Direction direction;

    switch (directionCharacter) {
      case '>':
        direction = Direction::RIGHT;
        break;
      case '^':
        direction = Direction::UP;
        break;
      case 'v':
        direction = Direction::DOWN;
        break;
      case '<':
        direction = Direction::LEFT;
        break;
      default:
        throw std::runtime_error{"Invalid direction type."};
    }
    return direction;
  }

  char parseDirection(const Direction& direction) const
  {
    char directionCharacter;

    switch (direction) {
      case Direction::UP:
        directionCharacter = '^';
        break;
      case Direction::DOWN:
        directionCharacter = 'v';
        break;
      case Direction::LEFT:
        directionCharacter = '<';
        break;
      case Direction::RIGHT:
        directionCharacter = '>';
        break;
      default:
        throw std::runtime_error{"Invalid direction type."};
    }
    return directionCharacter;
  }

  Tile parseTileCharacter(const char tileCharacter)
  {
    Tile tile;

    switch (tileCharacter) {
      case '#':
        tile = Tile::WALL;
        break;
      case 'O':
        tile = Tile::BOX;
        break;
      case '.':
        tile = Tile::EMPTY;
        break;
      default:
        throw std::runtime_error{"Invalid tile type."};
    }
    return tile;
  }

  char parseTile(const Tile& tile) const
  {
    char tileCharacter;

    switch (tile) {
      case Tile::WALL:
        tileCharacter = '#';
        break;
      case Tile::BOX:
        tileCharacter = 'O';
        break;
      case Tile::EMPTY:
        tileCharacter = '.';
        break;
      default:
        throw std::runtime_error{"Invalid tile type."};
    }
    return tileCharacter;
  }

public:
  Map(const std::string& filePath)
  {
    std::fstream file{filePath};

    if (!file.is_open())
    {
      throw std::runtime_error{"Unable to open file: " + filePath};
    }

    std::string currentLine{};
    size_t y = 0;
    while (std::getline(file, currentLine))
    {
      if (currentLine == "")
      {
        break;
      }

      std::vector<Tile> currentMapLine{};
      size_t x = 0;

      for (const auto& tileCharacter : currentLine)
      {
        if (tileCharacter == '@')
        {
          startPosition = {x, y};
          currentMapLine.push_back(Tile::EMPTY);
        }
        else
        {
          currentMapLine.push_back(parseTileCharacter(tileCharacter));
        }
        ++x;
      }
      data.push_back(currentMapLine);
      ++y;
    }

    while (std::getline(file, currentLine))
    {
      for (const auto& directionCharacter : currentLine)
      {
        moveInstructions.push_back(parseDirectionCharacter(directionCharacter));
      }
    }
  }

  std::vector<Direction> getMoveInstructions() const
  {
    return moveInstructions;
  }

  Position getStartPosition() const
  {
    return startPosition;
  }

  Tile get(const Position& pos) const
  {
    return data[pos.y][pos.x];
  }

  void set(const Position& pos, Tile value)
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
    return ((pos.x >= 0)
      && (pos.x < getSizeX())
      && (pos.y >= 0)
      && (pos.y < getSizeY()));
  }

  void print(const Position& robotPosition) const
  {
    for (size_t y = 0; y < getSizeY(); ++y)
    {
      for (size_t x = 0; x < getSizeY(); ++x)
      {
        if (robotPosition == Position{x, y})
        {
          std::cout << "@";
        }
        else
        {
          std::cout << parseTile(get({x,y}));
        }
      }
      std::cout << std::endl;
    }
  }

  void printMoveInstructions() const
  {
    for (const auto& moveInstruction : moveInstructions)
    {
      std::cout << parseDirection(moveInstruction);
    }
    std::cout << std::endl;
  }
};

class Robot {
  Map& map;
  Position position{};

  Position getStepPosition(const Direction& direction)
  {
    Position stepPosition{position};

    switch (direction)
    {
      case Direction::UP:
        stepPosition.y -= 1;
      break;
      case Direction::DOWN:
        stepPosition.y += 1;
      break;
      case Direction::LEFT:
        stepPosition.x -= 1;
      break;
      case Direction::RIGHT:
        stepPosition.x += 1;
      break;
    }
    return stepPosition;
  }

  Position getPositionInDirection(const Position& position, const Direction& direction, size_t n)
  {
    Position positionOffset{0, 0};

    switch (direction) {
      case Direction::UP:
        positionOffset.y = -1;
        break;
      case Direction::DOWN:
        positionOffset.y = 1;
        break;
      case Direction::LEFT:
        positionOffset.x = -1;
        break;
      case Direction::RIGHT:
        positionOffset.x = 1;
        break;
    }
    return position + n * positionOffset;
  }

  bool moveBoxes(const Position& newPositionCandidate, const Direction& direction)
  {
    bool movedBoxes = false;

    Tile nextNonBoxTile{Tile::BOX};
    Position lookupPosition{newPositionCandidate};
    size_t n = 0;

    while (nextNonBoxTile == Tile::BOX)
    {
      lookupPosition = getPositionInDirection(newPositionCandidate, direction, n++);
      nextNonBoxTile = map.get(lookupPosition);
    }

    if (nextNonBoxTile == Tile::EMPTY)
    {
      map.set(newPositionCandidate, Tile::EMPTY);
      map.set(lookupPosition, Tile::BOX);
      movedBoxes = true;
    }

    return movedBoxes;
  }

public:
  Robot(Map& map) : map(map), position(map.getStartPosition()) {}

  Position getPosition() const
  {
    return position;
  }

  void move(const Direction& direction)
  {
    Position newPositionCandidate = getStepPosition(direction);
    Tile tileAhead = map.get(newPositionCandidate);

    if (tileAhead == Tile::EMPTY)
    {
      position = newPositionCandidate;
    }
    else if (tileAhead == Tile::BOX)
    {
      if (moveBoxes(newPositionCandidate, direction))
      {
        position = newPositionCandidate;
      }
    }
  }
};

long long calculateGPSSum(const Map& map)
{
  long long gpsSum = 0;

  for (size_t y = 0; y < map.getSizeY(); ++y)
  {
    for (size_t x = 0; x < map.getSizeX(); ++x)
    {
      if (map.get({x, y}) == Tile::BOX)
      {
        gpsSum += y * 100 + x;
      }
    }
  }
  return gpsSum;
}

int main()
{
  Map map{"exampleInput.txt"};
  Map map{"input.txt"};
  Robot robot{map};

  for (const auto& moveInstruction : map.getMoveInstructions())
  {
    robot.move(moveInstruction);
  }
  map.print(robot.getPosition());

  std::cout << "GPS Sum: " << calculateGPSSum(map) << std::endl;

  return 0;
}