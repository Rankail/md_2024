#ifndef MD_2024_UID_H
#define MD_2024_UID_H

#include <functional>

typedef unsigned UID;

template <size_t N>
constexpr unsigned charHash(const char s[N]) {
    unsigned result = 0x3f2e3a57;
    for (int i = 0; i < N; ++i) {
        result ^= s[i];
        result *= 18947;
    }
    return result;
}

#define MAKE_UID(p1, p2) (UID)((p1 << 16) | (p2 & 0x0000FFFF))
#define RAND_UID MAKE_UID(__LINE__, __COUNTER__)

#define GET_UID_P1(uid) ((uid & 0xFF00) >> 16)
#define GET_UID_P2(uid) (uid & 0x00FF)

#endif //MD_2024_UID_H
