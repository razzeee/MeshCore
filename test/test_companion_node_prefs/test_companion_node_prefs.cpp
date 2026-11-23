#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <string>

#define THINKNODE_M1 1
#define DISPLAY_CLASS GxEPDDisplay
#include "../../src/helpers/CompanionNodePrefs.h"

bool CommonRadioPrefs::getByKey(const char*, char*, size_t) { return false; }
bool CommonRadioPrefs::setByKey(const char*, const char*) { return false; }

class ReplayStream : public Stream {
  const char* _text;
  int _pos = 0;
  int _len;

public:
  explicit ReplayStream(const char* text) : _text(text), _len(strlen(text)) { }

  int available() override { return _len - _pos; }
  int read() override { return _pos < _len ? _text[_pos++] : -1; }
  int peek() override { return _pos < _len ? _text[_pos] : -1; }
};

class CaptureStream : public Stream {
  std::string _text;

  size_t emit(long long value) {
    char text[24];
    int length = snprintf(text, sizeof(text), "%lld", value);
    return write(reinterpret_cast<const uint8_t*>(text), length);
  }

public:
  size_t write(uint8_t value) override {
    _text.push_back(static_cast<char>(value));
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    _text.append(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  size_t print(unsigned char value, int = DEC) override { return emit(value); }
  size_t print(int value, int = DEC) override { return emit(value); }
  size_t print(unsigned int value, int = DEC) override { return emit(value); }
  size_t print(long value, int = DEC) override { return emit(value); }
  size_t print(unsigned long value, int = DEC) override { return emit(value); }
  size_t print(long long value, int = DEC) override { return emit(value); }
  size_t print(unsigned long long value, int = DEC) override { return emit(value); }

  const std::string& text() const { return _text; }
};

#if 0
// Re-enable test once we can SET fem_ values in companion
TEST(CompanionNodePrefs, RxGainSettingsRoundTripIndependently) {
  NodePrefs saved;
  saved.rx_boosted_gain = 0;
  saved.radio_fem_rxgain = 1;
  saved.radio_fem_txgain = 0;

  CaptureStream output;
  ASSERT_TRUE(saved.saveSerial(output));
  EXPECT_NE(std::string::npos, output.text().find("rxgain:0"));
  EXPECT_NE(std::string::npos, output.text().find("fem_rxgain:1"));
  EXPECT_NE(std::string::npos, output.text().find("fem_txgain:0"));

  ReplayStream input("{radio:{rxgain:1,fem_rxgain:0,fem_txgain:1}}");
  NodePrefs loaded;
  loaded.rx_boosted_gain = 0;
  loaded.radio_fem_rxgain = 1;
  loaded.radio_fem_txgain = 0;

  ASSERT_TRUE(loaded.loadSerial(input));
  EXPECT_EQ(1, loaded.rx_boosted_gain);
  EXPECT_EQ(0, loaded.radio_fem_rxgain);
  EXPECT_EQ(1, loaded.radio_fem_txgain);
}
#endif

TEST(CompanionNodePrefs, M1KnobStaysEnabledOnFreshInstallAndUpgrade) {
  NodePrefs prefs;
  EXPECT_EQ(-1, prefs.ui_backlight_delay);
  ReplayStream oldDefault("{ui:{bl_dly:5}}");
  ASSERT_TRUE(prefs.loadSerial(oldDefault));
  EXPECT_EQ(-1, prefs.ui_backlight_delay);
}

TEST(CompanionNodePrefs, M1PreservesCustomTimeoutsIncludingFiveAfterUpgrade) {
  NodePrefs prefs;
  ReplayStream oldCustom("{ui:{bl_dly:30}}");
  ASSERT_TRUE(prefs.loadSerial(oldCustom));
  EXPECT_EQ(30, prefs.ui_backlight_delay);
  prefs.ui_backlight_delay = 5;
  CaptureStream saved;
  ASSERT_TRUE(prefs.saveSerial(saved));
  NodePrefs restored;
  ReplayStream input(saved.text().c_str());
  ASSERT_TRUE(restored.loadSerial(input));
  EXPECT_EQ(5, restored.ui_backlight_delay);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
