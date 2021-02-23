#include "student_code.h"
#include "mutablePriorityQueue.h"

using namespace std;

namespace CGL
{
  
// Perform one step of the Bezier curve's evaluation at t using de Casteljau's algorithm for subdivision.
// Store all of the intermediate control points into the 2D vector evaluatedLevels.

    void BezierCurve::evaluateStep()
  {
  	//Part 1 
    if(evaluatedLevels[evaluatedLevels.size()-1].size() == 1)return;

    //Part2
    std::vector<Vector2D> second_last = evaluatedLevels[evaluatedLevels.size()-1];
    evaluatedLevels.push_back(std::vector<Vector2D>());
    
    for(int i=0;i<second_last.size()-1;i++)
      evaluatedLevels[evaluatedLevels.size()-1].push_back( (t*second_last[i])  + (1-t)*second_last[i+1]);

  
  }



// Evaluate the Bezier surface at parameters (u, v) through 2D de Casteljau subdivision.
// (i.e. Unlike  evaluateStep() where we performed one subdivision level per call to evaluateStep, this function
// should apply de Casteljau's algorithm until it computes the final, evaluated point on the surface)

  Vector3D BezierPatch::evaluate(double u, double v) const
  {
  	// Part 4
    vector<Vector3D> curve_points;
    for(int i=0;i<controlPoints.size();i++)
    curve_points.push_back(evaluate1D(controlPoints[i],u));   //pushing 1 curve point


  	return evaluate1D(curve_points,v);
  }


  // Given an array of 4 points that lie on a single curve, evaluates the Bezier curve at parameter t using 1D de Casteljau subdivision.
  Vector3D BezierPatch::evaluate1D(std::vector<Vector3D> points, double t) const
  {
    
    //Part 3
    vector<vector< Vector3D> > levels; // Vector for all levels in the curve
    levels.push_back(points);          // Pushing the first level into `levels`

    while(levels[levels.size()-1].size() != 1)
    { 
      
      vector<Vector3D> n_level; 
      for(int i=0;i<levels[levels.size()-1].size()-1;i++)
        n_level.push_back( (t*levels[levels.size()-1][i]) + (1-t)*levels[levels.size()-1][i+1] );
      
      levels.push_back(n_level);

    }
    //single point
    return levels[levels.size()-1][0];
    
  }



  Vector3D Vertex::normal( void ) const
  {

    return Vector3D();
  }

  EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
  {
    if(e0->isBoundary()) return e0;

    //triangle 1
    HalfedgeIter h11 = e0->halfedge();
    HalfedgeIter h12 = h11->next();
    HalfedgeIter h13 = h12->next();
    VertexIter b = h11->vertex();
    VertexIter a = h13->vertex();
    FaceIter f1 = h11->face();

    //triangle 2
    HalfedgeIter h21 = h11->twin();
    HalfedgeIter h22 = h21->next();
    HalfedgeIter h23 = h22->next();
    VertexIter c = h21->vertex();
    VertexIter d = h23->vertex();
    FaceIter f2 = h21->face();

    //new half edges
    h11->edge() = e0;
    h11->next() = h23;
    h23->next() = h12;
    h12->next() = h11;

    h11->twin() = h21;

    h21->edge() = e0;
    h21->next() = h13;
    h13->next() = h22;
    h22->next() = h21;

    //new vertices
    h11->vertex() = a;
    h12->vertex() = c;
    h13->vertex() = a;
    h21->vertex() = d;
    h22->vertex() = b;
    h23->vertex() = d;

    //new faces
    h11->face() = f1;
    h12->face() = f1;
    h13->face() = f2;
    h21->face() = f2;
    h22->face() = f2;
    h23->face() = f1;

    //face half edges
    f1->halfedge() = h11;
    f2->halfedge() = h21;

    return e0;
  }

  VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
  {
    if (e0->halfedge()->isBoundary()){
        return e0->halfedge()->vertex();
    }


    //triangle 1
    HalfedgeIter h01 = e0->halfedge();
    HalfedgeIter h1 = h01->next();
    HalfedgeIter h2 = h1->next();
    VertexIter b = h01->vertex();
    VertexIter a = h2->vertex();
    FaceIter f1 = h01->face();


    //triangle 2
    HalfedgeIter h02 = h01->twin();
    HalfedgeIter h3 = h02->next();
    HalfedgeIter h4 = h3->next();
    VertexIter c = h02->vertex();
    VertexIter d = h4->vertex();
    FaceIter f2 = h02->face();

    //new things
    HalfedgeIter h11 = newHalfedge();
    HalfedgeIter h21 = newHalfedge();
    HalfedgeIter h22 = newHalfedge();
    HalfedgeIter h31 = newHalfedge();
    HalfedgeIter h32 = newHalfedge();
    HalfedgeIter h41 = newHalfedge();

    EdgeIter e1 = newEdge();
    EdgeIter e2 = newEdge();
    EdgeIter e3 = newEdge();

    FaceIter new_f1 = newFace();
    FaceIter new_f2 = newFace();

    VertexIter new_v = newVertex();

    //set all new half edges
    h1->setNeighbors(h21, h1->twin(), c, h1->edge(), new_f1);
    h2->setNeighbors(h01, h2->twin(), a, h2->edge(), f1);
    h3->setNeighbors(h41, h3->twin(), b, h3->edge(), new_f2);
    h4->setNeighbors(h02, h4->twin(), d, h4->edge(), f2);

    h01->setNeighbors(h22, h31, b, e0, f1);
    h02->setNeighbors(h32, h11, c, e1, f2);
    h11->setNeighbors(h1, h02, new_v, e1, new_f1);
    h21->setNeighbors(h11, h22, a, e2, new_f1);
    h22->setNeighbors(h2, h21, new_v, e2, f1);
    h31->setNeighbors(h3, h01, new_v, e0, new_f2);
    h32->setNeighbors(h4, h41, new_v, e3, f2);
    h41->setNeighbors(h31, h32, d, e3, new_f2);

    //set new vertex
    new_v->position = (b->position + c->position)/2.0;
    new_v->halfedge() = h11;

    //set all edges
    e1->halfedge() = h11;
    e2->halfedge() = h21;
    e3->halfedge() = h32;

    //set all faces
    f1->halfedge() = h01;
    f2->halfedge() = h02;
    new_f1->halfedge() = h11;
    new_f2->halfedge() = h31;


    e0->isNew = false;
    e1->isNew = false;
    e2->isNew = true;
    e3->isNew = true;
    new_v->isNew = true;


    return new_v;
  }



  void MeshResampler::upsample( HalfedgeMesh& mesh )
  {

  }

}
