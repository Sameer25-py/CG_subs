#include <opencv2/highgui.hpp>
#include "filters.cpp"

using namespace cv;


//Part 1: 
Mat morphing(Mat inp1, Mat inp2, float w){

  return (w*inp1) + ((1-w)*inp2);

}

//Part 4:
Mat filtered_morphing(Mat inp1, Mat inp2, float w){

  //blue light image
  inp1 = colored_light(inp1,0);
  
  //red light image
  inp2 = colored_light(inp2,2);

  //morphing
  
  return morphing(inp1,inp2,w);

}


