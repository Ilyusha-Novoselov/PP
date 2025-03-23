#include <array>
#include <omp.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>

#include <png.hpp>
#include <Table.hxx>

typedef std::vector <std::vector <double>> Filter;
typedef std::unordered_map <std::string, Filter> NameFilterMap;

double ApplyFilter (const NameFilterMap& theFilters, 
                    const size_t theNumOfThreads,
                    const std::string& theFileName,
                    const std::string& theFilterName,
                    Table& theTable,
                    bool theIsLogging,
                    const double theNotParallelTime = 0.0)
{
    png::image <png::rgba_pixel> anImage (theFileName);
    int aWidth = anImage.get_width();
    int aHeight = anImage.get_height();
    png::image <png::rgba_pixel> anFilteredImage (aWidth, aHeight);

    auto aStart = std::chrono::steady_clock::now();

    #pragma omp parallel for collapse (2) num_threads (theNumOfThreads)
    for (int y = 0; y < aHeight; ++y) {
        for (int x = 0; x < aWidth; ++x) {
            int r = 0, g = 0, b = 0;

            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    int xn = x + i, yn = y + j;
                    png::rgba_pixel aNeighbour = {0, 0, 0, 255};

                    if (xn >= 0 && yn >= 0 && xn < aWidth && yn < aHeight)
                        aNeighbour = anImage.get_pixel (xn, yn);

                    double aCoef = theFilters.at (theFilterName) [j + 1][i + 1];
                    r += aCoef * aNeighbour.red;
                    g += aCoef * aNeighbour.green;
                    b += aCoef * aNeighbour.blue;
                }
            }

            r = std::min (std::max(r, 0), 255);
            g = std::min (std::max(g, 0), 255);
            b = std::min (std::max(b, 0), 255);

            png::rgba_pixel pixel (r, g, b, anImage.get_pixel(x, y).alpha);
            anFilteredImage.set_pixel (x, y, pixel);
        }
    }

    auto anEnd = std::chrono::steady_clock::now();
    auto aTime = std::chrono::duration <double> (anEnd - aStart).count();

    if (theIsLogging) {
        std::string anOutputFileName = std::to_string (aWidth) + "x" + std::to_string (aHeight) + "_" + theFilterName + ".png";
        anFilteredImage.write (anOutputFileName);
        std::cout << "Processed " << theFileName << " with " << theFilterName << " filter in " << aTime << " seconds using " << theNumOfThreads << " threads" << std::endl;
    }
    
    theTable.addRow ({std::to_string (theNumOfThreads),
                      std::to_string (aWidth) + "x" + std::to_string (aHeight),
                      theFilterName,
                      std::to_string (aTime),
                      std::to_string (theNotParallelTime / aTime),
                      std::to_string ((theNotParallelTime / aTime) / theNumOfThreads)});

    return aTime;
}

int main() {
    const NameFilterMap aFilters = {
        {"Negative", {{-1, -1, -1},
                      {-1,  8, -1},
                      {-1, -1, -1}}},
        {"Sharp", {{0, -1,  0},
                   {-1, 5, -1},
                   {0, -1,  0}}},
        {"Blur", {{1.0 / 9., 1.0 / 9., 1.0 / 9.},
                  {1.0 / 9., 1.0 / 9., 1.0 / 9.},
                  {1.0 / 9., 1.0 / 9., 1.0 / 9.}}}
    };
    const std::string anImgDir = std::filesystem::current_path().string() + "/../Lab_3/img/";
    const std::string anImages [] = {anImgDir + "360x437.png",
                                     anImgDir + "1200x1200.png",
                                     anImgDir + "3000x3000.png",
                                     anImgDir + "6192x5785.png"};

    Table aTable;
    aTable.setHeaders({"Threads", "Image Resolution", "Filter", "Time", "Speedup", "Efficiency"});

    std::array <std::string, 3> aFiltersName = {"Negative", "Sharp", "Blur"};

    std::vector <double> aNotParallelTime;
    for (const auto& anImage : anImages) {
        for (const auto& aFilterName : aFiltersName) {
            aNotParallelTime.emplace_back (ApplyFilter (aFilters,
                                                        1,
                                                        anImage,
                                                        aFilterName,
                                                        aTable,
                                                        false));
        }
    }

    for (size_t aThreads = 2; aThreads <= 16; aThreads *= 2) {
        size_t anIndex = 0;
        for (const auto& anImage : anImages) {
            for (const auto& aFilterName : aFiltersName) {
                ApplyFilter (aFilters,
                             aThreads,
                             anImage,
                             aFilterName,
                             aTable,
                             aThreads == 16 ? true : false,
                             aNotParallelTime [anIndex]);
                anIndex++;
            }
        }
    }
    
    aTable.PrintInTerminal();
    aTable.PrintForWord();

    return 0;
}
