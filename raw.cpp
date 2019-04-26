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

bool Raw::FLGtoBinRaw(std::string output_path, KlgReader* reader,
                      unsigned int width, unsigned int height,
                      const float cx, const float cy, const float fx, const float fy){

    uint2 inputSize = Raw::make_uint2(width, height);
    static const uint2 imageSize = {width, height};

//  rgb and depth directorys
    std::string path = minibr::strip_filename(output_path);
    std::string file_name = minibr::strip_extension(minibr::get_filename(output_path));
    std::string rgb_dir = path + "/rgb/";
    std::string depth_dir = path + "/depth/";
    if (!file_exists(rgb_dir)){
      make_directory(rgb_dir);
    }
    if (!file_exists(depth_dir)){
      make_directory(depth_dir);
    }

//  make video
  std::string video_file = path + "/rgb.mp4";
  cv::VideoWriter video(video_file, CV_FOURCC('M','J','P','G'), 20.0, cv::Size(width,height));

    std::string output_file = path + "/" + file_name + ".raw";
    FILE* pFile = fopen(output_file.c_str(), "wb");
    if (!pFile) {
        std::cerr << "[ERROR] File opening failed : gt.raw" << std::endl;
        return false;
    }

    uchar3 * rgbRaw = (uchar3*) malloc(sizeof(uchar3) * width * height);
    ushort * depthRaw = (ushort*) malloc(sizeof(ushort) * width * height);

  std::string frames_path = path + "/frame.txt";
  std::ofstream frame_csv(frames_path, std::ios::out | std::ios::trunc);

    for (int iImg = 0; reader->nextFrame(); iImg++) {
        cv::Mat rgb = reader->rgb();
        cv::Mat depth = reader->depth();

//      std::string imgname = std::setfill('0') << std::setw(5) << iImg;
      std::string img_string = std::to_string(iImg);
      std::string imgname = std::string(5 - img_string.length(), '0') + img_string;

      cv::imwrite(rgb_dir + imgname +".png", rgb);
      video.write(rgb);
        cv::imwrite(depth_dir + imgname +".png", depth/5000.f);

        Raw::imageToUchar3(rgb, rgbRaw);
        Raw::depthToUshort(depth, depthRaw, cx, cy, fx, fy);

        // write rgb and depth images
        int total = 0;
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(depthRaw, sizeof(uint16_t), width * height, pFile);
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(rgbRaw, sizeof(uchar3), width * height, pFile);

        std::cout << "\r[INFO] Read frame " << std::setw(6) << iImg+1 << " ";
        if (iImg % 2) fflush(stdout);

      // write gt trajectory
      frame_csv << rgb_dir + imgname +".png" << depth_dir + imgname +".png" << std::endl;

    }
  video.release();
  frame_csv.close();
    return true;
}

bool Raw::CFtoBinRaw(std::string rgb_path, std::string depth_path, std::string gt_path, std::string output_path,
                     unsigned int width, unsigned int height,
                     const float cx, const float cy, const float fx, const float fy){

    std::ifstream gt_filestream(gt_path);

    uint2 inputSize = Raw::make_uint2(width, height);
    static const uint2 imageSize = {width, height};
    std::string dataset_path = output_path + "/gt.raw";
    std::string gt_file_path = output_path + "/gt.traj";

    std::ofstream gt_csv(gt_file_path, std::ios::out | std::ios::trunc);

    FILE* pFile = fopen(dataset_path.c_str(), "wb");
    if (!pFile) {
        std::cerr << "[ERROR] File opening failed : gt.raw" << std::endl;
        return false;
    }

    uchar3 * rgbRaw = (uchar3*) malloc(sizeof(uchar3) * width * height);
    ushort * depthRaw = (ushort*) malloc(sizeof(ushort) * width * height);

    std::string line;
    int ts;
    float px, py, pz, qx, qy, qz, qw;
    for(int iImg = 0; getline(gt_filestream, line); iImg++){
        // read a line from GT trajectory
        sscanf(line.c_str(), "%d%f%f%f%f%f%f%f", &ts, &px, &py, &pz, &qx, &qy, &qz, &qw);

        // read rgb image
        std::stringstream rgbImagePath_stream;
        rgbImagePath_stream << rgb_path  << "/Color"  << std::setfill('0') << std::setw(4) << ts << ".png";
        cv::Mat rgb = cv::imread(rgbImagePath_stream.str());

        if (rgb.total() == 0){
            std::cerr << "[ERROR] Could not read rgb image" << std::endl;
            return false;
        }

        // read depth .exr format
        std::stringstream depthImagePath_stream;
        depthImagePath_stream << depth_path  << "/Depth"  << std::setfill('0') << std::setw(4) << ts << ".exr";
        cv::Mat depth = cv::imread(depthImagePath_stream.str(), cv::IMREAD_UNCHANGED);

        if (depth.total() == 0){
            std::cerr << "[ERROR] Could not read depth (Empty)" << std::endl;
            return false;
        }

        if (depth.type() != CV_32FC1) {
            cv::Mat newDepth(depth.rows, depth.cols, CV_32FC1);
            if (depth.type() == CV_32FC3) {
                unsigned depthIdx = 0;
                for (int i = 0; i < depth.rows; ++i) {
                    cv::Vec3f* pixel = depth.ptr<cv::Vec3f>(i);
                    for (int j = 0; j < depth.cols; ++j) ((float*)newDepth.data)[depthIdx++] = pixel[j][0];
                }
            } else if (depth.type() == CV_16UC1) {
                std::cerr << "[INFO] Depth scale is likely to mismatch" << std::endl;
                unsigned depthIdx = 0;
                for (int i = 0; i < depth.rows; ++i) {
                    unsigned short* pixel = depth.ptr<unsigned short>(i);
                    for (int j = 0; j < depth.cols; ++j) ((float*)newDepth.data)[depthIdx++] = 0.0006f * pixel[j];
                }
            } else {
                std::cerr << "[ERROR] Unsupported depth format" << std::endl;
            }
            depth = newDepth;
        } else {

            // convert depth into mm
            cv::Mat newDepth(depth.rows, depth.cols, CV_16UC1);
            unsigned depthIdx = 0;
            for (int i = 0; i < depth.rows; ++i) {
                float* pixel = depth.ptr<float>(i);
                for (int j = 0; j < depth.cols; ++j) ((unsigned short*)newDepth.data)[depthIdx++] = 1000 * pixel[j];
            }
            depth = newDepth;

            // depth test
//            std::stringstream deptest_path_stream;
//            deptest_path_stream << "/home/wenbin/Desktop/cftest/" << std::setfill('0') << std::setw(4) << ts << ".png";
//            newDepth.convertTo(newDepth,CV_16UC1,5);
//            cv::imwrite(deptest_path_stream.str(),newDepth);
        }

        // write gt trajectory
        gt_csv << std::setfill('0') << std::setw(5) << iImg << " "
               << std::setprecision(7) << px << " " << py << " " << pz << " "
               << std::setprecision(7) << qw << " " << qx << " " << qy << " " << qz << std::endl;

        Raw::imageToUchar3(rgb, rgbRaw);
        Raw::depthToUshort(depth, depthRaw, cx, cy, fx, fy);

        // write rgb and depth images
        int total = 0;
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(depthRaw, sizeof(uint16_t), width * height, pFile);
        total += fwrite(&(inputSize), sizeof(imageSize), 1, pFile);
        total += fwrite(rgbRaw, sizeof(uchar3), width * height, pFile);

        std::cout << "\r[INFO] Read frame " << std::setw(6) << iImg+1 << " ";
        if (iImg % 2) fflush(stdout);
    }

    std::cout << std::endl;
    gt_csv.close();
    fclose(pFile);
    return true;
}

bool Raw::toBinRaw(std::string rgb_path, std::string depth_path, std::string output_path, int index,
                   unsigned int width, unsigned int height,
                   const float cx, const float cy, const float fx, const float fy){
    std::ifstream rgb_csv(rgb_path + "/data.csv");
    std::ifstream depth_csv(depth_path + "/data.csv");
    uint2 inputSize = Raw::make_uint2(width, height);
    static const uint2 imageSize = {width, height};
    std::string dataset_path = output_path+"/cam"+std::to_string(index)+"_gt.raw";
    FILE* pFile = fopen(dataset_path.c_str(), "wb");
    if (!pFile) {
        std::cerr << "[ERROR] File opening failed : cam" << std::to_string(index) << "_gt.raw" << std::endl;
        return false;
    }

    Raw::CSVRow rgbRow, depthRow;

    uchar3 * rgbRaw = (uchar3*) malloc(sizeof(uchar3) * width * height);
    ushort * depthRaw = (ushort*) malloc(sizeof(ushort) * width * height);

    rgb_csv >> rgbRow;
    depth_csv >> depthRow; // skip the first line

    for(int i = 0; rgb_csv >> rgbRow && depth_csv >> depthRow; ++i){
        std::string rgb_file, depth_file;
        if(rgbRow.size()>1)
            rgb_file = rgb_path+"/data/"+rgbRow[1];
        else
            rgb_file = rgb_path+"/data/"+rgbRow[0];
        remove_ending_return(rgb_file);

        if(depthRow.size()>1)
            depth_file = depth_path+"/data/"+depthRow[1];
        else
            depth_file = depth_path+"/data/"+depthRow[0];
        remove_ending_return(depth_file);

        cv::Mat rgbImage = cv::imread(rgb_file);
        cv::Mat depthImage = cv::imread(depth_file,CV_16UC1);

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



















