#include "resize.hpp"

bool extension_is_valid(const std::string &path, resize_opts &options)
{
    std::string ext = boost::filesystem::extension(path);
    if (ext.size() > 0 && ext[0] == '.')
        ext = ext.substr(1);
    return options.extensions.find(ext) != options.extensions.end();
}

void fill_set(std::set<std::string> &files, resize_opts &options, const std::string &path)
{
    if (!boost::filesystem::exists(path))
        return; // skip non-existing files

    // if file is a file, insert it
    if (boost::filesystem::is_regular_file(path))
    {
        if (extension_is_valid(path, options))
            files.insert(path);
        return;
    }

    // if file is a directory, insert all files in it if recursive is set
    for (auto &entry : boost::filesystem::recursive_directory_iterator(path))
    {
        if (boost::filesystem::is_regular_file(entry))
        {
            if (extension_is_valid(entry.path().string(), options))
                files.insert(entry.path().string());
        }
    }
}