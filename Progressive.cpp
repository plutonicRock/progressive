#include <stdio.h>
#include <opencv2/opencv.hpp>

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

 /// Global variables
 Mat src;
 Mat map_x, map_y;
//  char* remap_window = "Remap demo";
 int ind = 0;

void update_map( void )
{
  ind = ind%4;

  for( int j = 0; j < src.rows; j++ )
  { for( int i = 0; i < src.cols; i++ )
      {
        switch( ind )
        {
          case 0:
            if( i > src.cols*0.25 && i < src.cols*0.75 && j > src.rows*0.25 && j < src.rows*0.75 )
              {
                map_x.at<float>(j,i) = 2*( i - src.cols*0.25 ) + 0.5 ;
                map_y.at<float>(j,i) = 2*( j - src.rows*0.25 ) + 0.5 ;
              }
            else
              { map_x.at<float>(j,i) = 0 ;
                map_y.at<float>(j,i) = 0 ;
              }
                break;
          case 1:
                map_x.at<float>(j,i) = i ;
                map_y.at<float>(j,i) = src.rows - j ;
                break;
          case 2:
                map_x.at<float>(j,i) = src.cols - i ;
                map_y.at<float>(j,i) = j ;
                break;
          case 3:
                map_x.at<float>(j,i) = src.cols - i ;
                map_y.at<float>(j,i) = src.rows - j ;
                break;
        } // end of switch
      }
  }
ind++;
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
  // Kind of greenish
  Scalar colorScalar = cv::Scalar( 94, 206, 165 );
  Mat oneFrame;
  Point org;
  org.x = 0;
  org.y = 40;

  inputvideo >> oneFrame;
  map_x.create( oneFrame.size(), CV_32FC1 );
  map_y.create( oneFrame.size(), CV_32FC1 );



  namedWindow(window_name.c_str(), WINDOW_AUTOSIZE);
  moveWindow(window_name.c_str(), 400, 0);

  cv::Mat undistMap1, undistMap2;
  undistMap1.create( oneFrame.size(), CV_16SC2 );
  undistMap2.create( oneFrame.size(), CV_16SC2 );

  // Read in file data
  std::string camera_param_file = "camParams.xml";
  FileStorage fs(camera_param_file, FileStorage::READ);
  
  cv::Size image_size;
  Mat cameraMatrix = Mat_<double>::zeros(3,3);
  Mat dist_coeffs = Mat_<double>::zeros(1,5);
  fs["cameraResolution"] >> image_size;
  cv::Size small_image_size(1920,1080);
  
  // small_image_size.height(480);
  // small_image_size.width(640);
  PrintMatrix("cameraMatrix", cameraMatrix);
  fs["cameraMatrix"] >> cameraMatrix;
  // fs << "cameraMatrix" << cameraMatrix;
  PrintMatrix("cameraMatrix", cameraMatrix);
  fs["dist_coeffs"] >> dist_coeffs;
  

  return 0;
  

  // std::cout<<dist_coeffs[0]<<std::endl;

  

      // cv::initUndistortRectifyMap(mCalibData->cameraMatrix, mCalibData->distCoeffs, cv::noArray(),
                                // cv::getOptimalNewCameraMatrix(mCalibData->cameraMatrix, mCalibData->distCoeffs, mCalibData->imageSize, 0.0, mCalibData->imageSize),
                                // mCalibData->imageSize, CV_16SC2, mCalibData->undistMap1, mCalibData->undistMap2);

  // cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), 
  // cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, image_size, 0.0, image_size), 
  // image_size, CV_16SC2, undistMap1, undistMap2 );

    cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), cv::getOptimalNewCameraMatrix(cameraMatrix, dist_coeffs, small_image_size, 0.0, small_image_size), small_image_size, CV_16SC2, undistMap1, undistMap2 );
//cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, small_image_size, 0.0, small_image_size)
  for (;;){
    inputvideo >> oneFrame;
    ++ frameNum;
    // ((map1.type() == CV_32FC2 || map1.type() == CV_16SC2) && map2.empty()) 
    // || 
    // (map1.type() == CV_32FC1 && map2.type() == CV_32FC1)

    
    putText(oneFrame, "Testing text rendering", org, FONT_HERSHEY_SIMPLEX, 1.0, colorScalar);

    // Undistort
    cv::Mat oneFrame_noDistort;
    
    src = oneFrame;
    update_map();
    cv::remap(oneFrame, oneFrame_noDistort, undistMap1, undistMap2, cv::INTER_LINEAR);
    //cv::remap(oneFrame, oneFrame_noDistort, map_x, map_y, cv::INTER_LINEAR);//, cv::BORDER_CONSTANT, Scalar(0,0, 0) );
    PrintMatrixSize("original frame", oneFrame);
    PrintMatrixSize("undistort matrix 1", undistMap1);
    PrintMatrixSize("undistort matrix 2", undistMap2);
    PrintMatrixSize("NoDistort", oneFrame_noDistort);
    // imshow(window_name.c_str(), oneFrame_noDistort);
    imshow(window_name.c_str(), oneFrame_noDistort);

    char c = (char)waitKey(delay);
    if ( c==27 ) break;
  }
  return 0;
}