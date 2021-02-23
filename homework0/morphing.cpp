#include <opencv2/highgui.hpp>
#include "filters.cpp"

using namespace cv;
using namespace std;

//Assignment Part 2
Mat morphing4(Mat inp1, Mat inp2, Mat inp3, Mat inp4, float wh, float wv){
  
  cout << "Wh" << wh <<endl;
  cout << "wv" << wv <<endl;
  Mat horizontal = Mat(0,0,0);
  Mat vertical = Mat(0,0,0);
  
  
  if(wv >= 0.5){
    //right side
    horizontal = ((wv*inp1) + ((1-(wv))*inp2));
  }
  if(wv < 0.5){
    //left side
    horizontal = ((1-wv)*inp2) + ((wv)*inp1);
  }
  if(wh >= 0.5){
    //right side
    vertical = ((wh*inp3) + ((1-(wh))*inp4));
  }
  if(wh < 0.5){
    //left side
    vertical = (1-wh)*inp4 + (wh*inp3);
  }



  return (horizontal + vertical)/2;

}
// Assignment Part 3

Mat get_blue(Mat inp){
  for(int i=0;i<250;i++){
    for(int j=0;j<250;j++){
      Vec3b pixel = inp.at<Vec3b>(Point(i,j));
      pixel[1]=0;
      pixel[2]=0;

      inp.at<Vec3b>(Point(i,j)) = pixel;
    }
  }
  return inp;
}

Mat get_red(Mat inp){

  for(int i=0;i<250;i++){
    for(int j=0;j<250;j++){
      Vec3b pixel = inp.at<Vec3b>(Point(i,j));
      pixel[0]=0;
      pixel[1]=0;

      inp.at<Vec3b>(Point(i,j)) = pixel;
    }
  }
  return inp;
}

Mat get_green(Mat inp){
  for(int i=0;i<250;i++){
    for(int j=0;j<250;j++){
      Vec3b pixel = inp.at<Vec3b>(Point(i,j));
      pixel[0]=0;
      pixel[2]=0;

      inp.at<Vec3b>(Point(i,j)) = pixel;
    }
  }
  return inp;
}

Mat colored_morphing4(Mat inp1, Mat inp2, Mat inp3, Mat inp4, float wh, float wv){
  
 cout << "Wh" << wh <<endl;
 cout << "wv" << wv <<endl;

  Mat horizontal = Mat(0,0,0);
  Mat vertical = Mat(0,0,0);
  Mat horizontal1 = Mat(0,0,0);
  Mat vertical1 = Mat(0,0,0);
  Mat red = get_red(inp1);
  Mat blue = get_blue(inp2);
  Mat green = get_green(inp3);
    
  Mat horizon_trans1 = wv*(inp4) + (1-wv)*red;
  Mat horizon_trans2 = wv*(blue) + (1-wv)*green;
  
  


  return wh*(horizon_trans1) + (1-wh)*horizon_trans2;
}

