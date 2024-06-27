#include "light.h"
#include "surface.h"

Light::Light(LightType type, nlohmann::json config) {
    switch (type) {
        case LightType::POINT_LIGHT:
            this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
            break;
        case LightType::DIRECTIONAL_LIGHT:
            this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
            break;
        case LightType::AREA_LIGHT:  //3.1
            // TODO: Implement this
            this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
            this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
            this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
            this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
            break;
        default:
            std::cout << "WARNING: Invalid light type detected";
            break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

extern int sampling_method;
std::pair<Vector3f, LightSample> Light::sample(Interaction *si) {
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type) {
        case LightType::POINT_LIGHT:
            ls.wo = (position - si->p);
            ls.d = ls.wo.Length();
            ls.wo = Normalize(ls.wo);
            radiance = (1.f / (ls.d * ls.d)) * this->radiance;
            break;
        case LightType::DIRECTIONAL_LIGHT:
            ls.wo = Normalize(direction);
            ls.d = 1e10;
            radiance = this->radiance;
            break;
        case LightType::AREA_LIGHT:
            // TODO: Implement this
            
            if (si->didIntersect == true)
            {
                if (sampling_method == 0)   //4.1
                {
                    float a = next_float() * M_PI * 2;
                    // float b = next_float() * M_PI / 2;
                    float b = acos(next_float());
                    ls.wo = Vector3f(cos(a)*sin(b),sin(a)*sin(b),cos(b));
                    ls.d = 1e10;
                    radiance = this->radiance * 2 * M_PI;
                    ls.p = this->normal;
                }
                else if (sampling_method == 1)  //4.2
                {
                    float a = next_float() * M_PI * 2;
                    float b = acos(sqrt(next_float()));
                    ls.wo = Vector3f(cos(a)*sin(b),sin(a)*sin(b),cos(b));
                    ls.d = 1e10;
                    radiance = this->radiance * M_PI;
                    ls.p = this->normal;
                }                        
                else if (sampling_method == 2)   //4.3
                {
                    Vector3f ai = this->center + 2*(next_float() - 0.5)*this->vx  + 2*(next_float() - 0.5)*this->vy;
                    float radius_square = (ai - si->p).LengthSquared();
                    ls.wo = Normalize(ai - si->p);
                    ls.d = (ai - si->p).Length();
                    float cos_ = -Dot(this->normal, ls.wo);
                    float area = (this->vx).Length()* (this->vy).Length() * 4;
                    radiance = (this->radiance * cos_ * area)/radius_square;
                }
            }
            
            break;
    }
    return { radiance, ls };
}

//3.2
Interaction Light::intersectLight(Ray *ray) {
    Interaction si;
    memset(&si, 0, sizeof(si));

    if (type == LightType::AREA_LIGHT) {
        // TODO: Implement this
        Vector3f v1 = this->center + this->vx + this->vy;
        Vector3f v2 = this->center + this->vx - this->vy;
        Vector3f v3 = this->center - this->vx + this->vy;
        Vector3f v4 = this->center - this->vx - this->vy;
        Vector3f n = this->normal;
        Surface dummy;
        Interaction si1 = dummy.rayTriangleIntersect(*ray,v1,v2,v3,n);
        Interaction si2 = dummy.rayTriangleIntersect(*ray,v2,v3,v4,n);
            
        si.emissiveColor = this->radiance;
        if (si1.didIntersect == true)
        {
            si.t = si1.t;
            si.didIntersect = true;
        }
        else if (si2.didIntersect == true)
        {
            si.t = si2.t;
            si.didIntersect = true;
        }
        
        
    }

    return si;
}