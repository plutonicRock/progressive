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
  int delay = 1;
  int frameNum = -1;
  // Kind of greenish
  Scalar colorScalar = cv::Scalar( 94, 206, 165 );
  Mat oneFrame;
  Point org;
  org.x = 0;
  org.y = 40;


  namedWindow(window_name.c_str(), WINDOW_AUTOSIZE);
  moveWindow(window_name.c_str(), 400, 0);
  
  

  for (;;){
    inputvideo >> oneFrame;
    ++ frameNum;
    
    putText(oneFrame, "Testing text rendering", org, FONT_HERSHEY_SIMPLEX, 1.0, colorScalar);
    imshow(window_name.c_str(), oneFrame);

    char c = (char)waitKey(delay);
    if ( c==27 ) break;
  }
  return 0;
}