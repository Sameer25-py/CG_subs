#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

//Part2:
Mat color_enhancement_filter(Mat image, int color){

  //Setting up the loop bounds 
  int x_max = image.cols;
  int y_max = image.rows;

  //looping over the entire image
  for (int x=0; x<x_max; x++){
        for (int y=0; y<y_max; y++){

        //Retrieve a single pixel from the image
        Vec3b pixel = image.at<Vec3b>(Point(x,y));
        //perform the required operation here
        /////////////////////////////////////
        //pixel[color] = pixel[color] + ((255 - pixel[color])/4); 

        /////////////////////////////////////
        //bonus
        if(color == 1){
          pixel[1] += ((255 - pixel[1])/4);
          pixel[2] += ((255 - pixel[2])/4);
        }
        else if(color == 0){
          pixel[0] += ((255 - pixel[0])/4);
          pixel[1] += ((255 - pixel[1])/4);
        }
        else{
          pixel[0] += ((255 - pixel[0])/4);
          pixel[2] += ((255 - pixel[2])/4);
        }
        //Save the pixel back into the image
        image.at<Vec3b>(Point(x,y)) = pixel;
  
    }
  }

  return image;

}

//Part3:
Mat colored_light(Mat image, int color){

  int x_max = image.cols;
  int y_max = image.rows;

  //looping over the entire image
  for (int x=0; x<x_max; x++){
        for (int y=0; y<y_max; y++){

        //Retrieve a single pixel from the image
        Vec3b pixel = image.at<Vec3b>(Point(x,y));
        //perform the required operation here
        /////////////////////////////////////
        if(color == 0){
          pixel[1] = 0;
          pixel[2] = 0;
        }
        else if (color == 1){
          pixel[0] = 0;
          pixel[2] = 0;
        }
        else{
          pixel[0] = 0;
          pixel[1] = 0;
        }
        image.at<Vec3b>(Point(x,y)) = pixel;
  
    }
  }
  // //bonus task
  // for (int x=0; x<x_max; x++){
  //       for (int y=0; y<y_max; y++){

  //       //Retrieve a single pixel from the image
  //       Vec3b pixel = image.at<Vec3b>(Point(x,y));
  //       //perform the required operation here
  //       /////////////////////////////////////
  //       if(color == 0){
  //         pixel[1] = 0;
  //       }
  //       else if (color == 1){
  //         pixel[2] = 0;
  //       }
  //       else{
  //         pixel[0] = 0;
  //       }
  //       image.at<Vec3b>(Point(x,y)) = pixel;
  
  //   }
  // }



  return image;


}