#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c);
  } else {
    draw(node->l, c);
    draw(node->r, c);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c);
  } else {
    drawOutline(node->l, c);
    drawOutline(node->r, c);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
BBox centroid_box, bbox;
  
  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c);
  }

  BVHNode *node = new BVHNode(bbox);
  
  //Part1
  if (prims.size() <= max_leaf_size){
     node->prims = new vector<Primitive *>(prims);
     return node;
  }
  //Part2
  else{
    int b_axis;
    double max_axis = max(centroid_box.extent.x,max(centroid_box.extent.y,centroid_box.extent.z));
    
    if(centroid_box.extent.x == max_axis)b_axis = 0;
    else if(centroid_box.extent.y == max_axis) b_axis = 1;
    else b_axis = 2;


    double mid_point = centroid_box.centroid()[b_axis]; //mid point along biggest axis
    
    //Part3
    vector<Primitive *> left,right; 

    for (Primitive *p : prims) {
        BBox bb = p->get_bbox();
        if (bb.centroid()[b_axis] <= mid_point) left.push_back(p);
        else right.push_back(p);
       }
      
      //Part4
      node->l = construct_bvh(left,max_leaf_size);
      node->r = construct_bvh(right,max_leaf_size);
  }
 
  return node;

}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {

  double t1, t2 ;
  
  if (!node -> bb.intersect(ray, t1, t2))return false;

  else if ((t1 < ray.min_t)||(t1 > ray.max_t) && ((t2< ray.min_t)||(t2 > ray.max_t))) return false;
  
  else if(node->isLeaf()) 
  {
    bool hit = false ;
    for(Primitive *p : *(node -> prims)) 
    {
      total_isects++ ;
      if (p->intersect(ray)) 
        hit= true ;
    }

    return hit;
  }
  else {
    bool left = intersect(ray, node->l);
    bool right = intersect(ray, node->r);
    if (left)return left;
    if (right)return right;
  }  
  
  return false;
}

bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {

  double t1, t2 ;
  
  if (!node -> bb.intersect(ray, t1, t2))
    return false ;
  
  else if ((t1 < ray.min_t) ||  (t1 > ray.max_t) && ((t2< ray.min_t) || (t2 > ray.max_t))) return false;
  
  else if (node->isLeaf()) 
  {
    bool hit = false;
    for(Primitive *p : *(node -> prims)) 
    {
      total_isects++;
      if (p->intersect(ray, i)) 
        hit= true;
    }
    return hit;
  }
  else {
    bool left = intersect(ray, i, node->l) ;
    bool right = intersect(ray, i, node->r) ;
    if (left)return left;
    else if (right)return right;
  }
  return false;
  
}

}  // namespace StaticScene
}  // namespace CGL
