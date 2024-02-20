#ifndef MD_2024_RC_H
#define MD_2024_RC_H


#include "utils.h"
#include "uid.h"

#include <type_traits>
#include <memory>

#include "vec.h"
#include "atlas/render/buffer/VertexBuffer.h"
#include "atlas/render/buffer/VertexArray.h"

#define RENDER_COMMAND()\
private:                                                                            \
    inline static unsigned idx = -1;                                                    \
                                                                                    \
public:                                                                             \
    static void setIdx(unsigned index) { idx = index; }                                 \
    static unsigned getStaticIdx() { return idx; }                                      \
    unsigned getIdx() const override { return getStaticIdx(); }

#define RENDER_COMMAND_BUFFERS() \
protected:                                                                          \
    VertexArray* vao = nullptr;

class DC;

class RC {
public:
    struct AdditionalData {
        Mat4f transform;
    };

protected:
    AdditionalData additionalData;

public:
    RC() = default;

    virtual ~RC() = default;

    virtual void init(DC* dc) = 0;
    virtual void render() = 0;

    virtual unsigned getIdx() const = 0;

    void setAdditionalData(AdditionalData data);
};

template<typename T>
concept IsRenderCommand = requires(T a, unsigned size) {
    std::is_base_of_v<RC, T>;
    T::setIdx(size);
    T::getStaticIdx();
    a.getIdx();
};

template<typename T, typename... Ts>
concept HasRenderCommand = requires(T a, Ts... args) {
    { a.setData(args...) } -> std::same_as<void>;
};

#endif //MD_2024_RC_H
