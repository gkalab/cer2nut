#include <algorithm>

#include "CertaboParser.h"
#include "CertaboPiece.h"

using eboard::CertaboParser;
using eboard::CertaboPiece;

CertaboParser::CertaboParser(BoardTranslator& translator) : translator(translator) {}

void CertaboParser::parse(uint8_t* msg, size_t data_len) {
    std::vector<uint8_t> data = buffer;
    data.insert(data.end(), msg, msg + data_len);
    buffer.clear();

    if (data.size() > 15) {
        std::string input_str(data.begin(), data.end());
        if (input_str.find("\r\n") == std::string::npos) {
            buffer.insert(buffer.end(), data.begin(), data.end());
        } else {
            std::vector<std::string> split_data = strSplit<std::string>(input_str, ":");
            int index = 0;
            for (auto& part : split_data) {
                if (!part.empty() && !parsePart(part) && index == (split_data.size() - 1)) {
                    size_t position = data.size() - part.size() - 1;
                    buffer.insert(buffer.end(), data.begin() + static_cast<int>(position), data.end());
                }
                index++;
            }
        }
    } else {
        buffer = data;
    }
}

bool CertaboParser::parsePart(std::string& part) {
    part.erase(std::remove_if(part.begin(), part.end(),
                              [](unsigned char x) {
                                  return (x == '\r' || x == '\n' || x == 'L');
                              }),
               part.end());
    std::vector<std::string> split_input = strSplit<std::string>(part, " ");
    if (!pieceRecognition && split_input.size() > 8) {
        pieceRecognition = true;
        translator.hasPieceRecognition(true);
    }
    if (pieceRecognition) {
        return parseWithPieceInfo(split_input);
    } else {
        return parseWithoutPieceInfo(split_input);
    }
}

bool CertaboParser::parseWithPieceInfo(std::vector<std::string>& splitInput) {
    if (splitInput.size() >= 320) {
        std::vector<CertaboPiece> board;
        for (int square = 0; square < 64; square++) {
            std::array<uint8_t, 5> piece_id{0, 0, 0, 0, 0};
            for (int i = 0; i < 5; i++) {
                std::basic_string<char>& str = splitInput.at(square * 5 + i);
                errno = 0;
                char* p_end{};
                const char* p = str.c_str();
                const long value = std::strtol(p, &p_end, 10);
                if ((p && *p_end != 0) || p == p_end || errno == ERANGE) {
                    buffer.clear();
                    return false;
                } else {
                    piece_id[i] = value; // std::stoi(str); -- no error checking with stoi
                }
            }
            CertaboPiece piece(piece_id);
            board.push_back(piece);
        }
        translator.translate(board);
        buffer.clear();
        return true;
    } else {
        return false;
    }
}

bool CertaboParser::parseWithoutPieceInfo(std::vector<std::string>& splitInput) {
    if (splitInput.size() >= 8) {
        std::array<bool, 64> board{};
        for (int i = 0, row = 0; row < 8; row++) {
            std::basic_string<char>& str = splitInput.at(row);
            errno = 0;
            char* p_end{};
            const char* p = str.c_str();
            const long value = std::strtol(p, &p_end, 10);
            if ((p && *p_end != 0) || p == p_end || errno == ERANGE) {
                buffer.clear();
                return false;
            } else {
                uint8_t b = value; // std::stoi(str); -- no error checking with stoi
                for (int col = 7; col > -1; col--) {
                    if ((b & (1 << col)) != 0) {
                        board[i] = true; // occupied
                    }
                    i++;
                }
            }
        }
        translator.translateOccupiedSquares(board);
        buffer.clear();
        return true;
    } else {
        return false;
    }
}
