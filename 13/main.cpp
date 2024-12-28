#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Vec2 {
  size_t x;
  size_t y;
};

class SlotMachine {
  Vec2 translationButtonA{};
  Vec2 translationButtonB{};
  Vec2 pricePosition{};
public:
  SlotMachine(const Vec2& translationButtonA, const Vec2& translationButtonB, const Vec2& pricePosition)
    : translationButtonA(translationButtonA),
      translationButtonB(translationButtonB),
      pricePosition(pricePosition) {}

  size_t getMinimalTokensNeeded() const
  {
    return 1;
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

    return {std::stoul(xPosString), std::stoul(yPosString)};
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

    return {std::stoul(xPosString), std::stoul(yPosString)};
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
  std::vector<SlotMachine> slotMachines = readSlotMachines("exampleInput.txt");

  for (const auto& slotMachine : slotMachines)
  {
    slotMachine.print();
  }

  return 0;
}