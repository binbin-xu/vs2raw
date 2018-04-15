/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#ifndef RAW_H
#define RAW_H

#include <opencv2/opencv.hpp>

namespace minibr {

namespace Raw {

class CSVRow
{
    public:
        std::string const& operator[](std::size_t index) const {
            return m_data[index];
        }
        std::size_t size() const {
            return m_data.size();
        }
        void readNextRow(std::istream& str) {
            std::string line;
            std::getline(str, line);
            std::stringstream lineStream(line);
            std::string       cell;

            m_data.clear();
            while(std::getline(lineStream, cell, ',')) {
                m_data.push_back(cell);
            }
            // This checks for a trailing comma with no data after it.
            if (!lineStream && cell.empty()) {
                // If there was a trailing comma then add an empty element.
                m_data.push_back("");
            }
        }

    private:
        std::vector<std::string> m_data;
};

inline std::istream& operator>>(std::istream& str, CSVRow& data) {
    data.readNextRow(str);
    return str;
}

struct uchar3 {
    unsigned char x, y, z;
};
struct uchar4 {
    unsigned char x, y, z, w;
};

struct uint2 {
    unsigned int x, y;
};

inline uchar3 make_uchar3(unsigned char x, unsigned char y, unsigned char z) {
    uchar3 val;
    val.x = x;
    val.y = y;
    val.z = z;
    return val;
}

inline uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z,
                   unsigned char w) {
    uchar4 val;
    val.x = x;
    val.y = y;
    val.z = z;
    val.w = w;
    return val;
}

inline uint2 make_uint2(unsigned int x, unsigned int y) {
    uint2 val;
    val.x = x;
    val.y = y;
    return val;
}

bool imageToUchar3(const cv::Mat& mat, uchar3* image);
//bool imageToUchar3(std::string rgbfilename, uchar3 * rgbImage, unsigned int width = 640, unsigned int height = 480);
bool depthToUshort(const cv::Mat mat, ushort * depthMap,
                   const float cx = 320.0f, const float cy = 240.0f, const float fx = 600.0f, const float fy = 600.0f);
bool toBinRaw(std::string rgb_path, std::string depth_path, std::string output_path, int index = 0,
                unsigned int width = 640, unsigned int height = 480,
                const float cx = 320.0f, const float cy = 240.0f, const float fx = 600.0f, const float fy = 600.0f);
bool CFtoBinRaw(std::string rgb_path, std::string depth_path, std::string gt_path, std::string output_path,
                unsigned int width = 640, unsigned int height = 480,
                const float cx = 320.0f, const float cy = 240.0f, const float fx = 600.0f, const float fy = 600.0f);
}

}

#endif // RAW_H
