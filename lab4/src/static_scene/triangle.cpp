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

  double u,v,t;

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);

  Vector3D v0v1 = p2 - p1;
  Vector3D v0v2 = p3 - p1;

  Vector3D pvec = cross((r.d), v0v2);
  double det = dot(v0v1, pvec);

  if(det == 0) 
  {
    return false;
  }

  double invDet = 1/det;

  Vector3D tvec = r.o - p1;
  u = dot(tvec, pvec)*invDet;

  if(u < 0 || u > 1)
  {
    return false;
  }

  Vector3D qvec = cross(tvec, v0v1);
  v = dot((r.d),qvec) * invDet;

  if(v < 0 || v > 1)
  {
    return false;
  }

  t = dot(v0v2,qvec) * invDet;


  if (t <= r.max_t && t >= r.min_t) { // if there is an intersection, the if condition should be true
    r.max_t = t; //replace this with your value of t
    return true;
  }

  return false;
  
} 


bool Triangle::intersect(const Ray& r, Intersection *isect) const {


  double u,v,t;

  Vector3D p1(mesh->positions[v1]), p2(mesh->positions[v2]), p3(mesh->positions[v3]);
  Vector3D n1(mesh->normals[v1]), n2(mesh->normals[v2]), n3(mesh->normals[v3]);

  Vector3D v0v1 = p2 - p1;
  Vector3D v0v2 = p3 - p1;

  Vector3D pvec = cross((r.d), v0v2);
  double det = dot(v0v1, pvec);

  if(det == 0)
  {
    return false;
  }

  double invDet = 1/det;

  Vector3D tvec = r.o - p1;
  u = dot(tvec, pvec)*invDet;

  if(u < 0 || u > 1)
  {
    return false;
  }

  Vector3D qvec = cross(tvec, v0v1);
  v = dot((r.d), qvec) * invDet;
 
  if(v < 0 || v > 1)
  {
    return false;
  }

   t = dot(v0v2, qvec) * invDet;

   double w;
   w = 1 - (u + v);

  if(w < 0 || w > 1)
  {
    return false;
  }

   Vector3D nor;

   nor.x = n1.x*u + n2.x*v + n3.x*w;
   nor.y = n1.y*u + n2.y*v + n3.y*w;
   nor.z = n1.z*u + n2.z*v + n3.z*w;

  if (t <= r.max_t && t >= r.min_t){ // if there is an intersection, the if condition should be true
    r.max_t = t;
    isect->t = t; //replace this with your value of t
    isect->primitive = this;
    isect->n = nor; //replace this with your value of normal at the point of intersection
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
