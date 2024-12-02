#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <vector>

using Report = std::vector<int>;
using Reports = std::vector<Report>;

void readReports(const std::string& filePath, Reports& reports)
{
  std::ifstream fileInput{filePath};

  if (!fileInput.is_open())
  {
    throw std::invalid_argument("Input file not found.");
  }

  std::string line;

  while (std::getline(fileInput, line))
  {
    std::vector<int> report;
    std::stringstream levelStream(line);
    int level;

    while(levelStream >> level)
    {
      report.push_back(level);
    }
    reports.push_back(report);
  }
}

bool isReportStrictlyMonotone(const Report& report)
{
  bool isIncreasing = report[0] < report[1];

  for (int levelItr = 1; levelItr < report.size(); ++levelItr)
  {
    int prevLevel = report[levelItr - 1];
    int currentLevel = report[levelItr];

    if (isIncreasing)
    {
      if (prevLevel >= currentLevel)
      {
        return false;
      }
    }
    else
    {
      if (prevLevel <= currentLevel)
      {
        return false;
      }
    }
  }

  return true;
}

bool isReportDifferenceSafe(const Report& report)
{
  for (int levelItr = 1; levelItr < report.size(); ++levelItr)
  {
    int levelDistance = std::abs(report[levelItr - 1] - report[levelItr]);
    
    if ((levelDistance < 1) || (levelDistance > 3))
    {
      return false;
    }
  }

  return true;
}

bool isReportSafe(const Report& report)
{
  return isReportStrictlyMonotone(report) && isReportDifferenceSafe(report);
}

bool isReportSafeWithDampener(const Report& report)
{
  for (int dampenedLevelItr = 0; dampenedLevelItr < report.size(); ++dampenedLevelItr)
  {
    Report dampenedReport(report);
    dampenedReport.erase(dampenedReport.begin() + dampenedLevelItr);

    if (isReportSafe(dampenedReport))
    {
      return true;
    }
  }

  return false;
}

int countSafeReports(const Reports& reports)
{
  int safeReportsCount = 0;

  for (const auto& report : reports)
  {
    if (isReportSafe(report) || isReportSafeWithDampener(report))
    {
      ++safeReportsCount;
    }
  }

  return safeReportsCount;
}

int main()
{
  Reports reports;

  readReports("input.txt", reports);
  std::cout << "Number of safe reports(" << reports.size() << "): " << countSafeReports(reports) << std::endl;

  return 0;
}