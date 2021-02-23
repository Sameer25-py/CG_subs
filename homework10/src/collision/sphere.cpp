#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../misc/sphere_drawing.h"
#include "sphere.h"

using namespace nanogui;
using namespace CGL;

void Sphere::collide(PointMass &pm) {
  Vector3D position_last = pm.position;
  Vector3D dir_ray = position_last - origin;
  Vector3D dir_unit = dir_ray/dir_ray.norm();
  if(dir_ray.norm()*dir_ray.norm() <= radius2){
    Vector3D tan_point = origin + dir_unit*radius;
    Vector3D correction = tan_point- position_last;
  pm.position += correction * (1-friction);
}


}

void Sphere::render(GLShader &shader) {
  // We decrease the radius here so flat triangles don't behave strangely
  // and intersect with the sphere when rendered
  Misc::draw_sphere(shader, origin, radius * 0.92);
}
