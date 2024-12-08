
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

struct Equation {
  long long result;
  std::vector<int> numbers;
};
using Equations = std::vector<Equation>;

Equations readInput(const std::string& filePath)
{
  std::fstream file{filePath};
  Equations equations;

  if (!file.is_open())
  {
    throw std::invalid_argument{"Failed to open input file."};
  }

  std::string currentLine;

  while (std::getline(file, currentLine))
  {
    std::stringstream ss{currentLine};
    Equation equation{};
    int number;

    ss >> equation.result;
    ss.ignore(1);

    while (ss >> number)
    {
      equation.numbers.push_back(number);
    }
    equations.push_back(equation);
  }

  return equations;
}

enum class Operation {
  ADD,
  MUL,
  CONCAT
};

struct MidResultNode {
  long long midResult;
  int nextNumberIndex;
  std::vector<Operation> remainingOperations;
};

long long applyOperator(long long a, long long b, Operation operation)
{
  if (operation == Operation::ADD)
  {
    return a + b;
  }
  else if (operation == Operation::MUL)
  {
    return a * b;
  }
  else if (operation == Operation::CONCAT)
  {
    return std::stoll(std::to_string(a) + std::to_string(b));
  }
  else {
    throw std::invalid_argument("Invalid operation!");
  }
}

bool isEquationValid(const Equation& equation)
{
  auto& numbers = equation.numbers;
  auto& result = equation.result;

  std::vector<MidResultNode> processingStack {{numbers.front(), 1, {Operation::ADD, Operation::MUL, Operation::CONCAT}}};

  while (!processingStack.empty())
  {
    MidResultNode& node = processingStack.back();
    int nextNumberIndex = node.nextNumberIndex;

    if ((nextNumberIndex >= numbers.size())
      || node.remainingOperations.empty())
    {
      processingStack.pop_back();
      continue;
    }

    // get next operation
    Operation operation = node.remainingOperations.back();
    node.remainingOperations.pop_back();

    long long newMidResult = applyOperator(node.midResult, numbers[nextNumberIndex], operation);
    if ((newMidResult == result) && (nextNumberIndex == numbers.size() - 1))
    {
      return true;
    }

    MidResultNode newNode{newMidResult, nextNumberIndex + 1, {Operation::ADD, Operation::MUL, Operation::CONCAT}};
    processingStack.push_back(newNode);
  }
  return false;
}

void filterInvalidEquations(Equations& equations)
{
  Equations validEquations;
  for (const auto& equation : equations)
  {
    if (isEquationValid(equation))
    {
      validEquations.push_back(equation);
    }
  }

  equations = validEquations;
}

long long sumValidEquationResults(Equations& equations)
{
  long long sum = 0;
  filterInvalidEquations(equations);
  std::cout << std::endl;
  for (auto& [result, numbers] : equations)
  {
    std::cout << result << ": ";
    for (const auto& number : numbers)
    {
      std::cout << number << " ";
    }
    std::cout << std::endl;
    sum += result;
  }
  return sum;
}

int main()
{
  Equations equations = readInput("input.txt");
  std::cout << "Sum: " << sumValidEquationResults(equations) << std::endl;
}