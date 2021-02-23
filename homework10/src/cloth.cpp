#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

double randFromRange(double lower, double upper)
{
  return ((double(rand()) / double(RAND_MAX)) * (upper - lower)) + lower;
}

int getIndex(int y, int x, int nwp, int nhp){
  return true;
}

bool isPinned(const vector<vector<int>> &pinned, int x, int y){

  return false;
}

void Cloth::buildGrid() {
  srand(time(NULL));
  double delta_w = width/num_width_points;
  double delta_h = height/num_height_points;
  int &h = num_height_points;
  int &w = num_width_points;
  printf("%d, %d/n", num_width_points,num_height_points);
  for(int j = 0; j < h; ++j)
  {
    for(int i = 0;i < w; ++i)
    {
      double rand_offset = 1.0 / ((rand()%10000)+1000);
      point_masses.push_back(PointMass(Vector3D(
                  i * delta_w,(orientation == HORIZONTAL)? 1 : j * delta_h,
                  (orientation == HORIZONTAL)? j * delta_w : rand_offset),
                  isPinned(pinned, i, j)
      ));
    }
  }
  for(int j = 0; j < h;++j)
  {
    for(int i = 0; i < w;++i)
    {
      PointMass *pm = &point_masses[j*w+i];
      if(i!=0)
        springs.push_back(Spring(pm,&point_masses[j*w+(i-1)],STRUCTURAL));
      if(j!=0)
        springs.push_back(Spring(pm,&point_masses[(j-1)*w+(i+1)],STRUCTURAL));
      if(i!=0 && j!=0)
        springs.push_back(Spring(pm,&point_masses[(j-1)*w+(i-1)],SHEARING));
      if(i < w-1 && j!=0)
        springs.push_back(Spring(pm, &point_masses[(j-1)*w+(i-1)],SHEARING));
      if(i < w-2)
        springs.push_back(Spring(pm, &point_masses[j*w+(i+2)],BENDING));
      if(j > 1)
        springs.push_back(Spring(pm,&point_masses[(j-2)*w+i],BENDING));

    }
  }



}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;
  Vector3D acc(0,0,0);
  for(Vector3D &a : external_accelerations)
    acc += a;
  for(PointMass &particle : point_masses)
    particle.forces = acc * mass;

  for(Spring &s : springs)
  {
    const int &type = s.spring_type;
    bool &st_c = cp->enable_structural_constraints;
    bool &sh_c = cp->enable_shearing_constraints;
    bool &be_c = cp->enable_bending_constraints;

    if((st_c && type == STRUCTURAL) || (sh_c && type == SHEARING) || (be_c && type == BENDING))
    {

      Vector3D l = s.pm_b->position - s.pm_a->position;
      Vector3D e = l - (s.rest_length * l.unit());
      Vector3D F = cp->ks * e;
      s.pm_a->forces += F;
      s.pm_b->forces -= F;
    }


  }

  for(PointMass &p : point_masses)
  {
    if(p.pinned)
      continue;
    Vector3D pos = p.position;
    p.position = pos + (1 - cp->damping / 100) * (pos -p.last_position) + p.forces/mass*delta_t*delta_t;
    p.last_position = pos;
  }

  for(Spring &s : springs)
  {
    Vector3D ab = s.pm_b->position - s.pm_a->position;
    double len = ab.norm();
    if (len/s.rest_length<1.1)
      continue;
    bool &a_pin = s.pm_a->pinned;
    bool &b_pin = s.pm_b->pinned;
    double surplus = len - 1.1 * s.rest_length;

    if(!a_pin && !b_pin)
    {
      s.pm_a->position += ab.unit() * surplus/2;
      s.pm_b->position += -ab.unit() * surplus/2;
    }
    if(a_pin && !b_pin)
      s.pm_b->position += -ab.unit() * surplus;
    if(!a_pin && b_pin)
      s.pm_a->position += ab.unit() * surplus;
  }
  for(PointMass &pm : point_masses)
  {
    for(CollisionObject *co : *collision_objects )
    {
      co->collide(pm);
    }
  }



}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for (int p = 0; p < point_masses.size(); p++){
    float hkey = hash_position(point_masses[p].position);
    if (!map.count(hkey)){ //if it is already not in map using unordered map's count
      map[hkey] = new vector<PointMass*>(); //for a new key dynamically allocate memory for a point mass* vector
    }
    else{ //if key already exists, just push back to its vector
      map[hkey]->push_back(&point_masses[p]);
    }
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // // TODO (Part 4): Handle self-collision for a given point mass.
  float hkey = hash_position(pm.position);
  if (map.count(hkey)){ //if key exists in hashtable
    Vector3D avg_correction(0, 0, 0);//apply corrections
    int num_corrections = 0;
    vector<PointMass*> hashedPMs = *map[hkey];
    for (int p = 0; p < hashedPMs.size(); p++){
      PointMass* candidatePM = hashedPMs[p]; //get every point mass in current map[hash]'s pm vector
      if (candidatePM->position == pm.position) //to prevent self collision
        continue;

      Vector3D PtoC = candidatePM->position - pm.position;
      double separation = PtoC.norm(); //magnitude of vector from PM pos to candidate PM pos
      double exceededDist = 2.0*thickness - separation;
      if (exceededDist > 0){ //too close, add contribution for this candidate PM to correction vector
        Vector3D CtoPdir = pm.position - candidatePM->position;
        CtoPdir.normalize(); //just need direction
        avg_correction += (exceededDist * CtoPdir);
        num_corrections++;
      }
    }
    avg_correction /= (double)num_corrections; //to get average

    if (num_corrections > 0){ //there are some corrections to be made (particles self colliding)
      pm.position += avg_correction / simulation_steps; //apply avg correction scaled down by simulation steps
    }
  }
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents
  // membership in some uniquely identified 3D box volume.
  float h = 3 * height/num_height_points; //3 empirically proven constant for spatial hashing accuracy
  float w = 3 * width/num_width_points;
  float t = max(h, w);
  //dimensions of each 3d box w*h*t

  //truncating pos to closest 3d box
  float newx = (pos.x-fmod(pos.x, w))/w; //truncate x coordinate with remainder of pos.x/w to get to closest 3Dbox coords and divide over w (xrange dimension of box)
  float newy = (pos.y-fmod(pos.y, h))/h;
  float newz = (pos.z-fmod(pos.z, t))/t;

  //float unique_key = newx + (newy * h) + (newz * w * h); // for an order sensitive, unique hash
  float unique_key = newx + (newy * h) + (newz * w * h); // for an order sensitive, unique hash
  return unique_key;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm, pm + num_width_points, pm + 1));
      triangles.push_back(new Triangle(pm + 1, pm + num_width_points,
                                       pm + num_width_points + 1));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
