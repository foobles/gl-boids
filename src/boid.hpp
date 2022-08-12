//
// Created by foobles on 7/29/2022.
//

#ifndef SDL_GLEW_TEST_BOID_HPP
#define SDL_GLEW_TEST_BOID_HPP

#include "GL/glew.h"
#include "matrix.hpp"
#include "transform.hpp"

class Boid {
public:
    class Mindset {
    public:
        GLfloat obstacle_avoiding_bias;
        GLfloat centering_bias;
        GLfloat conforming_bias;

        GLfloat maximum_movement;
    };

    class MovementDecision {
    public:
        Vec3<GLfloat> decided_velocity;
    };

    class SituationalAwareness {
    public:
        GLfloat total_inv_dist_sq = 0;
        Vec3<GLfloat> total_scaled_directions = {{0, 0, 0}};
        Vec3<GLfloat> total_scaled_velocities = {{0, 0, 0}};

        [[nodiscard]] MovementDecision into_decision(Mindset const &mindset) const noexcept;
    };

    Vec3<GLfloat> pos;
    Vec3<GLfloat> velocity;

    void consider(SituationalAwareness &awareness, Boid const &other) const noexcept;
    void act_upon(MovementDecision const &decision) noexcept;

    [[nodiscard]] Transform<GLfloat> transform() const noexcept;
};

#endif //SDL_GLEW_TEST_BOID_HPP
