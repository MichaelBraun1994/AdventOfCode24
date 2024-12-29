#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <regex>
#include <chrono>

using namespace std::literals::chrono_literals;

template<typename T>
struct Vec2 {
  T x;
  T y;

  Vec2 operator+(const Vec2& other)
  {
    return {x + other.x, y + other.y};
  }
  Vec2 operator-(const Vec2& other)
  {
    return {x - other.x, y - other.y};
  }
};

using Vec2i = Vec2<int64_t>;

constexpr Vec2i MAX_POSITION = {100, 102};
constexpr Vec2i MID_POSITION = {MAX_POSITION.x / 2, MAX_POSITION.y / 2};

class Robot{
  Vec2i position{};
  Vec2i velocity{};

  Vec2i calculateTeleportLocation(const Vec2i& newPosition) const
  {
    Vec2i teleportLocation{newPosition};

    const int64_t DX = MAX_POSITION.x + 1;
    const int64_t DY = MAX_POSITION.y + 1;

    if (newPosition.x < 0)
    {
      teleportLocation.x = MAX_POSITION.x - (std::abs(newPosition.x + 1) % DX);
    }
    if (newPosition.y < 0)
    {
      teleportLocation.y = MAX_POSITION.y - (std::abs(newPosition.y + 1) % DY);
    }

    if (newPosition.x > MAX_POSITION.x)
    {
      teleportLocation.x = (std::abs(newPosition.x - MAX_POSITION.x - 1) % DX);
    }
    if (newPosition.y > MAX_POSITION.y)
    {
      teleportLocation.y = (std::abs(newPosition.y - MAX_POSITION.y - 1) % DY);
    }
    return teleportLocation;
  }

public:
  Robot(const Vec2i& position,
        const Vec2i& velocity)
    : position(position), velocity(velocity) {}
  
  Vec2i getPosition() const
  {
    return position;
  }

  void setPosition(const Vec2i& value)
  {
    position = value;
  }

  Vec2i getVelocity() const
  {
    return velocity;
  }

  void setVelocity(const Vec2i& value)
  {
    velocity = value;
  }

  void translate()
  {
    Vec2i newPosition = position + velocity;

    position = calculateTeleportLocation(newPosition);
  }

  void print() const
  {
    std::cout << "p(" << std::setw(2) << position.x << ", " << std::setw(2) << position.y << ") "
              << "v(" << std::setw(2) << velocity.x << ", " << std::setw(2) << velocity.y << ")"
              << std::endl;
  }
};

std::vector<Robot> readRobots(const std::string& inputPath)
{
  std::vector<Robot> robots{};

  std::fstream file{inputPath};

  if (!file.is_open())
  {
    throw std::runtime_error{"Unable to open file: " + inputPath};
  }

  std::regex pattern{"p=(\\d+),(\\d+)\\s+v=(-?\\d+),(-?\\d+)"};
  std::smatch matches{};

  std::string currentLine{};
  while (std::getline(file, currentLine))
  {
    if (!std::regex_search(currentLine, matches, pattern))
    {
      throw std::runtime_error{"Invalid robot input format."};
    }
    
    Vec2i robotPosition{std::stoll(matches[1]), std::stoll(matches[2])};
    Vec2i robotVelocity{std::stoll(matches[3]), std::stoll(matches[4])};

    robots.emplace_back(robotPosition, robotVelocity);
  }
  return robots;
}

struct Quadrant {
  Vec2i min{};
  Vec2i max{};

  unsigned long long robotCount{};

  bool contains(const Vec2i& position)
  {
    return ((position.x >= min.x)
         && (position.x <= max.x)
         && (position.y >= min.y)
         && (position.y <= max.y));
  }
};

std::vector<Quadrant> countRobotsInQuadrants(const std::vector<Robot>& robots)
{

  std::vector<Quadrant> quadrants{
    {{                 0,                  0}, {MID_POSITION.x - 1, MID_POSITION.y - 1}},
    {{MID_POSITION.x + 1,                  0}, {MAX_POSITION.x,     MID_POSITION.y - 1}},
    {{                 0, MID_POSITION.y + 1}, {MID_POSITION.x - 1, MAX_POSITION.y    }},
    {{MID_POSITION.x + 1, MID_POSITION.y + 1}, {MAX_POSITION.x,     MAX_POSITION.y    }}
  };

  for (const auto& robot : robots)
  {
    for (auto& quadrant : quadrants)
    {
      if (quadrant.contains(robot.getPosition()))
      {
        ++quadrant.robotCount;
      }
    }
  }
  return quadrants;
}

long long calculateSafetyFactor(std::vector<Robot>& robots)
{
  long long safetyFactor = 1;

  for (auto& quadrant : countRobotsInQuadrants(robots))
  {
    safetyFactor *= quadrant.robotCount;
  }
  return safetyFactor;
}

bool isInTree(const Vec2i& pos)
{
  return ((pos.x >= MID_POSITION.x - pos.y) && (pos.x <= MID_POSITION.x + pos.y));
}

void printRobots(const std::vector<Robot>& robots)
{
  size_t dx = MAX_POSITION.x + 1;
  size_t dy = MAX_POSITION.y + 1;

  std::vector<std::vector<bool>> field(dy, std::vector<bool>(dx, false));

  for (const auto& robot : robots)
  {
    Vec2i robotPosition = robot.getPosition();

    field[robotPosition.y][robotPosition.x] = true;
  }

  for (size_t y = 0; y < field.size(); ++y)
  {
    for (size_t x = 0; x < field.front().size(); ++x)
    {
      if (isInTree({x, y}))
      {
        if (field[y][x])
        {
          std::cout << "\033[32m";
        }
        else
        {
          std::cout << "\033[31m";
        }
      }

      if (field[y][x])
      {
        std::cout << 'X';
      }
      else
      {
        std::cout << '.';
      }
      std::cout << "\033[0m";
    }
    std::cout << std::endl;
  }
}

double calculateTreeScore(const std::vector<Robot>& robots)
{
  double robotsInShape = 0;
  for (const auto& robot : robots)
  {
    if (isInTree(robot.getPosition()))
    {
      ++robotsInShape;
    }
  }
  return robotsInShape / robots.size();
}

int main()
{
  //std::vector<Robot> robots = readRobots("exampleInput.txt");
  std::vector<Robot> robots = readRobots("input.txt");

  for (size_t seconds = 0; seconds < 10000; ++seconds)
  {
    for (auto& robot : robots)
    {
      const Vec2i robotPosition = robot.getPosition();
      const Vec2i robotVelocity = robot.getVelocity();

      robot.translate();
    }
    if (calculateTreeScore(robots) > 0.90)
    {
      printRobots(robots);
      std::cout << seconds << std::endl;
    }
  }


  std::cout << "Safety factor: " << calculateSafetyFactor(robots) << std::endl;

  return 0;
}