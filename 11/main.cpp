#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>
#include <unordered_map>

using Stones = std::list<long long>;

Stones readStones(std::string_view filePath)
{
  Stones stones{};
  std::string terminatedFilePath{filePath};
  std::fstream file{terminatedFilePath};

  if (!file.is_open())
  {
    throw std::runtime_error{"Could not open file: " + terminatedFilePath};
  }

  std::string line{};
  std::getline(file, line);

  std::stringstream ss{line};

  long long stoneNumber;
  while (ss >> stoneNumber)
  {
    stones.push_back(stoneNumber);
  }

  return stones;
}

void print(const Stones& stones)
{
  for (const auto& stone : stones)
  {
    std::cout << stone << " ";
  }
  std::cout << std::endl;
}

void blink(Stones& stones)
{
  for (auto itr = stones.begin(); itr != stones.end(); ++itr)
  {
    long long& stone = *itr;
    std::string stoneInscription = std::to_string(stone);
    size_t numberOfDigits = stoneInscription.size();

    if (*itr == 0)
    {
      *itr = 1;
    }
    else if (numberOfDigits % 2 == 0)
    {
      long long leftStone = std::stoll(stoneInscription.substr(0, numberOfDigits / 2));
      long long rightStone = std::stoll(stoneInscription.substr(numberOfDigits / 2));

      stone = leftStone;
      itr = stones.insert(++itr, rightStone);
    }
    else
    {
      (*itr) *= 2024;
    }
  }
}

void blink(Stones& stones, size_t times)
{
  for (size_t blinks = 0; blinks < times; ++blinks)
  {
    blink(stones);
  }
}

long long calculateStonesSum(const Stones& stones, size_t numberOfBlinks)
{
  Stones stonesWorkingCopy{stones};

  blink(stonesWorkingCopy, numberOfBlinks);

  return stonesWorkingCopy.size();
}


long long calculateStoneSumDFS(long long initialStone, size_t numberOfBlinks)
{
  struct ProcessingNode{
    long long inscription;
    int height;
  };
  std::vector<ProcessingNode> processingStack{{initialStone, 0}};
  long long sum = 0;

  while (!processingStack.empty())
  {
    ProcessingNode currentNode = processingStack.back();
    processingStack.pop_back();

    if (currentNode.height == numberOfBlinks)
    {
      ++sum;
      continue;
    }

    std::string stoneInscription = std::to_string(currentNode.inscription);
    size_t numberOfDigits = stoneInscription.size();

    if (numberOfDigits % 2 == 0)
    {
      long long leftStone = std::stoll(stoneInscription.substr(0, numberOfDigits / 2));
      long long rightStone = std::stoll(stoneInscription.substr(numberOfDigits / 2));

      processingStack.push_back({rightStone, currentNode.height + 1});
      processingStack.push_back({leftStone,  currentNode.height + 1});
    }
    else
    {
      if (currentNode.inscription == 0)
      {
        currentNode.inscription = 1;
      }
      else
      {
        currentNode.inscription *= 2024;
      }
      ++currentNode.height;
      processingStack.push_back(currentNode);
    }
  }

  return sum;
}

using ResultCache = std::unordered_map<long long, long long>;

long long calculateStoneSumDFSCached(long long stone, size_t numberOfBlinks, ResultCache& resultCache)
{
  long long result = 0;
  long long sum = 0;

  if (resultCache.contains(stone))
  {
    sum += resultCache[stone];
  }
  else
  {
    result = calculateStoneSumDFS(stone, numberOfBlinks);
    resultCache[stone] = result;
    sum += result;
  }
  return sum;
}

long long calculateStonesSumDFS(const Stones& stones, size_t numberOfBlinks)
{
  long long sum = 0;
  size_t iterationSum = stones.size();
  size_t iteration = 0;

  ResultCache resultCache;
  long long result;

  for (const auto& stone : stones)
  {
    if ((iteration % 30000u) == 0u)
      std::cout << "(" << iteration << "/" << iterationSum << ")\n";

    iteration++;
    sum += calculateStoneSumDFSCached(stone, numberOfBlinks, resultCache);
  }

  return sum;
}

int main()
{
  Stones stones = readStones("input.txt");
  print(stones);

  constexpr int MAX_BLINKS = 38;
  blink(stones, 75 - MAX_BLINKS);

  std::cout << "Number of stones: " << stones.size() << std::endl;

  long long sum = calculateStonesSumDFS(stones, MAX_BLINKS);
  std::cout << "Number of stones (DFS): " << sum << std::endl;

  return 0;
}