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

namespace ViSimToBinRaw {

void readNext2f(std::ifstream &camera_info, float& x, float& y)
{
    std::string line;
    getline(camera_info, line);
    getline(camera_info, line);
    sscanf(line.c_str(), "%f%f", &x, &y);
}

}

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "[INFO] Usage: ./vs2Raw [path-to-ViSim-Folder] [camera-index] [optional: low-quality-image (0 or 1)]" << std::endl;
        return 0;
    }

    std::string path = argv[1];
    std::string index = "0";
    if(argc == 3){
        index = argv[2];
    }
    std::string isLow = "0";
    if (argc == 4) {
        isLow = argv[3];
    }

    std::ifstream camera_info;

    std::string info = path + "/cam" + index + ".info";

    if(minibr::file_exists(info)){
        camera_info.open(info.c_str());
    }
    else {
        info = path + "/cameraInfo.txt";
        camera_info.open(info.c_str());
    }

    std::cerr << "[INFO] Open cameraInfo file: " << info << std::endl;

    float resX, resY, fx, fy, cx, cy;
    ViSimToBinRaw::readNext2f(camera_info, resX, resY);
    ViSimToBinRaw::readNext2f(camera_info, fx, fy);
    ViSimToBinRaw::readNext2f(camera_info, cx, cy);

    std::cerr << "[INFO] Image resolution, width: " << (unsigned int)resX << ", height: " << (unsigned int)resY << std::endl;
    std::cerr << "[INFO] Camera focal lens, fx: " << fx << ", fy: " << fy << std::endl;
    std::cerr << "[INFO] Camera center, cx: " << cx << ", cy: " << cy << std::endl;

    camera_info.close();

    std::string rgb_path = path + "/cam" + index;

    if(isLow == "1" || !minibr::directory_exists(rgb_path)){
        rgb_path = path + "/cam" + index + "_low";
    }

    std::string depth_path = path + "/depth" + index;

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

    std::cerr << "[INFO] Start to convert images..." << std::endl;

    if(!minibr::Raw::toBinRaw(rgb_path, depth_path, path, std::stoi(index),
                              (unsigned int)resX, (unsigned int)resY,
                              cx, cy, fx, fy)){
        std::cerr << "[ERROR] ViSim to Raw convertion fail" << std::endl;
        return 0;
    }

    std::cerr << "[INFO] Finish..." << std::endl;

    return 1;
}
