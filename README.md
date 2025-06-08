# MULTITHREADED-FILE-COMPRESSION-TOOL

*COMPANY*: CODTECH IT SOLUTIONS

*NAME*: SOHINI SAMUI

*INTERN ID*: CT04DM489

*DOMAIN*: C++

*DURATION*: 4 WEEKS

*MENTOR*: NEELA SANTHOSH

Multithreaded File Compression Tool

📌 Overview

This project is a high-performance C++ application designed to compress and decompress large files using multithreading for optimized speed and efficiency. Traditional file compression tools process data sequentially, which becomes a bottleneck with large files. This tool overcomes that limitation by splitting input files into chunks and processing each chunk in parallel using multiple threads. The result is significantly faster compression and decompression times, especially on multi-core systems.

Built with the C++17 Standard Library and the popular zlib compression library, this project showcases how multithreading can be used in real-world scenarios to boost performance and resource utilization.

🚀 Features

✅ Multithreaded Compression: File is divided into equal-sized chunks and compressed in parallel by multiple threads.

✅ Multithreaded Decompression: Compressed chunks are decompressed concurrently and reassembled in the correct order.

#OUTPUT

![Image](https://github.com/user-attachments/assets/f3b05852-9295-45ab-b880-678a4c4f0e16)

✅ Chunk-Based Design: Efficient memory and CPU usage; works well for large files.

✅ Performance Measurement: Built-in timing mechanism to measure execution time of compression and decompression phases.

✅ Cross-Platform: Uses only standard C++ and zlib, making it portable across Linux, macOS, and Windows.

🧠 How It Works

1. Compression Phase:

The input file is read and split into fixed-size chunks (e.g., 1 MB).

Each chunk is assigned to a separate thread.

Threads compress their chunks using zlib::compress2.

The compressed data and metadata (original size, chunk index) are written to a binary file.

2. Decompression Phase:

The compressed file is read and chunk metadata is parsed.

Each chunk is handed to a separate thread.

Threads decompress the data using zlib::uncompress.

The decompressed chunks are combined and saved in the correct order.

3. Timing:

Execution times for both compression and decompression are printed, allowing you to benchmark performance against single-threaded tools.

🧪 Performance Gains

The tool demonstrates clear performance benefits by leveraging parallelism. For instance, on a typical 4-core CPU:

A 100MB file might compress in under 0.5 seconds, compared to 2–3 seconds in a single-threaded version.

Decompression shows even better speedups, thanks to the reduced computational load per chunk.


Actual performance depends on:

File size

Number of available CPU cores

Disk I/O speed

📂 Deliverables

main.cpp: Complete source code of the tool.

README.md: This documentation.

Sample binary files to demonstrate functionality.

Terminal screenshots or logs showing compression/decompression time comparisons.

🛠 Requirements

C++17 or later

zlib development library

Install via package manager:
sudo apt install zlib1g-dev (Linux)
or use vcpkg / brew on Windows/macOS

g++ or any C++ compiler supporting C++17

Make or shell environment for building

📈 Future Enhancements

Progress bar or real-time status updates

Adjustable thread count

GUI using Qt or ImGui

Support for different compression algorithms (LZ4, BZIP2)

This project is ideal for demonstrating real-world performance gains from multithreading in C++, and serves as a great example for students, developers, and engineers interested in systems programming and optimization.

