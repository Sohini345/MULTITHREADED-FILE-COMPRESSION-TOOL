#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <zlib.h>

const size_t CHUNK_SIZE = 1024 * 1024; // 1MB chunks
std::mutex file_mutex;

struct CompressedChunk {
    std::vector<unsigned char> data;
    size_t originalSize;
    size_t chunkIndex;
};

bool compress_chunk(const std::vector<unsigned char>& input, CompressedChunk& output) {
    uLongf destLen = compressBound(input.size());
    output.data.resize(destLen);

    int res = compress2(output.data.data(), &destLen, input.data(), input.size(), Z_BEST_SPEED);
    if (res != Z_OK) return false;

    output.data.resize(destLen);
    output.originalSize = input.size();
    return true;
}

bool decompress_chunk(const CompressedChunk& input, std::vector<unsigned char>& output) {
    output.resize(input.originalSize);
    uLongf destLen = input.originalSize;
    int res = uncompress(output.data(), &destLen, input.data.data(), input.data.size());
    return res == Z_OK;
}

void compress_worker(std::ifstream& inFile, std::vector<CompressedChunk>& chunks, size_t index) {
    std::vector<unsigned char> buffer(CHUNK_SIZE);
    size_t offset = index * CHUNK_SIZE;

    {
        std::lock_guard<std::mutex> lock(file_mutex);
        inFile.seekg(offset);
        inFile.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
    }

    size_t readSize = inFile.gcount();
    buffer.resize(readSize);

    CompressedChunk chunk;
    chunk.chunkIndex = index;

    if (compress_chunk(buffer, chunk)) {
        chunks[index] = chunk;
    }
}

void decompress_worker(const std::vector<CompressedChunk>& chunks, std::vector<std::vector<unsigned char>>& output, size_t index) {
    std::vector<unsigned char> decompressed;
    if (decompress_chunk(chunks[index], decompressed)) {
        output[index] = decompressed;
    }
}

void multithreaded_compress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    size_t numChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
    std::vector<CompressedChunk> chunks(numChunks);
    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numChunks; ++i) {
        threads.emplace_back(compress_worker, std::ref(inFile), std::ref(chunks), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Compression] Time taken: " << std::chrono::duration<double>(end - start).count() << "s\n";

    inFile.close();

    std::ofstream outFile(outputFile, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&numChunks), sizeof(size_t));
    for (const auto& chunk : chunks) {
        size_t compressedSize = chunk.data.size();
        outFile.write(reinterpret_cast<const char*>(&chunk.originalSize), sizeof(size_t));
        outFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(size_t));
        outFile.write(reinterpret_cast<const char*>(chunk.data.data()), compressedSize);
    }
    outFile.close();
}

void multithreaded_decompress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    size_t numChunks;
    inFile.read(reinterpret_cast<char*>(&numChunks), sizeof(size_t));

    std::vector<CompressedChunk> chunks(numChunks);

    for (size_t i = 0; i < numChunks; ++i) {
        size_t origSize, compSize;
        inFile.read(reinterpret_cast<char*>(&origSize), sizeof(size_t));
        inFile.read(reinterpret_cast<char*>(&compSize), sizeof(size_t));

        CompressedChunk chunk;
        chunk.originalSize = origSize;
        chunk.chunkIndex = i;
        chunk.data.resize(compSize);
        inFile.read(reinterpret_cast<char*>(chunk.data.data()), compSize);
        chunks[i] = chunk;
    }
    inFile.close();

    std::vector<std::vector<unsigned char>> decompressedChunks(numChunks);
    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numChunks; ++i) {
        threads.emplace_back(decompress_worker, std::cref(chunks), std::ref(decompressedChunks), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Decompression] Time taken: " << std::chrono::duration<double>(end - start).count() << "s\n";

    std::ofstream outFile(outputFile, std::ios::binary);
    for (const auto& chunk : decompressedChunks) {
        outFile.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
    }
    outFile.close();
}

int main() {
    std::string inputFile = "input.dat";
    std::string compressedFile = "compressed.dat";
    std::string decompressedFile = "decompressed.dat";

    std::cout << "Compressing...\n";
    multithreaded_compress(inputFile, compressedFile);

    std::cout << "Decompressing...\n";
    multithreaded_decompress(compressedFile, decompressedFile);

    std::cout << "Done.\n";
    return 0;
}
