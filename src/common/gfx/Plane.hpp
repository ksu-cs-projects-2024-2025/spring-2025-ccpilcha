#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

// Structure to hold a plane equation
struct Plane {
    glm::vec3 normal;
    float d;

    // Normalize the plane equation
    void normalize() {
        float len = glm::length(normal);
        normal /= len;
        d /= len;
    }
};

// Extract frustum planes from a View-Projection matrix
std::array<Plane, 6> extractFrustumPlanes(const glm::mat4& VP) {
    std::array<Plane, 6> planes;

    // Left
    planes[0].normal = glm::vec3(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP[2][0]);
    planes[0].d = VP[3][3] + VP[3][0];

    // Right
    planes[1].normal = glm::vec3(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP[2][0]);
    planes[1].d = VP[3][3] - VP[3][0];

    // Bottom
    planes[2].normal = glm::vec3(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP[2][1]);
    planes[2].d = VP[3][3] + VP[3][1];

    // Top
    planes[3].normal = glm::vec3(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP[2][1]);
    planes[3].d = VP[3][3] - VP[3][1];

    // Near
    planes[4].normal = glm::vec3(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP[2][2]);
    planes[4].d = VP[3][3] + VP[3][2];

    // Far
    planes[5].normal = glm::vec3(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP[2][2]);
    planes[5].d = VP[3][3] - VP[3][2];

    // Normalize the planes
    for (auto& plane : planes) {
        plane.normalize();
    }

    return planes;
}
