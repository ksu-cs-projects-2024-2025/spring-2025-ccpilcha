#include <glm/glm.hpp>
#include <algorithm>
#include <limits>
#include <vector>
#include <cmath>

struct Hit
{
    double t;          // time of impact ∈ [0,1], =1.0 means “no hit”
    glm::dvec3 normal; // collision normal at impact
};

struct CollisionInfo {
    bool hitX0 = false;
    bool hitX1 = false;
    bool hitY0 = false;
    bool hitY1 = false;
    bool hitZ0 = false;
    bool hitZ1 = false;
};

/**
 * @brief Handle Swept‐AABB vs AABB; A is moving, B is stationary
 * 
 * @param posA Center of the sweeping AABB cuboid
 * @param halfA The 3D radius of the cuboid
 * @param posB Center of the stationary AABB cube
 * @param halfB The 3D radius of the cube
 * @param vel The velocity of A
 * @return Hit object (provides the time t when it hits and the normal at the impact)
 */
Hit sweepAABB(
    const glm::dvec3 &posA,
    const glm::dvec3 &halfA,
    const glm::dvec3 &posB,
    const glm::dvec3 &halfB,
    const glm::dvec3 &vel)
{
    // Compute A’s bounds at t=0
    glm::dvec3 minA = posA - halfA;
    glm::dvec3 maxA = posA + halfA;
    // Compute B’s bounds
    glm::dvec3 minB = posB - halfB;
    glm::dvec3 maxB = posB + halfB;

    double entryT = 0.0;
    double exitT = 1.0;
    glm::dvec3 normal{0.0};

    // For each axis X, Y, Z
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(vel[i]) < 1e-8)
        {
            // Parallel movement: if A is already overlapping on this axis, OK; else no collision.
            if (maxA[i] < minB[i] || minA[i] > maxB[i])
            {
                return {1.0, glm::dvec3{0.0}}; // never hits
            }
        }
        else
        {
            // Compute times when the bodies touch
            double invV = 1.0 / vel[i];
            double t1 = (minB[i] - maxA[i]) * invV;
            double t2 = (maxB[i] - minA[i]) * invV;
            // entry = min(t1,t2), exit = max(t1,t2)
            double axisEntry = std::min(t1, t2);
            double axisExit = std::max(t1, t2);

            // Shrink overall [entryT, exitT]
            if (axisEntry > entryT)
            {
                entryT = axisEntry;
                // record normal on whichever face we hit first
                normal = glm::dvec3{0.0};
                normal[i] = (t1 < t2 ? -1.0 : +1.0);
            }
            exitT = std::min(exitT, axisExit);

            // No hit if interval invalid
            if (entryT > exitT || exitT < 0.0 || entryT > 1.0)
                return {1.0, glm::dvec3{0.0}};
        }
    }

    // Clamp entry to [0,1]
    entryT = std::clamp(entryT, 0.0, 1.0);
    return {entryT, normal};
}

/**
 * @brief This function handles collisions upon movement
 * 
 * @param startPos 
 * @param halfExtents 
 * @param delta The velocity
 * @param blockCenters All the block coordinates found within the swept AABB boundaries
 * @param blockSize The size of the voxels as cubes
 * @return glm::dvec3 the modified velocity of the object such that the AABB cannot phase into blocks
 */
glm::dvec3 moveWithSweptAABB(
    const glm::dvec3 &start,
    const glm::dvec3 &radii,
    const glm::dvec3 &delta,
    const std::vector<glm::dvec3> &blockCenters,
    double blockSize = 1.0,
    CollisionInfo *info = nullptr)
{
    const double EPS = 1e-5;
    if (info) *info = {};                 // clear all hits
    glm::dvec3 curr = start;
    glm::dvec3 rem  = delta;

    // ← REPLACE your old loop with this:
    for (int iter = 0; iter < 3 && glm::length(rem) > EPS; ++iter) {
        double bestT = 1.0;
        glm::dvec3 bestN{0.0};

        // find the earliest block collision
        for (auto &bc : blockCenters) {
            auto hit = sweepAABB(
                curr, radii,
                bc, glm::dvec3(blockSize * 0.5),
                rem
            );
            if (hit.t < bestT) {
                bestT = hit.t;
                bestN = hit.normal;
            }
        }

        // if no collision, do the full move
        if (bestT >= 1.0 - EPS) {
            curr += rem;
            break;
        }

        if (info) {
            // record which axis was hit
            if (bestN.x < -0.5) info->hitX1 = true;
            if (bestN.y < -0.5) info->hitY1 = true;
            if (bestN.z < -0.5) info->hitZ1 = true;
            if (bestN.x >  0.5) info->hitX0 = true;
            if (bestN.y >  0.5) info->hitY0 = true;
            if (bestN.z >  0.5) info->hitZ0 = true;
        }

        // move up to impact
        curr += rem * bestT;
        // nudge out to avoid re‑colliding
        curr += bestN * EPS;
        // compute the remainder and slide
        glm::dvec3 after = rem * (1.0 - bestT);
        rem = after - glm::dot(after, bestN) * bestN;
    }

    return curr;
}
