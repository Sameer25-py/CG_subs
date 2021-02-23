#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

//PART 4
bool Sphere::test(const Ray& r, double& t1, double& t2) const {
    
    double A = dot(r.d,r.d);
    double B = 2*dot(r.d,r.o-o);
    double C = dot(r.o-o,r.o-o)-(r2);

    double det = (B*B) - (4*A*C);

    if(det < 0){
      //no intersection
      return false;  
    } 
      t1 = (-B + sqrt(det))/(2*A);
      t2 = (-B - sqrt(det))/(2*A);
      
    return true;
}

//PART 5
bool Sphere::intersect(const Ray& r, Intersection *i) const {

  double t1,t2;
  bool intersection = test(r,t1,t2);
  if(!intersection)return false;
    t1 = min(t1,t2);
    if(t1 > r.min_t && t1 < r.max_t){  
      Vector3D normal = (r.o+t1*r.d-o);
      normal.normalize();
      r.max_t = t1;
      i->t=t1;
      i->primitive=this;
      i->n = normal;
      i->bsdf=get_bsdf();
      return true;
    }

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

bool Sphere::intersect(const Ray& r) const {
  double t1,t2;
  if(!test(r,t1,t2)) return false;
  if(t1>r.min_t && t1 <= r.max_t) return true;
}


void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
