/**
 * \file Main.cpp
 * \brief Combines multiple files into a single archive
 */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

/** \brief Size of a CD-ROM sector */
const auto sectorSize = 0x800;

/** \brief Prints the help message to stdout */
static void printHelp() {
    std::cout << "Usage: fps6-builder files...\n";
    std::cout << "Combines multiple files into a single archive. The format resembles the one ";
    std::cout << "found in GTA SA.\n";
}

/**
 * \brief Gets the filename from a path
 *
 * This strips the directory information from the path and leaves the filename part e.g.
 * `C:\test\image.png` becomes `image.png`
 *
 * \param path the path to get the filename from
 * \return the filename
 */
static std::string getFilename(const std::string& path) {
    const auto pos = path.find_last_of("/\\");
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

/**
 * \brief Reads a file and returns it's data
 *
 * \param path the path to the file
 * \return the binary data
 * \throw std::runtime_error if the functions fails to read from the file
 */
static std::vector<char> readFile(const std::string& path) {
    try {
        std::ifstream f(path, std::ios::binary);
        f.exceptions(std::ios::failbit | std::ios::badbit);

        f.seekg(0, std::ios::end);
        const auto size = f.tellg();
        f.seekg(0, std::ios::beg);

        std::vector<char> data(size);
        f.read(data.data(), size);

        return data;
    }
    catch (const std::ifstream::failure& ex) {
        throw std::runtime_error(path + ": failed to read from file");
    }
}

/**
 * \brief Calculates the number of sectors needed to fit the given size
 *
 * \param size the size in bytes
 * \return the number of sectors needed
 */
static int numSectors(int size) {
    return (size + sectorSize - 1) / sectorSize;
}

/**
 * \brief Writes the binary representation of a value to the stream
 *
 * \param stream the stream where to write to
 * \param value the value which is written
 */
template<class T>
void streamWrite(std::ostream& stream, T value) {
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

/**
 * \brief Fills the stream with a certain amount of null bytes
 *
 * \param stream the stream where to write to
 * \param size the number of bytes to write
 */
static void streamFill(std::ostream& stream, int size) {
    for (auto i = 0; i < size; ++i) {
        stream.put(0);
    }
}

/**
 * \brief Entrypoint of the application
 *
 * \param argc the number of arguments
 * \param argv the arguments itself
 */
int main(int argc, char** argv) {
    try {
        if (argc <= 1) {
            printHelp();
            return 0;
        }

        std::map<std::string, std::vector<char>> files;

        // Read files in
        for (auto i = 1; i < argc; ++i) {
            const auto filename = getFilename(argv[i]);
            files[filename] = readFile(argv[i]);
        }

        const auto imgMagicNumber = 0x32524556;
        const auto nameLength = 24;

        // Write img file
        std::ofstream f("data.img", std::ios::binary);

        // Write header
        const auto headerSize = 2 * sizeof(std::uint32_t) + 32 * files.size();
        streamWrite<std::uint32_t>(f, imgMagicNumber);
        streamWrite<std::uint32_t>(f, files.size());

        auto offset = numSectors(headerSize);

        for (const auto& file : files) {
            if (file.first.size() > nameLength) {
                throw std::runtime_error(
                    file.first + " exceeds the " +
                    std::to_string(nameLength) + "-byte limit for names"
                );
            }

            const auto size = numSectors(file.second.size());

            streamWrite<std::uint32_t>(f, offset);
            streamWrite<std::uint16_t>(f, size);
            streamWrite<std::uint16_t>(f, 0);

            for (auto c : file.first) {
                f.put(c);
            }

            for (auto i = file.first.size(); i < nameLength; ++i) {
                f.put(0);
            }

            offset += size;
        }

        // Fill up until sector ends
        streamFill(f, numSectors(headerSize) * sectorSize - headerSize);

        // Write content
        for (const auto& file : files) {
            f.write(file.second.data(), file.second.size());

            const auto bytesLeft = numSectors(file.second.size()) * sectorSize - file.second.size();
            streamFill(f, bytesLeft);
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Failed: " << ex.what() << "\n";
    }

    return 0;
}
