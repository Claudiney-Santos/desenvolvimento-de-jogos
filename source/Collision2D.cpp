#include "Collision2D.hpp"

[[nodiscard]] bool AABB::intersects(const AABB& other) const noexcept {
    return !(max.x < other.min.x || other.max.x < min.x
            || max.y < other.min.y || other.max.y < min.y);
}

[[nodiscard]] AABB Collision2D::bounds(const Vector2D& position) const noexcept {
    return { position - halfExtents, position + halfExtents };
}
