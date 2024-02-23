#ifndef MD_2024_UTILS_H
#define MD_2024_UTILS_H

#include <vector>
#include <string>

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long long ulong;
typedef unsigned char byte;
typedef std::pair<unsigned, unsigned> uPair;

typedef const char *cstr;

typedef bool flag;

#define BIT(x) (1 << x)

constexpr int narrow(uint u) {
    return ((u > 0xEFFFFFFF) ? (int)0xEFFFFFFF : (int)u);
}

std::vector<std::string> splitAt(const std::string& s, const std::string& delims);

std::string selectFileFromDir(const std::string& directoryPath);

#endif //MD_2024_UTILS_H
