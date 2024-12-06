#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

using WordMatrix = std::vector<std::string>;

enum class DIRECTION {
  LEFT,
  LEFT_UP,
  LEFT_DOWN,
  RIGHT,
  RIGHT_UP,
  RIGHT_DOWN,
  UP,
  DOWN
};

struct Position {
  int x;
  int y;
};

WordMatrix readWordMatrix(const std::string& path)
{
  std::fstream file{path};
  WordMatrix retVal;

  if (!file.is_open())
  {
    throw std::invalid_argument{"Failed to open word matrix file."};
  }

  std::string currentLine;
  while(std::getline(file, currentLine))
  {
    retVal.push_back(currentLine);
  }
  return retVal;
}

void advanceSearchPos(Position& pos, DIRECTION dir)
{
  switch (dir) {
    case DIRECTION::LEFT:
      --pos.x;
      break;
    case DIRECTION::LEFT_UP:
      --pos.x;
      ++pos.y;
      break;
    case DIRECTION::LEFT_DOWN:
      --pos.x;
      --pos.y;
      break;
    case DIRECTION::RIGHT:
      ++pos.x;
      break;
    case DIRECTION::RIGHT_UP:
      ++pos.x;
      ++pos.y;
      break;
    case DIRECTION::RIGHT_DOWN:
      ++pos.x;
      --pos.y;
      break;
    case DIRECTION::UP:
      ++pos.y;
      break;
    case DIRECTION::DOWN:
      --pos.y;
      break;
  }
}

inline bool isPositionInBounds(Position& pos, int maxX, int maxY)
{
  return (pos.x >= 0)
      && (pos.y >= 0)
      && (pos.x < maxX)
      && (pos.y < maxY);
  }

/**
 * @brief Searches for a word beginning at pos in direction dir.
 */
bool searchWord(WordMatrix& wordMatrix, Position pos, DIRECTION dir)
{
  const std::string SEARCH_WORD = "XMAS";
  const int MAX_Y = wordMatrix.size();
  const int MAX_X = wordMatrix[0].size();

  Position searchPos = pos;

  for(const auto c : SEARCH_WORD)
  {
    if (!isPositionInBounds(searchPos, MAX_X, MAX_Y)
      || (c != wordMatrix[searchPos.y][searchPos.x]))
    {
      return false;
    }

    advanceSearchPos(searchPos, dir);
  }

  return true;
}

/**
 * @brief Counts all word occurrences beginning at pos xPos yPos.
 */
int countWordsAtPosition(WordMatrix& wordMatrix, Position pos)
{
  int wordCounter = 0;

  if (searchWord(wordMatrix, pos, DIRECTION::LEFT)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::LEFT_UP)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::LEFT_DOWN)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::RIGHT)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::RIGHT_UP)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::RIGHT_DOWN)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::UP)) { ++wordCounter; }
  if (searchWord(wordMatrix, pos, DIRECTION::DOWN)) { ++wordCounter; }

  return wordCounter;
}

int countWords(WordMatrix& wordMatrix)
{
  const int MAX_Y = wordMatrix.size();
  const int MAX_X = wordMatrix[0].size();

  int wordCounter = 0;

  for (int y = 0; y < MAX_Y; ++y)
  {
    for (int x = 0; x < MAX_X; ++x)
    {
      if (wordMatrix[y][x] == 'X')
      {
        wordCounter += countWordsAtPosition(wordMatrix, {x, y});
      }
    }
  }

  return wordCounter;
}

bool isXmas(WordMatrix& wordMatrix, Position pos)
{
  Position leftUp   { .x = pos.x - 1, .y = pos.y + 1 };
  Position leftDown { .x = pos.x - 1, .y = pos.y - 1 };
  Position rightUp  { .x = pos.x + 1, .y = pos.y + 1 };
  Position rightDown{ .x = pos.x + 1, .y = pos.y - 1 };

  int masCounter = 0;

  if ((wordMatrix[rightUp.y][rightUp.x] == 'M') && (wordMatrix[leftDown.y][leftDown.x] == 'S')) { ++masCounter; }
  if ((wordMatrix[rightUp.y][rightUp.x] == 'S') && (wordMatrix[leftDown.y][leftDown.x] == 'M')) { ++masCounter; }
  if ((wordMatrix[rightDown.y][rightDown.x] == 'M') && (wordMatrix[leftUp.y][leftUp.x] == 'S')) { ++masCounter; }
  if ((wordMatrix[rightDown.y][rightDown.x] == 'S') && (wordMatrix[leftUp.y][leftUp.x] == 'M')) { ++masCounter; }

  return (masCounter == 2);
}

int countXmas(WordMatrix& wordMatrix)
{
  const int MAX_Y = wordMatrix.size();
  const int MAX_X = wordMatrix[0].size();

  int wordCounter = 0;

  for (int y = 1; y < MAX_Y - 1; ++y)
  {
    for (int x = 1; x < MAX_X - 1; ++x)
    {
      if ((wordMatrix[y][x] == 'A') && isXmas(wordMatrix, {x, y}))
      {
        ++wordCounter;
      }
    }
  }

  return wordCounter;
}

int main(int argc, char** argv)
{
  WordMatrix wordMatrix = readWordMatrix("input.txt");

  std::cout << "XMAS  found " << countWords(wordMatrix) << " times." << std::endl;
  std::cout << "X-MAS found " << countXmas(wordMatrix) << " times." << std::endl;

  return 0;
}