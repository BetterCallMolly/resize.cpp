# 🤏 resize.cpp 

C++ small yet efficient utility that uses OpenCV to resize images.

## Why

I was tired of writing the same code over and over again to resize images with Python, so I decided to write a small utility once and for all.

- Completely written in C++ to be somewhat memory efficient (and to practice).
- No ultra-edge-cases handling, just the basics.
- Feel free to do whatever you want with it, this project is using the "The Unlicense" license.
- Following as much as possible the [KISS principle](https://en.wikipedia.org/wiki/KISS_principle)

# 📚 Table of contents

- [📚 Table of contents](#-table-of-contents)
- [📝 Use cases](#-use-cases)
- [📖 Help](#-help)
- [📦 Dependencies](#-dependencies)

# 📝 Use cases

## Basic recursive

```bash
./resize -w 512 -h 512 -r .

# Will search for all images in the current directory and resize them to 512x512, overwriting the originals, keeping the same format
```

## Using `find`

```bash
find . -type f -name "*.jpg" -exec ./resize {} -w 512 -h 512 \;
```

## Using `fd`

```bash
fd -e jpg -x ./resize -w 512 -h 512
```

# 📖 Help

```
Usage: resize [options] <file_or_directory>...

--help
    Print this help message and exit

--width <width>
    es

--height <height>
    Height of the resized image

--recursive
    Recursively resize all images in the given path

--keep
    Keep the original image, resized images will have the '_resized' suffix

--scale <scale>
    Scale factor to apply to the image (-w and -h will be ignored)

--suffix=<suffix>
    Suffix to append to the resized image (default: '_resized') (works only with --keep)

--verbose
    Print verbose messages (disables progress bar)

--threads <threads>
    Number of threads to use (default: all threads)

--no-progress
    Disable progress bar

--extension <extension>
    Extension of the images to resize (default: jpg, jpeg, png) (case insensitive) (comma separated)

--output-format <format>
    Output format of the resized image (default: same as input)

--up_interpolation <interpolation>
    Interpolation method to use when upscaling (default: cubic) (case insensitive)

--down_interpolation <interpolation>
    Interpolation method to use when downscaling (default: default: INTER_AREA) (case insensitive)

--jpeg-quality <quality>
    JPEG quality (default: 95)
```

_[see OpenCV docs for interpolation methods](https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#ga5bb5a1fea74ea38e1a5445ca803ff121)_

# 📦 Dependencies

- [OpenCV](https://opencv.org/)
- [Boost](https://www.boost.org/)

# 📈 Performances

## resize.cpp

| Threads | N Images | Time (s) | Pixels/s  |
| ------- | -------- | -------- | --------- |
| 1       | 2048     | 4.22     | 127220595 |
| 2       | 2048     | 2.83     | 189707036 |
| 4       | 2048     | 1.65     | 325376310 |
| 8       | 2048     | 1.09     | 492542121 |
| 16      | 2048     | 0.826    | 649799581 |
| 24      | 2048     | 0.809    | 663000039 |