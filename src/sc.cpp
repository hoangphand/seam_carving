#include "sc.h"
#include <cmath>
#include <algorithm>

using namespace cv;
using namespace std;

bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}

bool seam_carving_greedy(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }
    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}

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

int** calculate_vertical_seams(int** pixel_energy, int width, int height) {
    int** result = new int*[height];
    int top_left, top, top_right;

    for (int i = 0; i < height; i++) {
        result[i] = new int[width];
        for (int j = 0; j < width; j++) {
            if (i == 0) {
                result[i][j] = pixel_energy[i][j];
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

                result[i][j] = pixel_energy[i][j] + result[i - 1][coordinate_min];
            }
        }
    }

    return result;
}

int** calculate_horizontal_seams(int** pixel_energy, int width, int height) {
    int** result = new int*[height];
    int left_top, left, left_bottom;

    for (int i = 0; i < height; i++) {
        result[i] = new int[width];
    }

    for (int j = 0; j < width; j++) {
        for (int i = 0; i < height; i++) {
            if (j == 0) {
                result[i][j] = pixel_energy[i][j];
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

                result[i][j] = pixel_energy[i][j] + result[coordinate_min][j - 1];
            }
        }
    }

    return result;
}

int get_lowest_energy_vertical_point(int** vertical_seams, int width, int height) {
    int min_cumulative = INT_MAX;
    int starting_point;

    for (int i = 0; i < width; i++) {
        if (vertical_seams[height - 1][i] < min_cumulative) {
            min_cumulative = vertical_seams[height - 1][i];
            starting_point = i;
        }
    }

    return starting_point;
}

int get_lowest_energy_horizontal_point(int** horizontal_seams, int width, int height) {
    int min_cumulative = INT_MAX;
    int starting_point;

    for (int i = 0; i < height; i++) {
        if (horizontal_seams[i][width - 1] < min_cumulative) {
            min_cumulative = horizontal_seams[i][width - 1];
            starting_point = i;
        }
    }

    return starting_point;
}

int* find_lowest_energy_vertical_seam(int** pixel_energy, int width, int height, int starting_point) {
    int* result = new int[height];

    result[height - 1] = starting_point;
    int current_point = starting_point;
    int top_left, top, top_right;

    for (int i = height - 2; i >= 0; i--) {
        top_left = (current_point == 0) ? INT_MAX : pixel_energy[i][current_point - 1];
        top = pixel_energy[i][current_point];
        top_right = (current_point == width - 1) ? INT_MAX : pixel_energy[i][current_point + 1];

        int min = std::min(top, std::min(top_left, top_right));

        if (min == top_left) {
            current_point = current_point - 1;
        } else if (min == top_right) {
            current_point = current_point + 1;
        }

        result[i] = current_point;
    }

    return result;
}

int* find_lowest_energy_horizontal_seam(int** pixel_energy, int width, int height, int starting_point) {
    int* result = new int[width];
    int min_cumulative = INT_MAX;

    result[width - 1] = starting_point;
    int current_point = starting_point;
    int left_top, left, left_bottom;

    for (int i = width - 2; i >= 0; i--) {
        left_top = (current_point == 0) ? INT_MAX : pixel_energy[current_point - 1][i];
        left = pixel_energy[current_point][i];
        left_bottom = (current_point == height - 1) ? INT_MAX : pixel_energy[current_point + 1][i];

        int min = std::min(left, std::min(left_bottom, left_top));

        if (min == left_top) {
            current_point = current_point - 1;
        } else if (min == left_bottom) {
            current_point = current_point + 1;
        }

        result[i] = current_point;
    }

    return result;
}

// seam carves by removing trivial seams
bool my_seam_carving_greedy(Mat& in_image, int new_width, int new_height, Mat& out_image) {
    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();

    while(iimage.rows != new_height || iimage.cols != new_width) {
        cout<<"New rows: "<<iimage.rows<<endl;
        cout<<"New cols: "<<iimage.cols<<endl;
        if(iimage.cols > new_width) {
            reduce_horizontal_seam_greedy(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.rows > new_height) {
            reduce_vertical_seam_greedy(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

bool reduce_horizontal_seam_greedy(Mat& in_image, Mat& out_image) {
    int rows = in_image.rows;
    int cols = in_image.cols - 1;

    int** pixel_energy = calculate_pixel_energy(in_image);
    int** vertical_seams = calculate_vertical_seams(pixel_energy, in_image.cols, in_image.rows);
    int starting_vertical_point = get_lowest_energy_vertical_point(vertical_seams, in_image.cols, in_image.rows);
    int* lowest_vertical_seam = find_lowest_energy_vertical_seam(pixel_energy, in_image.cols, in_image.rows, starting_vertical_point);
    
    // cout<<"Pixel energy:"<<endl;
    // for (int i = 0; i < in_image.rows; i++) {
    //     for (int j = 0; j < in_image.cols; j++) {
    //         cout<<pixel_energy[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    
    // cout<<"Vertical seams:"<<endl;
    // for (int i = 0; i < in_image.rows; i++) {
    //     for (int j = 0; j < in_image.cols; j++) {
    //         cout<<vertical_seams[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }

    // cout<<"Lowest vertical seam:"<<endl;
    // for (int i = 0; i < in_image.rows; i++) {
    //     cout<<lowest_vertical_seam[i]<<" ";
    // }
    // cout<<endl;

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

    // for (int i = 0; i < out_image.rows; i++) {
    //     for (int j = 0; j < out_image.cols; j++) {
    //         cout<<out_image.at<Vec3b>(i, j)<<" ";
    //     }
    //     cout<<endl;
    // }

    return true;
}

bool reduce_vertical_seam_greedy(Mat& in_image, Mat& out_image) {
    int rows = in_image.rows - 1;
    int cols = in_image.cols;

    int** pixel_energy = calculate_pixel_energy(in_image);
    int** horizontal_seams = calculate_horizontal_seams(pixel_energy, in_image.cols, in_image.rows);
    int starting_horizontal_point = get_lowest_energy_horizontal_point(horizontal_seams, in_image.cols, in_image.rows);
    int* lowest_horizontal_seam = find_lowest_energy_horizontal_seam(pixel_energy, in_image.cols, in_image.rows, starting_horizontal_point);
    
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

// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    while(iimage.rows!=new_height || iimage.cols!=new_width){
        // horizontal seam if needed
            cout<<"Current rows:"<<iimage.rows;
            cout<<"Current cols:"<<iimage.cols;

        if(iimage.rows>new_height){
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.cols>new_width){
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){

    // retrieve the dimensions of the new image
    int rows = in_image.rows-1;
    int cols = in_image.cols;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);
    
    //populate the image
    int middle = in_image.rows / 2;
    
    for(int i=0;i<=middle;++i)
        for(int j=0;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            
            /* at operator is r/w
            pixel[0] = 255;
            pixel[1] =255;
            pixel[2]=255;
            */
            
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    for(int i=middle+1;i<rows;++i)
        for(int j=0;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i+1, j);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }

    return true;
}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
    // retrieve the dimensions of the new image
    int rows = in_image.rows;
    int cols = in_image.cols-1;
    
    // create an image slighly smaller
    out_image = Mat(rows, cols, CV_8UC3);
    
    //populate the image
    int middle = in_image.cols / 2;
    
    for(int i=0;i<rows;++i)
        for(int j=0;j<=middle;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    for(int i=0;i<rows;++i)
        for(int j=middle+1;j<cols;++j){
            Vec3b pixel = in_image.at<Vec3b>(i, j+1);
            
            /* at operator is r/w
             pixel[0] --> red
             pixel[1] --> green
             pixel[2] --> blue
             */
            
            
            out_image.at<Vec3b>(i,j) = pixel;
        }
    
    return true;
}
