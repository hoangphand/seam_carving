#include "sc.h"
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

int** calculate_pixel_energy(Mat& in_image){
    int width = in_image.cols;
    int height = in_image.rows;
    int** result = new int*[height];

    for (int i = 0; i < height; i++) {
        result[i] = new int[width];
        int top_y = (i == 0) ? height - 1 : i - 1;
        int bottom_y = (i == height - 1) ? 0 : i + 1;

        for (int j = 0; j < width; j++) {
            int left_x = (j == 0) ? width - 1 : j - 1;
            int right_x = (j == width - 1) ? right_x = 0 : j + 1;

            Vec3b pixel_left_x = in_image.at<Vec3b>(i, left_x);
            Vec3b pixel_right_x = in_image.at<Vec3b>(i, right_x);
            Vec3b pixel_top_y = in_image.at<Vec3b>(top_y, j);
            Vec3b pixel_bottom_y = in_image.at<Vec3b>(bottom_y, j);

            int square_delta_x = pow(abs(pixel_left_x[0] - pixel_right_x[0]), 2) + pow(abs(pixel_left_x[1] - pixel_right_x[1]), 2) + pow(abs(pixel_left_x[2] - pixel_right_x[2]), 2);
            int square_delta_y = pow(abs(pixel_top_y[0] - pixel_bottom_y[0]), 2) + pow(abs(pixel_top_y[1] - pixel_bottom_y[1]), 2) + pow(abs(pixel_top_y[2] - pixel_bottom_y[2]), 2);

            result[i][j] = square_delta_x + square_delta_y;
        }
    }

    return result;
}

int* calculate_vertical_seams(int** pixel_energy, int width, int height) {
    int** vertical_seams = new int*[height];
    int** trace = new int*[height];
    int* lowest_vertical_seam = new int[height];
    int top_left, top, top_right;

    // calculate seams
    for (int i = 0; i < height; i++) {
        vertical_seams[i] = new int[width];
        trace[i] = new int[width];

        for (int j = 0; j < width; j++) {
            if (i == 0) {
                vertical_seams[i][j] = pixel_energy[i][j];
                trace[i][j] = 0;
            } else {
                top_left = (j == 0) ? INT_MAX : pixel_energy[i - 1][j - 1];
                top = pixel_energy[i - 1][j];
                top_right = (j == width - 1) ? INT_MAX : pixel_energy[i - 1][j + 1];

                int min = std::min(top, std::min(top_left, top_right));

                int coordinate_min;
                if (min == top_left) {
                    coordinate_min = j - 1;
                } else if (min == top) {
                    coordinate_min = j;
                } else if (min == top_right) {
                    coordinate_min = j + 1;
                }

                vertical_seams[i][j] = pixel_energy[i][j] + vertical_seams[i - 1][coordinate_min];
                trace[i][j] = coordinate_min - j;
            }
        }
    }

    // determine col with lowest seam energy
    int min_cumulative = INT_MAX;
    int starting_point;

    for (int i = 0; i < width; i++) {
        if (vertical_seams[height - 1][i] < min_cumulative) {
            min_cumulative = vertical_seams[height - 1][i];
            starting_point = i;
        }
    }

    // store path of lowest energy seam
    lowest_vertical_seam[height - 1] = starting_point;

    for (int i = height - 2; i >= 0; i--) {
        lowest_vertical_seam[i] = lowest_vertical_seam[i + 1] + trace[i + 1][lowest_vertical_seam[i + 1]];
    }

    return lowest_vertical_seam;
}

int* calculate_horizontal_seams(int** pixel_energy, int width, int height) {
    int** horizontal_seams = new int*[height];
    int** trace = new int*[height];
    int* lowest_horizontal_seam = new int[width];
    int left_top, left, left_bottom;

    // calculate seams
    for (int i = 0; i < height; i++) {
        horizontal_seams[i] = new int[width];
        trace[i] = new int[width];
    }

    for (int j = 0; j < width; j++) {
        for (int i = 0; i < height; i++) {
            if (j == 0) {
                horizontal_seams[i][j] = pixel_energy[i][j];
                trace[i][j] = 0;
            } else {
                left_top = (i == 0) ? INT_MAX : pixel_energy[i - 1][j - 1];
                left = pixel_energy[i][j - 1];
                left_bottom = (i == height - 1) ? INT_MAX : pixel_energy[i + 1][j - 1];

                int min = std::min(left, std::min(left_bottom, left_top));

                int coordinate_min;
                if (min == left_top) {
                    coordinate_min = i - 1;
                } else if (min == left) {
                    coordinate_min = i;
                } else if (min == left_bottom) {
                    coordinate_min = i + 1;
                }

                horizontal_seams[i][j] = pixel_energy[i][j] + horizontal_seams[coordinate_min][j - 1];
                trace[i][j] = coordinate_min - i;
            }
        }
    }

    // determine col with lowest seam energy
    int min_cumulative = INT_MAX;
    int starting_point;

    for (int i = 0; i < height; i++) {
        if (horizontal_seams[i][width - 1] < min_cumulative) {
            min_cumulative = horizontal_seams[i][width - 1];
            starting_point = i;
        }
    }

    // store path of lowest energy seam
    lowest_horizontal_seam[width - 1] = starting_point;

    for (int i = width - 2; i >= 0; i--) {
        lowest_horizontal_seam[i] = lowest_horizontal_seam[i + 1] + trace[lowest_horizontal_seam[i + 1]][i + 1];
    }

    return lowest_horizontal_seam;
}

// seam carves by removing trivial seams
bool my_seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image) {
    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();

    while(iimage.rows != new_height || iimage.cols != new_width) {
        cout<<"New rows: "<<iimage.rows<<endl;
        cout<<"New cols: "<<iimage.cols<<endl;
        if(iimage.cols > new_width) {
            reduce_horizontal_seam(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.rows > new_height) {
            reduce_vertical_seam(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

bool reduce_horizontal_seam(Mat& in_image, Mat& out_image) {
    int rows = in_image.rows;
    int cols = in_image.cols - 1;

    int** pixel_energy = calculate_pixel_energy(in_image);
    int* lowest_vertical_seam = calculate_vertical_seams(pixel_energy, in_image.cols, in_image.rows);

    out_image = Mat(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; i++) {
        int sep = lowest_vertical_seam[i];

        for (int j = 0; j < sep; j++) {
            Vec3b pixel = in_image.at<Vec3b>(i, j);

            out_image.at<Vec3b>(i, j) = pixel;
        }

        for (int j = sep; j < cols; j++) {
            Vec3b pixel = in_image.at<Vec3b>(i, j + 1);

            out_image.at<Vec3b>(i, j) = pixel;
        }
    }

    return true;
}

bool reduce_vertical_seam(Mat& in_image, Mat& out_image) {
    int rows = in_image.rows - 1;
    int cols = in_image.cols;

    int** pixel_energy = calculate_pixel_energy(in_image);
    int* lowest_horizontal_seam = calculate_horizontal_seams(pixel_energy, in_image.cols, in_image.rows);
    
    out_image = Mat(rows, cols, CV_8UC3);

    for (int i = 0; i < cols; i++) {
        int sep = lowest_horizontal_seam[i];

        for (int j = 0; j < sep; j++) {
            Vec3b pixel = in_image.at<Vec3b>(j, i);

            out_image.at<Vec3b>(j, i) = pixel;
        }

        for (int j = sep; j < rows; j++) {
            Vec3b pixel = in_image.at<Vec3b>(j + 1, i);

            out_image.at<Vec3b>(j, i) = pixel;
        }
    }

    return true;
}