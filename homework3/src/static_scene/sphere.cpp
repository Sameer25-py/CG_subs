#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

//PART 4
bool Sphere::test(const Ray& r, double& t1, double& t2) const {

    return false;

}

//PART 5
bool Sphere::intersect(const Ray& r, Intersection *i) const {


  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

bool Sphere::intersect(const Ray& r) const {}


void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
