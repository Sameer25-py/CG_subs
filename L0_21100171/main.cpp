#include <opencv2/highgui.hpp>
#include <iostream>

#include "morphing.cpp"

using namespace cv;
using namespace std;

//////////////////////////
float image_width = 315.0;
//////////////////////////

float x_pix;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

    x_pix;
    if ( event == EVENT_MOUSEMOVE )
    {
        x_pix = x/image_width;
        
    }

          
}

int main( int argc, char** argv) {

  Mat left = imread("img/left.jpg" , cv::IMREAD_COLOR);
  
  Mat right = imread("img/right.jpg" , cv::IMREAD_COLOR);

  Mat amb = imread("img/amb.jpg" , cv::IMREAD_COLOR);

  Mat both = imread("img/both.jpg" , cv::IMREAD_COLOR);
  
  if(! left.data || ! right.data) {
      cout <<  "Could not open or find the image" << endl ;
      return -1;
    }

  float w = 0;

  left = left - amb;
  right = right - amb;
  both = both - amb;  

  namedWindow( "Display window", WINDOW_AUTOSIZE ); 

  if ((**(argv+1) == '1')){

    while(true){
      Mat out = morphing(left,right,w);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
      w = x_pix;
    }  

  }  
  else if(**(argv+1) == '2'){
    while(true){
    both = imread("img/both.jpg" , cv::IMREAD_COLOR);      
    both = both - amb;
    if(x_pix < 0.3){
    
      Mat out = color_enhancement_filter(both,0);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    else if(x_pix < 0.6){
      
      
      Mat out = color_enhancement_filter(both,1);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    else{
          
      Mat out = color_enhancement_filter(both,2);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    }
  }
  else if(**(argv+1) == '3'){
    while(true){
    both = imread("img/both.jpg" , cv::IMREAD_COLOR);      
    both = both - amb;
    if(x_pix < 0.3){
    
      Mat out = colored_light(both,0);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    else if(x_pix < 0.6){
      
      
      Mat out = colored_light(both,1);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    else{
          
      Mat out = colored_light(both,2);
      imshow( "Display window", out );
      setMouseCallback("Display window", CallBackFunc, NULL);
      waitKey(1); 
    
    }
    }
  }

  else if(**(argv+1) == '4'){
    while(true){
    Mat out =  filtered_morphing(left,right,w);
    imshow( "Display window", out );
    setMouseCallback("Display window", CallBackFunc, NULL);
    waitKey(1); 
    w = x_pix;
    }  
  }
  else{
    cout<<"INVALID ARGUMENT";
  }




  return 0;


  }




