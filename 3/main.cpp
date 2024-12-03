#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>

std::string readProgram(const std::string& filePath)
{
  std::ifstream fileInput{filePath};

  if (!fileInput.is_open())
  {
    throw std::invalid_argument("Input file not found.");
  }

  std::string program;
  std::getline(fileInput, program);

  return program;
}

long long processMul(std::smatch regexMatch)
{
  const std::regex numbersRegex("(\\d+)[^0-9]+(\\d+)");

  std::smatch numbersRegexMatch;
  std::string numbers = regexMatch.str();
  std::regex_search(numbers, numbersRegexMatch, numbersRegex);
  int firstNumber = std::stol(numbersRegexMatch[1]);
  int secondNumber = std::stol(numbersRegexMatch[2]);

  return firstNumber * secondNumber;
}

int main()
{
  std::string program = readProgram("input.txt");
  std::regex operationRegex("mul\\(\\d{1,3},\\d{1,3}\\)|do\\(\\)|don\'t\\(\\)");

  std::smatch operationRegexMatch;

  long long result = 0;
  bool mulEnabled = true;

  while (std::regex_search(program, operationRegexMatch, operationRegex))
  {
    std::string operation = operationRegexMatch.str();

    if (operation.substr(0, 5).compare("don\'t") == 0)
    {
      mulEnabled = false;
    }
    else if (operation.substr(0, 2).compare("do") == 0)
    {
      mulEnabled = true;
    }
    else if (mulEnabled && (operation[0] == 'm'))
    {
      result += processMul(operationRegexMatch);
    }

    program = operationRegexMatch.suffix().str();
  }

  std::cout << "Result: " << result << std::endl;

  return 0;
}