#include <resize.hpp>
#include <iomanip>

static std::mutex mtx; // to avoid data races
static size_t progress = 0;

void update_progress_bar(size_t &total)
{
    mtx.lock();
    progress++;
    // print a fancy progress bar
    std::cerr << "\033[2K\r";
    std::cerr << "[";
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
    else
    {
        width = opts.width;
        height = opts.height;
    }

    // check if width and height are not equal to the original size
    if (width == image.cols && height == image.rows)
    {
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
        update_progress_bar(total);
    }
}
