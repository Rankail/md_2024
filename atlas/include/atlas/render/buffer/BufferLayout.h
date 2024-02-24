#ifndef MD_2024_BUFFERLAYOUT_H
#define MD_2024_BUFFERLAYOUT_H

#include "utils/utils.h"

#include <initializer_list>
#include <vector>

#include "BufferElement.h"

class BufferLayout {
private:
    std::vector<BufferElement> elements;
    unsigned stride = 0;

public:
    template<typename... Ts> requires (std::is_constructible_v<BufferElement, Ts> && ...)
    BufferLayout(Ts... elements)
        : elements{BufferElement(elements)...}{
        calcOffsetAndStride();
    }

private:
    void calcOffsetAndStride();

public:
    unsigned getStride() const { return stride; }
    const std::vector<BufferElement>& getElements() const { return elements; }

    std::vector<BufferElement>::iterator begin() { return elements.begin(); }
    std::vector<BufferElement>::iterator end() { return elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return elements.end(); }
};

#endif //MD_2024_BUFFERLAYOUT_H
