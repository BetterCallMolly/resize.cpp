#pragma once

#include <thread>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace po = boost::program_options;

enum class RESIZE_METHOD
{
    SCALE,
    HEIGHT_WIDTH
};

struct resize_opts
{
    bool keep;      // Default : false
    bool progress;  // Default : true
    bool recursive; // Default : false
    bool verbose;   // Default : false
    cv::InterpolationFlags down_interpolation; // Default : cv::INTER_AREA
    cv::InterpolationFlags up_interpolation;   // Default : cv::INTER_LINEAR
    float scale;      // Compulsory if height and width are not set
    int jpeg_quality; // Default : 95
    std::set<std::string> extensions; // Default : {".jpg", ".jpeg", ".png"}
    std::string output_format; // Default : "" (same as input)
    std::string suffix;   // Default : "_resized" (keep must be set)
    int threads; // Default : std::thread::hardware_concurrency()
    int height;  // Compulsory if width is set (scale mustn't be enabled)
    int width;   // Compulsory if height is set (scale mustn't be enabled)
    RESIZE_METHOD method;
};

resize_opts interpret_options(po::variables_map &vm);