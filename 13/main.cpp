#include <cstddef>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

struct Vec2 {
  long long x;
  long long y;

  Vec2 operator+(const Vec2& other) const
  {
    return {x + other.x, y + other.y};
  }
  bool operator==(const Vec2& other) const
  {
    return ((x == other.x) && (y == other.y));
  }
};

long long getDeterminant(const Vec2& v1, const Vec2& v2)
{
  return (v1.x * v2.y) - (v2.x * v1.y);
}

class SlotMachine {
  Vec2 translationButtonA{};
  Vec2 translationButtonB{};
  Vec2 pricePosition{};
public:
  SlotMachine(const Vec2& translationButtonA, const Vec2& translationButtonB, const Vec2& pricePosition)
    : translationButtonA(translationButtonA),
      translationButtonB(translationButtonB),
      pricePosition(pricePosition) {}

  Vec2 getTranslationA() const
  {
    return translationButtonA;
  }

  Vec2 getTranslationB() const
  {
    return translationButtonB;
  }

  Vec2 getPricePosition() const
  {
    return pricePosition;
  }

  /**
   * @brief Get the Minimal Tokens Needed Using Cramers Rule.
   * 
   * @return std::optional<long long> 
   */
  std::optional<long long> getMinimalTokensNeeded() const
  {
    long long detA = getDeterminant(pricePosition, translationButtonB);
    long long detB = getDeterminant(translationButtonA, pricePosition);
    long long det  = getDeterminant(translationButtonA, translationButtonB);

    if ((det == 0) || (detA % det != 0) || (detB % det != 0))
    {
      return std::nullopt;
    }

    long long timesA = detA / det;
    long long timesB = detB / det;

    return timesA * 3 + timesB;
  }

  void print() const
  {
    std::cout << "A(" << translationButtonA.x << ", " << translationButtonA.y << ") "
              << "B(" << translationButtonB.x << ", " << translationButtonB.y << ") "
              << "Price(" << pricePosition.x << ", " << pricePosition.y << ")" << std::endl;
  }
};

Vec2 readButtonTranslation(std::string_view buttonDescription)
{
    size_t xPos = buttonDescription.find("X+");
    size_t xDelimiterPos= buttonDescription.find(",");
    if ((xPos == std::string::npos) || (xDelimiterPos == std::string::npos))
    {
      throw std::runtime_error{"Invalid file format, unable to find X+ pos."};

    }
    xPos += 2;

    std::string buttonDescriptionString{buttonDescription};
    std::string xPosString = buttonDescriptionString.substr(xPos, xDelimiterPos - xPos);

    size_t yPos = buttonDescription.find("Y+");
    if (yPos == std::string::npos)
    {
      throw std::runtime_error{"Invalid file format, unable to find Y+ pos."};

    }
    yPos += 2;

    std::string yPosString = buttonDescriptionString.substr(yPos, buttonDescriptionString.size() - yPos);

    return {std::stoll(xPosString), std::stoll(yPosString)};
}

Vec2 readPricePosition(std::string_view priceDescription)
{
    size_t xPos = priceDescription.find("X=");
    size_t xDelimiterPos= priceDescription.find(",");
    if ((xPos == std::string::npos) || (xDelimiterPos == std::string::npos))
    {
      throw std::runtime_error{"Invalid file format, unable to find X+ pos."};

    }
    xPos += 2;

    std::string buttonDescriptionString{priceDescription};
    std::string xPosString = buttonDescriptionString.substr(xPos, xDelimiterPos - xPos);

    size_t yPos = priceDescription.find("Y=");
    if (yPos == std::string::npos)
    {
      throw std::runtime_error{"Invalid file format, unable to find Y+ pos."};

    }
    yPos += 2;

    std::string yPosString = buttonDescriptionString.substr(yPos, buttonDescriptionString.size() - yPos);

    //return { std::stoll(xPosString), std::stoll(yPosString)};
    return { 10000000000000 + std::stoll(xPosString), 10000000000000 + std::stoll(yPosString)};
}

std::vector<SlotMachine> readSlotMachines(std::string_view filePath)
{
  std::string filePathTerminated{filePath};
  std::fstream file{filePathTerminated};
  std::vector<SlotMachine> slotMachines{};

  if (!file.is_open())
  {
    throw std::runtime_error{"Unable to open file: " + filePathTerminated};
  }

  std::string currentLine{};
  while (std::getline(file, currentLine))
  {
    Vec2 translationButtonA = readButtonTranslation(currentLine);
    std::getline(file, currentLine);

    Vec2 translationButtonB = readButtonTranslation(currentLine);
    std::getline(file, currentLine);

    Vec2 pricePosition = readPricePosition(currentLine);

    std::getline(file, currentLine);

    slotMachines.emplace_back(translationButtonA, translationButtonB, pricePosition);
  }
  return slotMachines;
}

int main()
{
  std::vector<SlotMachine> slotMachines = readSlotMachines("input.txt");

  long long tokensNeeded = 0;
  for (const auto& slotMachine : slotMachines)
  {
    slotMachine.print();
    tokensNeeded += slotMachine.getMinimalTokensNeeded().value_or(0);
  }

  std::cout << "Tokens needed: " << tokensNeeded<< std::endl;

  return 0;
}