#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

void printsize(Mat matrix){

};

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

  // Read in file data
  std::string camera_param_file = "camParams.xml";
  FileStorage fs(camera_param_file, FileStorage::READ);
  // 
  cv::Mat undistMap1, undistMap2;

  cv::Size image_size;
  Mat cameraMatrix = Mat_<double>::zeros(3,3);
  Mat dist_coeffs = Mat_<double>::zeros(1,5);
  fs << "cameraResolution" << image_size;
  fs << "cameraMatrix" << cameraMatrix;
  fs << "dist_coeffs" << dist_coeffs;

  

      // cv::initUndistortRectifyMap(mCalibData->cameraMatrix, mCalibData->distCoeffs, cv::noArray(),
                                // cv::getOptimalNewCameraMatrix(mCalibData->cameraMatrix, mCalibData->distCoeffs, mCalibData->imageSize, 0.0, mCalibData->imageSize),
                                // mCalibData->imageSize, CV_16SC2, mCalibData->undistMap1, mCalibData->undistMap2);

  // cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), 
  // cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, image_size, 0.0, image_size), 
  // image_size, CV_16SC2, undistMap1, undistMap2 );

    cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), cameraMatrix, image_size, CV_16SC2, undistMap1, undistMap2 );

  for (;;){
    inputvideo >> oneFrame;
    ++ frameNum;
    
    
    putText(oneFrame, "Testing text rendering", org, FONT_HERSHEY_SIMPLEX, 1.0, colorScalar);

    // Undistort
    cv::Mat oneFrame_noDistort;
    std::cout<< oneFrame.size().height<< " " << oneFrame.size().width << std::endl;
    cv::remap(oneFrame, oneFrame_noDistort, undistMap1, undistMap2, cv::INTER_LINEAR);
    std::cout<< oneFrame_noDistort.size().height<< " " << oneFrame_noDistort.size().width << std::endl;
    imshow(window_name.c_str(), oneFrame_noDistort);

    char c = (char)waitKey(delay);
    if ( c==27 ) break;
  }
  return 0;
}