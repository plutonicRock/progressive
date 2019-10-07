#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>

using namespace cv;

void PrintMatrixSize(std::string name, Mat matrix){
  std::cout<<name<<"\t"<< matrix.size().height<< " " << matrix.size().width << std::endl;
};

void PrintMatrix(std::string name, cv::Mat matrix){
  std::cout << name << "\t"<< matrix.rows << "\t" << matrix.cols << std::endl;
  for (int i = 0; i< matrix.rows; i++){
    const double* matrix_i = matrix.ptr<double>(i);
    for(int j = 0; j< matrix.cols; j++){
      std::cout << matrix_i[j] << std::endl;
    }
  }
};

int main(int argc, char** argv )
{
  VideoCapture inputvideo(0);
  inputvideo.set(CV_CAP_PROP_FRAME_WIDTH,1920);
  inputvideo.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
  if (!inputvideo.isOpened()){
    std::cout  << "Could not open reference " << 0 << std::endl;
    return -1;
  }

  std::string window_name = "This is a window";
  int delay = 1;
  int frameNum = -1;  
  
  Mat oneFrame;

  // Screen text data
  Point org;
  org.x = 0;
  org.y = 40;
  std::string screen_text = "Testing text rendering";
  // Kind of greenish
  Scalar colorScalar = cv::Scalar( 94, 206, 165 );

  // Create window
  namedWindow(window_name.c_str(), WINDOW_AUTOSIZE);
  moveWindow(window_name.c_str(), 400, 0);

  cv::Mat undistMap1, undistMap2;

  // Read in file data
  std::string camera_param_file = "../camParams.xml";
  FileStorage fs(camera_param_file, FileStorage::READ);
  if (!fs.isOpened()){
    std::cout<< "Failed to open" << std::endl;
  }
  
  cv::Size image_size;
  Mat cameraMatrix = Mat_<double>::zeros(3,3);
  Mat dist_coeffs = Mat_<double>::zeros(1,5);
  
  fs["cameraResolution"] >> image_size;  
  fs["cameraMatrix"] >> cameraMatrix;
  fs["dist_coeffs"] >> dist_coeffs;  

  // Generate undistortion maps
  cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), cameraMatrix, image_size, CV_16SC2, undistMap1, undistMap2 );

  for (;;){
    inputvideo >> oneFrame;
    ++ frameNum;
    
    putText(oneFrame, screen_text, org, FONT_HERSHEY_SIMPLEX, 1.0, colorScalar);

    // Undistort
    cv::Mat oneFrame_noDistort;    
    cv::remap(oneFrame, oneFrame_noDistort, undistMap1, undistMap2, cv::INTER_LINEAR);

    auto mArucoDictionary = cv::aruco::getPredefinedDictionary(
                    cv::aruco::PREDEFINED_DICTIONARY_NAME(4));
    std::vector<std::vector<cv::Point2f> > corners, rejected;
    std::vector<int> ids;
    cv::aruco::detectMarkers(oneFrame_noDistort, mArucoDictionary, corners, ids, cv::aruco::DetectorParameters::create(), rejected);
    std::cout<<"ID size:" << ids.size() << std::endl;
    
    // Show image on screen
    imshow(window_name.c_str(), oneFrame_noDistort);

    char c = (char)waitKey(delay);
    if ( c==27 ) break;
  }
  return 0;
}