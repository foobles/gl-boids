//
// Created by foobles on 7/31/2022.
//
#include <cmath>
#include "boid.hpp"

#include <stdio.h>

static GLfloat magnitude_sq(Vec3<GLfloat> v) noexcept {
    auto [x, y, z] = v.arr;
    return x*x + y*y + z*z;
}

void Boid::consider(Boid::SituationalAwareness &awareness, Boid const &other) const noexcept {
    Vec3<GLfloat> diff = other.pos - pos;
    GLfloat inv_dist_sq = 1.0f / std::max(magnitude_sq(diff), 0.00001f);

    awareness.total_inv_dist_sq += inv_dist_sq;
    awareness.total_scaled_directions += inv_dist_sq * diff;
    awareness.total_scaled_velocities += inv_dist_sq * other.velocity;
}

static GLfloat accumulate_movement(Vec3<GLfloat> &acc, GLfloat remaining_movement_sq, Vec3<GLfloat> movement) noexcept {
    auto msq = magnitude_sq(movement);
    if (msq < remaining_movement_sq) {
        acc += movement;
        return remaining_movement_sq - msq;
    } else {
        auto scale = std::sqrt(remaining_movement_sq / msq);
        acc += scale * movement;
        return 0;
    }
}

[[nodiscard]] Boid::MovementDecision Boid::SituationalAwareness::into_decision(const Mindset &mindset) const noexcept {
    auto obstacle_avoiding = mindset.obstacle_avoiding_bias * -total_scaled_directions;
    auto conforming = mindset.conforming_bias / total_inv_dist_sq * total_scaled_velocities;
    auto centering = mindset.centering_bias / total_inv_dist_sq * total_scaled_directions;

    GLfloat remaining_movement_sq = mindset.maximum_movement * mindset.maximum_movement;
    Vec3<GLfloat> velocity_decision = {{0, 0, 0}};

    for (auto const *influence : {&obstacle_avoiding, &conforming, &centering}) {
        if (remaining_movement_sq == 0) {
            break;
        }
        remaining_movement_sq = accumulate_movement(velocity_decision, remaining_movement_sq, *influence);
    }

    return {velocity_decision};

}

void Boid::act_upon(Boid::MovementDecision const &decision) noexcept {
    pos += velocity;
    velocity = decision.decided_velocity;
}


[[nodiscard]] Transform<GLfloat> Boid::transform() const noexcept {
    GLfloat veloc_mag_sq = magnitude_sq(velocity);
    Vec3<GLfloat> a = (veloc_mag_sq != 0)?
        1/std::sqrt(veloc_mag_sq) * velocity : Vec3<GLfloat>{{0, 0, 1}};

    Vec3<GLfloat> b = {{a[2], 0, -a[0]}};
    GLfloat b_mag = std::sqrt(b[0]*b[0] + b[2]*b[2]);
    b *= 1/b_mag;

    Vec3<GLfloat> c = {{a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]}};

    return Transform<GLfloat>{{{
        b[0],   b[1],   b[2],   0,
        c[0],   c[1],   c[2],   0,
        a[0],   a[1],   a[2],   0,
        pos[0], pos[1], pos[2], 1,
    }}};
}
