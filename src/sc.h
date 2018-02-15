#ifndef SEAMCARVINGCOMP665156
#define SEAMCARVINGCOMP665156

#include <opencv2/opencv.hpp>

// the function you need to implement - by defaiult it calls seam_carving_trivial
bool seam_carving(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);


bool seam_carving_trivial(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);

bool reduce_horizontal_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);


bool reduce_vertical_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);

int** calculate_pixel_energy(cv::Mat& in_image);

int** calculate_vertical_seams(int** pixel_energy, int width, int height);
int** calculate_horizontal_seams(int** pixel_energy, int width, int height);

bool my_seam_carving_greedy(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);
int get_lowest_energy_vertical_point(int** vertical_seams, int width, int height);
int get_lowest_energy_horizontal_point(int** horizontal_seams, int width, int height);
int* find_lowest_energy_vertical_seam(int** vertical_seams, int width, int height, int starting_point);
int* find_lowest_energy_horizontal_seam(int** horizontal_seams, int width, int height, int starting_point);
bool reduce_vertical_seam_greedy(cv::Mat& in_image, cv::Mat& out_image);
bool reduce_horizontal_seam_greedy(cv::Mat& in_image, cv::Mat& out_image);

#endif
