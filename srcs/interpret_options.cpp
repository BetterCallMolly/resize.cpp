#include "resize.hpp"

/**
 * @brief Checks if the options are valid
 *
 * @param opts Reference to the options
 * @return true If the options are valid
 * @return false If the options are invalid
 */
bool sanity_checks(resize_opts &opts)
{
    bool error = false;

    if (opts.method == RESIZE_METHOD::HEIGHT_WIDTH)
    {
        if (opts.height <= 0)
        {
            std::cerr << "Height must be a positive number" << std::endl;
            error = true;
        }
        if (opts.width <= 0)
        {
            std::cerr << "Width must be a positive number" << std::endl;
            error = true;
        }
    }
    else if (opts.method == RESIZE_METHOD::HEIGHT_WIDTH_DYN)
    {
        // No values can be under 0, one has to be 0 and the other has to be > 0
        if (opts.height < 0 || opts.width < 0)
        {
            std::cerr << "Height and width must be positive numbers" << std::endl;
            error = true;
        }
        if (opts.height == 0 && opts.width == 0)
        {
            std::cerr << "Height and width can't both be 0" << std::endl;
            error = true;
        }
    }
    else if (opts.method == RESIZE_METHOD::MIN_HEIGHT_WIDTH)
    {
        // check that min_height and min_width are valid
        if (opts.min_height <= 0 || opts.min_width <= 0)
        {
            std::cerr << "min_height and min_width must be positive numbers" << std::endl;
            error = true;
        }
    }
    else
    { // Scale
        if (opts.scale <= 0.0f)
        {
            std::cerr << "Scale must be a positive number" << std::endl;
            error = true;
        }
    }

    if (opts.jpeg_quality < 0 || opts.jpeg_quality > 100)
    {
        std::cerr << "JPEG quality must be between 0 and 100" << std::endl;
        error = true;
    }

    if (opts.threads <= 0)
    {
        std::cerr << "Number of threads must be a positive number" << std::endl;
        error = true;
    }

    if (opts.threads > static_cast<int>(std::thread::hardware_concurrency()))
    {
        std::cerr << "Warning : number of threads is greater than the number of cores" << std::endl;
    }

    if (opts.output_format != "" && opts.output_format != "jpg" && opts.output_format != "jpeg" && opts.output_format != "png")
    {
        std::cerr << "Invalid output format, possible values are : jpg, jpeg, png" << std::endl;
        error = true;
    }

    // scale is checked before height and width
    if (opts.method == RESIZE_METHOD::SCALE && opts.height != 0 && opts.width != 0)
    {
        std::cerr << "Warning : height and width will be ignored" << std::endl;
    }

    // check that extensions are valid (jpg, jpeg, png)
    for (auto &ext : opts.extensions)
    {
        if (ext != "jpg" && ext != "jpeg" && ext != "png" && ext != "webp" && ext != "avif")
        {
            std::cerr << "Invalid extension : " << ext << std::endl;
            error = true;
        }
    }

    // check that there's at least one extension
    if (opts.extensions.size() == 0)
    {
        std::cerr << "Warning : no extension specified, defaulting to jpg, jpeg and png" << std::endl;
        opts.extensions = {"jpg", "jpeg", "png"};
    }
    return (!error);
}

const static std::map<std::string, cv::InterpolationFlags> interpolation_map = {
    {"nearest", cv::INTER_NEAREST},
    {"linear", cv::INTER_LINEAR},
    {"area", cv::INTER_AREA},
    {"cubic", cv::INTER_CUBIC},
    {"lanczos", cv::INTER_LANCZOS4}};

cv::InterpolationFlags find_interpolation(const std::string &str)
{
    auto it = interpolation_map.find(str);
    if (it != interpolation_map.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("Invalid interpolation value, possible values are : nearest, linear, area, cubic, lanczos");
    }
}

resize_opts interpret_options(po::variables_map &vm)
{
    resize_opts opts;

    opts.keep = vm["keep"].as<bool>();
    opts.progress = vm["progress"].as<bool>();
    opts.recursive = vm["recursive"].as<bool>();
    opts.verbose = vm["verbose"].as<bool>();
    opts.delete_fails = vm["delete_fails"].as<bool>();
    opts.dry_run = vm["dry_run"].as<bool>();
    opts.summary = vm["summary"].as<bool>();

    if (opts.verbose || opts.dry_run)
        opts.progress = false;

    // Options with default values
    opts.down_interpolation = cv::INTER_AREA;
    opts.up_interpolation = cv::INTER_LINEAR;
    opts.jpeg_quality = 95;
    opts.threads = std::thread::hardware_concurrency();
    opts.extensions = {"jpg", "jpeg", "png"};
    opts.output_format = "";
    opts.suffix = "_resized";

    // Compulsory options
    if (vm.count("scale"))
    {
        opts.scale = vm["scale"].as<float>();
        opts.method = RESIZE_METHOD::SCALE;
    }
    else if (vm.count("height") || vm.count("width"))
    {
        if (vm.count("height"))
        {
            opts.height = vm["height"].as<int>();
        }
        if (vm.count("width"))
        {
            opts.width = vm["width"].as<int>();
        }
        // HEIGHT_WIDTH if both set, HEIGHT_WIDTH_DYN if only one set
        opts.method = (vm.count("height") && vm.count("width")) ? RESIZE_METHOD::HEIGHT_WIDTH : RESIZE_METHOD::HEIGHT_WIDTH_DYN;
    }
    else if (vm.count("min_height") && vm.count("min_width"))
    {
        opts.min_height = vm["min_height"].as<int>();
        opts.min_width = vm["min_width"].as<int>();
        opts.method = RESIZE_METHOD::MIN_HEIGHT_WIDTH;
    }
    else
        throw std::runtime_error("Either scale or height or width or min_height and min_width must be specified");

    // Interpret interpolation options (if any) (lowercase)
    if (vm.count("down_interpolation"))
    {
        std::string _down_interpolation_str = vm["down_interpolation"].as<std::string>();
        boost::algorithm::to_lower(_down_interpolation_str);
        opts.down_interpolation = find_interpolation(_down_interpolation_str);
    }

    if (vm.count("up_interpolation"))
    {
        std::string _up_interpolation_str = vm["up_interpolation"].as<std::string>();
        boost::algorithm::to_lower(_up_interpolation_str);
        opts.up_interpolation = find_interpolation(_up_interpolation_str);
    }

    // Interpret jpeg quality option (if any)
    if (vm.count("jpeg_quality"))
    {
        opts.jpeg_quality = vm["jpeg_quality"].as<int>();
    }

    // Interpret threads option (if any)
    if (vm.count("threads"))
    {
        opts.threads = vm["threads"].as<int>();
    }

    // Interpret extensions option (if any)
    if (vm.count("extensions"))
    {
        boost::split(opts.extensions, vm["extensions"].as<std::string>(), boost::is_any_of(" "));
    }

    // Interpret output format option (if any)
    if (vm.count("output_format"))
    {
        opts.output_format = vm["output_format"].as<std::string>();
    }

    // Interpret suffix option (if any)
    if (vm.count("suffix"))
    {
        opts.suffix = vm["suffix"].as<std::string>();
    }

    if (!sanity_checks(opts))
    {
        throw std::runtime_error("Invalid options were passed");
    }
    return opts;
}