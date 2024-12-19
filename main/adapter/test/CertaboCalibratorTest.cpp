#include <gmock/gmock.h>

#include <vector>

#include "BoardTranslator.h"
#include "CertaboCalibrator.h"

using ::testing::AtLeast;

using eboard::BoardTranslator;
using eboard::CertaboCalibrator;
using eboard::CertaboPiece;

class CertaboCalibratorTest : public ::testing::Test {
  protected:
    void SetUp() override {
        calibrator = std::make_unique<CertaboCalibrator>(
            [this](const eboard::Stones&) {
                complete = true;
            },
            [](int square) {}, [](bool hasRgbLeds) {});
    }

    void givenCalibrationWith(std::string const& str) {
        whenCalibratingWith(str);
    }

    void whenCalibratingWith(std::string const& str) {
        std::vector<uint8_t> data(str.begin(), str.end());
        calibrator->calibrate(&data.front(), data.size());
    }

    void thenCalibrationShouldBeComplete() const {
        EXPECT_TRUE(complete);
    }

    void thenCalibrationShouldNotBeComplete() const {
        EXPECT_FALSE(complete);
    }

  private:
    std::unique_ptr<CertaboCalibrator> calibrator;
    bool complete = false;
};

TEST_F(CertaboCalibratorTest, calibrationComplete_A) {
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 118 144 181 62 197 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 217 253 175 8 35 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 114 85 242 66 216 0 0 0 0 0 83 11 112 12 24 186 1 169 12 164 250 132 247 7 "
        "20 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 217 253 174 18 155 0 0 0 0 0 121 34 76 2 42 0 0 0 0 0 218 65 165 5 "
        "112 0 0 0 0 0 170 11 16 170 222 170 47 132 30 7 0 0 0 0 0 14 90 80 27 133 0 0 0 0 0 3 0 84 44 165 3 0 84 68 "
        "112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 "
        "229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 24 161 129 32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 202 59 166 9 21 243 192 45 1 80 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 80 228 49 128 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 170 102 "
        "170 23 171 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 "
        "78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 "
        "106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16 218 88 139 184 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 62 216 31 210 10 0 0 0 0 0 247 12 10 129 250 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 92 181 "
        "207 8 166 0 0 0 0 0 0 0 0 0 0 133 210 226 37 96 0 0 0 0 0 0 0 0 0 0 165 162 120 93 77 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 172 90 8 5 7 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 "
        "85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 "
        "84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 81 0 111 152 16 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 225 241 64 0 133 46 7 134 0 0 0 0 0 0 69 100 21 209 155 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 129 55 135 224 80 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 "
        "209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 "
        "106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 207 92 165 5 26 245 232 148 128 161 0 0 0 0 0 196 72 1 128 21 172 71 4 224 210 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 144 34 15 55 160 0 0 0 0 0 0 0 0 "
        "0 0 93 22 18 56 95 164 88 112 226 129 0 0 0 0 0 225 17 133 55 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 90 95 154 60 162 0 0 0 0 0 223 177 96 112 224 70 128 24 7 0 58 168 95 144 196 240 "
        "75 127 160 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20 16 5 202 239 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 139 175 129 5 245 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 "
        "85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 "
        "84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 157 233 132 168 48 0 0 0 0 0 58 54 80 28 35 186 2 100 0 20 239 220 64 5 4 0 0 0 0 0 0 0 0 0 0 188 61 "
        "203 17 168 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 6 197 32 122 20 0 0 0 0 0 252 232 14 2 17 159 "
        "181 46 152 130 0 0 0 0 0 122 54 160 24 13 251 160 77 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 228 90 46 143 34 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 "
        "237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 "
        "0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    whenCalibratingWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 14 30 235 218 9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 135 32 136 182 23 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 195 195 86 80 48 168 90 2 21 30 0 0 0 0 0 46 88 "
        "47 224 61 0 0 0 0 0 225 167 178 224 191 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 13 85 235 33 233 "
        "108 26 26 33 7 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    thenCalibrationShouldBeComplete();
}

TEST_F(CertaboCalibratorTest, calibrationComplete_B) {
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 216 17 15 75 200 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 108 "
        "34 78 133 25 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 97 178 7 211 42 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 167 174 77 194 155 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "212 178 207 153 37 0 0 0 0 0 164 33 92 32 112 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 "
        "1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 "
        "105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 100 197 186 0 177 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 167 160 194 241 229 0 0 "
        "0 0 0 133 69 139 194 168 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 86 209 130 96 165 0 0 0 0 0 232 248 225 7 20 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 "
        "184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 "
        "128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 213 109 228 "
        "14 1 0 0 0 0 0 214 162 162 17 164 244 206 4 100 36 0 0 0 0 0 136 7 152 2 226 184 82 127 4 25 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 76 253 152 97 90 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 44 160 0 "
        "41 83 247 185 70 88 14 0 0 0 0 0 0 0 0 0 0 136 52 124 138 3 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 "
        "252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 "
        "121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 141 162 25 72 44 0 0 0 0 0 0 0 0 0 0 255 222 34 113 116 141 112 19 173 4 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 130 229 3 39 1 0 0 0 0 0 0 0 "
        "0 0 0 166 90 165 97 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 26 240 19 45 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 "
        "0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 "
        "0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 248 10 239 32 31 0 0 0 0 0 0 0 0 0 0 162 237 132 65 2 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30 6 173 168 38 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "245 160 36 128 128 146 248 142 240 165 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 "
        "3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 "
        "3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 218 162 90 66 132 0 0 0 0 0 88 47 112 66 68 0 0 0 0 0 78 120 18 171 27 0 0 0 0 0 175 195 88 7 177 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 245 161 172 42 77 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 45 233 54 212 12 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 80 132 "
        "182 19 167 0 0 0 0 0 0 0 0 0 0 13 20 4 10 165 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 155 219 2 159 132 0 0 0 0 0 0 0 0 0 0 61 5 48 35 196 134 157 160 7 252 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 "
        "242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 "
        "84 247 87 3 0 84 252 15\r\n");
    whenCalibratingWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 211 194 3 0 26 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 237 176 92 170 7 0 0 0 0 0 0 0 0 0 0 "
        "5 163 133 92 16 0 0 0 0 0 218 139 224 147 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 252 241 188 8 15 207 40 12 1 175 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 214 168 13 178 26 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 160 23 192 128 240 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    thenCalibrationShouldBeComplete();
}

TEST_F(CertaboCalibratorTest, calibrationComplete_C) {
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 170 93 132 12 152 205 225 186 8 54 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 188 5 129 177 210 "
        "234 168 30 206 164 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 183 90 224 236 106 61 165 2 6 17 38 57 "
        "116 163 30 0 0 0 0 0 134 248 42 114 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 194 112 54 212 64 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 181 85 2 4 152 3 0 84 44 165 3 0 84 68 112 3 0 84 237 "
        "98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 "
        "67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 "
        "0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 "
        "3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 "
        "15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "245 89 226 68 214 0 0 0 0 0 94 89 112 184 74 191 96 237 68 43 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 85 68 106 46 170 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 224 85 88 172 100 245 39 90 160 57 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 "
        "1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 "
        "105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 50 39 142 14 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 218 70 104 224 227 0 0 0 0 0 131 219 216 223 49 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 140 147 90 116 47 0 0 0 0 0 156 220 140 88 101 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 139 157 "
        "174 94 120 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 200 245 234 2 228 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 "
        "252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 "
        "121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 128 252 122 80 159 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 144 84 84 100 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 88 49 "
        "186 8 96 0 0 0 0 0 241 154 126 37 232 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 222 75 6 98 161 229 "
        "53 15 203 0 0 0 0 0 0 0 0 0 0 0 20 25 40 4 128 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 "
        "85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 "
        "84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 171 13 107 64 112 131 218 66 217 158 120 250 71 225 13 216 245 101 227 "
        "32 0 0 0 0 0 0 0 0 0 0 87 3 148 90 160 0 0 0 0 0 200 129 208 126 250 0 0 0 0 0 0 0 0 0 0 224 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 227 140 101 64 62 193 231 224 180 44 90 253 199 84 111 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 218 72 0 129 60 0 0 0 0 0 3 0 84 44 165 3 0 84 68 "
        "112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 "
        "229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 19 252 28 21 28 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 90 1 135 3 132 0 0 0 0 0 "
        "152 45 232 42 34 198 85 98 140 160 208 160 141 188 160 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 158 212 3 133 63 0 0 0 0 0 218 122 64 160 231 0 0 0 0 0 0 0 0 0 0 49 45 228 106 37 "
        "0 0 0 0 0 198 88 107 169 32 0 0 0 0 0 0 0 0 0 0 161 7 20 190 5 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 "
        "237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 "
        "0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    whenCalibratingWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 248 193 30 115 247 254 225 190 208 122 253 5 179 67 72 186 3 159 140 164 128 9 10 184 112 "
        "221 219 177 138 23 252 49 21 110 4 0 0 0 0 0 228 30 96 27 23 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 131 210 79 129 145 0 0 0 0 0 248 196 87 8 116 253 133 249 64 169 174 139 116 41 56 0 0 0 0 0 254 128 179 90 "
        "46 63 181 220 81 8 242 170 153 56 40 0 0 0 0 0 244 134 64 142 48 0 0 0 0 0 26 149 15 3 8 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 34 162 7 226 35 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 "
        "3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 "
        "231 3 0 84 247 87 3 0 84 252 15\r\n");
    thenCalibrationShouldBeComplete();
}

TEST_F(CertaboCalibratorTest, calibrationComplete_D) {
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 38 6 66 80 21 0 0 0 0 0 162 241 129 99 192 0 0 0 0 0 207 47 56 108 32 83 "
        "165 155 103 64 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 154 79 197 7 221 0 0 0 0 0 122 56 73 112 216 255 176 144 254 224 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 167 11 174 112 168 138 114 129 133 29 0 0 0 0 0 0 0 0 0 0 187 243 14 112 128 "
        "186 14 90 147 242 155 158 91 140 114 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 247 80 3 250 192 3 0 "
        "84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 "
        "0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 "
        "15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 123 87 67 224 89 0 0 0 0 0 5 10 216 152 255 16 107 0 0 166 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 90 216 186 43 13 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 "
        "0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 "
        "3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 205 10 16 242 66 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 190 250 154 1 148 115 103 2 227 64 "
        "0 0 0 0 0 0 0 0 0 0 248 44 122 136 68 129 135 112 26 81 0 0 0 0 0 0 0 0 0 0 248 203 210 8 3 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 190 21 148 18 67 0 0 0 0 0 0 0 0 0 0 "
        "222 124 64 84 80 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 106 217 32 129 43 0 0 0 0 0 245 120 2 20 30 251 143 31 17 151 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 21 196 95 92 160 0 0 0 0 0 0 0 0 0 0 30 56 3 0 160 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 185 126 130 0 142 0 0 0 0 0 179 139 91 236 63 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 "
        "85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 "
        "84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 165 213 56 1 208 0 0 0 0 0 0 0 0 0 0 191 182 79 195 96 244 133 167 192 1 221 225 194 133 176 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 218 168 80 161 35 0 0 0 0 0 0 0 0 0 0 2 5 194 31 0 7 140 79 55 144 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 213 202 114 4 228 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 "
        "170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 "
        "142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 80 50 231 133 5 80 99 0 24 110 0 0 0 0 0 0 0 0 0 0 49 134 72 64 162 244 30 94 16 188 210 248 "
        "102 128 148 0 0 0 0 0 96 112 167 50 68 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 235 90 26 "
        "0 0 24 73 179 224 161 220 16 66 14 138 68 181 133 197 164 0 0 0 0 0 144 230 20 16 162 0 0 0 0 0 91 240 214 "
        "129 42 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 "
        "68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 "
        "229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 56 39 38 160 214 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 132 225 74 16 69 0 0 0 0 0 213 5 112 66 36 "
        "214 87 230 132 52 0 0 0 0 0 123 232 224 214 22 0 0 0 0 0 255 46 140 32 165 112 26 80 17 149 0 0 0 0 0 225 61 "
        "32 145 14 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 122 36 202 160 178 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 170 68 227 180 36 "
        "255 149 195 64 145 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 238 90 112 162 24 0 0 0 0 0 0 0 0 0 0 225 106 32 68 245 3 0 "
        "84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 "
        "0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 "
        "15\r\n");
    whenCalibratingWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 66 216 208 135 129 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 28 32 126 0 16 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 234 122 168 36 162 0 0 0 0 0 0 0 0 0 0 7 7 247 37 168 0 0 0 0 0 75 28 16 20 224 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 "
        "78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 "
        "106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    thenCalibrationShouldBeComplete();
}

TEST_F(CertaboCalibratorTest, calibrationIncomplete) {
    givenCalibrationWith(
        "1 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 38 6 66 80 21 0 0 0 0 0 162 241 129 99 192 0 0 0 0 0 207 47 56 108 32 83 "
        "165 155 103 64 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 154 79 197 7 221 0 0 0 0 0 122 56 73 112 216 255 176 144 254 224 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 167 11 174 112 168 138 114 129 133 29 0 0 0 0 0 0 0 0 0 0 187 243 14 112 128 "
        "186 14 90 147 242 155 158 91 140 114 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 247 80 3 250 192 3 0 "
        "84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 "
        "0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 "
        "15\r\n");
    givenCalibrationWith(
        "2 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 123 87 67 224 89 0 0 0 0 0 5 10 216 152 255 16 107 0 0 166 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 90 216 186 43 13 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 "
        "0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 "
        "3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 84 40 81 32 122 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 197 178 27 13 144 0 0 0 0 0 0 0 0 0 0 232 244 169 224 97 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 "
        "84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 "
        "0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "4 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 204 1 147 24 79 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 102 253 108 0 245 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 74 249 112 128 80 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 "
        "84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 "
        "0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "2 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 246 83 48 133 156 109 240 18 34 5 120 234 69 36 33 0 0 0 0 0 218 238 116 112 129 219 110 25 "
        "82 33 96 100 176 46 224 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 82 108 88 26 161 0 0 0 0 0 0 0 0 0 0 228 215 "
        "144 10 126 0 0 0 0 0 0 0 0 0 0 183 196 31 0 167 221 25 164 160 130 0 0 0 0 0 247 225 174 211 35 0 0 0 0 0 103 "
        "165 40 46 228 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 218 182 164 5 126 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 "
        "44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 "
        "85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 "
        "15\r\n");
    givenCalibrationWith(
        "1 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 184 175 74 5 25 144 16 2 16 1 199 47 58 66 3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 105 218 64 72 225 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 152 14 1 82 1 0 0 0 0 0 0 0 0 0 0 252 112 102 0 52 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 213 88 "
        "69 1 70 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 "
        "0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 "
        "0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    givenCalibrationWith(
        "1 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 164 65 80 164 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 39 192 8 140 0 0 0 0 0 160 27 35 64 10 215 15 199 32 "
        "120 0 0 0 0 0 0 0 0 0 0 187 135 185 78 178 187 174 24 161 81 103 160 12 242 66 0 0 0 0 0 115 255 33 134 0 0 0 "
        "0 0 0 217 138 113 162 78 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 220 31 78 1 160 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 101 68 62 100 39 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 "
        "0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 "
        "0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    whenCalibratingWith(
        "8 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 3 0 85 0 107 3 0 84 255 "
        "174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 3 0 84 240 84 3 0 84 240 65 3 0 84 "
        "68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 14 141 176 66 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 149 120 52 164 20 24 112 60 140 177 0 0 0 0 0 168 135 143 162 31 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 182 96 24 206 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 3 0 85 1 184 3 0 "
        "84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 "
        "84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
    thenCalibrationShouldNotBeComplete();
}
