import subprocess
from typing import List

BINARY = "./resize"


def try_run(args: List[str], fails: bool = False) -> bool:
    process_error = False
    try:
        subprocess.check_call(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError as e:
        process_error = True

    if fails and not process_error:
        print(f"Test failed: {args} should have failed")
        return False
    elif not fails and process_error:
        print(f"Test failed: {args} should have passed")
        return False

# Ugly, I'll refactor this asap, I just want to get this working quickly
if __name__ == "__main__":
    try_run([BINARY, "--help"], False)  # Should pass
    try_run([BINARY, "--dsfhijgsdfklhj"], True)  # Should fail (invalid option)
    try_run(
        [
            BINARY,
            "--width",
            "100",
            "--height",
            "100",
        ],
        False,
    )  # Should pass

    try_run(
        [
            BINARY,
            "--width",
            "A",
            "--height",
            "100",
        ],
        True,
    )  # Should fail (invalid width)
    try_run(
        [
            BINARY,
            "--width",
            "100",
        ],
        True,
    )  # Should fail (missing height)
    try_run(
        [
            BINARY,
            "--height",
            "100",
        ],
        True,
    )  # Should fail (missing width)
    try_run(
        [
            BINARY,
            "--scale",
            "1.1",
        ],
        False,
    )  # Should pass
    try_run(
        [
            BINARY,
            "--scale",
            "1.1a",
        ],
        True,
    )  # Should fail (invalid scale)
    BASE_VALID_ARGS = [BINARY, "--width", "100", "--height", "100"]
    try_run(
        BASE_VALID_ARGS + ["--scale", "1.1"],
        False,
    )  # Should pass (scale is ignored)
    possibilities = [
        "nearest",
        "NEAREST",
        "linear",
        "LINEAR",
        "cubic",
        "CUBIC",
        "area",
        "AREA",
        "lanczos",
        "LANCZOS",
    ]
    for possibility in possibilities:
        try_run(
            BASE_VALID_ARGS + ["--down_interpolation", possibility],
            False,
        )
        try_run(
            BASE_VALID_ARGS + ["--up_interpolation", possibility],
            False,
        )
    try_run(
        BASE_VALID_ARGS + ["--down_interpolation", "invalid"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--up_interpolation", "invalid"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--down_interpolation", "nearest", "--up_interpolation", "invalid"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--down_interpolation", "invalid", "--up_interpolation", "nearest"],
        True,
    )

    try_run(
        [
            BINARY,
            "--width",
            "-100",
            "--height",
            "100",
        ],
        True,
    )
    try_run(
        [
            BINARY,
            "--width",
            "100",
            "--height",
            "-100",
        ],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--scale", "-1"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--scale", "0"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--threads", "-1"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--threads", "0"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--threads", "1"],
        False,
    )
    try_run(
        BASE_VALID_ARGS + ["--jpeg_quality", "-1"],
        True,
    )
    try_run(
        BASE_VALID_ARGS + ["--jpeg_quality", "0"],
        False,
    )
    try_run(
        BASE_VALID_ARGS + ["--jpeg_quality", "1"],
        False,
    )
    for valid_extension in ["jpg", "jpeg", "png"]:
        try_run(
            BASE_VALID_ARGS + ["--output_format", valid_extension],
            False,
        )
    try_run(
        BASE_VALID_ARGS + ["--output_format", "invalid"],
        True,
    )
    # Currently un-tested:
    # - --extensions (not implemented yet)