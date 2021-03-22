#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "ros/ros.h"

#include <visual_odometry/nanoflann.hpp>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
// #include <opencv2/core.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/highgui/highgui.hpp>

using std::cout;
using std::endl;

const std::string file_path_prefix = "data/2011_09_26/";
const std::string calib_cam_to_cam_file_path = file_path_prefix + "calib_cam_to_cam.txt";
const std::string calib_velo_to_cam_file_path = file_path_prefix + "calib_velo_to_cam.txt";
const std::string bin_file_path = file_path_prefix + "2011_09_26_drive_0001_sync/velodyne_points/data/0000000000.bin";

void loadTransformations(Eigen::Matrix4f& T_velo_cam, Eigen::Matrix4f& T_cam_rect0, Eigen::MatrixXf& P_rect0) {
    std::string line;
    std::string delim = " ";
    float value;
    std::string curr;

    std::fstream input1(calib_velo_to_cam_file_path.c_str(), std::ios::in);
    if(!input1.good()){
        std::cerr << "Could not read file: " << calib_velo_to_cam_file_path << endl;
        exit(EXIT_FAILURE);
    }

    while(getline(input1, line)){ //read data from file object and put it into string.
        // std::cout << line << "\n"; //print the data of the string
        int start = 0;
        int end = line.find(delim);
        curr = line.substr(start, end - start);
        // std::cout << curr << ", "; 
        if (curr == "R:") {
            int index = 0;
            while (end != std::string::npos) {
                start = end + delim.length();
                end = line.find(delim, start);
                curr = line.substr(start, end - start);
                value = boost::lexical_cast<float>(line.substr(start, end - start));
                // std::cout << value << ", "; 
                T_velo_cam(index/3, index%3) = value;
                ++index;
            }
        }
        if (curr == "T:") {
            int index = 0;
            while (end != std::string::npos) {
                start = end + delim.length();
                end = line.find(delim, start);
                curr = line.substr(start, end - start);
                value = boost::lexical_cast<float>(line.substr(start, end - start));
                // std::cout << value << ", "; 
                T_velo_cam(index, 3) = value;
                ++index;
            }
        }
    }
    T_velo_cam(3,3) = 1;
    std::cout << "T_velo_cam = \n" << T_velo_cam << "\n" << std::endl;

    std::fstream input2(calib_cam_to_cam_file_path.c_str(), std::ios::in);
    if(!input2.good()){
        std::cerr << "Could not read file: " << calib_cam_to_cam_file_path << endl;
        exit(EXIT_FAILURE);
    }

    while(getline(input2, line)){ //read data from file object and put it into string.
        // std::cout << line << "\n"; //print the data of the string
        int start = 0;
        int end = line.find(delim);
        curr = line.substr(start, end - start);
        // std::cout << curr << ", "; 
        if (curr == "R_rect_00:") {
            int index = 0;
            while (end != std::string::npos) {
                start = end + delim.length();
                end = line.find(delim, start);
                curr = line.substr(start, end - start);
                value = boost::lexical_cast<float>(line.substr(start, end - start));
                // std::cout << value << ", "; 
                T_cam_rect0(index/3, index%3) = value;
                ++index;
            }
            T_cam_rect0(3,3) = 1;
        }
        if (curr == "P_rect_00:") {
            int index = 0;
            while (end != std::string::npos) {
                start = end + delim.length();
                end = line.find(delim, start);
                curr = line.substr(start, end - start);
                value = boost::lexical_cast<float>(line.substr(start, end - start));
                // std::cout << value << ", "; 
                P_rect0(index/4, index%4) = value;
                ++index;
            }
        }
    }

    std::cout << "T_cam_rect0 = \n" << T_cam_rect0 << "\n" << std::endl;
    std::cout << "P_rect0 = \n" << P_rect0 << "\n" << std::endl;
    
    input1.close(); 
    input2.close(); 
}

void loadPointCloud(Eigen::MatrixXf& point_cloud_3d_tilde) {
// void loadPointCloud(std::vector<Eigen::Vector4f>& point_cloud_3d_tilde) {
    // nanoflann supports std::vector and Eigen matrix
    // One official KITTI repo has sample code to read point cloud to pcd: https://github.com/yanii/kitti-pcl/blob/master/src/kitti2pcd.cpp
    // OpenCV also has a projection function: http://docs.ros.org/en/hydro/api/image_geometry/html/c++/classimage__geometry_1_1PinholeCameraModel.html#a30b3761aadfa4b91c7fedb97442c2f13

	// std::fstream input(bin_file_path.c_str(), std::ios::in | std::ios::binary);
	// if(!input.good()){
	// 	std::cerr << "Could not read file: " << bin_file_path << endl;
	// 	exit(EXIT_FAILURE);
	// }

    // // input.seekg (0, input.end);
    // // int length = input.tellg();
    // // input.seekg (0, input.beg);
 
    // // std::cout << length << std::endl; // length \approx point num * 16, eg. 1936240/121016 \approx 15.9998677861

	// input.seekg(0, std::ios::beg);
 
	// // Eigen::MatrixXd point_cloud_3d;
 
	// int i;
    // float x, y, z, intensity;
    
    // Eigen::Vector4f point_3d_tilde;
    // point_3d_tilde(3) = 1; // homogeneous coordinate
	// for (i=0; input.good() && !input.eof(); ++i) {
	// 	input.read((char *) &x, sizeof(float));
	// 	input.read((char *) &y, sizeof(float));
	// 	input.read((char *) &z, sizeof(float));
	// 	input.read((char *) &intensity, sizeof(float)); // TODO: check what to do with intensity. Thresholding?
		
    //     point_3d_tilde(0) = x;
    //     point_3d_tilde(1) = y;
    //     point_3d_tilde(2) = z;

    //     point_cloud_3d_tilde.push_back(point_3d_tilde);

    //     // if (i%100 == 0) {
    //     //     std::cout << i << "-th point is (" << x << ", " << y << ", " << z << ")" << std::endl;
    //     // }
	// }
    // // std::cout << "total num of point is " << point_cloud_3d_tilde.size() << std::endl;
	// input.close();

    int32_t num = 1000000; // about 10 times larger than the real point cloud size
    float *data = (float*)malloc(num*sizeof(float));

    // pointers
    float *px = data+0;
    float *py = data+1;
    float *pz = data+2;
    float *pr = data+3;

    // load point cloud
    FILE *stream;
    stream = fopen(bin_file_path.c_str(), "rb");
    num = fread(data,sizeof(float), num, stream)/4;
    point_cloud_3d_tilde = Eigen::MatrixXf::Ones(num, 4);
    // std::cout << num << std::endl;
    for (int32_t i=0; i<num; i++) {
        point_cloud_3d_tilde(i, 0) = *px;
        point_cloud_3d_tilde(i, 1) = *py;
        point_cloud_3d_tilde(i, 2) = *pz;
        px+=4; py+=4; pz+=4; pr+=4;
    }

    fclose(stream);
    free(data);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "test_kitti_pointcloud");

    // nanoflann::KNNResultSet<float> resultSet(3);

    // Eigen::MatrixXd m;

    Eigen::Matrix4f T_velo_cam = Eigen::Matrix4f::Zero();
    Eigen::Matrix4f T_cam_rect0 = Eigen::Matrix4f::Zero();
    Eigen::MatrixXf P_rect0 = Eigen::MatrixXf::Zero(3, 4);
    loadTransformations(T_velo_cam, T_cam_rect0, P_rect0);

    Eigen::MatrixXf point_cloud_3d_tilde; // row size is dynamic, and will be decided when load the point cloud; column size is fixed as 4
    // std::vector<Eigen::Vector4f> point_cloud_3d_tilde; // TODO: check if Eigen::MatrixXd is better for projection multiplication and kdtree query. Check this: https://github.com/yanii/kitti-pcl/blob/3b4ebfd49912702781b7c5b1cf88a00a8974d944/KITTI_README.TXT#L69
    loadPointCloud(point_cloud_3d_tilde);
    std::cout << "total num of point is " << point_cloud_3d_tilde.rows() << "\n" << std::endl;
    std::cout << "first 5 rows are \n" << point_cloud_3d_tilde.topRows(5) << "\n" << std::endl;

    // Projection
    Eigen::MatrixXf point_cloud_3d = point_cloud_3d_tilde * T_velo_cam.transpose() * T_cam_rect0.transpose() * P_rect0.transpose(); // shape = point_cloud_3d_tilde.rows(), 3; in rect cam0 coordinate; last column is the depth

    // Screen out back points
    Eigen::VectorXi select_front = (point_cloud_3d.col(2).array() > 0.1).cast<int>(); // depth should not just be positive, but greater than a threshold
    Eigen::MatrixXf point_cloud_3d_front(select_front.sum(), 3);
    int i, j = 0;
    for (i = 0; i < point_cloud_3d.rows(); ++i) {
        if (select_front(i)) {
            point_cloud_3d_front.row(j) = point_cloud_3d.row(i);
            ++j;
        }
    }
    
    // From homogeneous to normal coordiante
    Eigen::MatrixXf point_cloud_2d = point_cloud_3d_front.leftCols(2).array().colwise() * Eigen::inverse(point_cloud_3d_front.col(2).array());

    std::cout << "min x = " << point_cloud_2d.col(0).minCoeff() << std::endl;
    std::cout << "max x = " << point_cloud_2d.col(0).maxCoeff() << std::endl;
    std::cout << "mean x = " << point_cloud_2d.col(0).mean() << std::endl;
    std::cout << "std x = " << std::sqrt(
        (point_cloud_2d.col(0).array() - point_cloud_2d.col(0).mean()).square().sum() / (point_cloud_2d.rows() - 1)
    ) << std::endl;

    std::cout << "\nmin y = " << point_cloud_2d.col(1).minCoeff() << std::endl;
    std::cout << "max y = " << point_cloud_2d.col(1).maxCoeff() << std::endl;
    std::cout << "mean y = " << point_cloud_2d.col(1).mean() << std::endl;
    std::cout << "std y = " << std::sqrt(
        (point_cloud_2d.col(1).array() - point_cloud_2d.col(1).mean()).square().sum() / (point_cloud_2d.rows() - 1)
    ) << std::endl;

    std::cout << "\nmin depth = " << point_cloud_3d_front.col(2).minCoeff() << std::endl;
    std::cout << "max depth = " << point_cloud_3d_front.col(2).maxCoeff() << std::endl;
    std::cout << "mean depth = " << point_cloud_3d_front.col(2).mean() << std::endl;
    std::cout << "std depth = " << std::sqrt(
        (point_cloud_3d_front.col(2).array() - point_cloud_3d_front.col(2).mean()).square().sum() / (point_cloud_3d_front.rows() - 1)
    ) << std::endl;
    // min depth = 0.100293, max depth = 78.3935


    cv::Mat point_cloud_2d_image(375, 1242, CV_8UC3, cv::Scalar(0, 0, 0));
    // TODO: remove points outside of Cam0 FOV

    cv::Mat image0 = cv::imread("data/2011_09_26/2011_09_26_drive_0001_sync/image_00/data/0000000000.png", cv::IMREAD_GRAYSCALE);
    cv::Mat image0_depth = cv::imread("data/2011_09_26/2011_09_26_drive_0001_sync/image_00/data/0000000000.png", cv::IMREAD_GRAYSCALE);
    cv::cvtColor(image0_depth, image0_depth, cv::COLOR_GRAY2BGR);

    float depth, depth_min = 0.1f, depth_max = 50.0f, ratio;
    for (i = 0; i < point_cloud_2d.rows(); ++i) {
        depth = point_cloud_3d_front(i, 2);
        ratio = std::max(std::min((depth-depth_min)/(depth_max-depth_min), 1.0f), 0.0f);
        if (ratio < 0.5) {
            cv::circle(
                image0_depth, 
                cv::Point(point_cloud_2d(i, 0), point_cloud_2d(i, 1)),
                1,
                cv::Scalar(0, 255*ratio*2, 255*(1-ratio*2)),
                cv::FILLED,
                cv::LINE_8
            );
            cv::circle(
                point_cloud_2d_image, 
                cv::Point(point_cloud_2d(i, 0), point_cloud_2d(i, 1)),
                1,
                cv::Scalar(0, 255*ratio*2, 255*(1-ratio*2)),
                cv::FILLED,
                cv::LINE_8
            );
        }
        else {
            cv::circle(
                image0_depth, 
                cv::Point(point_cloud_2d(i, 0), point_cloud_2d(i, 1)),
                1,
                cv::Scalar(255*(ratio-0.5)*2, 255*(1-(ratio-0.5)*2), 0),
                cv::FILLED,
                cv::LINE_8
            );
            cv::circle(
                point_cloud_2d_image, 
                cv::Point(point_cloud_2d(i, 0), point_cloud_2d(i, 1)),
                1,
                cv::Scalar(255*(ratio-0.5)*2, 255*(1-(ratio-0.5)*2), 0),
                cv::FILLED,
                cv::LINE_8
            );
        }
    }

    cv::namedWindow("Point cloud in 3D projected to 2D in rectified Camera 0", 7);
    cv::imshow("Point cloud in 3D projected to 2D in rectified Camera 0", point_cloud_2d_image);

    cv::namedWindow("Display Kitti Sample Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Kitti Sample Image", image0);

    cv::namedWindow("Display Kitti Sample Image With Depth", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Kitti Sample Image With Depth", image0_depth);

    cv::waitKey(0); // wait for key to be pressed

    return 0;
}