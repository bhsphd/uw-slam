/**
* This file is part of UW-SLAM.
* 
* Copyright 2018.
* Developed by Fabio Morales,
* Email: fabmoraleshidalgo@gmail.com; GitHub: @fmoralesh
*
* UW-SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* UW-SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with UW-SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <Options.h>
///Basic C and C++ libraries
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <dirent.h>

/// OpenCV libraries. May need review for the final release
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/calib3d.hpp"
#include <opencv2/video.hpp>

// Ceres library
#include "ceres/ceres.h"

// Eigen library
#include <eigen3/Eigen/Core>
#include <opencv2/core/eigen.hpp>

// Sophus
#include "sophus/se3.hpp"
#include "sophus/so3.hpp"

/// CUDA specific libraries
#include <opencv2/cudafilters.hpp>
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudaarithm.hpp"

// Namespaces
using namespace cv;
using namespace std;

namespace uw
{

class Frame;

// A seed is a probabilistic depth estimate for a single pixel
struct Seed {
    int batch_counter;
    int seed_counter;
    int batch_id;       // Batch id is the id of the keyframe for which the seed was created
    int id;             // Seed ID, only used for visualization

    float a;            // a of Beta distribution: When high, probability of inlier is large
    float b;            // b of Beta distribution: When high, probability of outlier is large
    
    float mu;           // Mean of normal distribution;
    float z_range;      // Max range of the possible depth
    float sigma2;       // Variance of normal distribution
    Mat patch_cov;      // Patch covariance in reference image

    Seed(Frame* frame, float depth_mean, float depth_min);
};

// Mapper implements the Bayesian Depth Filter Update proposed in:
// 
/// "Video-based, Real-Time Multi View Stereo" by G. Vogiatzis and C. Hernández.
/// In Image and Vision Computing, 29(7):434-441, 2011.
class Mapper {
public:

    Mapper(int _width, int _height, Mat _K);
    ~Mapper();

    void TriangulateCloudPoints(Frame* _previous_frame, Frame* _current_frame);
    
    void AddPointCloudFromRGBD(Frame* frame);

    SE3 previous_world_pose_;
    
    Mat recent_cloud_points_;

    int max_num_keyframes_;                      // maximum number of keyframes to maintain seeds
    double sigma_i_sq_;                          // image noise
    double seed_convergence_sigma2_thresh_;      // threshold on depth uncertainty for convergence

    // Width and height of images for each pyramid level available  
    vector<int> w_ = vector<int>(PYRAMID_LEVELS);
    vector<int> h_ = vector<int>(PYRAMID_LEVELS);

    vector<float> fx_ = vector<float>(PYRAMID_LEVELS);
    vector<float> fy_ = vector<float>(PYRAMID_LEVELS);
    vector<float> cx_ = vector<float>(PYRAMID_LEVELS);
    vector<float> cy_ = vector<float>(PYRAMID_LEVELS);
    vector<float> invfx_ = vector<float>(PYRAMID_LEVELS);
    vector<float> invfy_ = vector<float>(PYRAMID_LEVELS);
    vector<float> invcx_ = vector<float>(PYRAMID_LEVELS);
    vector<float> invcy_ = vector<float>(PYRAMID_LEVELS);

    vector<Mat> K_ = vector<Mat>(PYRAMID_LEVELS);
};
}