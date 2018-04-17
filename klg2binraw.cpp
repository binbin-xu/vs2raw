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
#include "string_tools.h"
#include "KlgReader.h"

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "[INFO] Usage: ./klg2Raw [path-to-klg-file] [imWidth] [imHeight] [cx] [cy] [fx] [fy]" << std::endl;
        return 0;
    }

    int resX = 640;
    int resY = 480;
    float cx = 320;
    float cy = 240;
    float fx = 300;
    float fy = 300;

    if(argc == 8){
        resX = std::stoi(argv[2]);
        resY = std::stoi(argv[3]);
        cx = std::stof(argv[4]);
        cy = std::stof(argv[5]);
        fx = std::stof(argv[6]);
        fy = std::stof(argv[7]);
    }else if(argc > 8){
        std::cerr << "[ERROR] Not proper input" << std::endl;
        return 0;
    }

    std::string klgfile = argv[1];
    KlgReader* reader = new KlgReader(klgfile);
    if(!minibr::Raw::FLGtoBinRaw(klgfile, reader,
                                (unsigned int)resX, (unsigned int)resY,
                                cx, cy, fx, fy)){
        std::cerr << "[ERROR] Co-fusion to Raw convertion fail" << std::endl;
        return 0;
    }

    std::cerr << "[INFO] Finish..." << std::endl;
    return 1;
}
