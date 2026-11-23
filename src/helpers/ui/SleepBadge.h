#pragma once

namespace sleepBadge {
#ifdef THINKNODE_M1
constexpr int width = 12;
constexpr int height = 9;
constexpr int reservedWidth = width + 2;
constexpr int reservedHeight = height + 1;
#else
constexpr int reservedWidth = 0;
constexpr int reservedHeight = 0;
#endif
}
