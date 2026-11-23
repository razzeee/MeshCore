#pragma once

#include <gfxfont.h>
#include <stdint.h>
#include <string.h>

namespace einkEmoji {
constexpr uint8_t first = 0x80;
static const char *const names[] = {
  "📻", "🔋", "📡", "❤", "⭐", "⚡", "☀", "🌙", "🏠", "🌲", "🙂", "😀", "🔒", "🚀", "👻"
};
constexpr size_t count = sizeof(names) / sizeof(names[0]);

#define ROW(r) ((r) >> 8), ((r) & 0xff)
#define ROWS(a,b,c,d) ROW(a), ROW(b), ROW(c), ROW(d)
static const uint8_t bitmap[][32] PROGMEM = {
  {ROWS(0x0003,0x000e,0x0038,0x00e0), ROWS(0x0380,0x0e00,0x7ffe,0xffff),
   ROWS(0xc003,0xdfbb,0xc003,0xc703), ROWS(0xcb83,0xc703,0xffff,0x7ffe)}, // radio
  {ROWS(0x03c0,0x03c0,0x1ff8,0x1ff8), ROWS(0x1818,0x1998,0x1bd8,0x1bd8),
   ROWS(0x1bd8,0x1bd8,0x1bd8,0x1bd8), ROWS(0x1998,0x1818,0x1ff8,0x1ff8)}, // battery
  {ROWS(0x0008,0x0014,0x0022,0x0449), ROWS(0x4210,0x6090,0x7120,0x7a40),
   ROWS(0x3d80,0x1e00,0x0c00,0x0600), ROWS(0x0700,0x0d80,0x1ff0,0x1ff0)}, // dish
  {ROWS(0x0000,0x3c3c,0x7e7e,0xffff), ROWS(0xffff,0xffff,0xffff,0x7ffe),
   ROWS(0x3ffc,0x1ff8,0x0ff0,0x07e0), ROWS(0x03c0,0x0180,0x0000,0x0000)}, // heart
  {ROWS(0x0180,0x0180,0x03c0,0x03c0), ROWS(0x07e0,0xffff,0x7ffe,0x3ffc),
   ROWS(0x1ff8,0x0ff0,0x1ff8,0x1ff8), ROWS(0x3c3c,0x381c,0x300c,0x2004)}, // star
  {ROWS(0x0078,0x00f0,0x01e0,0x03c0), ROWS(0x0780,0x0f00,0x1ffe,0x3ffc),
   ROWS(0x0078,0x00f0,0x01e0,0x03c0), ROWS(0x0780,0x0e00,0x1800,0x1000)}, // bolt
  {ROWS(0x0180,0x2184,0x1008,0x07e0), ROWS(0x0ff0,0x1ff8,0x1ff8,0xdffb),
   ROWS(0xdffb,0x1ff8,0x1ff8,0x0ff0), ROWS(0x07e0,0x1008,0x2184,0x0180)}, // sun
  {ROWS(0x01f0,0x07c0,0x0f80,0x1f00), ROWS(0x3e00,0x3e00,0x7c00,0x7c00),
   ROWS(0x7e00,0x3e01,0x3f03,0x1fcf), ROWS(0x1ffe,0x0ffc,0x07f8,0x01e0)}, // moon
  {ROWS(0x0180,0x03c0,0x07e0,0x0e70), ROWS(0x1c38,0x381c,0x700e,0xffff),
   ROWS(0x3ffc,0x300c,0x33cc,0x33cc), ROWS(0x33cc,0x33cc,0x3ffc,0x3ffc)}, // house
  {ROWS(0x0180,0x03c0,0x07e0,0x0ff0), ROWS(0x03c0,0x07e0,0x0ff0,0x1ff8),
   ROWS(0x07e0,0x0ff0,0x1ff8,0x3ffc), ROWS(0x7ffe,0x0180,0x0180,0x0180)}, // tree
  {ROWS(0x07e0,0x1ff8,0x381c,0x6006), ROWS(0x6c36,0xcc33,0xc003,0xc003),
   ROWS(0xc003,0xc813,0xcc33,0x67e6), ROWS(0x6006,0x381c,0x1ff8,0x07e0)}, // smile
  {ROWS(0x07e0,0x1ff8,0x381c,0x6006), ROWS(0x6c36,0xcc33,0xc003,0xc003),
   ROWS(0xcff3,0xc813,0xcc33,0x67e6), ROWS(0x6006,0x381c,0x1ff8,0x07e0)}, // grin
  {ROWS(0x07e0,0x0ff0,0x1c38,0x1818), ROWS(0x1818,0x1818,0x7ffe,0x7ffe),
   ROWS(0x7ffe,0x7c3e,0x7c3e,0x7e7e), ROWS(0x7e7e,0x7ffe,0x7ffe,0x3ffc)}, // lock
  {ROWS(0x001f,0x007f,0x00ff,0x01cf), ROWS(0x039f,0x073e,0x0efc,0x3ff8),
   ROWS(0x7ff0,0xffe0,0x1fc0,0x1b80), ROWS(0x5300,0xa600,0x4800,0x8000)}, // rocket
  {ROWS(0x07c0,0x0c60,0x1830,0x1250), ROWS(0x1250,0xd016,0xd317,0x739e),
   ROWS(0x319c,0x1018,0x1010,0x1010), ROWS(0x3010,0x6318,0x7f4e,0x39c7)}, // ghost
};
#undef ROWS
#undef ROW

#define EINK_ICON(i) {32 * (i), 16, 16, 18, 1, -14}
static const GFXglyph glyphs[] PROGMEM = {
  EINK_ICON(0), EINK_ICON(1), EINK_ICON(2), EINK_ICON(3), EINK_ICON(4), EINK_ICON(5), EINK_ICON(6),
  EINK_ICON(7), EINK_ICON(8), EINK_ICON(9), EINK_ICON(10), EINK_ICON(11), EINK_ICON(12), EINK_ICON(13),
  EINK_ICON(14)
};
#undef EINK_ICON
static_assert(sizeof(bitmap) / sizeof(bitmap[0]) == count && sizeof(glyphs) / sizeof(glyphs[0]) == count,
              "Each emoji needs one bitmap and glyph");
static const GFXfont font PROGMEM = {(uint8_t *)bitmap, (GFXglyph *)glyphs, first, first + count - 1, 22};

inline const char *symbolEnd(const char *src) {
  ++src;
  while ((static_cast<uint8_t>(*src) & 0xc0) == 0x80)
    ++src;
  if (!strncmp(src, "\xef\xb8\x8f", 3) || !strncmp(src, "\xef\xb8\x8e", 3)) src += 3;
  if (!strncmp(src, "\xf0\x9f\x8f", 3) && uint8_t(src[3]) >= 0xbb && uint8_t(src[3]) <= 0xbf) src += 4;
  return src;
}

inline bool regionalIndicator(const char *src) {
  return !strncmp(src, "\xf0\x9f\x87", 3) && uint8_t(src[3]) >= 0xa6 && uint8_t(src[3]) <= 0xbf;
}

inline const char *flagTagEnd(const char *src) {
  const char *p = src;
  while (!strncmp(p, "\xf3\xa0", 2) &&
         ((uint8_t(p[2]) == 0x80 && uint8_t(p[3]) >= 0xa0 && uint8_t(p[3]) <= 0xbf) ||
          (uint8_t(p[2]) == 0x81 && uint8_t(p[3]) >= 0x80 && uint8_t(p[3]) <= 0xbe))) {
    p += 4;
  }
  return p != src && !strncmp(p, "\xf3\xa0\x81\xbf", 4) ? p + 4 : src;
}

inline uint8_t nextUtf8Glyph(const char *&src) {
  uint8_t c = *src;
  if (c < 0x80) {
    ++src;
    if (c == '#' || c == '*' || (c >= '0' && c <= '9')) {
      const char *suffix = src;
      if (!strncmp(suffix, "\xef\xb8\x8f", 3)) suffix += 3;
      if (!strncmp(suffix, "\xe2\x83\xa3", 3)) {
        src = suffix + 3;
        return '?';
      }
    }
    return c;
  }
  const char *start = src;
  src = symbolEnd(src);
  if (regionalIndicator(start) && regionalIndicator(src)) {
    src = symbolEnd(src);
  } else if (!strncmp(src, "\xe2\x80\x8d", 3)) {
    do {
      src += 3;
      if (*src) src = symbolEnd(src);
    } while (!strncmp(src, "\xe2\x80\x8d", 3));
  } else if (!strncmp(start, "🏴", 4)) {
    src = flagTagEnd(src);
  } else {
    for (size_t i = 0; i < count; ++i) {
      if (!strncmp(start, names[i], strlen(names[i]))) return first + i;
    }
  }
  return '?';
}

inline uint8_t nextDisplayGlyph(const char *&src) {
  uint8_t c = *src;
  if (c >= first && c < first + count) {
    ++src;
    return c;
  }
  return nextUtf8Glyph(src);
}

inline void encode(char *dest, const char *src, size_t size) {
  if (!size) return;
  size_t used = 0;
  while (*src && used + 1 < size) {
    uint8_t c = nextUtf8Glyph(src);
    if (c >= 32 && c != 127) dest[used++] = c;
  }
  dest[used] = 0;
}
}
