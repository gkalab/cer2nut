#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "BoardTranslator.h"

namespace eboard {

/**
 * CertaboParser parses board messages from Certabo and calls a translator with the raw piece information.
 */
class CertaboParser {

  public:
    explicit CertaboParser(BoardTranslator& translator);
    void parse(uint8_t* data, size_t data_len);

  private:
    template <typename T> std::vector<T> strSplit(const T& str, const T& delimiters) {
        std::vector<T> v;
        typename T::size_type start = 0;
        auto pos = str.find_first_of(delimiters, start);
        while (pos != T::npos) {
            if (pos != start) { // ignore empty tokens
                v.emplace_back(str, start, pos - start);
            }
            start = pos + 1;
            pos = str.find_first_of(delimiters, start);
        }
        if (start < str.length()) {                           // ignore trailing delimiter
            v.emplace_back(str, start, str.length() - start); // add what's left of the string
        }
        return v;
    }

    bool parsePart(std::string & part);
    bool parseWithPieceInfo(std::vector<std::string> &splitInput);
    bool parseWithoutPieceInfo(std::vector<std::string> &splitInput);

    BoardTranslator& translator;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> lastBoard;
    bool reversed = false;
    bool pieceRecognition = false;
};

} // namespace eboard