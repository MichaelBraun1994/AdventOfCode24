#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

enum class Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

Direction turnRight(Direction direction)
{
  switch (direction) {
    case Direction::UP:
      return Direction::RIGHT;
    case Direction::DOWN:
      return Direction::LEFT;
    case Direction::LEFT:
      return Direction::UP;
    case Direction::RIGHT:
      return Direction::DOWN;
  }
}

struct Position {
  size_t x;
  size_t y;

  void stepTo(Direction direction)
  {
    switch (direction) {
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
      default:
        throw std::runtime_error{"Invalid direction."};
    }
  }
  bool operator==(const Position& other)
  {
    return ((x == other.x) && (y == other.y));
  }
  bool operator!=(const Position& other)
  {
    return !(*this == other);
  }
};

struct TurnRecord {
  Position pos;
  Direction incomingDirection;

  bool operator==(const TurnRecord& other)
  {
    return ((pos == other.pos) && (incomingDirection == other.incomingDirection));
  }
  bool operator!=(const TurnRecord& other)
  {
    return !(*this == other);
  }
};

class Map {
  std::vector<std::vector<char>> data{};
  Position startPosition{};

  void load(const std::string& filePath)
  {
    std::fstream file{filePath};

    if (!file.is_open())
    {
      throw std::invalid_argument{"Failed to open input file."};
    }

    std::string currentLine;
    while (std::getline(file, currentLine))
    {
      std::vector<char> line;

      for (const auto& mark : currentLine)
      {
        line.push_back(mark);

        if (mark == '^')
        {
          startPosition = {line.size() - 1, data.size() };
        }
      }
      data.push_back(line);
    }
  }

public:
  const char OBSTACLE_MARK = '#';

  Map() = default;
  Map(const std::string& filePath)
  {
    load(filePath);
  }
  Map(const Map& other) = default;
 
  Map& operator=(const Map& other)
  {
    if (this == &other)
    {
      return *this;
    }

    startPosition = other.startPosition;
    data = other.data;

    return *this;
  }

  int getSizeX() { return data.front().size(); }
  int getSizeY() { return data.size(); }
  Position getStartPosition() { return startPosition; }

  bool isObstacle(const Position& pos)
  {
    return (data[pos.y][pos.x] == OBSTACLE_MARK);
  };

  void setObstacle(const Position& pos)
  {
    data[pos.y][pos.x] = OBSTACLE_MARK;
  };

  bool isInMap(const Position& pos)
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
      for (const auto& mark : line)
      {
        std::cout << mark;
      }
      std::cout << std::endl;
    }
  }
};

class Agent {
  struct State
  {
    Map map{};
    Position position{};
    std::vector<TurnRecord> turnRecords{};
    Direction direction{};
  } state;

  std::vector<Position> obstacleCandidates;

  enum class PatrolStatus {
    LOOP_DETECTED,
    LEFT_MAP,
    PROCESSING
  };

  const char VISITED_MARK = 'X';

public:
  Agent(Map map) {
    state.map = map;
    state.position = state.map.getStartPosition();
    state.direction = Direction::UP;

    findObstacleCandidates();
  };

  int getDistinctAgentPositions()
  {
    return obstacleCandidates.size();
  }

  void findObstacleCandidates()
  {
      State stateBackup = state;

      while (patrol() == PatrolStatus::PROCESSING)
      {
        if ((state.position != state.map.getStartPosition()) && (std::find(obstacleCandidates.begin(), obstacleCandidates.end(), state.position) == obstacleCandidates.end()))
        {
          obstacleCandidates.push_back(state.position);
        }
      }

      state = stateBackup;
  }


  bool turnWasTaken(TurnRecord turnRecord)
  {
    return (std::find(state.turnRecords.begin(), state.turnRecords.end(), turnRecord) != state.turnRecords.end());
  }

  PatrolStatus patrol()
  {
    Position newPosition{state.position};
    newPosition.stepTo(state.direction);

    if (!state.map.isInMap(newPosition))
    {
      return PatrolStatus::LEFT_MAP;
    }

    if (!state.map.isObstacle(newPosition))
    {
      state.position = newPosition;
    }
    else
    {
      TurnRecord turnRecord{state.position, state.direction};

      if (turnWasTaken(turnRecord))
      {
        return PatrolStatus::LOOP_DETECTED;
      }

      state.turnRecords.push_back(turnRecord);
      state.direction = turnRight(state.direction);
    }

    return PatrolStatus::PROCESSING;
  }

  int countPossibleLoops()
  {
    int possibleLoopCounter = 0;

    while (!obstacleCandidates.empty())
    {
      State stateBackup = state;

      state.map.setObstacle(obstacleCandidates.back());
      obstacleCandidates.pop_back();

      PatrolStatus patrolStatus;
      do {
        patrolStatus = patrol();
      }while (patrolStatus == PatrolStatus::PROCESSING);

      if (patrolStatus == PatrolStatus::LOOP_DETECTED)
      {
        ++possibleLoopCounter;
      }

      state = stateBackup;
    }

    return possibleLoopCounter;
  }
};

int main(int argc, char** argv)
{
  Map map{"input.txt"};
  Agent agent{map};

  std::cout << "Distinct positions (task1): " << agent.getDistinctAgentPositions() << std::endl;
  std::cout << "Possible loops (task2): " << agent.countPossibleLoops() << std::endl;

  return 0;
}