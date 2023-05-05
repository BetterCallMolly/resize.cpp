#include "resize.hpp"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description desc("Usage: " + std::string(argv[0]) + " [OPTION]... [FILE / DIRS]... ");
    desc.add_options()
        ("help", "print this help message and exit")
        ("keep", po::bool_switch()->default_value(false), "keep original files (default: false)")
        ("progress", po::bool_switch()->default_value(true), "show progress bar (default: true)")
        ("recursive", po::bool_switch()->default_value(false), "resize files in subdirectories (default: false)")
        ("verbose", po::bool_switch()->default_value(false), "verbose mode (default: false)")
        ("delete_fails", po::bool_switch()->default_value(true), "delete files that failed to resize (default: true)")
        ("width", po::value<int>(), "width of the resized image")
        ("height", po::value<int>(),"height of the resized image")
        ("scale", po::value<float>(),"scale of the resized image")
        ("down_interpolation", po::value<std::string>(), "interpolation method for downscaling (default: INTER_AREA)")
        ("up_interpolation", po::value<std::string>(), "interpolation method for upscaling (default: INTER_LINEAR)")
        ("jpeg_quality", po::value<int>(), "jpeg quality (default: 95)")
        ("threads", po::value<int>(), "number of threads to use (default: all available)")
        ("extensions", po::value<std::string>(), "extensions to consider (default: jpg jpeg png) (space separated)")
        ("output_format", po::value<std::string>(), "output format (default: same as input)")
        ("suffix", po::value<std::string>(), "suffix to append to the filename (default: _resized)")
        ("files", po::value<std::vector<std::string>>(), "files to resize")
    ;

    po::positional_options_description p;
    p.add("files", -1);

    po::variables_map vm;
    try {
        po::store(boost::program_options::command_line_parser(argc, argv).
            options(desc).
            positional(p).run()
        , vm);
        po::notify(vm);
    } catch (boost::program_options::error &e) {
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

    // we're using sets to just skip duplicates and not do any extra work
    std::set<std::string> files;

    // print files passed as arguments
    for (auto &file : vm["files"].as<std::vector<std::string>>())
    {
        if (opts.verbose)
            std::cout << "Processing : " << file << std::endl;
        fill_set(files, opts, file);
    }

    size_t total = files.size();
    if (opts.verbose)
        std::cout << "Found " << total << " files to process" << std::endl;

    // if we're using only one thread, just process the files
    if (opts.threads == 1)
    {
        for (auto &file : files)
        {
            process_image(opts, file);
            update_progress_bar(total);
        }
    }
    else
    {
        std::vector<std::vector<std::string>> chunks;
        chunks.resize(opts.threads);
        unsigned int i = 0;
        for (auto it = files.begin(); it != files.end(); ++it)
        {
            chunks[i % opts.threads].push_back(*it);
            i++;
        }
        files.clear(); // clear the set to save memory
        // process chunks
        std::vector<std::thread> threads;
        for (auto &chunk : chunks)
        {
            threads.push_back(std::thread(process_chunk, std::ref(opts), std::ref(chunk), std::ref(total)));
        }
        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    std::cerr << std::endl;

    return (0);
}