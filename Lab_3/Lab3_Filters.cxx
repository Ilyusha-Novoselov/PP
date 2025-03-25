#include <array>
#include <omp.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <limits>

#include <png.hpp>
#include <Table.hxx>

typedef std::vector <std::vector <double>> Filter;
typedef std::unordered_map <std::string, Filter> NameFilterMap;

Filter GenerateBlurKernel (int theSize) {
    double aValue = 1.0 / (theSize * theSize);
    Filter aKernel (theSize, std::vector<double>(theSize, aValue));
    return aKernel;
}

Filter GenerateSharpKernel (int theSize) {
    Filter aKernel (theSize, std::vector <double> (theSize, -1));
    int aCenter = theSize / 2;
    aKernel [aCenter][aCenter] = theSize * theSize;
    return aKernel;
}

Filter GenerateNegativeKernel (int theSize) {
    Filter aKernel (theSize, std::vector <double> (theSize, -1));
    int aCenter = theSize / 2;
    aKernel [aCenter][aCenter] = theSize * theSize - 1;
    return aKernel;
}

int SelectKernelSize (int theWidth, int theHeight) {
    int aMinDim = std::min (theWidth, theHeight);
    if (aMinDim <= 500) {
        return 5;
    }
    if (aMinDim <= 1500) {
        return 7;
    } 
    if (aMinDim <= 4000) {
        return 9;
    }
    return 11;
}

double ApplyFilter (const size_t theNumOfThreads,
                    const std::string& theFileName,
                    const std::string& theFilterName,
                    Table& theTable,
                    bool theIsLogging,
                    const double theNotParallelTime = 0.0)
{
    png::image <png::rgba_pixel> anImage (theFileName);
    int aWidth = anImage.get_width();
    int aHeight = anImage.get_height();

    int aKernelSize = SelectKernelSize (aWidth, aHeight);
    NameFilterMap aFilters;
    aFilters ["Blur"] = GenerateBlurKernel (aKernelSize);
    aFilters ["Sharp"] = GenerateSharpKernel (aKernelSize);
    aFilters ["Negative"] = GenerateNegativeKernel (aKernelSize);

    png::image <png::rgba_pixel> anFilteredImage (aWidth, aHeight);

    auto aStart = std::chrono::steady_clock::now();
    int anOffset = aKernelSize / 2;

    #pragma omp parallel num_threads (theNumOfThreads)
    {
        #pragma omp for collapse(2) schedule (static)
        for (int y = 0; y < aHeight; ++y) {
            for (int x = 0; x < aWidth; ++x) {
                double r = 0, g = 0, b = 0;

                const Filter& aKernel = aFilters [theFilterName];

                for (int j = -anOffset; j <= anOffset; ++j) {
                    for (int i = -anOffset; i <= anOffset; ++i) {
                        int xn = x + i, yn = y + j;
                        png::rgba_pixel aNeighbour = {0, 0, 0, 255};

                        if (xn >= 0 && yn >= 0 && xn < aWidth && yn < aHeight)
                            aNeighbour = anImage.get_pixel (xn, yn);

                        double aCoef = aKernel [j + anOffset][i + anOffset];
                        r += aCoef * aNeighbour.red;
                        g += aCoef * aNeighbour.green;
                        b += aCoef * aNeighbour.blue;
                    }
                }

                r = std::min (std::max (static_cast <int>(r), 0), 255);
                g = std::min (std::max (static_cast <int>(g), 0), 255);
                b = std::min (std::max (static_cast <int>(b), 0), 255);

                png::rgba_pixel pixel (r, g, b, anImage.get_pixel (x, y).alpha);
                anFilteredImage.set_pixel (x, y, pixel);
            }
        }
    }

    auto anEnd = std::chrono::steady_clock::now();
    auto aTime = std::chrono::duration <double> (anEnd - aStart).count();

    if (theIsLogging) {
        std::string anOutputFileName = std::to_string (aWidth) + "x" + std::to_string (aHeight) + "_" + theFilterName + ".png";
        anFilteredImage.write (anOutputFileName);
    }

    double aSpeedup = theNotParallelTime / aTime;
    double anEfficiency = aSpeedup / theNumOfThreads;
    theTable.addRow ({std::to_string (theNumOfThreads),
                      std::to_string (aWidth) + "x" + std::to_string (aHeight),
                      theFilterName,
                      std::to_string (aTime),
                      std::abs (aSpeedup) < std::numeric_limits <double>::min() ? "-" : std::to_string (aSpeedup),
                      std::abs (anEfficiency) < std::numeric_limits <double>::min() ? "-" : std::to_string (anEfficiency)});

    return aTime;
}

int main() {
    const std::string anImgDir = std::filesystem::current_path().string() + "/../Lab_3/img/";
    const std::vector <std::string> anImages = {
        anImgDir + "360x437.png",
        anImgDir + "1200x1200.png",
        anImgDir + "3000x3000.png",
        anImgDir + "6192x5785.png"
    };

    Table aTable;
    aTable.setHeaders ({"Threads", "Image Resolution", "Filter", "Time", "Speedup", "Efficiency"});

    std::array <std::string, 3> aFiltersName = {"Negative", "Sharp", "Blur"};

    std::unordered_map <std::string, double> aNotParallelTime;
    for (const auto& anImage : anImages) {
        for (const auto& aFilterName : aFiltersName) {
            aNotParallelTime [anImage + aFilterName] = ApplyFilter (1,
                                                                    anImage,
                                                                    aFilterName,
                                                                    aTable,
                                                                    false);
        }
    }

    for (const auto& anImage : anImages) {
        for (const auto& aFilterName : aFiltersName) {
            for (size_t aThreads = 2; aThreads <= 16; aThreads *= 2) {
                ApplyFilter (aThreads,
                             anImage,
                             aFilterName,
                             aTable,
                             aThreads == 16,
                             aNotParallelTime [anImage + aFilterName]);
            }
        }
    }

    aTable.PrintInTerminal();
    aTable.PrintForWord();

    return 0;
}
