#include <resize.hpp>
#include <iomanip>

static std::mutex mtx; // to avoid data races
static size_t progress = 0;

void update_progress_bar(size_t &total)
{
    mtx.lock();
    progress++;
    // print a fancy progress bar
    std::cerr << "\r[";
    for (size_t i = 0; i < 50; i++)
    {
        if (i < (progress * 50) / total)
            std::cerr << "=";
        else
            std::cerr << " ";
    }
    std::cerr << "] " << std::setprecision(2) << std::fixed << (progress * 100) / total << "%" << std::flush;
    mtx.unlock();
}

void dry_run_print(const std::string &path, int &width, int &height, bool noop, const std::string &output_path)
{
    mtx.lock();
    if (noop)
        std::cout << "[NO-OP] " << path << " is already at the correct size (" << width << "x" << height << ")" << std::endl;
    else
        std::cout << "[RESIZE] " << path << " -> " << output_path << " (" << width << "x" << height << ")" << std::endl;
    mtx.unlock();
}

/**
 * @brief Processes an image
 * 
 * @param opts Reference to command line options
 * @param path Path to the image
 */
void process_image(resize_opts &opts, const std::string & path)
{
    // open the image
    cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (image.empty())
    {
        if (opts.verbose)
            std::cerr << "Failed to open " << path << std::endl;
        if (opts.delete_fails)
            std::remove(path.c_str());
        return;
    }

    int width, height;
    // calculate the new size using scale or width and height
    if (opts.method == RESIZE_METHOD::SCALE)
    {
        width = image.cols * opts.scale;
        height = image.rows * opts.scale;
    }
    else if (opts.method == RESIZE_METHOD::HEIGHT_WIDTH)
    {
        width = opts.width;
        height = opts.height;
    }
    else if (opts.method == RESIZE_METHOD::HEIGHT_WIDTH_DYN)
    {
        float ratio = (float)image.cols / (float)image.rows;
        width = (opts.width != 0) ? opts.width : opts.height * ratio;
        height = (opts.height != 0) ? opts.height : opts.width / ratio;
    }
    else if (opts.method == RESIZE_METHOD::MIN_HEIGHT_WIDTH)
    {
        // if the image is smaller than the minimum size, don't resize it
        if (image.cols < opts.min_width && image.rows < opts.min_height)
        {
            if (opts.dry_run)
                return dry_run_print(path, image.cols, image.rows, true, "");
            if (opts.verbose)
                std::cerr << "Skipping " << path << " (too small)" << std::endl;
            return;
        }
        float ratio = (float)image.cols / (float)image.rows;
        // stay over minimum size, but keep the ratio (only use opts.min_width and opts.min_height, they're not 0)
        if (opts.min_width / ratio > opts.min_height)
        {
            width = opts.min_width;
            height = opts.min_width / ratio;
        }
        else
        {
            width = opts.min_height * ratio;
            height = opts.min_height;
        }
    } else {
        throw std::runtime_error("Invalid resize method, this should never happen");
    }

    // check if width and height are not equal to the original size
    if (width == image.cols && height == image.rows)
    {
        if (opts.dry_run)
            return dry_run_print(path, width, height, true, "");
        if (opts.verbose)
            std::cerr << "Skipping " << path << " (same size)" << std::endl;
        return;
    }

    // remove the extension
    std::string output_path = path.substr(0, path.find_last_of("."));

    // if keep is set, append the suffix to the filename (before the extension)
    if (opts.keep)
    {
        output_path += opts.suffix;
    }

    // replace the extension, if output_format is empty, no-op
    if (!opts.output_format.empty())
    {
        output_path += "." + opts.output_format;
    }
    else
    {
        output_path += "." + path.substr(path.find_last_of(".") + 1);
    }

    bool upscale = false; // store whether the image is being upscaled or not

    if (opts.dry_run)
        return dry_run_print(path, width, height, false, output_path);

    // resize the image
    try {
        cv::resize(
            image,
            image,
            cv::Size(width, height), 0, 0,
            // use the correct interpolation
            (upscale) ? opts.up_interpolation : opts.down_interpolation
        );
    } catch (cv::Exception &e) {
        if (opts.verbose)
            std::cerr << "Failed to resize " << path << ": " << e.what() << std::endl;
        if (opts.delete_fails)
            std::remove(path.c_str());
        return;
    }

    // write the image
    try {
        cv::imwrite(output_path, image, {cv::IMWRITE_JPEG_QUALITY, opts.jpeg_quality});
    } catch (cv::Exception &e) {
        if (opts.verbose)
            std::cerr << "Failed to write " << output_path << ": " << e.what() << std::endl;
        if (opts.delete_fails)
            std::remove(path.c_str());
        return;
    }
}

void process_chunk(resize_opts &opts, std::vector<std::string> & files, size_t &total)
{
    for (auto &file : files)
    {
        process_image(opts, file);
        if (opts.progress)
            update_progress_bar(total);
    }
}
