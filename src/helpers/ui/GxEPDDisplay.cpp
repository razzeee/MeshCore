
#include "GxEPDDisplay.h"

#ifndef DISPLAY_ROTATION
  #define DISPLAY_ROTATION 3
#endif

#ifdef ESP32
  SPIClass SPI1 = SPIClass(FSPI);
#endif

#ifndef EPD_WASHING_MACHINE_CYCLES
  #define EPD_WASHING_MACHINE_CYCLES 0
#endif

// Color scheme
ColorVal UIColor::window_bkg = GxEPD_WHITE;
ColorVal UIColor::title_bkg = GxEPD_WHITE;
ColorVal UIColor::title_txt = GxEPD_BLACK;
ColorVal UIColor::primary_txt = GxEPD_BLACK;
ColorVal UIColor::secondary_txt = GxEPD_BLACK;
ColorVal UIColor::warning_txt = GxEPD_BLACK;
ColorVal UIColor::popup_bkg = GxEPD_WHITE;
ColorVal UIColor::popup_txt = GxEPD_BLACK;
ColorVal UIColor::corp_blue = GxEPD_BLACK;

enum class DrawOp : uint8_t { Font, Color, Cursor, Text, Fill, Outline, Bitmap };

bool GxEPDDisplay::begin() {
  display.epd2.selectSPI(SPI1, SPISettings(4000000, MSBFIRST, SPI_MODE0));
#ifdef ESP32
  SPI1.begin(PIN_DISPLAY_SCLK, PIN_DISPLAY_MISO, PIN_DISPLAY_MOSI, PIN_DISPLAY_CS);
#else
  SPI1.begin();
#endif
  display.init(115200, true, 2, false);
  display.setRotation(DISPLAY_ROTATION);
  setTextSize(1);  // Default to size 1

  display.setFullWindow();

  for (int i = 0; i < EPD_WASHING_MACHINE_CYCLES; i++) {
    display.fillScreen(GxEPD_BLACK);
    display.display(false);
    delay(2000);
    display.fillScreen(GxEPD_WHITE);
    display.display(false);
    delay(2000);
  }

  display.setPartialWindow(0, 0, display.width(), display.height());
  _partial_refresh_count = 0;
  _has_displayed_frame = false;

  #if DISP_BACKLIGHT
  digitalWrite(DISP_BACKLIGHT, LOW);
  pinMode(DISP_BACKLIGHT, OUTPUT);
  #endif
  _init = true;
  _isOn = true;
  return true;
}

void GxEPDDisplay::turnOn() {
  if (!_init) begin();
  if (!_isOn) {
    _isOn = true;
  }
}

void GxEPDDisplay::turnOn(int max_partial) {
  setMaxPartialRefresh(max_partial);
  turnOn();
}

// 24x24 sleep icon, MSB-first (matches GxEPDDisplay::drawXbm)
static const uint8_t sleep_icon_24x24[] = {
  0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xe0, 0x0e, 0x03, 0xc0, 0x18,
  0x0f, 0x80, 0x70, 0x1f, 0x80, 0xff, 0x3f, 0x80, 0xff, 0x3f, 0x00, 0x00,
  0x7f, 0x3f, 0x00, 0x7f, 0x3f, 0x00, 0xff, 0x0e, 0x00, 0xff, 0x1c, 0x7c,
  0xff, 0xbf, 0x7c, 0xff, 0xbf, 0x38, 0xff, 0xc0, 0x7c, 0xff, 0xe0, 0x7c,
  0xff, 0xf0, 0x00, 0x7f, 0xfc, 0x00, 0x7f, 0xff, 0xf8, 0x3f, 0xff, 0xf0,
  0x1f, 0xff, 0xf0, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0x80, 0x01, 0xff, 0x00
};

void GxEPDDisplay::drawSleepIcon() {
  const int W = 24, H = 24;
  int x = (int)(display.width() / scale_x) - W;   // top-right, logical coordinates
  int y = 0;

  display.fillRect((x - 5) * scale_x - 1, y * scale_y, (W+5) * scale_x + 2, (H-2) * scale_y + 1, GxEPD_WHITE);
  _curr_color = GxEPD_BLACK;
  drawXbm(x, y, sleep_icon_24x24, W, H);
  if (_busy_hook) display.epd2.setBusyCallback(busyCallback, this);
  display.display(true);
  display.epd2.setBusyCallback(NULL);
}

void GxEPDDisplay::turnOff(bool blank) {
  if (blank) {
    blankScreen();
  } else {
    drawSleepIcon();
  }
  _isOn = false;
  _has_displayed_frame = false;
  display.hibernate();
}

void GxEPDDisplay::blankScreen() {
  // do full refresh before powering off to clear screen
  // no full refresh needed at wakeup
  if (_busy_hook) display.epd2.setBusyCallback(busyCallback, this);
  display.clearScreen(0xFF); // Clears microcontroller side RAM
  display.writeScreenBuffer(0xFF); // Forces 0xFF (White) into the display controller's history registers
  display.epd2.setBusyCallback(NULL);
  _partial_refresh_count = 0;
  _has_displayed_frame = false;
}

void GxEPDDisplay::clear() {
  startFrame(GxEPD_WHITE);
}

void GxEPDDisplay::startFrame(ColorVal bkg) {
  display.fillScreen(bkg);
  display.setTextColor(_curr_color = UIColor::primary_txt);
  display_crc.reset();
  display_crc.update<ColorVal>(bkg);
}

void GxEPDDisplay::setTextSize(int sz) {
  display_crc.update(DrawOp::Font);
  display_crc.update<int>(sz);
  switch(sz) {
    case 1:  // Small
      display.setFont(&FreeSans9pt7b);
      break;
    case 2:  // Medium Bold
      display.setFont(&FreeSansBold12pt7b);
      break;
    case 3:  // Large
      display.setFont(&FreeSans18pt7b);
      break;
    default:
      display.setFont(&FreeSans9pt7b);
      break;
  }
}

void GxEPDDisplay::setColor(ColorVal c) {
  display_crc.update(DrawOp::Color);
  display_crc.update<ColorVal> (c);
  display.setTextColor(_curr_color = c);
}

void GxEPDDisplay::setCursor(int x, int y) {
  display_crc.update(DrawOp::Cursor);
  display_crc.update<int>(x);
  display_crc.update<int>(y);
  display.setCursor((x+offset_x)*scale_x, (y+offset_y)*scale_y);
}

void GxEPDDisplay::print(const char* str) {
  size_t length = strlen(str);
  display_crc.update(DrawOp::Text);
  display_crc.update(length);
  display_crc.update<char>(str, length);
  display.print(str);
}

void GxEPDDisplay::hashRect(int x, int y, int w, int h) {
  display_crc.update<int>(x);
  display_crc.update<int>(y);
  display_crc.update<int>(w);
  display_crc.update<int>(h);
}

void GxEPDDisplay::fillRect(int x, int y, int w, int h) {
  display_crc.update(DrawOp::Fill);
  hashRect(x, y, w, h);
  display.fillRect(x*scale_x, y*scale_y, w*scale_x, h*scale_y, _curr_color);
}

void GxEPDDisplay::drawRect(int x, int y, int w, int h) {
  display_crc.update(DrawOp::Outline);
  hashRect(x, y, w, h);
  display.drawRect(x*scale_x, y*scale_y, w*scale_x, h*scale_y, _curr_color);
}

void GxEPDDisplay::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  if (w <= 0 || h <= 0) return;
  display_crc.update(DrawOp::Bitmap);
  hashRect(x, y, w, h);
  // Calculate the base position in display coordinates
  uint16_t startX = x * scale_x;
  uint16_t startY = y * scale_y;
  
  // Width in bytes for bitmap processing
  uint16_t widthInBytes = (w + 7) / 8;
  display_crc.update<uint8_t>(bits, widthInBytes * h);
  
  // Process the bitmap row by row
  for (uint16_t by = 0; by < h; by++) {
    // Calculate the target y-coordinates for this logical row
    int y1 = startY + (int)(by * scale_y);
    int y2 = startY + (int)((by + 1) * scale_y);
    int block_h = y2 - y1;
    
    // Scan across the row bit by bit
    for (uint16_t bx = 0; bx < w; bx++) {
      // Calculate the target x-coordinates for this logical column
      int x1 = startX + (int)(bx * scale_x);
      int x2 = startX + (int)((bx + 1) * scale_x);
      int block_w = x2 - x1;
      
      // Get the current bit
      uint16_t byteOffset = (by * widthInBytes) + (bx / 8);
      uint8_t bitMask = 0x80 >> (bx & 7);
      bool bitSet = pgm_read_byte(bits + byteOffset) & bitMask;
      
      // If the bit is set, draw a block of pixels
      if (bitSet) {
        // Draw the block as a filled rectangle
        display.fillRect(x1, y1, block_w, block_h, _curr_color);
      }
    }
  }
}

uint16_t GxEPDDisplay::getTextWidth(const char* str) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return ceil((w + 1) / scale_x);
}

void GxEPDDisplay::endFrame() {
  if (_isOn == false) return;
  uint32_t crc = display_crc.finalize();
  if (_has_displayed_frame && crc == last_display_crc_value && !_force_full_refresh) return;

  bool partialRefresh = !_force_full_refresh &&
                        (_max_partial_refresh <= 0 || _partial_refresh_count < _max_partial_refresh);
  if (_busy_hook) display.epd2.setBusyCallback(busyCallback, this);
  display.display(partialRefresh);
  display.epd2.setBusyCallback(NULL);
  if (!partialRefresh) _partial_refresh_count = 0;
  else if (_max_partial_refresh > 0) _partial_refresh_count++;
  _force_full_refresh = false;
  _has_displayed_frame = true;
  last_display_crc_value = crc;
}
