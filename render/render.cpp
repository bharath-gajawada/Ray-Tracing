#include "render.h"

int sampling_method;

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int spp, int area_light_samples)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            // 2
            Vector3f result(0, 0, 0);
            for (int i = 0; i < spp; i++)
            {
                Ray sampleRay = this->scene.camera.generateRay(x + next_float() - 0.5, y + next_float() - 0.5);
                Interaction si = this->scene.rayIntersect(sampleRay);
                Interaction si1 = this->scene.rayEmitterIntersect(sampleRay); // 3.2
                Vector3f sample_result(0, 0, 0);

                if (si.didIntersect)
                {
                    Vector3f radiance;
                    LightSample ls;

                    if (sampling_method == 0 || sampling_method == 1)
                    {
                        for (Light &light : this->scene.lights)
                        {
                            if (light.type == 2)
                            {
                                // 4.1 && 4.2
                                for (int j = 0; j < area_light_samples; j++)
                                {
                                    std::tie(radiance, ls) = light.sample(&si);
                                    ls.wo = si.toWorld(ls.wo, si.p - this->scene.camera.from);

                                    Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                    Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                    Ray shadowRay1(si.p + 1e-3f * si.n, ls.wo);
                                    Interaction siEmitted = this->scene.rayEmitterIntersect(shadowRay1);

                                    if (siEmitted.didIntersect)
                                    {
                                        if (!siShadow.didIntersect || siShadow.t > siEmitted.t)
                                        {
                                            if (Dot(ls.p, si.n) <= 0)
                                            {
                                                if (sampling_method == 0) // 4.1
                                                    sample_result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                                else if (sampling_method == 1) // 4.2
                                                    sample_result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance;
                                            }
                                        }
                                    }
                                }
                                sample_result /= area_light_samples;
                            }
                            else
                            {
                                std::tie(radiance, ls) = light.sample(&si);

                                Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                if (!siShadow.didIntersect || siShadow.t > ls.d)
                                {
                                    sample_result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                }
                            }
                        }
                    }
                    else
                    {
                        if (sampling_method == 2)   //4.3
                        {
                            for (Light &light2 : this->scene.lights)
                            {
                                if (light2.type == 2)
                                {
                                    for (int j = 0; j < area_light_samples; j++)
                                    {
                                        std::tie(radiance, ls) = light2.sample(&si);

                                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                                        {
                                            sample_result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                        }
                                    }
                                    
                                    sample_result /= area_light_samples;
                                }
                                else
                                {
                                    std::tie(radiance, ls) = light2.sample(&si);

                                    Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                    Interaction siShadow = this->scene.rayIntersect(shadowRay);

                                    if (!siShadow.didIntersect || siShadow.t > ls.d)
                                    {
                                        sample_result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                                    }
                                }
                            }
                        }
                    }
                }

                result += sample_result;
                if (si1.didIntersect)
                    result = si1.emissiveColor * spp;
            }
            result /= spp;
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    // if (argc != 5)
    if (argc != 6)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy> <area_light_samples>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    sampling_method = atoi(argv[4]);
    int area_light_samples = atoi(argv[5]);
    auto renderTime = rayTracer.render(spp, area_light_samples);

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
