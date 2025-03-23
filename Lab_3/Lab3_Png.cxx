#include <omp.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>

#include <png.hpp>
#include <Table.hxx>

using namespace std;

// Фильтры
const float filt_negative[3][3] = {
    {-1, -1, -1},
    {-1, 8, -1},
    {-1, -1, -1}}; // Выделение границ (различие будет хорошо заметно)

const float filt_sharp[3][3] = {
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0}}; // Классическая матрица резкости

const float filt_blur[3][3] = {
    {1.0 / 9, 1.0 / 9, 1.0 / 9},
    {1.0 / 9, 1.0 / 9, 1.0 / 9},
    {1.0 / 9, 1.0 / 9, 1.0 / 9}}; // Простое размытие (усреднение)

string base_path = filesystem::current_path().string();  
const string images[] = {base_path + "/../Lab_3/png/360x437.png",
                         base_path + "/../Lab_3/png/1200x1200.png",
                         base_path + "/../Lab_3/png/3000x3000.png",
                         base_path + "/../Lab_3/png/6192x5785.png"};
const pair<string, const float (*)[3][3]> filters[] = {
    {"negative", &filt_negative},
    {"sharp", &filt_sharp},
    {"blur", &filt_blur}};

Table resultsTable;

void apply_filter(const string &filename, const string &filter_name, const float (*filter)[3][3], int threads, bool isLogging) {
    png::image<png::rgba_pixel> image(filename);
    int width = image.get_width();
    int height = image.get_height();
    png::image<png::rgba_pixel> filtered(width, height);

    auto begin = chrono::steady_clock::now();

    #pragma omp parallel for collapse(2) num_threads(threads)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int r = 0, g = 0, b = 0;

            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    int xn = x + i, yn = y + j;
                    png::rgba_pixel neighbour = {0, 0, 0, 255};

                    if (xn >= 0 && yn >= 0 && xn < width && yn < height)
                        neighbour = image.get_pixel(xn, yn);

                    float coef = (*filter)[j + 1][i + 1];
                    r += coef * neighbour.red;
                    g += coef * neighbour.green;
                    b += coef * neighbour.blue;
                }
            }

            r = min(max(r, 0), 255);
            g = min(max(g, 0), 255);
            b = min(max(b, 0), 255);

            png::rgba_pixel pixel(r, g, b, image.get_pixel(x, y).alpha);
            filtered.set_pixel(x, y, pixel);
        }
    }

    auto end = chrono::steady_clock::now();
    auto time = chrono::duration <double>(end - begin).count();

    if (isLogging) {
        string output_filename = filename.substr(0, filename.find_last_of(".")) + "_" + filter_name + ".png";
        filtered.write(output_filename);
        cout << "Processed " << filename << " with " << filter_name << " filter in " << time << " microseconds using " << threads << " threads." << endl;
    }
    
    resultsTable.addRow({to_string(threads), to_string(width) + "x" + to_string(height), filter_name, to_string(time)});
}

int main() {
    resultsTable.setHeaders({"Threads", "Image Resolution", "Filter", "Time"});
    
    int thread_counts[] = {1, 2, 4, 8, 16};
    for (int threads : thread_counts) {
        for (const auto &image : images) {
            for (const auto &[filter_name, filter] : filters) {
                apply_filter(image, filter_name, filter, threads, threads == 16 ? true : false);
            }
        }
    }
    
    resultsTable.PrintInTerminal();
    return EXIT_SUCCESS;
}
