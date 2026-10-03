#include "RigidBody2D.hpp"

void RigidBody2D::integrate(float dt) noexcept {
    velocity += acceleration * dt;
    position += velocity * dt;
}
