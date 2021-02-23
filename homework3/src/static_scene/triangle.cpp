#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  BBox bb(p1);
  bb.expand(p2); 
  bb.expand(p3);
  return bb;

}
bool Triangle::intersect(const Ray& r) const {

  // Task 2 Part 6: implement ray-triangle intersection
  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  //setup
  Vector3D E1 = p2-p1;
  Vector3D E2 = p3-p1;
  Vector3D T  = r.o-p1;
  float denom = dot(cross(E2,r.d),E1);
  Vector3D normal = cross(E1,E2);
  if(dot(normal,r.d) == 0 ) return false; //no intersection parallel vectors
  if(denom <= 0) return false;
  
  //calculating u
  float inv_denom = 1/denom;
  float u = dot(cross(E2,r.d),T) * inv_denom;
  if(u < 0 || u > 1) return false;

  //calculating v
  float v = dot(cross(E1,T),r.d)*inv_denom;
  if(v < 0 || v+u > 1) return false;

  //calculating t
  float t= dot(cross(E1,T),E2)*inv_denom;
  if(t > r.min_t && t < r.max_t){r.max_t = t; return true;}
  return false;
} 


bool Triangle::intersect(const Ray& r, Intersection *isect) const {

  // Task 2 Part 6:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  Vector3D n1(mesh->normals[v1]), n2(mesh->normals[v2]), n3(mesh->normals[v3]);
  if(intersect(r)){
    Vector3D E1 = p2-p1;
    Vector3D E2 = p3-p1;
    Vector3D T  = r.o-p1;
    float denom = dot(cross(r.d,E2),E1);
    float inv_denom = 1/denom;
    float u = dot(cross(E2,r.d),T) * inv_denom;
    float v = dot(cross(E1,T),r.d)*inv_denom;
    float t= dot(cross(E1,T),E2)*inv_denom;

    isect->t = t;
    isect->primitive = this;
    isect->n = (1-u-v)*n1+(u*n2)+(v*n3);
    isect->bsdf = get_bsdf();
    return true;

  }

  return false;
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}



} // namespace StaticScene
} // namespace CGL
