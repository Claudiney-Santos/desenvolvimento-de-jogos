#include "Transform2D.hpp"
#include <cmath>

[[nodiscard]] Transform2D Transform2D::translation(float tx, float ty) noexcept {
    Transform2D matrix;
    matrix.m[2][0] = tx;
    matrix.m[2][1] = ty;
    return matrix;
}

[[nodiscard]] Transform2D Transform2D::rotation(float angle_rad) noexcept {
    Transform2D matrix;
    matrix.m[0][0] = std::cos(angle_rad);
    matrix.m[0][1] = std::sin(angle_rad);
    matrix.m[1][0] = -std::sin(angle_rad);
    matrix.m[1][1] = std::cos(angle_rad);
    return matrix;
}

[[nodiscard]] Transform2D Transform2D::scale(float sx, float sy) noexcept {
    Transform2D matrix;
    matrix.m[0][0] = sx;
    matrix.m[1][1] = sy;
    return matrix;
}

[[nodiscard]] Transform2D Transform2D::operator*(const Transform2D& rhs) const noexcept {
    Transform2D matrix;
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            matrix.m[i][j] = 0;
            for(int k=0;k<3;k++) {
                matrix.m[i][j] += m[i][k]*rhs.m[k][j];
            }
        }
    }
    return matrix;
}

Transform2D& Transform2D::operator*=(const Transform2D& rhs) noexcept {
    Transform2D matrix = (*this) * rhs;
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) {
            m[i][j] = matrix.m[i][j];
        }
    }
    return *this;
}

[[nodiscard]] Vector2D Transform2D::transform_point(const Vector2D& point) const noexcept {
    Vector2D vec;
    vec.x = point.x * m[0][0] + point.y * m[1][0] + m[2][0];
    vec.y = point.x * m[0][1] + point.y * m[1][1] + m[2][1];
    return vec;
}

[[nodiscard]] Vector2D Transform2D::transform_vector(const Vector2D& direction) const noexcept {
    Vector2D vec;
    vec.x = direction.x * m[0][0] + direction.y * m[1][0];
    vec.y = direction.x * m[0][1] + direction.y * m[1][1];
    return vec;
}
