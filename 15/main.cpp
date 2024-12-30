#include <conio.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>

#include <queue>

struct Position
{
  size_t x;
  size_t y;

  bool operator==(const Position& other) const { return ((x == other.x) && (y == other.y)); }
  bool operator!=(const Position& other) const { return !(*this == other); }
  Position operator+(const Position& other) const { return {x + other.x, y + other.y}; }
  Position operator-(const Position& other) const { return {x - other.x, y - other.y}; }
};

Position operator*(const Position& position, size_t value)
{
  return {position.x * value, position.y * value};
}

Position operator*(size_t value, const Position& position)
{
  return position * value;
}

enum class Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class Tile
{
  WALL,
  BOX,
  BOX_LEFT,
  BOX_RIGHT,
  EMPTY
};

class Map
{
  std::vector<std::vector<Tile>> data{};
  Position startPosition{};
  std::vector<Direction> moveInstructions{};

public:
  Direction parseDirectionCharacter(const char directionCharacter)
  {
    Direction direction;

    switch (directionCharacter)
    {
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

    switch (direction)
    {
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

  std::vector<Tile> parseTilesCharacter(const char tileCharacter)
  {
    std::vector<Tile> tiles;

    switch (tileCharacter)
    {
      case '#':
        tiles.push_back(Tile::WALL);
        tiles.push_back(Tile::WALL);
        break;
      case 'O':
        tiles.push_back(Tile::BOX_LEFT);
        tiles.push_back(Tile::BOX_RIGHT);
        break;
      case '.':
        tiles.push_back(Tile::EMPTY);
        tiles.push_back(Tile::EMPTY);
        break;
      default:
        throw std::runtime_error{"Invalid tile type."};
    }
    return tiles;
  }

  char parseTile(const Tile& tile) const
  {
    char tileCharacter;

    switch (tile)
    {
      case Tile::WALL:
        tileCharacter = '#';
        break;
      case Tile::BOX:
        tileCharacter = 'O';
        break;
      case Tile::BOX_LEFT:
        tileCharacter = '[';
        break;
      case Tile::BOX_RIGHT:
        tileCharacter = ']';
        break;
      case Tile::EMPTY:
        tileCharacter = '.';
        break;
      default:
        throw std::runtime_error{"Invalid tile type."};
    }
    return tileCharacter;
  }

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
      if (currentLine.empty())
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
          currentMapLine.push_back(Tile::EMPTY);
        }
        else
        {
          std::vector<Tile> tiles = parseTilesCharacter(tileCharacter);

          for (const auto& tile : tiles)
          {
            currentMapLine.push_back(tile);
          }
        }
        x += 2;
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

  std::vector<Direction> getMoveInstructions() const { return moveInstructions; }
  Position getStartPosition() const { return startPosition; }
  Tile get(const Position& pos) const { return data[pos.y][pos.x]; }
  void set(const Position& pos, Tile value) { data[pos.y][pos.x] = value; }
  size_t getSizeX() const { return data.front().size(); }
  size_t getSizeY() const { return data.size(); }

  bool contains(const Position& pos) const
  {
    return ((pos.x >= 0) && (pos.x < getSizeX()) && (pos.y >= 0) && (pos.y < getSizeY()));
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

struct BoxPosition
{
  Position left;
  Position right;

  BoxPosition() = default;
  BoxPosition(const BoxPosition& other) = default;

  BoxPosition(const Position& leftPos, const Position& rightPos) : left(leftPos), right(rightPos) {}

  BoxPosition(const Position& pos, const Map& map)
  {
    Tile tile = map.get(pos);

    if (tile == Tile::BOX_LEFT)
    {
      left = pos;
      right = {pos.x + 1, pos.y};
    }
    else if (tile == Tile::BOX_RIGHT)
    {
      right = pos;
      left = {pos.x - 1, pos.y};
    }
    else
    {
      throw std::runtime_error{"Invalid tile used for boxposition initialization."};
    }
  }
};

void printMap(const Map& map, const Position& robotPosition)
{
  for (size_t y = 0; y < map.getSizeY(); ++y)
  {
    for (size_t x = 0; x < map.getSizeX(); ++x)
    {
      if (robotPosition == Position{x, y})
      {
        std::cout << "@";
      }
      else
      {
        std::cout << map.parseTile(map.get({x, y}));
      }
    }
    std::cout << "\n";
  }
}

void clearScreen()
{
  HANDLE hConsole;
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  COORD startPosition = {0, 0};  // home for the cursor
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
  DWORD dwConSize;

  if (!GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo))
  {
    return;
  }

  dwConSize = consoleScreenBufferInfo.dwSize.X * consoleScreenBufferInfo.dwSize.Y;

  if (!FillConsoleOutputCharacter(hConsole,  // Handle to console screen buffer
        (TCHAR)' ',                          // Character to write to the buffer
        dwConSize,                           // Number of cells to write
        startPosition,                       // Coordinates of first cell
        &cCharsWritten))                     // Receive number of characters written
  {
    return;
  }

  // Get the current text attribute.
  if (!GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo))
  {
    return;
  }

  if (!FillConsoleOutputAttribute(hConsole,   // Handle to console screen buffer
        consoleScreenBufferInfo.wAttributes,  // Character attributes to use
        dwConSize,                            // Number of cells to set attribute
        startPosition,                        // Coordinates of first cell
        &cCharsWritten))                      // Receive number of characters written
  {
    return;
  }

  SetConsoleCursorPosition(hConsole, startPosition);
}

class Robot
{
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

  Position advancePositionInDirection(const Position& position, const Direction& direction, size_t n = 1)
  {
    Position positionOffset{0, 0};

    switch (direction)
    {
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

  std::vector<BoxPosition> getBoxesToMoveHorizontally(const Position& newPositionCandidate, const Direction& direction)
  {
    std::vector<BoxPosition> boxesToMove{
      {newPositionCandidate, map}
    };

    Position newPosition;
    Tile newPositionTile;

    do {
      newPosition = advancePositionInDirection(newPositionCandidate, direction, boxesToMove.size() * 2);
      newPositionTile = map.get(newPosition);

      if (newPositionTile == Tile::WALL)
      {
        return {};
      }
      else if (isBox(newPositionTile))
      {
        boxesToMove.push_back(BoxPosition{newPosition, map});
      }
    } while (newPositionTile != Tile::EMPTY);

    return boxesToMove;
  }

  std::vector<BoxPosition> getBoxesToMoveVertically(const Position& newPositionCandidate, const Direction& direction)
  {
    std::vector<BoxPosition> boxesToMove{};

    BoxPosition currentBox{newPositionCandidate, map};
    std::queue<BoxPosition> processingQueue{};
    processingQueue.push(currentBox);

    while (!processingQueue.empty())
    {
      currentBox = processingQueue.front();
      processingQueue.pop();

      boxesToMove.push_back(currentBox);

      Position newPositionLeft = advancePositionInDirection(currentBox.left, direction);
      Position newPositionRight = advancePositionInDirection(currentBox.right, direction);

      Tile newPositionTileLeft = map.get(newPositionLeft);
      Tile newPositionTileRight = map.get(newPositionRight);

      if ((newPositionTileLeft == Tile::WALL) || (newPositionTileRight == Tile::WALL))
      {
        return {};
      }

      if (isBox(newPositionTileLeft))
      {
        processingQueue.push(BoxPosition{newPositionLeft, map});
      }
      if (isBox(newPositionTileRight))
      {
        processingQueue.push(BoxPosition{newPositionRight, map});
      }
    }
    return boxesToMove;
  }

  std::vector<BoxPosition> getBoxesToMove(const Position& newPositionCandidate, const Direction& direction)
  {
    if ((direction == Direction::LEFT) || (direction == Direction::RIGHT))
    {
      return getBoxesToMoveHorizontally(newPositionCandidate, direction);
    }
    else if ((direction == Direction::UP) || (direction == Direction::DOWN))
    {
      return getBoxesToMoveVertically(newPositionCandidate, direction);
    }
    else
    {
      throw std::runtime_error{"Invalid box moving direction."};
    }
  }

  void moveBoxInDirection(const BoxPosition& boxPosition, const Direction& direction)
  {
    BoxPosition newBoxPos{};
    newBoxPos.left = advancePositionInDirection(boxPosition.left, direction);
    newBoxPos.right = advancePositionInDirection(boxPosition.right, direction);

    map.set(boxPosition.left, Tile::EMPTY);
    map.set(boxPosition.right, Tile::EMPTY);

    map.set(newBoxPos.left, Tile::BOX_LEFT);
    map.set(newBoxPos.right, Tile::BOX_RIGHT);
  }

  bool moveBoxes(const Position& newPositionCandidate, const Direction& direction)
  {
    std::vector<BoxPosition> boxesToMove = getBoxesToMove(newPositionCandidate, direction);

    if (boxesToMove.empty())
    {
      return false;
    }

    while (!boxesToMove.empty())
    {
      BoxPosition currentBox = boxesToMove.back();
      boxesToMove.pop_back();

      moveBoxInDirection(currentBox, direction);
    }

    return true;
  }

  bool isBox(const Tile& tile) { return ((tile == Tile::BOX_LEFT) || (tile == Tile::BOX_RIGHT)); }

public:
  Robot(Map& map) : map(map), position(map.getStartPosition()) {}

  Position getPosition() const { return position; }

  void move(const Direction& direction)
  {
    Position newPositionCandidate = getStepPosition(direction);
    Tile tileAhead = map.get(newPositionCandidate);

    if (tileAhead == Tile::EMPTY)
    {
      position = newPositionCandidate;
    }
    else if (isBox(tileAhead))
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
      if (map.get({x, y}) == Tile::BOX_LEFT)
      {
        gpsSum += y * 100 + x;
      }
    }
  }
  return gpsSum;
}

std::optional<Direction> getUserInput()
{
  int ch = _getch();

  // Arrow keys are returned as two-character codes
  if (ch == 0 || ch == 224)
  {
    switch (_getch())
    {
      case 72:
        return Direction::UP;
        break;
      case 80:
        return Direction::DOWN;
        break;
      case 75:
        return Direction::LEFT;
        break;
      case 77:
        return Direction::RIGHT;
        break;
      default:
        return std::nullopt;
    }
  }
  else
  {
    return std::nullopt;
  }
}

int main(int argc, char** argv)
{
  Map map{"input.txt"};
  Robot robot{map};

  if ((argc == 2) && std::string{argv[1]} == "-i")
  {
    std::optional<Direction> userInput{};
    do {
      clearScreen();
      printMap(map, robot.getPosition());

      userInput = getUserInput();
      robot.move(userInput.value_or(Direction::UP));
    } while (userInput.has_value());
  }
  else
  {
    for (const auto& moveInstruction : map.getMoveInstructions())
    {
      robot.move(moveInstruction);
    }
    printMap(map, robot.getPosition());
  }

  std::cout << "GPS Sum: " << calculateGPSSum(map) << std::endl;

  return 0;
}