/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#include "raw.h"
#include <cmath>

#include "string_tools.h"
#include "file_tools.h"

namespace minibr {

bool Raw::imageToUchar3(const cv::Mat& mat, Raw::uchar3* image){

    unsigned char* pixelPtr = (unsigned char*)mat.data;
    int cn = mat.channels();
    int nRows = mat.rows;
    int nCols = mat.cols;

    for(int i = 0; i < nRows; ++i)
        for(int j = 0; j < nCols; ++j) {
            unsigned char b = pixelPtr[i*mat.cols*cn + j*cn + 0]; // B
            unsigned char g = pixelPtr[i*mat.cols*cn + j*cn + 1]; // G
            unsigned char r = pixelPtr[i*mat.cols*cn + j*cn + 2]; // R
            image[nCols*i+j] = Raw::make_uchar3(r,g,b);
        }
    return true;
}

bool Raw::depthToUshort(const cv::Mat mat, ushort * depthMap,
                        const float cx, const float cy, const float fx, const float fy){

    int cn = mat.channels();
    if(cn!=1){
        std::cerr << "Input Error: Not a supported depth image." << std::endl;
        return false;
    }
    int nRows = mat.rows;
    int nCols = mat.cols;

    for(int i = 0; i < nRows; ++i)
        for(int j = 0; j < nCols; ++j) {
            double d = static_cast<double>(mat.at<uint16_t>(i,j));

            double u_u0_by_fx = (j - cx) / fx;
            double v_v0_by_fy = (i - cy) / fy;

            depthMap[j+nCols*i] = d / std::sqrt( u_u0_by_fx * u_u0_by_fx + v_v0_by_fy * v_v0_by_fy + 1);

        }

    return true;
}

bool Raw::CFtoBinRaw(std::string rgb_path, std::string depth_path, std::string output_path,
                     unsigned int width = 640, unsigned int height = 480,
                     const float cx = 320.0f, const float cy = 240.0f, const float fx = 600.0f, const float fy = 600.0f){

    // Load Depth
//    result.depth = cv::imread(depthImagePath, cv::IMREAD_UNCHANGED);
//    if (result.depth.total() == 0) throw std::invalid_argument("Could not read depth-image file. (Empty)");
//    if (result.depth.type() != CV_32FC1) {
//      cv::Mat newDepth(result.depth.rows, result.depth.cols, CV_32FC1);
//      if (result.depth.type() == CV_32FC3) {
//        unsigned depthIdx = 0;
//        for (int i = 0; i < result.depth.rows; ++i) {
//          cv::Vec3f* pixel = result.depth.ptr<cv::Vec3f>(i);
//          for (int j = 0; j < result.depth.cols; ++j) ((float*)newDepth.data)[depthIdx++] = pixel[j][0];
//        }

//      } else if (result.depth.type() == CV_16UC1) {
//        std::cout << "Warning -- your depth scale is likely to mismatch. Check ImageLogReader.cpp!" << std::endl;
//        unsigned depthIdx = 0;
//        for (int i = 0; i < result.depth.rows; ++i) {
//          unsigned short* pixel = result.depth.ptr<unsigned short>(i);
//          for (int j = 0; j < result.depth.cols; ++j) ((float*)newDepth.data)[depthIdx++] = 0.0006f * pixel[j];
//        }
//      } else {
//        throw std::invalid_argument("Unsupported depth-files: " + cvTypeToString(result.depth.type()));
//      }
//      result.depth = newDepth;
//    }

}

bool Raw::toBinRaw(std::string rgb_path, std::string depth_path, std::string output_path, int index,
                unsigned int width, unsigned int height,
                const float cx, const float cy, const float fx, const float fy){
    std::ifstream rgb_csv(rgb_path + "/data.csv");
    std::ifstream depth_csv(depth_path + "/data.csv");
    uint2 inputSize = Raw::make_uint2(width, height);
    static const uint2 imageSize = {width, height};
    std::string dataset_path = output_path+"/cam"+std::to_string(index)+"_gt.slambench";
    FILE* pFile = fopen(dataset_path.c_str(), "wb");
    if (!pFile) {
        std::cerr << "[ERROR] File opening failed : cam" << std::to_string(index) << "_gt.slambench" << std::endl;
        return false;
    }

    Raw::CSVRow rgbRow, depthRow;

    uchar3 * rgbRaw = (uchar3*) malloc(sizeof(uchar3) * width * height);
    ushort * depthRaw = (ushort*) malloc(sizeof(ushort) * width * height);

    rgb_csv >> rgbRow;
    depth_csv >> depthRow; // skip the first line

    for(int i = 0; rgb_csv >> rgbRow && depth_csv >> depthRow; ++i){
        cv::Mat rgbImage = cv::imread(rgb_path+"/data/"+rgbRow[0]);
        if(rgbRow.size()>1){
            rgbImage = cv::imread(rgb_path+"/data/"+rgbRow[1]);
        }
        cv::Mat depthImage = cv::imread(depth_path+"/data/"+depthRow[0],CV_16UC1);
        if(depthRow.size()>1){
            rgbImage = cv::imread(rgb_path+"/data/"+depthRow[1]);
        }
        Raw::imageToUchar3(rgbImage, rgbRaw);
        Raw::depthToUshort(depthImage, depthRaw, cx, cy, fx, fy);

        int total = 0;
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(depthRaw, sizeof(uint16_t), width * height, pFile);
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(rgbRaw, sizeof(uchar3), width * height, pFile);

        std::cout << "\r[INFO] Read frame " << std::setw(6) << i << " ";
        if (i % 2) fflush(stdout);
    }
    std::cout << std::endl;
    fclose(pFile);
    return true;
}

}



















