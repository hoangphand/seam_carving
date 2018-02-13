#include <opencv2/opencv.hpp>

#include "sc.h"
using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if(argc!=5){
        cout<<"Usage: ../sc input_image new_width new_height output_image"<<endl;
        return -1;
    }
    
    // Load the input image
    // the image should be a 3 channel image by default but we will double check that in teh seam_carving
    Mat in_image;
    in_image = imread(argv[1]/*, CV_LOAD_IMAGE_COLOR*/);
   
    if(!in_image.data)
    {
        cout<<"Could not load input image!!!"<<endl;
        return -1;
    }

    if(in_image.channels()!=3){
        cout<<"Image does not have 3 channels!!! "<<in_image.depth()<<endl;
        return -1;
    }
    
    // get the new dimensions from the argument list
    int new_width = atoi(argv[2]);
    int new_height = atoi(argv[3]);
    
    // the output image
    Mat out_image;
    
    // if(!seam_carving(in_image, new_width, new_height, out_image)){
    //     return -1;
    // }
    Mat test_image = Mat(4, 3, CV_8UC3);
    test_image.at<Vec3b>(0, 0) = Vec3b(255, 101, 51);
    test_image.at<Vec3b>(0, 1) = Vec3b(255, 101, 153);
    test_image.at<Vec3b>(0, 2) = Vec3b(255, 101, 255);
    test_image.at<Vec3b>(1, 0) = Vec3b(255, 153, 51);
    test_image.at<Vec3b>(1, 1) = Vec3b(255, 153, 153);
    test_image.at<Vec3b>(1, 2) = Vec3b(255, 153, 255);
    test_image.at<Vec3b>(2, 0) = Vec3b(255, 203, 51);
    test_image.at<Vec3b>(2, 1) = Vec3b(255, 204, 153);
    test_image.at<Vec3b>(2, 2) = Vec3b(255, 205, 255);
    test_image.at<Vec3b>(3, 0) = Vec3b(255, 255, 51);
    test_image.at<Vec3b>(3, 1) = Vec3b(255, 255, 153);
    test_image.at<Vec3b>(3, 2) = Vec3b(255, 255, 255);

    int** pixel_energy = calculate_pixel_energy(test_image);

    cout<<"Pixel energy:"<<endl;
    for (int i = 0; i < test_image.rows; i++) {
        for (int j = 0; j < test_image.cols; j++) {
            cout<<pixel_energy[i][j]<<" ";
        }
        cout<<endl;
    }
    
    cout<<"Vertical seams:"<<endl;
    int** vertical_seams = calculate_vertical_seams(pixel_energy, test_image.cols, test_image.rows);
    for (int i = 0; i < test_image.rows; i++) {
        for (int j = 0; j < test_image.cols; j++) {
            cout<<vertical_seams[i][j]<<" ";
        }
        cout<<endl;
    }

    cout<<"Lowest vertical seam:"<<endl;
    int starting_vertical_point = get_lowest_energy_vertical_point(vertical_seams, test_image.cols, test_image.rows);
    int* lowest_vertical_seam = find_lowest_energy_vertical_seam(pixel_energy, test_image.cols, test_image.rows, starting_vertical_point);
    for (int i = 0; i < test_image.rows; i++) {
        cout<<lowest_vertical_seam[i]<<" ";
    }
    cout<<endl;
    
    cout<<"Horizontal seams:"<<endl;
    int** horizontal_seams = calculate_horizontal_seams(pixel_energy, test_image.cols, test_image.rows);
    for (int i = 0; i < test_image.rows; i++) {
        for (int j = 0; j < test_image.cols; j++) {
            cout<<horizontal_seams[i][j]<<" ";
        }
        cout<<endl;
    }

    // write it on disk
    // imwrite( argv[4], out_image);
    
    // also display them both
    
    // namedWindow( "Original image", WINDOW_AUTOSIZE );
    // namedWindow( "Seam Carved Image", WINDOW_AUTOSIZE );
    // imshow( "Original image", in_image );
    // imshow( "Seam Carved Image", out_image );
    // waitKey(0);
    return 0;
}
