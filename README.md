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
./resize --width 512 --height 512 --recursive .

# Will search for all images in the current directory and resize them to 512x512, overwriting the originals, keeping the same format
```

# 📖 Help

```
Usage: resize [OPTION]... [FILE / DIRS]... :
  --help                   print this help message and exit
  --keep                   keep original files (default: false)
  --progress               show progress bar (default: true)
  --recursive              resize files in subdirectories (default: false)
  --verbose                verbose mode (default: false)
  --delete_fails           delete files that failed to resize (default: true)
  --width arg              width of the resized image
  --height arg             height of the resized image
  --scale arg              scale of the resized image
  --down_interpolation arg interpolation method for downscaling (default: INTER_AREA)
  --up_interpolation arg   interpolation method for upscaling (default: INTER_LINEAR)
  --jpeg_quality arg       jpeg quality (default: 95)
  --threads arg            number of threads to use (default: all available)
  --extensions arg         extensions to consider (default: jpg jpeg png) (space separated)
  --output_format arg      output format (default: same as input)
  --suffix arg             suffix to append to the filename (default: _resized)
  --files arg              files to resize
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
