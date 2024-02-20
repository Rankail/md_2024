#ifndef MD_2024_WINDOWFLAG_H
#define MD_2024_WINDOWFLAG_H

#include "utils.h"

enum class WindowFlag {
    NONE =              0,
    FULLSCREEN =        BIT(0),
    SHOWN =             BIT(2),
    HIDDEN =            BIT(3),
    BORDERLESS =        BIT(4),
    RESIZABLE =         BIT(5),
    MINIMIZED =         BIT(6),
    MAXIMIZED =         BIT(7)
};

inline WindowFlag operator|(WindowFlag lhs, WindowFlag rhs) {
    return static_cast<WindowFlag>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

#endif //MD_2024_WINDOWFLAG_H
