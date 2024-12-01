#include <iostream>
#include <fstream>
#include <stdexcept>

#include <vector>
#include <algorithm>
#include <map>

void readLists(const std::string& filePath, std::vector<int>& listA, std::vector<int>& listB)
{
  std::ifstream fileInput{filePath};

  if (!fileInput.is_open())
  {
    throw std::invalid_argument("Input file not found.");
  }

  std::string text;

  while (std::getline(fileInput, text)) {
  listA.push_back(stoi(text.substr(0, 6)));
  listB.push_back(stoi(text.substr(8, 6)));
  }
}

int calculateDistanceScore(std::vector<int>& listA, std::vector<int>& listB)
{
  std::sort(listA.begin(), listA.end());
  std::sort(listB.begin(), listB.end());

  int totalDistance = 0;

  for (int i = 0; i < listA.size(); i++) 
  {
    totalDistance += std::abs(listA[i] - listB[i]);
  }

  return totalDistance;
}

int calculateSimilarityScore(std::vector<int>& listA, std::vector<int>& listB)
{
  std::map<int, int> idCounter;
  int similarityScore = 0;

  for (const auto& elementA : listA)
  {
    similarityScore += elementA * std::count(listB.begin(), listB.end(), elementA);
  }

  return similarityScore;
}

int main()
{
  std::vector<int> listA;
  std::vector<int> listB;

  readLists("input.txt", listA, listB);
  std::cout << "Distance score:   " << calculateDistanceScore(listA, listB) << std::endl;
  std::cout << "Similarity score: " << calculateSimilarityScore(listA, listB) << std::endl;

  return 0;
}