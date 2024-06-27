#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render(int spp,int area_light_samples);

    Scene scene;
    Texture outputImage;
};