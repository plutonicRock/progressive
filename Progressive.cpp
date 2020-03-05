#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco/charuco.hpp>

using namespace cv;

int GetIDIndex(std::vector<int> input, int id)
{
  for (int i = 0; i < input.size(); i++)
    if (input[i] == id)
      return i;
  return -1;
}

void PrintMatrixSize(std::string name, Mat matrix)
{
  std::cout << name << "\t" << matrix.size().height << " " << matrix.size().width << std::endl;
};

void PrintMatrix(std::string name, cv::Mat matrix)
{
  std::cout << name << "\t" << matrix.rows << "\t" << matrix.cols << std::endl;
  for (int i = 0; i < matrix.rows; i++)
  {
    const double *matrix_i = matrix.ptr<double>(i);
    for (int j = 0; j < matrix.cols; j++)
    {
      std::cout << matrix_i[j] << std::endl;
    }
  }
};

int main(int argc, char **argv)
{
  VideoCapture inputvideo(0);
  inputvideo.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
  inputvideo.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
  if (!inputvideo.isOpened())
  {
    std::cout << "Could not open reference " << 0 << std::endl;
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
  Scalar colorScalar = cv::Scalar(94, 206, 165);

  // Create window
  namedWindow(window_name.c_str(), WINDOW_AUTOSIZE);
  moveWindow(window_name.c_str(), 400, 0);

  cv::Mat undistMap1, undistMap2;

  // Read in file data
  std::string camera_param_file = "../camParams.xml";
  FileStorage fs(camera_param_file, FileStorage::READ);
  if (!fs.isOpened())
  {
    std::cout << "Failed to open" << std::endl;
  }

  cv::Size image_size;
  Mat cameraMatrix = Mat_<double>::zeros(3, 3);
  Mat dist_coeffs = Mat_<double>::zeros(1, 5);

  fs["cameraResolution"] >> image_size;
  fs["cameraMatrix"] >> cameraMatrix;
  fs["dist_coeffs"] >> dist_coeffs;

  // Generate undistortion maps
  cv::initUndistortRectifyMap(cameraMatrix, dist_coeffs, cv::noArray(), cameraMatrix, image_size, CV_16SC2, undistMap1, undistMap2);

  // Define charuco board
  auto mArucoDictionary = cv::aruco::getPredefinedDictionary(
      cv::aruco::PREDEFINED_DICTIONARY_NAME(4));
  const int squaresX = 5;
  const int squaresY = 5;
  const double squareLength = 0.0332;
  const double markerLength = 0.021;
  auto board = cv::aruco::CharucoBoard::create(5, 5, squareLength, markerLength, mArucoDictionary);

  for (;;)
  {
    inputvideo >> oneFrame;
    ++frameNum;

    // Write basic text
    putText(oneFrame, screen_text, org, FONT_HERSHEY_SIMPLEX, 1.0, colorScalar);

    // Undistort
    cv::Mat oneFrame_noDistort;
    cv::remap(oneFrame, oneFrame_noDistort, undistMap1, undistMap2, cv::INTER_LINEAR);

    // Extract marker corners and ids

    std::vector<std::vector<cv::Point2f>> corners, rejected;
    std::vector<int> ids;
    auto detector_params = cv::aruco::DetectorParameters::create();
    detector_params->cornerRefinementMethod = aruco::CORNER_REFINE_SUBPIX;
    cv::aruco::detectMarkers(oneFrame_noDistort, mArucoDictionary, corners, ids, detector_params, rejected);

    // Draw marker corners and IDs
    if (ids.size() > 0)
      cv::aruco::drawDetectedMarkers(oneFrame_noDistort, corners, ids, cv::Scalar(255, 0, 0));

    // int square_id = 0;
    // int index = GetIDIndex(ids, square_id);
    // // std::vector<cv::Point2f> coors;
    // if (index >= 0)
    // {
    //   auto coors = corners.at(index);
    //   std::cout << "Coordinates for tile number: " << square_id << std::endl;
    //   for (auto coor : coors)
    //   {
    //     std::cout << coor << std::endl;
    //   }
    // }

    // std::cout << "ID size:" << ids.size() << std::endl;

    // Pose estimation
    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, dist_coeffs, rvecs, tvecs);

    // Pose estimation using board
    static cv::Vec3d rvecs_board, tvecs_board;
    std::vector<cv::Point2f> ch_corners;
    std::vector<int> ch_ids;
    if (ids.size() > 0)
    {
      cv::aruco::interpolateCornersCharuco(corners, ids, oneFrame_noDistort, board, ch_corners, ch_ids, cameraMatrix, dist_coeffs);

      if (ch_ids.size() > 0)
      {
        cv::aruco::drawDetectedCornersCharuco(oneFrame_noDistort, ch_corners, ch_ids);
        cv::aruco::estimatePoseCharucoBoard(ch_corners, ch_ids, board, cameraMatrix, dist_coeffs, rvecs_board, tvecs_board);
        std::cout<<"Distance: "<< cv::norm(tvecs_board) << std::endl;
      }
    }

    // Draw Axis on image
    double axis_size = 0.03;
    // Draw charuco board axis
    cv::aruco::drawAxis(oneFrame_noDistort, cameraMatrix, dist_coeffs, rvecs_board, tvecs_board, axis_size * 5);
    // Draw individual marker axes
    for (int i = 0; i < rvecs.size(); i++)
    {
      cv::aruco::drawAxis(oneFrame_noDistort, cameraMatrix, dist_coeffs, rvecs[i], tvecs[i], axis_size);
    }

    // Calculate average of norm of all vectors
    double sum = 0;
    double total = 0;
    for (auto tvec : tvecs)
    {
      sum += cv::norm(tvec);
      total++;
    }
    // std::cout << "Distance: " << ((bool)total ? sum / total : 0) << "m" << std::endl;

    // Show image on screen
    imshow(window_name.c_str(), oneFrame_noDistort);

    char c = (char)waitKey(delay);
    if (c == 27)
      break;
  }
  return 0;
}