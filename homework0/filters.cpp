#include <opencv2/highgui.hpp>

using namespace cv;

Mat colored_light(Mat image, int color){

  int x_max = image.cols;
  int y_max = image.rows;

  for (int x=0; x<x_max; x++){

        for (int y=0; y<y_max; y++){

        // get a pixel
        Vec3b pixel = image.at<Vec3b>(Point(x,y));

        if(color == 0){

          pixel[1] = 0;
          pixel[2] = 0;

        }
        else if(color == 1){

          pixel[0] = 0;
          pixel[2] = 0;

        }
        else if(color == 2){

          pixel[0] = 0;
          pixel[1] = 0;

        }

        // set a pixel back to the image
        image.at<Vec3b>(Point(x,y)) = pixel;
  
    }
  }

  return image;


}