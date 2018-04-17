// developed by John McCormac
// https://bitbucket.org/jbmcc/objecttagger

#include "KlgReader.h"

#include <iostream>


KlgReader::KlgReader()
  : number_frames_left_rgb_(0)
  , number_frames_left_ground_truth_(0)
  , number_frames_left_artificial_depth_(0)
  , timestamp_(0)
  , artificial_depth_(false)
{}

KlgReader::KlgReader(const std::string& video)
  : number_frames_left_rgb_(0)
  , number_frames_left_ground_truth_(0)
  , number_frames_left_artificial_depth_(0)
  , timestamp_(0)
  , artificial_depth_(false)
{
  openRGBD(video);
}

void KlgReader::openRGBD(const std::string& video)
{
  reset();
  video_stream_.open(video.c_str(), std::ios::binary);
  if (!video_stream_.is_open()) {
    std::cerr << "The video files failed to open" << std::endl;
  } else {
  video_stream_.read(reinterpret_cast<char*>(&number_frames_left_rgb_), 
                     sizeof(int));
  }
}

void KlgReader::openGroundTruth(const std::string& ground_truth)
{
  // Setup ground truth stream
  if (ground_truth_stream_.is_open()) 
    ground_truth_stream_.close();
  ground_truth_stream_.open(ground_truth.c_str(), std::ios::binary);
  if (!ground_truth_stream_.is_open()) {
    std::cerr << "The video files failed to open" << std::endl;
    ground_truth_ = false;
  } else {
    ground_truth_stream_.read(reinterpret_cast<char*>(&number_frames_left_ground_truth_), 
                              sizeof(int));
    ground_truth_ = true;
  }
}

void KlgReader::openArtificialDepth(const std::string& depth)
{
  // Setup depth stream
  if (artificial_depth_stream_.is_open()) 
    artificial_depth_stream_.close();
  artificial_depth_stream_.open(depth.c_str(), std::ios::binary);
  if (!artificial_depth_stream_.is_open()) {
    std::cerr << "The video files failed to open" << std::endl;
    artificial_depth_ = false;
  } else {
    artificial_depth_stream_.read(reinterpret_cast<char*>(&number_frames_left_artificial_depth_), 
                      sizeof(int));
    artificial_depth_ = true;
  }
}

bool KlgReader::isOpen() const 
{
  return (video_stream_.is_open() && 
          (!ground_truth_ || ground_truth_stream_.is_open()) && 
          (!artificial_depth_ || artificial_depth_stream_.is_open()));
}

bool KlgReader::hasNextFrame() const 
{
  return (video_stream_.is_open() && number_frames_left_rgb_ > 0 &&
          (!ground_truth_ || (ground_truth_stream_.is_open() && number_frames_left_ground_truth_ > 0)) && 
          (!artificial_depth_ || (artificial_depth_stream_.is_open() && number_frames_left_artificial_depth_ > 0)));
}

bool KlgReader::nextFrame()
{
  int64_t seg_timestamp = 0;
  while (hasNextFrame()) {
    int image_size, depth_size, segmentation_size;
    // Read in the video rgb and depth frame
    if (!seg_timestamp || seg_timestamp > timestamp_) {
      video_stream_.read(reinterpret_cast<char*>(&timestamp_), sizeof(int64_t));
      video_stream_.read(reinterpret_cast<char*>(&depth_size), sizeof(int));
      video_stream_.read(reinterpret_cast<char*>(&image_size), sizeof(int));
      depth_buffer_.resize(depth_size);
      video_stream_.read(reinterpret_cast<char*>(depth_buffer_.data()), depth_buffer_.size());
      image_buffer_.resize(image_size);
      video_stream_.read(reinterpret_cast<char*>(image_buffer_.data()), image_buffer_.size());
      number_frames_left_rgb_--;
    }
    if (seg_timestamp < timestamp_) {
      int test_depth_size = 0;
      if (ground_truth_) {
        ground_truth_stream_.read(reinterpret_cast<char*>(&seg_timestamp), sizeof(int64_t));
        ground_truth_stream_.read(reinterpret_cast<char*>(&test_depth_size), sizeof(int));
        assert(test_depth_size == 0 && "Ground truth data should not have depth");
        ground_truth_stream_.read(reinterpret_cast<char*>(&segmentation_size), sizeof(int));
        ground_truth_buffer_.resize(segmentation_size);
        ground_truth_stream_.read(reinterpret_cast<char*>(ground_truth_buffer_.data()), ground_truth_buffer_.size());
        number_frames_left_ground_truth_--;
      }
      int64_t artificial_depth_timestamp = 0;
      if (artificial_depth_) {
        artificial_depth_stream_.read(reinterpret_cast<char*>(&artificial_depth_timestamp), sizeof(int64_t));
        artificial_depth_stream_.read(reinterpret_cast<char*>(&test_depth_size), sizeof(int));
        assert(test_depth_size == 0 && "Artificial depth data should not have depth");
        artificial_depth_stream_.read(reinterpret_cast<char*>(&segmentation_size), sizeof(int));
        artificial_depth_buffer_.resize(segmentation_size);
        artificial_depth_stream_.read(reinterpret_cast<char*>(artificial_depth_buffer_.data()), artificial_depth_buffer_.size());
        number_frames_left_artificial_depth_--;
      }
      if (artificial_depth_ && ground_truth_) {
        assert(artificial_depth_timestamp == seg_timestamp && "Segmentation and artificial depth must have same timestamps");
      }
    }
    if ((!ground_truth_ && !artificial_depth_) || seg_timestamp == timestamp_) {
      if (ground_truth_) {
        current_frame_ground_truth_ = cv::imdecode(cv::Mat(ground_truth_buffer_),CV_LOAD_IMAGE_COLOR);
        cv::cvtColor(current_frame_ground_truth_, current_frame_ground_truth_, CV_BGR2RGB);
      }
      if (artificial_depth_) {
        current_frame_artificial_depth_ = cv::imdecode(cv::Mat(artificial_depth_buffer_),-1);
      }
      cv::Mat decodedImage = cv::imdecode(cv::Mat(image_buffer_),CV_LOAD_IMAGE_COLOR);
      cv::cvtColor(decodedImage, current_frame_rgb_, CV_BGR2RGB);
      // If depth doesn't match, decompress using zlib
      if (static_cast<int>(depth_buffer_.size()) != 
          current_frame_rgb_.rows * current_frame_rgb_.cols * 2) {
        unsigned long decomp_length = current_frame_rgb_.rows * current_frame_rgb_.cols * 2;
        depth_decompression_buffer_.resize(decomp_length);
        uncompress(depth_decompression_buffer_.data(), (unsigned long *)&decomp_length, 
                   (const Bytef *)depth_buffer_.data(), depth_buffer_.size());
        current_frame_depth_ = cv::Mat(current_frame_rgb_.rows,current_frame_rgb_.cols, CV_16UC1,
                                       static_cast<void*>(depth_decompression_buffer_.data()));
      } else {
        current_frame_depth_ = cv::Mat(current_frame_rgb_.rows,current_frame_rgb_.cols, CV_16UC1,
                                       static_cast<void*>(depth_buffer_.data()));
      }
      return true;
    }
  }
  reset();
  return false;
}

cv::Mat KlgReader::rgb() const {
  return current_frame_rgb_;
}

cv::Mat KlgReader::depth() const {
  return current_frame_depth_;
}

cv::Mat KlgReader::groundTruth() const {
  if (ground_truth_) {
    return current_frame_ground_truth_;
  } 
  return cv::Mat();
}

cv::Mat KlgReader::groundTruthDepth() const {
  if (artificial_depth_) {
    return current_frame_depth_;
  }
  return cv::Mat();
}

unsigned long long int KlgReader::timestamp() const {
  return timestamp_;
}

int KlgReader::classAtPixel(const int x, const int y) const {
  if (ground_truth_ && isOpen()) {
    cv::Vec3b color = current_frame_ground_truth_.at<cv::Vec3b>(y,x);
    int16_t object_class = *(reinterpret_cast<int16_t*>(&color[0]));
    return int(object_class);
  }
  return 0;
}

int KlgReader::instanceAtPixel(const int x, const int y) const {
  if (ground_truth_ && isOpen()) {
    cv::Vec3b color = current_frame_ground_truth_.at<cv::Vec3b>(y,x);
    int8_t object_instance = *(reinterpret_cast<int8_t*>(&color[2]));
    return int(object_instance);
  }
  return 0;
}

int KlgReader::depthAtPixel(const int x, const int y) const {
  if (ground_truth_ && isOpen()) {
    unsigned short color = current_frame_depth_.at<unsigned short>(y,x);
    return int(color);
  }
  return 0;
}

int KlgReader::artificialDepthAtPixel(const int x, const int y) const {
  if (artificial_depth_ && isOpen()) {
    unsigned short color = current_frame_artificial_depth_.at<unsigned short>(y,x);
    return int(color);
  }
  return 0;
}

void KlgReader::reset()
{
  number_frames_left_rgb_ = 0;
  number_frames_left_ground_truth_ = 0;
  number_frames_left_artificial_depth_ = 0;
  ground_truth_ = false;
  artificial_depth_ = false;
  if (video_stream_.is_open()) {
    video_stream_.close();
  }
  if (ground_truth_stream_.is_open()) {
    ground_truth_stream_.close();
  }
  if (artificial_depth_stream_.is_open()) {
    artificial_depth_stream_.close();
  }
}
