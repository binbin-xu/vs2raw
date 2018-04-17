// developed by John McCormac
// https://bitbucket.org/jbmcc/objecttagger


#ifndef KLGREADER_H
#define KLGREADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <zlib.h>

class KlgReader
{
public:
  KlgReader ();
  KlgReader (const std::string& video);
  virtual ~KlgReader () {}

  void openRGBD(const std::string& video);
  void openGroundTruth(const std::string& ground_truth);
  void openArtificialDepth(const std::string& depth);

  bool isOpen() const;
  bool hasNextFrame() const;
  bool nextFrame();

  unsigned long long int timestamp() const;

  int classAtPixel(const int x, const int y) const;
  int instanceAtPixel(const int x, const int y) const;

  int depthAtPixel(const int x, const int y) const;
  int artificialDepthAtPixel(const int x, const int y) const;

  cv::Mat rgb() const;
  cv::Mat depth() const;
  cv::Mat groundTruth() const;
  cv::Mat groundTruthDepth() const;

private:
  void reset();

  std::ifstream video_stream_;
  std::ifstream ground_truth_stream_;
  std::ifstream artificial_depth_stream_;

  std::vector<unsigned char> image_buffer_; 
  std::vector<unsigned char> depth_buffer_;
  std::vector<Bytef> depth_decompression_buffer_;
  std::vector<unsigned char> ground_truth_buffer_;
  std::vector<unsigned char> artificial_depth_buffer_;

  cv::Mat current_frame_rgb_;
  cv::Mat current_frame_depth_;
  cv::Mat current_frame_ground_truth_;
  cv::Mat current_frame_artificial_depth_;

  int number_frames_left_rgb_; 
  int number_frames_left_ground_truth_;
  int number_frames_left_artificial_depth_;
  int64_t timestamp_;
  bool ground_truth_;
  bool artificial_depth_;
};

#endif
