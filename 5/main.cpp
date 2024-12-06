#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <utility>

struct Rules {
  std::map<int, std::vector<int>> leftRules;
  std::map<int, std::vector<int>> rightRules;
};

using Updates = std::vector<std::vector<int>>;
using Update = std::vector<int>;

void readInput(const std::string& filePath, Rules& rules, Updates& updates)
{
  std::fstream file{filePath};
  Rules retVal{};

  if (!file.is_open())
  {
    throw std::invalid_argument{"Failed to open input file."};
  }

  std::string currentLine;
  while (std::getline(file, currentLine) && (currentLine != ""))
  {
    std::stringstream ss{currentLine};
    int leftNumber;
    int rightNumber;

    ss >> leftNumber;
    ss.ignore(1);
    ss >> rightNumber;

    rules.leftRules[rightNumber].push_back(leftNumber);
    rules.rightRules[leftNumber].push_back(rightNumber);
  }

  while (std::getline(file, currentLine))
  {
    std::stringstream ss{currentLine};
    std::string page{};
    std::vector<int> update{};

    while (getline(ss, page, ','))
    {
      update.push_back(std::stoi(page));
    }

    updates.push_back(update);
  }
}

bool isUpdateValid(Update& update, Rules& rules)
{
  for (auto currentPageIter = update.begin(); currentPageIter != update.end(); ++currentPageIter)
  {
    int currentPage = *currentPageIter;
    const auto& leftRules = rules.leftRules[currentPage];
    const auto& rightRules = rules.rightRules[currentPage];

    for (auto leftPagesIter = update.begin(); leftPagesIter != currentPageIter; ++leftPagesIter)
    {
      if (std::find(rightRules.begin(), rightRules.end(), *leftPagesIter) != rightRules.end())
      {
        return false;
      }
    }

    for (auto rightPagesIter = currentPageIter + 1; rightPagesIter != update.end(); ++rightPagesIter)
    {
      if (std::find(leftRules.begin(), leftRules.end(), *rightPagesIter) != leftRules.end())
      {
        return false;
      }
    }
  }
  return true;
}

void fixUpdate(Update& update, Rules& rules)
{
  for (auto currentPageIter = update.begin(); currentPageIter != update.end(); ++currentPageIter)
  {
    int currentPage = *currentPageIter;
    const auto& leftRules = rules.leftRules[currentPage];
    const auto& rightRules = rules.rightRules[currentPage];

    for (auto leftPagesIter = update.begin(); leftPagesIter != currentPageIter; ++leftPagesIter)
    {
      if (std::find(rightRules.begin(), rightRules.end(), *leftPagesIter) != rightRules.end())
      {
        std::swap(*leftPagesIter, *currentPageIter);
        currentPageIter = update.begin();
        break;
      }
    }

    for (auto rightPagesIter = currentPageIter + 1; rightPagesIter != update.end(); ++rightPagesIter)
    {
      if (std::find(leftRules.begin(), leftRules.end(), *rightPagesIter) != leftRules.end())
      {
        std::swap(*rightPagesIter, *currentPageIter);
        currentPageIter = update.begin();
        break;
      }
    }
  }
}

int getMiddlePage(Update& update)
{
  return update[update.size() / 2];
}

void getMiddlePageSums(Updates& updates, Rules& rules, int& validUpdatesSum, int& fixedUpdatesSum)
{
  for (auto& update : updates)
  {
    if (isUpdateValid(update, rules))
    {
      validUpdatesSum += getMiddlePage(update);
    }
    else
    {
      fixUpdate(update, rules);
      if (!isUpdateValid(update, rules))
      {
        throw std::runtime_error{"Update fixing failed!"};
      }
      fixedUpdatesSum += getMiddlePage(update);
    }
  }
}

int main(int argc, char** argv)
{
  Rules rules{};
  Updates updates{};

  readInput("input.txt", rules, updates);

  int validUpdatesMiddlePageSum = 0;
  int fixedUpdatesMiddlePageSum = 0;
  getMiddlePageSums(updates, rules, validUpdatesMiddlePageSum, fixedUpdatesMiddlePageSum);

  std::cout << "Sum of valid updates middle pages: " << validUpdatesMiddlePageSum << std::endl;
  std::cout << "Sum of fixed updates middle pages: " << fixedUpdatesMiddlePageSum << std::endl;

  return 0;
}