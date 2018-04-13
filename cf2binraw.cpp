/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <string>
#include "raw.h"
#include <opencv2/opencv.hpp>
#include "file_tools.h"

namespace cfToBinRaw {

void readcal(std::ifstream &camera_info, float& fx, float& fy, float& cx, float& cy, float& rx, float& ry) {
    std::string line;
    getline(camera_info, line);
    sscanf(line.c_str(), "%f%f%f%f%f%f", &fx, &fy, &cx, &cy, &rx, &ry);
}

}

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "[INFO] Usage: ./cf2Raw [path-to-ViSim-Folder]" << std::endl;
        return 0;
    }

    std::string path = argv[1];

    std::ifstream camera_info;

    std::string info = path + "/calibration.txt";

    if(minibr::file_exists(info)){
        camera_info.open(info.c_str());
    }
    else {
        std::cerr << "[INFO] Can't not open calibration file: " << info << std::endl;
        return 0;
    }

    std::cerr << "[INFO] Open cameraInfo file: " << info << std::endl;

    float resX, resY, fx, fy, cx, cy;
    cfToBinRaw::readcal(camera_info, fx, fy, cx, cy, resX, resY);

    std::cerr << "[INFO] Image resolution, width: " << (unsigned int)resX << ", height: " << (unsigned int)resY << std::endl;
    std::cerr << "[INFO] Camera focal lens, fx: " << fx << ", fy: " << fy << std::endl;
    std::cerr << "[INFO] Camera center, cx: " << cx << ", cy: " << cy << std::endl;

    camera_info.close();

    std::string rgb_path = path + "/colour";
    std::string depth_path = path + "/depth_original";
    std::string gt_file = path + "/trajectories/gt-cam-0.txt";

    if(!minibr::directory_exists(rgb_path)) {
        std::cerr << "[ERROR] RGB folder not exists" << std::endl;
        return 0;
    }
    else {
        std::cerr << "[INFO] loading RGB from: " << rgb_path << std::endl;
    }

    if(!minibr::directory_exists(depth_path)) {
        std::cerr << "[ERROR] Depth folder not exists" << std::endl;
        return 0;
    }
    else {
        std::cerr << "[INFO] loading Depth from: " << depth_path << std::endl;
    }

    if(!minibr::file_exists(gt_file)) {
        std::cerr << "[ERROR] GT camera trajectory not exists" << std::endl;
        return 0;
    }
    else {
        std::cerr << "[INFO] loading GT camera trajectory from: " << depth_path << std::endl;
    }

    std::cerr << "[INFO] Start to convert images..." << std::endl;

//    if(!minibr::Raw::toBinRaw(rgb_path, depth_path, path, std::stoi(index),
//                              (unsigned int)resX, (unsigned int)resY,
//                              cx, cy, fx, fy)){
//        std::cerr << "[ERROR] ViSim to Raw convertion fail" << std::endl;
//        return 0;
//    }

    std::cerr << "[INFO] Finish..." << std::endl;

    return 1;
}
