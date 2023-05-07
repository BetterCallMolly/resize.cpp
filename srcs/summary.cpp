#include "resize.hpp"

std::string stringify_interpolation(cv::InterpolationFlags &flag)
{
    switch (flag)
    {
    case cv::INTER_NEAREST:
        return "nearest";
    case cv::INTER_LINEAR:
        return "linear";
    case cv::INTER_AREA:
        return "area";
    case cv::INTER_CUBIC:
        return "cubic";
    case cv::INTER_LANCZOS4:
        return "lanczos";
    default:
        return "Unknown";
    }
}

std::string stringify_method(RESIZE_METHOD &method)
{
    switch (method)
    {
    case RESIZE_METHOD::SCALE:
        return "Scale";
    case RESIZE_METHOD::HEIGHT_WIDTH:
        return "Fixed Height/Width";
    case RESIZE_METHOD::HEIGHT_WIDTH_DYN:
        return "Dynamic Height/Width (keep aspect ratio)";
    case RESIZE_METHOD::MIN_HEIGHT_WIDTH:
        return "Minimum Height/Width (keep aspect ratio)";
    default:
        return "Unknown";
    }
}

void print_summary(resize_opts &opts, const std::vector<std::string> &paths)
{
    std::cout << "Summary:" << std::endl;
    std::cout << "\tKeep original image     : " << (opts.keep ? "true" : "false") << std::endl;
    std::cout << "\tShow progress bar       : " << (opts.progress ? "true" : "false") << std::endl;
    std::cout << "\tRecursive               : " << (opts.recursive ? "true" : "false") << std::endl;
    std::cout << "\tVerbose                 : " << (opts.verbose ? "true" : "false") << std::endl;
    std::cout << "\tDelete on failure       : " << (opts.delete_fails ? "true" : "false") << std::endl;
    std::cout << "\tDry run                 : " << (opts.dry_run ? "true" : "false") << std::endl;
    std::cout << "\tDownscale interpolation : " << stringify_interpolation(opts.down_interpolation) << std::endl;
    std::cout << "\tUpscale interpolation   : " << stringify_interpolation(opts.up_interpolation) << std::endl;
    std::cout << "\tResize method           : " << stringify_method(opts.method) << std::endl;
    switch (opts.method)
    {
    case RESIZE_METHOD::SCALE:
        std::cout << "\tScale factor            : " << opts.scale << std::endl;
        break;
    case RESIZE_METHOD::HEIGHT_WIDTH:
        std::cout << "\tHeight                  : " << opts.height << std::endl;
        std::cout << "\tWidth                   : " << opts.width << std::endl;
        break;
    case RESIZE_METHOD::HEIGHT_WIDTH_DYN:
        std::cout << "\tHeight                  : " << (opts.height ? std::to_string(opts.height) : "auto") << std::endl;
        std::cout << "\tWidth                   : " << (opts.width ? std::to_string(opts.width) : "auto") << std::endl;
        break;
    case RESIZE_METHOD::MIN_HEIGHT_WIDTH:
        std::cout << "\tMinimal Height          : " << opts.min_height << std::endl;
        std::cout << "\tMinimal Width           : " << opts.min_width << std::endl;
        break;
    default:
        std::cout << "\tUnknown method" << std::endl;
        break;
    }
    std::cout << "\tJPEG quality            : " << opts.jpeg_quality << std::endl;
    std::cout << "\tOutput format           : " << (opts.output_format.empty() ? "same as input" : opts.output_format) << std::endl;
    {
        std::cout << "\tTarget extensions       : ";
        for (auto &ext : opts.extensions)
            std::cout << ext << " ";
        std::cout << std::endl;
    }
    if (opts.keep)
        std::cout << "\tSuffix                  : " << opts.suffix << std::endl;
    std::cout << "\tThreads                 : " << opts.threads << std::endl;
    std::cout << "Input paths : " << std::endl;
    for (auto &path : paths)
        std::cout << "\t- " << path << " ";
}