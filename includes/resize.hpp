#pragma once

#include <thread>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace po = boost::program_options;

enum class RESIZE_METHOD
{
    SCALE,
    HEIGHT_WIDTH,
    HEIGHT_WIDTH_DYN,
    MIN_HEIGHT_WIDTH
};

struct resize_opts
{
    bool keep;      // Default : false
    bool progress;  // Default : true
    bool recursive; // Default : false
    bool verbose;   // Default : false
    bool delete_fails; // Default : true
    bool dry_run; // Default : false
    bool summary; // Default : false
    cv::InterpolationFlags down_interpolation; // Default : cv::INTER_AREA
    cv::InterpolationFlags up_interpolation;   // Default : cv::INTER_LINEAR
    float scale;      // Compulsory if height and width are not set
    int jpeg_quality; // Default : 95
    std::set<std::string> extensions; // Default : {"jpg", "jpeg", "png"}
    std::string output_format; // Default : "" (same as input)
    std::string suffix;   // Default : "_resized" (keep must be set)
    int threads; // Default : std::thread::hardware_concurrency()
    int height;  // scale will override this
    int width;   // scale will override this
    int min_height; // compulsory if min_width is set
    int min_width; // compulsory if min_height is set
    RESIZE_METHOD method;
};

resize_opts interpret_options(po::variables_map &vm);
void process_image(resize_opts &opts, const std::string & path);
void process_chunk(resize_opts &opts, std::vector<std::string> & files, size_t &total);
void fill_set(std::set<std::string> &files, resize_opts &options, const std::string &path);
void update_progress_bar(size_t &total);
void print_summary(resize_opts &opts, const std::vector<std::string> &paths);