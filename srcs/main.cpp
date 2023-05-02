#include "resize.hpp"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description desc("Usage: " + std::string(argv[0]) + " [OPTION]... [FILE]...");
    desc.add_options()
        ("help", "print this help message and exit")
        ("keep", "keep original files")
        ("no_progress", "show progress bar")
        ("recursive", "resize files in subdirectories")
        ("verbose", "verbose mode")
        ("width", po::value<int>(), "width of the resized image")
        ("height", po::value<int>(),"height of the resized image")
        ("scale", po::value<float>(),"scale of the resized image")
        ("down_interpolation", po::value<std::string>(), "interpolation method for downscaling")
        ("up_interpolation", po::value<std::string>(), "interpolation method for upscaling")
        ("jpeg_quality", po::value<int>(), "jpeg quality")
        ("threads", po::value<int>(), "number of threads to use")
        ("extensions", po::value<std::vector<std::string>>(), "extensions to consider")
        ("output_format", po::value<std::string>(), "output format")
        ("suffix", po::value<std::string>(), "suffix to append to the filename")
    ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (std::exception &e) {
        std::cerr << "Failed to parse command line: " << e.what() << std::endl;
        return (1);
    }

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return (0);
    }

    resize_opts opts;
    try {
        opts = interpret_options(vm);
    } catch (std::runtime_error &e) {
        std::cerr << "Failed to parse options: " << e.what() << std::endl;
        return (1);
    } catch (boost::bad_any_cast &e) {
        std::cerr << "Failed to interpret options: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}