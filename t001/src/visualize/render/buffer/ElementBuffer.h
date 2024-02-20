#ifndef MD_2024_ELEMENTBUFFER_H
#define MD_2024_ELEMENTBUFFER_H

#include "utils.h"

class ElementBuffer {
private:
    unsigned glID;
    unsigned count;

public:
    /**
     * Creates a new Elementbuffer
     * @param count - in bytes
     * @param indices - data
     */
    ElementBuffer(unsigned count, unsigned* indices);

    ElementBuffer(const ElementBuffer& o) = delete;

    virtual ~ElementBuffer();

    void bind() const;
    void unbind() const;

    unsigned getCount() const { return count; }
};

#endif //MD_2024_ELEMENTBUFFER_H
