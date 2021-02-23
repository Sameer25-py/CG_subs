#include <opencv2/highgui.hpp>
#include <iostream>

#include "morphing.cpp"

using namespace cv;
using namespace std;


// SPECIFY IMAGE SIZE HERE
//////////////////////////
float image_width = 250; //250
float image_height = 250;//250
//////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


float x_pix;
float y_pix;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    x_pix;
    y_pix;
    if ( event == EVENT_MOUSEMOVE )
    {
        x_pix = x/image_width;
        y_pix = y/image_height;
        
    }

          
}

int main( int argc, char** argv) {

  Mat im1 = imread("img/right.jpg" , cv::IMREAD_COLOR);
  
  Mat im2 = imread("img/left.jpg" , cv::IMREAD_COLOR);

  Mat im3 = imread("img/top.jpg" , cv::IMREAD_COLOR);
  
  Mat im4 = imread("img/bot.jpg" , cv::IMREAD_COLOR);

  
  if(! im1.data || ! im2.data) {
      cout <<  "Could not open or find the image" << endl ;
      return -1;
    }

  float wh = 0;
  float wv = 0;


  namedWindow( "Display window", WINDOW_AUTOSIZE ); 

  if ((**(argv+1) == '0')){

    while(true){
      Mat out = morphing4(im1,im2,im3,im4,wv,wh);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
      wh = x_pix;
      wv = y_pix;
    }  
 
  }
  else if ((**(argv+1) == '1')){

    while(true){
      Mat out = colored_morphing4(im1,im2,im3,im4,wv,wh);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
      wh = x_pix;
      wv = y_pix;
    }  
 
  }  
  else{
    cout<<"INVALID ARGUMENT";
  }




  return 0;


  }




