#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction {
    // Position of interaction
    Vector3f p;
    // Normal of the surface at interaction
    Vector3f n;
    // The uv co-ordinates at the intersection point
    Vector2f uv;
    // The viewing direction in local shading frame
    Vector3f wi; 
    // Distance of intersection point from origin of the ray
    float t = 1e30f; 
    // Used for light intersection, holds the radiance emitted by the emitter.
    Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
    // BSDF at the shading point
    BSDF* bsdf;
    // Vectors defining the orthonormal basis
    Vector3f a, b, c;

    bool didIntersect = false;

    Vector3f toWorld(Vector3f w, Vector3f dir) {
        // TODO: Implement this
        Vector3f z1 = Normalize(this->n);
        Vector3f y1 = Normalize(Cross(z1, dir));
        Vector3f x1 = Normalize(Cross(y1, z1));

        Vector3f a1 = Vector3f(x1.x, y1.x, z1.x);
        Vector3f a2 = Vector3f(x1.y, y1.y, z1.y);
        Vector3f a3 = Vector3f(x1.z, y1.z, z1.z);

        return Vector3f(Dot(a1, w), Dot(a2, w), Dot(a3, w));
    }

    Vector3f toLocal(Vector3f w) {
        // TODO: Implement this
        // Vector3f z1 = Normalize(n);
        // Vector3f y1 = Normalize(Cross(z1, dir));
        // Vector3f x1 = Normalize(Cross(y1, z1));

        // Vector3f a1 = Vector3f(x1.x, x1.y, x1.z);
        // Vector3f a2 = Vector3f(y1.x, y1.y, y1.z);
        // Vector3f a3 = Vector3f(z1.x, z1.y, z1.z);

        // return Vector3f(Dot(a1, world), Dot(a2, world), Dot(a3, world));
         return Vector3f(0, 0, 0);
    }
};