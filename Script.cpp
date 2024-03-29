#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"

using namespace std;

namespace prog {
    // Use to read color values from a script file.
    istream& operator>>(istream& input, Color& c) {
        int r, g, b;
        input >> r >> g >> b;
        c.red() = r;
        c.green() = g;
        c.blue() = b;
        return input;
    }

    Script::Script(const string& filename) :
            image(nullptr), input(filename) {

    }
    void Script::clear_image_if_any() {
        if (image != nullptr) {
            delete image;
            image = nullptr;
        }
    }
    Script::~Script() {
        clear_image_if_any();
    }

    void Script::run() {
        string command;
        while (input >> command) {
            cout << "Executing command '" << command << "' ..." << endl;
            if (command == "open") {
                open();
                continue;
            }
            if (command == "blank") {
                blank();
                continue;
            }
            // Other commands require an image to be previously loaded.
            if (command == "save") {
                save();
                continue;
            } 
            if (command == "invert") {
                invert();
                continue;
            }
            if (command == "to_gray_scale") {
                to_gray_scale();
                continue;
            }
            if (command == "replace") {
                replace();
                continue;
            }
            if (command == "fill") {
                fill();
                continue;
            }
            if (command == "h_mirror") {
                h_mirror();
                continue;
            }
            if (command == "v_mirror") {
                v_mirror();
                continue;
            }
            if (command == "add"){
                add();
                continue;
            }
            if (command == "crop") {
                crop();
                continue;
            }
            if (command == "rotate_left") {
                rotate_left();
                continue;
            }
            if (command == "rotate_right") {
                rotate_right();
                continue;
            }
            if (command == "median_filter") {
                median_filter();
                continue;
            }
            if (command == "xpm2_open") {
                xpm2_open();
                continue;
            }
            if (command == "xpm2_save") {
                xpm2_save();
                continue;
            }
        }
    }

    void Script::open() {
        // Replace current image (if any) with image read from PNG file.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromPNG(filename);
    }

    void Script::blank() {
        // Replace current image (if any) with blank image.
        clear_image_if_any();
        int w, h;
        Color fill;
        input >> w >> h >> fill;
        image = new Image(w, h, fill);
    }

    void Script::save() {
        // Save current image to PNG file.
        string filename;
        input >> filename;
        saveToPNG(filename, image);
    }
    
    void Script::invert() {
        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                Color color = image->at(x, y);
                image->at(x, y) = Color(
                    255 - color.red(),
                    255 - color.green(),
                    255 - color.blue()
                );
            }
        }
    }

    void Script::to_gray_scale() {
        int r, g, b;
        int v;

        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                r = image->at(x, y).red();
                g = image->at(x, y).green();
                b = image->at(x, y).blue();
                
                v = (r + g + b) / 3;
                image->at(x, y) = Color(v, v, v);
            }
        }
    }
     
    void Script::replace() {
        Color c1, c2;
        input >> c1 >> c2;

        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++) {
                if (image->at(x, y) == c1)
                    image->at(x, y) = c2;
            }
        }
    }

    void Script::fill() {
        int x, y, w, h; 
        Color filler;
        input >> x >> y >> w >> h >> filler;
        for (int i = x; i < x + w; i++) {
            for (int j = y; j < y + h; j++)
                image->at(i, j) = filler;
        }
    }

    void Script::h_mirror() {
        for (int x = 0; x < image->width() / 2; x++) {
            for (int y = 0; y < image->height(); y++) {
                Color temp = image->at(x, y);
                image->at(x, y) = image->at(image->width() - 1 - x, y);
                image->at(image->width() - 1 - x, y) = temp;
            }
        }
    }

    void Script::v_mirror() {
        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height() / 2; y++) {
                Color temp = image->at(x, y);
                image->at(x, y) = image->at(x, image->height()- 1 - y);
                image->at(x, image->height() - 1 - y) = temp;
            }
        }
    }

    void Script::add() {
        string filename;
        int x, y;
        Color neutral;
        input >> filename >> neutral >> x >> y;
        
        int temp = y;
        Image* given_image = loadFromPNG(filename);

        for (int x_given = 0; x_given < given_image->width(); x_given++, x++) {
            for (int y_given = 0; y_given < given_image->height(); y_given++, y++) {
                if (given_image->at(x_given, y_given) != neutral){
                    image->at(x, y) = given_image->at(x_given, y_given);
                }
            }
            y = temp;
        }
        delete given_image;
    }

    void Script::crop() {
        int x, y, w, h;
        input >> x >> y >> w >> h;
        Image* new_image = new Image(w, h);

        for (int nx = 0; nx < w; nx++) {
            for (int ny = 0; ny < h; ny++)
                new_image->at(nx, ny) = image->at(x + nx, y + ny);
        }

        clear_image_if_any();
        image = new_image;
    }
    
    void Script::rotate_left(){
        int w, h;
        w = image->width();
        h = image->height();
        Image* new_image = new Image(h, w);

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++)
                new_image->at(y, w - 1 - x) = image->at(x, y);
        }

        clear_image_if_any();
        image = new_image;
    }

    void Script::rotate_right() {
        int w, h;
        w = image->width();
        h = image->height();
        Image* new_image = new Image(h, w);

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++)
                new_image->at(h - 1 - y, x) = image->at(x, y);
        }

        clear_image_if_any();
        image = new_image;
    }
    
    /**
     * @brief Partially sorts a rgb_value array (like std::nth_element) and
     *        returns the median.
     * @author Bruno Oliveira
     * 
     * @param values Rgb_value array
     * @param n Size of array
     * @return Median
     */
    rgb_value partial_median_sort(rgb_value values[], size_t n) {
        if (n == 0)
            return 0;

        nth_element(values, values + n / 2, values + n);
        if (n % 2 == 1) {
            return values[n / 2];
        }
        nth_element(values, values + n / 2 - 1, values + n / 2);
        return (values[n / 2 - 1] + values[n / 2]) / 2;
    }

    /**
     * @brief Calculates the "median" color of the neighboring pixels of pixel
     *        (x, y) inside a ws * ws window.
     * @author Bruno Oliveira
     * 
     * @param image Image
     * @param x Coordinate x of the pixel
     * @param y Coordinate y of the pixel
     * @param ws Window size
     * @return The color with each component being the respective median in
     *         the window's colors
     */
    Color square_median(Image* image, int x, int y, int ws) {
        int xmin = max(x - ws / 2, 0), xmax = min(x + ws / 2, image->width() - 1),
            ymin = max(y - ws / 2, 0), ymax = min(y + ws / 2, image->height() - 1),
            arr_size = (xmax - xmin + 1) * (ymax - ymin + 1);
        rgb_value *reds = new rgb_value[arr_size],
            *greens = new rgb_value[arr_size],
            *blues = new rgb_value[arr_size];

        for (int nx = xmin, i = 0; nx <= xmax; nx++) {
            for (int ny = ymin; ny <= ymax; ny++, i++) {
                Color color = image->at(nx, ny);
                reds[i] = color.red();
                greens[i] = color.green();
                blues[i] = color.blue();
            }
        }

        rgb_value median_red = partial_median_sort(reds, arr_size),
            median_green = partial_median_sort(greens, arr_size),
            median_blue = partial_median_sort(blues, arr_size);
        delete [] reds;
        delete [] greens;
        delete [] blues;
        return Color(median_red, median_green, median_blue);
    }

    void Script::median_filter() {
        int ws;
        input >> ws;
        Image* new_image = new Image(image->width(), image->height());

        for (int x = 0; x < image->width(); x++) {
            for (int y = 0; y < image->height(); y++)
                new_image->at(x, y) = square_median(image, x, y, ws);
        }

        clear_image_if_any();
        image = new_image;
    }

    void Script::xpm2_open() {
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromXPM2(filename);
    }

    void Script::xpm2_save(){
        string filename;
        input >> filename;
        saveToXPM2(filename, image);
    }
}