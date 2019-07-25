#ifndef H_SHEET_CONTROLLER
#define H_SHEET_CONTROLLER

#include <fstream>
#include <sstream>
#include <string>

#include "Settings.h"

class NullBuffer : public std::streambuf {
  public:
    int overflow (int c) { return c; }
};

template <typename T>
class SheetController {
  private:
    enum class State { Fresh, Open, Closed };

  public:
    SheetController (
        const std::string &SheetName
      , const Settings &Settings
    )
      : sheetName(SheetName)
      , settings(Settings)
      , state(State::Fresh)
    {}

    ~SheetController () {
      close();
    }

    T* getSheet () {
      if (State::Fresh == state) {
        outFile = std::make_unique<std::ofstream>(getOutFilePath());
        nullStream = std::make_unique<std::ostream>(&nullBuffer);

        if (0 == settings.benchmarkMode) {
          sheet = std::make_unique<T>(*outFile);
        } else {
          sheet = std::make_unique<T>(*nullStream);
        }

        sheet->writeHeaderRow();

        state = State::Open;
      }

      return sheet.get();
    }

    void close () {
      if (State::Open == state) {
        outFile->close();
      }

      state = State::Closed;
    }

  private:
    const std::string sheetName;
    const Settings &settings;
    State state;
    std::unique_ptr<T> sheet;
    std::unique_ptr<std::ofstream> outFile;
    std::unique_ptr<std::ostream> nullStream;
    NullBuffer nullBuffer;

    std::string getOutFilePath () {
      std::stringstream ss;

      ss << settings.pathPrefix
        << sheetName
        << ".csv";

      return ss.str();
    }
};

#endif
