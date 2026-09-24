#include "Vector2D.hpp"

[[nodiscard]] Vector2D Vector2D::normalized() const {
    float len = length();
    return Vector2D(x/len, y/len);
}

void Vector2D::normalize() {
    float len = length();
    x /= len;
    y /= len;
}

[[nodiscard]] Vector2D Vector2D::operator+(const Vector2D& rhs) const noexcept {
    return Vector2D(x + rhs.x, y + rhs.y);
}

[[nodiscard]] Vector2D Vector2D::operator-(const Vector2D& rhs) const noexcept {
    return Vector2D(x - rhs.x, y - rhs.y);
}

[[nodiscard]] Vector2D Vector2D::operator*(float scalar) const noexcept {
    return Vector2D(scalar * x, scalar * y);
}

// @pre std::abs(scalar) > EPSILON.
[[nodiscard]] Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D(x/scalar, y/scalar);
}

Vector2D& Vector2D::operator+=(const Vector2D& rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& rhs) noexcept {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2D& Vector2D::operator*=(float scalar) noexcept {
    x *= scalar;
    y *= scalar;
    return *this;
}

// @pre std::abs(scalar) > EPSILON.
Vector2D& Vector2D::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

[[nodiscard]] Vector2D operator*(float scalar, const Vector2D& vec) noexcept {
    return vec*scalar;
}
