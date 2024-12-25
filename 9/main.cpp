#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>

constexpr int START_ID = 0;
constexpr int EMPTY_ENTRY_ID = -1;

using Filesystem = std::vector<long long>;

struct Record
{
  int id;
  int size;
  int originalPosition;
};

std::string readFilesystem(const std::string& filePath)
{
  std::fstream file{filePath};
  if (!file.is_open())
  {
    throw std::runtime_error("Unable to open file: " + filePath);
  }

  std::string retVal;
  std::getline(file, retVal);

  return retVal;
}

int charToInt(char c)
{
  return static_cast<int>(c - '0');
}

char intToChar(int i)
{
  return static_cast<int>('0' + i);
}

Filesystem expandFilesystem(const std::string& filesystem)
{
  Filesystem expandedFileSystem{};

  bool fileExpansion = true;
  int currentId = START_ID;

  for (const char c : filesystem)
  {
    int multiplicity = charToInt(c);

    if (fileExpansion)
    {
      expandedFileSystem.insert(expandedFileSystem.end(), multiplicity, currentId++);
    }
    else
    {
      expandedFileSystem.insert(expandedFileSystem.end(), multiplicity, EMPTY_ENTRY_ID);
    }
    fileExpansion = !fileExpansion;
  }
  return expandedFileSystem;
}

Filesystem sortFileSystem(const Filesystem& expandedFilesystem)
{
  Filesystem sortedFilesystem{expandedFilesystem};
  size_t insertIndex = 0;
  size_t copyIndex = expandedFilesystem.size() - 1;

  while (insertIndex != copyIndex)
  {
    if (sortedFilesystem[insertIndex] == EMPTY_ENTRY_ID)
    {
      while (expandedFilesystem[copyIndex] == EMPTY_ENTRY_ID)
      {
        --copyIndex;
      }

      std::swap(sortedFilesystem[insertIndex], sortedFilesystem[copyIndex]);
      --copyIndex;
    }

    ++insertIndex;
  }
  return sortedFilesystem;
}

std::vector<Record> findFreeSpaces(const Filesystem& expandedFilesystem)
{
  std::vector<Record> freeSpaces{};

  bool newEntry = true;
  Record currentRecord{};

  for (size_t i = 0; i < expandedFilesystem.size(); ++i)
  {
    int id = expandedFilesystem[i];

    if (id != EMPTY_ENTRY_ID)
    {
      if (!newEntry)
      {
        freeSpaces.push_back(currentRecord);
        newEntry = true;
      }
      continue;
    }

    if (newEntry)
    {
      newEntry = false;
      currentRecord.id = id;
      currentRecord.size = 1;
      currentRecord.originalPosition = i;
    }
    else
    {
      ++currentRecord.size;
    }
  }
  return freeSpaces;
}

std::vector<Record> findFiles(const Filesystem& expandedFilesystem)
{
  std::vector<Record> files{};

  Record currentRecord{};
  bool newRecord = true;

  for (size_t i = 0; i < expandedFilesystem.size(); ++i)
  {
    int id = expandedFilesystem[i];

    if (id == EMPTY_ENTRY_ID)
    {
      continue;
    }

    if (newRecord)
    {
      currentRecord.id = id;
      currentRecord.originalPosition = i;
      currentRecord.size = 1;
      newRecord = false;
    }
    else
    {
      ++currentRecord.size;
    }

    if ((i == expandedFilesystem.size() - 1) || (expandedFilesystem[i + 1] != id))
    {
      files.push_back(currentRecord);
      newRecord = true;
    }
  }
  return files;
}

void printFilesystem(const Filesystem& expandedFilesystem)
{
  for (const auto fileID : expandedFilesystem)
  {
    if (fileID == EMPTY_ENTRY_ID)
    {
      std::cout << '.';
    }
    else
    {
      std::cout << fileID;
    }
  }
  std::cout << std::endl;
}

void swapRecords(Filesystem& expandedFilesystem, Record& src, Record& dest)
{
  for (size_t i = 0; i < src.size; ++i)
  {
    std::swap(expandedFilesystem[src.originalPosition + i],
      expandedFilesystem[dest.originalPosition + i]);
  }
}

Filesystem sortFiles(const Filesystem& expandedFilesystem)
{
  Filesystem sortedFilesystem{expandedFilesystem};
  std::vector<Record> files = findFiles(expandedFilesystem);

  while (!files.empty())
  {
    Record currentFile = files.back();
    files.pop_back();

    std::vector<Record> freeSpaces = findFreeSpaces(sortedFilesystem);
    auto freeSlotItr = std::find_if(freeSpaces.begin(), freeSpaces.end(),
      [&currentFile](const auto& record)
      { return ((record.size >= currentFile.size)
        && (record.originalPosition < currentFile.originalPosition));
      });

    if (freeSlotItr == freeSpaces.end())
    {
      continue;
    }

    swapRecords(sortedFilesystem, currentFile, *freeSlotItr);
  }

  return sortedFilesystem;
}

long long calculateChecksum(const Filesystem& expandedFilesystem)
{
  size_t position = 0;
  long long checksum = 0;

  for (const auto& id : expandedFilesystem)
  {
    if (id != EMPTY_ENTRY_ID)
    {
      checksum += id * position;
    }
    ++position;
  }

  return checksum;
}

int main(int argc, char** argv)
{
  auto filesystem = readFilesystem("input.txt");

  Filesystem expandedFilesystem = expandFilesystem(filesystem);
  Filesystem sortedFilesystem = sortFileSystem(expandedFilesystem);

  long long checksum = calculateChecksum(sortedFilesystem);
  std::cout << "Checksum(1): " << checksum << std::endl;

  Filesystem sortedFiles = sortFiles(expandedFilesystem);
  checksum = calculateChecksum(sortedFiles);
  std::cout << "Checksum(2): " << checksum << std::endl;

  return 0;
}