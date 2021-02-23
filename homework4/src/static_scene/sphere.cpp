#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  Vector3D c = object->o;
  double rad = object->r;

  double A = dot(r.d, r.d);
  double B = dot(2*(r.d), (r.o) - c);
  double C = (dot((r.o) - c, (r.o) - c)) - (rad * rad);

  double disc = (B*B) - (4 * A * C);

  if(disc < 0)
  {
    return false;
  }

  t1 = ((-B) + sqrt(disc))/(2*A);
  t2 = ((-B) - sqrt(disc))/(2*A);

  if(t1 > t2)
  {
    swap(t1,t2);
  }

  return true;

}


bool Sphere::intersect(const Ray& r, Intersection *i) const {


    double t1, t2;

    if(test(r, t1, t2) == false)
    {
      return false;
    }

    double rad = object->r;
    Vector3D c = object->o;

    Vector3D p = (r.o) + (t1 * (r.d));

    Vector3D n = (p - c)/rad;

    if (t1 <= r.max_t && t1 >= r.min_t){ 
    r.max_t = t1;
    i->t = t1; 
    i->primitive = this;
    i->n = n; 
    i->bsdf = get_bsdf();

    return true;
  }
  
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
