#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
  VideoCapture inputvideo(0);
  if (!inputvideo.isOpened()){
    std::cout  << "Could not open reference " << 0 << std::endl;
    return -1;
  }

  std::string window_name = "This is a window";

  namedWindow(window_name.c_str(), WINDOW_AUTOSIZE);

    // if ( argc != 2 )
    // {
    //     printf("usage: DisplayImage.out <Image_Path>\n");
    //     return -1;
    // }

    // Mat image;
    // image = imread( argv[1], 1 );

    // if ( !image.data )
    // {
    //     printf("No image data \n");
    //     return -1;
    // }
    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    // imshow("Display Image", image);

    // waitKey(0);

    // return 0;
}