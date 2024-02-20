#ifndef MD_2024_VEC_H
#define MD_2024_VEC_H

#include <type_traits>
#include <iostream>
#include <complex>
#include <numbers>
#include <cstring>
#include <cassert>

#define MAT_TEMPLATE template<typename U> requires (std::is_convertible_v<U, T>)
#define MAT_QUAD(dim) requires (M == N && M == dim)
#define MAT_QUAD_MIN(dim) requires (M == N && M >= dim)

template<int M, int N, typename T>
class Matrix {
public:
    typedef Matrix<M, 1, T> ColType;
    typedef Matrix<1, N, T> RowType;
    typedef Matrix<N, M, T> TransposeType;
    typedef Matrix<M, N, T> Type;
    typedef T ValueType;

private:
    T data[M * N];

public:
    Matrix() : data{(T)0} {};

    template<typename... Ts> requires (std::is_convertible_v<T, Ts> && ... && (sizeof...(Ts) == M * N))
    Matrix(Ts... elements)
        : data { (T)elements...} {}

    template<typename... Ts> requires (std::is_same_v<Matrix<1, N, T>, Ts> && ... && (sizeof...(Ts) == M))
    Matrix(Ts... rows) {
        int i = 0;
        for (const auto row : {rows...}) {
            for (int j = 0; j < N; j++) {
                data[i * N + j] = row.data[j];
            }
            i++;
        }
    }

    template<typename... Ts> requires (std::is_same_v<Matrix<M, 1, T>, Ts> && ... && (sizeof...(Ts) == M))
    Matrix(Ts... cols) {
        int i = 0;
        for (const auto col : {cols...}) {
            for (int j = 0; j < N; j++) {
                data[j * N + i] = col.data[j];
            }
            i++;
        }
    }

    Matrix(Type const& m)
        : data{} {
        std::memcpy(this->data, m.data, M * N * sizeof(T));
    }

    template<typename U>
    explicit Matrix(const Matrix<M, N, U>& m) requires std::is_convertible_v<U, T> {
        for (int i = 0; i < M * N; ++i) {
            data[i] = static_cast<T>(m.get(i));
        }
    }

public:

    MAT_TEMPLATE
    void set(int i, U d) {
        data[i] = d;
    }

    MAT_TEMPLATE
    void set(int i, int j, U d) {
        data[i * N + j] = (T)d;
    }

    T get(int i) const { return data[i]; }

    T x() const { return data[0]; }
    T y() const { return data[1]; }
    T z() const { return data[2]; }

    T& x() { return data[0]; }
    T& y() { return data[1]; }
    T& z() { return data[2]; }

    T get(int i, int j) const { return data[i * N + j]; }

    const T* getValuePtr() const {
        return &data[0];
    }

    MAT_TEMPLATE
    Type& operator =(Matrix<M, N, U> const& m) {
        std::memcpy(this->data, m.data, M*N*sizeof(T));
        return *this;
    }

    MAT_TEMPLATE
    Type& operator +=(U const& s) {
        for (int i = 0; i < M*N; i++) {
            data[i] += s;
        }
    }

    MAT_TEMPLATE
    Type& operator +=(Matrix<M, N, U> const& m) {
        for (int i = 0; i < M*N; i++) {
            data[i] += m.data[i];
        }
        return *this;
    }

    MAT_TEMPLATE
    Type& operator -=(U const& s) {
        for (int i = 0; i < M*N; i++) {
            data[i] -= s;
        }
    }

    MAT_TEMPLATE
    Type& operator -=(Matrix<M, N, U> const& m) {
        for (int i = 0; i < M*N; i++) {
            data[i] -= m.data[i];
        }
        return *this;
    }

    MAT_TEMPLATE
    Type& operator *=(U const& s) {
        for (int i = 0; i < M*N; i++) {
            data[i] *= s;
        }
    }

    MAT_TEMPLATE
    Type& operator *=(Matrix<M, N, U> const& m) {
        return (*this = *this * m);
    }

    MAT_TEMPLATE
    Type& operator /=(U const& s) {
        for (int i = 0; i < M*N; i++) {
            data[i] /= s;
        }
    }

    MAT_TEMPLATE
    Type& operator /=(Matrix<N, N, U> const& m) {
        return *this *= m.inversed();
    }

    Type operator ++() {
        for (int i = 0; i < M*N; i++) {
            ++data[i];
        }
        return *this;
    }

    Type operator --() {
        for (int i = 0; i < M*N; i++) {
            --data[i];
        }
        return *this;
    }

    Type operator ++(int) {
        auto result = Type(*this);
        ++*this;
        return result;
    }

    Type operator --(int) {
        auto result = Type(*this);
        --*this;
        return result;
    }

    Type operator +() const {
        return *this;
    }

    Type operator -() const {
        auto result = Type();
        for (int i = 0; i < M * N; i++) {
            result.set(i, -get(i));
        }
        return result;
    }

    MAT_TEMPLATE
    Type operator +(Matrix<M, N, U> const& m) const {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, get(i) + m.get(i));
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator +(Type const& m, U const& s) {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, m.get(i) + s);
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator +(U const& s, Type const& m) {
        return m + s;
    }

    MAT_TEMPLATE
    Type operator -(Matrix<M, N, U> const& m) const {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, get(i) - m.get(i));
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator -(Type const& m, U const& s) {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, m.get(i) - s);
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator -(U const& s, Type const& m) {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, s - m.get(i));
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator *(Type const& m, U const& s) {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, m.get(i) * s);
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator *(U const& s, Type const& m) {
        auto result = Type();
        for (int i = 0; i < M*N; i++) {
            result.set(i, s * m.get(i));
        }
        return result;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && N == 1)
    T operator *(Matrix<M, 1, U> const& v2) const {
        T sum = (T)0;
        for (int i = 0; i < M; i++) {
            sum += (T)(get(i) * v2.get(i));
        }
        return sum;
    }

    template<int L, typename U> requires (std::is_convertible_v<U, T>)
    Matrix<M, L, T> operator *(Matrix<N, L, U> const& m) const {
        auto result = Matrix<M, L, T>();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < L; j++) {
                T sum = (T)0;
                for (int k = 0; k < N; k++) {
                    sum += (T)(this->get(i, k) * m.get(k, j));
                }
                result.set(i, j, sum);
            }
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator /(Type const& m, U const& s) {
        auto result = Type();
        for (int i = 0; i < M * N; i++) {
            result.set(i, m.get(i) / static_cast<T>(s));
        }
        return result;
    }

    MAT_TEMPLATE
    friend Type operator /(U const& s, Type const& m) {
        auto result = Type();
        for (int i = 0; i < M * N; i++) {
            result.set(i, s / m.get(i));
        }
        return result;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && N == 1)
    friend Type operator /(const Type& m, const Matrix<M, N, U>& n) {
        auto result = Type();
        for (int i = 0; i < M * N; i++) {
            result.set(i, m.get(i) / static_cast<T>(n.get(i)));
        }
        return result;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && N != 1)
    friend Type operator /(const Type& m, const Matrix<M, N, U>& n) {
        return m * n.inversed();
    }

    Type& set(Type const& m) {
        return *this = m;
    }

    MAT_TEMPLATE
    Type& add(Matrix<M, N, U> const& m) {
        return *this += m;
    }

    MAT_TEMPLATE
    Type& add(U const& s) {
        return *this += s;
    }

    MAT_TEMPLATE
    Type& subtract(Matrix<M, N, U> const& m) {
        return *this -= m;
    }

    MAT_TEMPLATE
    Type& subtract(U const& s) {
        return *this -= s;
    }

    MAT_TEMPLATE
    Type& multiply(Matrix<M, N, U> const& m) {
        return *this *= m;
    }

    MAT_TEMPLATE
    Type& multiply(U const& s) {
        return *this *= s;
    }

    MAT_TEMPLATE
    Type& divide(Matrix<M, N, U> const& m) {
        return *this /= m;
    }

    MAT_TEMPLATE
    Type& divide(U const& s) {
        return *this /= s;
    }

    MAT_TEMPLATE
    Type adding(U const& s) const {
        return *this + s;
    }

    MAT_TEMPLATE
    Type adding(Matrix<M, N, U> const& m2) const {
        return *this + m2;
    }

    MAT_TEMPLATE
    Type subtracting(U const& s) const {
        return *this - s;
    }

    MAT_TEMPLATE
    Type subtracting(Matrix<M, N, U> const& m2) const {
        return *this - m2;
    }

    MAT_TEMPLATE
    Type multiplying(U const& s) const {
        return *this * s;
    }

    template<int L, typename U> requires (std::is_convertible_v<U, T>)
    Matrix<M, L, T> multiplying(Matrix<N, L, U> const& m) const {
        return *this * m;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && N == 1)
    T dot(Matrix<M, N, U> const& m) const {
        return *this * m;
    }

    MAT_TEMPLATE
    Type dividing(U const& s) const {
        return *this / s;
    }

    MAT_TEMPLATE
    Type dividing(const Matrix<M, N, U>& m) const {
        return *this / m;
    }

    template<int L, typename U> requires (std::is_convertible_v<U, T>)
    Matrix<M, L, T> dividing(Matrix<N, L, U> const& m) const {
        return *this / m;
    }

    T det() const MAT_QUAD(1) {
        return get(0);
    }

    T det() const MAT_QUAD(2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    T det() const MAT_QUAD(3) {
        return data[0] * data[4] * data[8] + data[1] * data[5] * data[6] + data[2] * data[3] * data[7]
               - (data[2] * data[4] * data[6] + data[1] * data[3] * data[8] + data[0] * data[5] * data[7]);
    }

    T det() const requires (M == N && M > 3) {
        auto cut = cutRow(0);
        bool negative = false;
        T sum = (T)0;
        for (int i = 0; i < N; i++) {
            sum += (1 - 2 * negative) * cut.cutCol(i).det() * get(i);
            negative = !negative;
        }
        return sum;
    }

    Type inversed() const MAT_QUAD(1) {
        return Matrix<M, N, T>(1. / data[0]);
    }

    Type inversed() const MAT_QUAD(2) {
        T oneOverDet = 1. / det();

        return Matrix<M, N, T>(
            data[3] * oneOverDet, -data[1] * oneOverDet,
            -data[2] * oneOverDet, data[0] * oneOverDet
        );
    }

    Type inversed() const MAT_QUAD(3) {
        T d = det();
        if (d == 0) return Type();

        return Matrix<M, N, T>(
            data[4] * data[8] - data[5] * data[7],
            -(data[1] * data[8] - data[2] * data[7]),
            data[1] * data[5] - data[2] * data[4],
            -(data[3] * data[8] - data[5] * data[6]),
            data[0] * data[8] - data[2] * data[6],
            -(data[0] * data[5] - data[2] * data[3]),
            data[3] * data[7] - data[4] * data[6],
            -(data[0] * data[7] - data[1] * data[6]),
            data[0] * data[4] - data[1] * data[3]
        ) / d;
    }

    Type& inverse() requires (M == N && M < 4) {
        return *this = inversed();
    }

    Type transposed() const {
        auto m = Matrix<N, M, T>();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                m.set(j, i, get(i, j));
            }
        }
        return m;
    }

    Type& transpose() {
        return *this = transposed();
    }

    Matrix<1, N, T> row(int i) const {
        auto m = Matrix<1, N, T>();
        for (int j = 0; j < N; j++) {
            m.set(j, get(i, j));
        }
        return m;
    }

    Matrix<M, 1, T> col(int i) const {
        auto m = Matrix<M, 1, T>();
        for (int j = 0; j < M; j++) {
            m.set(j, get(j, i));
        }
        return m;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && M == 3 && N == 1)
    Type cross(Matrix<3, 1, U> const& m) const {
        return Matrix<3, 1, T>(
            data[1] * m.data[2] - data[2] * m.data[1],
            data[2] * m.data[0] - data[0] * m.data[2],
            data[0] * m.data[1] - data[1] * m.data[0]
        );
    }

    template<typename U, int L> requires (std::is_convertible_v<U, T>)
    Matrix<M+L, N, T> concatenatedY(Matrix<L, N, U> const& m) const {
        auto result = Matrix<M+L, N, T>();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                result.set(j, i, get(j, i));
            }
            for (int j = 0; j < L; j++) {
                result.set(M+j, i, (T)m.get(j, i));
            }
        }
        return result;
    }

    template<typename U, int L> requires (std::is_convertible_v<U, T>)
    Matrix<M, N+L, T> concatenatedX(Matrix<M, L, U> const& m) const {
        auto result = Matrix<M, N+L, T>();
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                result.set(i, j, get(i, j));
            }
            for (int j = 0; j < L; j++) {
                result.set(i, N+j, (T)m.get(i, j));
            }
        }
        return result;
    }

    friend std::ostream& operator <<(std::ostream& os, Type const& m) {
        os << '{';
        for (int i = 0; i < M; i++) {
            if (i != 0) os << ' ';
            os << '{';
            for (int j = 0; j < N; j++) {
                os << m.get(i, j);
                if (j != N-1) os << ", ";
            }
            if (i != M-1) os << "},\n";
        }
        os << "}}";
        return os;
    }

    double norm() const requires (N == 1) {
        return std::sqrt(*this * *this);
    }

    static Type unit() requires (M == N) {
        auto m = Type();
        int i = 0;
        while (i < M) {
            m.set(i,i, (T)1);
            ++i;
        }
        return m;
    }

    static Type rotation2d(double a) MAT_QUAD_MIN(2) {
        auto m = Type::unit();
        m.set(0, 0, cos(a));
        m.set(0, 1, -sin(a));
        m.set(1, 0, sin(a));
        m.set(1, 1, cos(a));
        return m;
    }

    static Type rotationYZ(double a) MAT_QUAD_MIN(3) {
        auto m = Type::unit();
        m.set(1, 1, cos(a));
        m.set(1, 2, -sin(a));
        m.set(2, 1, sin(a));
        m.set(2, 2, cos(a));
        return m;
    }

    static Type rotationXZ(double a) MAT_QUAD_MIN(3) {
        auto m = Type::unit();
        m.set(0, 0, cos(a));
        m.set(0, 2, sin(a));
        m.set(2, 0, -sin(a));
        m.set(2, 2, cos(a));
        return m;
    }

    static Type rotationXY(double a) MAT_QUAD_MIN(2) {
        auto m = Type::unit();
        m.set(0, 0, cos(a));
        m.set(1, 0, -sin(a));
        m.set(0, 1, sin(a));
        m.set(1, 1, cos(a));
        return m;
    }

    static Type rotation(double x, double y, double z) MAT_QUAD_MIN(3) {
        auto m = Type::unit();
        m.set(0, 0, cos(x) * cos(y));
        m.set(0, 1, cos(x) * sin(y) * sin(z) - sin(x) * cos(z));
        m.set(0, 2, cos(x) * sin(y) * cos(z) + sin(x) * sin(z));
        m.set(1, 0, sin(x) * cos(y));
        m.set(1, 1, sin(x) * sin(y) * sin(z) + cos(x) * cos(z));
        m.set(1, 2, sin(x) * sin(y) * cos(z) - cos(x) * sin(z));
        m.set(2, 0, -sin(y));
        m.set(2, 1, cos(y) * sin(z));
        m.set(2, 2, cos(y) * cos(z));

        return m;
    }

    Type rotated2d(double a) const requires (M >= 2) {
        return Matrix<M, M, T>::rotation2d(a) * *this;
    }

    Type rotated(double x, double y, double z) const MAT_QUAD_MIN(3) {
        return rotation(x, y, z) * *this;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && M == N && M >= 3)
    Type rotated(Matrix<3, 1, U> const& rot) const {
        return rotation(
            static_cast<T>(rot.get(0)),
            static_cast<T>(rot.get(1)),
            static_cast<T>(rot.get(2))) * *this;
    }

    Type rotatedYZ(double a) const MAT_QUAD_MIN(3) {
        return rotationYZ(a) * *this;
    }

    Type rotatedXZ(double a) const MAT_QUAD_MIN(3) {
        return rotationXZ(a) * *this;
    }

    Type rotatedXY(double a) const MAT_QUAD_MIN(2) {
        return rotationXY(a) * *this;
    }

    Type rotate2d(double a) const MAT_QUAD_MIN(2) {
        return *this = rotated2d(a);
    }

    Type& rotate(double x, double y, double z) MAT_QUAD_MIN(3) {
        return *this = rotated(x, y, z);
    }

    Type& rotateYZ(double a) MAT_QUAD_MIN(3) {
        return *this = rotatedYZ(a);
    }

    Type& rotateXZ(double a) MAT_QUAD_MIN(3) {
        return *this = rotatedXZ(a);
    }

    Type& rotateXY(double a) MAT_QUAD_MIN(2) {
        return *this = rotatedXY(a);
    }

    Matrix<M-1, N, T> cutRow(int i) const requires (M > 1) {
        auto m = Matrix<M-1, N, T>();
        for (int j = 0; j < N * i; j++) {
            m.set(j, get(j));
        }
        for (int j = N*i; j < (M-1) * N; j++) {
            m.set(j, get(j+N));
        }
        return m;
    }

    Matrix<M, N-1, T> cutCol(int i) const requires (N > 1) {
        auto m = Matrix<M, N-1, T>();
        for (int k = 0; k < M; k++) {
            for (int j = 0; j < i; j++) {
                m.set(k, j, get(k, j));
            }
            for (int j = i; j < N-1; j++) {
                m.set(k, j, get(k, j + 1));
            }
        }
        return m;
    }

    Type normalized() const requires (N == 1) {
        return *this / norm();
    }

    Type normalize() const requires (N == 1) {
        return *this /= norm();
    }

    ///eye - Camera position
    ///center - camera direction
    ///up - up direction
    static Matrix<4, 4, T> lookAt(Matrix<3, 1, T> const& eye, Matrix<3, 1, T> const& center, Matrix<3, 1, T> const& up) {
        Matrix<3, 1, T> const f((center - eye).normalized());
        Matrix<3, 1, T> const s(f.cross(up).normalized());
        Matrix<3, 1, T> const u(s.cross(f));

        Matrix<4, 4, T> result = Matrix<4, 4, T>::unit();
        result.set(0, 0, s.get(0));
        result.set(1, 0, s.get(1));
        result.set(2, 0, s.get(2));
        result.set(0, 1, u.get(0));
        result.set(1, 1, u.get(1));
        result.set(2, 1, u.get(2));
        result.set(0, 2, -f.get(0));
        result.set(1, 2, -f.get(1));
        result.set(2, 2, -f.get(2));
        result.set(3, 0, -(s * eye));
        result.set(3, 1, -(u * eye));
        result.set(3, 2, f * eye);

        return result;
    }

    static Matrix<4, 4, T> ortho(T left, T right, T bottom, T top) {
        Matrix<4, 4, T> result = Matrix<4, 4, T>::unit();
        result.set(0, 0, static_cast<T>(2) / (right - left));
        result.set(1, 1, static_cast<T>(2) / (top - bottom));
        result.set(2, 2, -static_cast<T>(1));
        result.set(0, 3, -(right + left) / (right - left));
        result.set(1, 3, -(top + bottom) / (top - bottom));
        return result;
    }

    static Matrix<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
        Matrix<4, 4, T> result = Matrix<4, 4, T>::unit();
        result.set(0, 0, (static_cast<T>(2) * zNear) / (right - left));
        result.set(1, 1, (static_cast<T>(2) * zNear) / (top - bottom));
        result.set(2, 0, (right + left) / (right - left));
        result.set(2, 1, (top + bottom) / (top - bottom));
        result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
        result.set(2, 3, static_cast<T>(-1));
        result.set(3, 2, -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear));
        return result;
    }

    static Matrix<4, 4, T> perspective(T fovy, T aspect, T zNear, T zFar) {
        assert(std::abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

        T const tanHalfFovy = tan(fovy / static_cast<T>(2));

        auto result = Matrix<4, 4, T>();
        result.set(0, 0, static_cast<T>(1) / (aspect * tanHalfFovy));
        result.set(1, 1, static_cast<T>(1) / tanHalfFovy);
        result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
        result.set(2, 3, -static_cast<T>(1));
        result.set(3, 2, -(static_cast<T>(2) * zNear * zFar) / (zFar - zNear));

        return result;
    }

    static Matrix<4, 4, T> perspectiveFov(T fov, T width, T height, T zNear, T zFar) {
        assert(width > static_cast<T>(0));
        assert(height > static_cast<T>(0));
        assert(fov > static_cast<T>(0));

        T const rad = fov;
        T const h = cos(static_cast<T>(0.5) * rad) / sin(static_cast<T>(0.5) * rad);
        T const w = h * height / width;

        auto result = Matrix<4, 4, T>();
        result.set(0, 0, w);
        result.set(1, 1, h);
        result.set(2, 2, -(zFar + zNear) / (zFar - zNear));
        result.set(2, 3, -static_cast<T>(1));
        result.set(3, 2, -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear));

        return result;
    }

    template<typename U> requires (std::is_convertible_v<U, T> && M == N && M == 4)
    Type translated(Matrix<3, 1, U> const& m) const {
        auto result = Type(*this);
        result.set(3, 0, result.get(3, 0) + static_cast<T>(m.get(0)));
        result.set(3, 1, result.get(3, 1) + static_cast<T>(m.get(1)));
        result.set(3, 2, result.get(3, 2) + static_cast<T>(m.get(2)));
        return result;
    }

    Type translated(T dx, T dy, T dz) const requires (M == N && M == 4) {
        auto m = Type(*this);
        m.set(3, 0, m.get(3, 0) + dx);
        m.set(3, 1, m.get(3, 1) + dy);
        m.set(3, 2, m.get(3, 2) + dz);
        return m;
    }

    Type& translate(T dx, T dy, T dz) requires (M == N && M == 4) {
        return *this = translated(dx, dy, dz);
    }


    template<int START, int END>
    Matrix<END - START, 1, T> sliced() requires (N == 1 && 0 <= START && START < M && 0 <= END && END < M && END >= START) {
        auto vec = Matrix<END - START, 1, T>();
        for (int i = 0; i < END - START; ++i) {
            vec.set(i, this->get(START + i));
        }
        return vec;
    }

    Type abs() {
        auto m = *this;
        for (int i = 0; i < M * N; ++i) {
            m.data[i] = std::abs(m.data[i]);
        }
        return m;
    }

    float smallestAngleTo(const Type& o) const requires (N == 1) {
        const auto scalar = *this * o;
        const auto normProd = this->norm() * o.norm();
        return acos(scalar / normProd);
    }

    float angleTo(const Type& o) const requires (N == 1) {
        const auto angle = atan2(x() * o.y() - y() * o.x(), *this * o);
        return std::fmod(angle + std::numbers::pi * 2, std::numbers::pi * 2);
    }

    Matrix<M, N, float> toFloat() {
        return Matrix<M, N, float>(*this);
    }

    Matrix<M, N, double> toDouble() {
        return Matrix<M, N, double>(*this);
    }

};

inline float degToRad(float deg) {
    return (float)(deg / 180. * std::numbers::pi);
}

inline float radToDeg(float rad) {
    return (float)(rad / std::numbers::pi * 180.);
}

typedef Matrix<2, 1, float> Vec2f;
typedef Matrix<3, 1, float> Vec3f;
typedef Matrix<4, 1, float> Vec4f;
typedef Matrix<2, 2, float> Mat2f;
typedef Matrix<3, 3, float> Mat3f;
typedef Matrix<4, 4, float> Mat4f;

typedef Matrix<2, 1, double> Vec2d;
typedef Matrix<3, 1, double> Vec3d;
typedef Matrix<4, 1, double> Vec4d;
typedef Matrix<2, 2, double> Mat2d;
typedef Matrix<3, 3, double> Mat3d;
typedef Matrix<4, 4, double> Mat4d;

typedef Matrix<2, 1, unsigned> Vec2u;
typedef Matrix<2, 1, int> Vec2i;

#undef MAT_TEMPLATE
#undef MAT_QUAD
#undef MAT_QUAD_MIN

#endif //MD_2024_VEC_H
