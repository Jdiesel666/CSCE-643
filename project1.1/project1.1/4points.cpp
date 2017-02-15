/*Problem 1.1*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;

void printM(Mat m);

//float in[8] = { 696, 101, 1240, 150, 756, 1522, 1253, 1361 };	// coordinates in original image
//float out[8] = { 696, 101, 1250, 101, 696, 1522, 1250, 1522};   // coordinates in output image

float in[8] = { 859, 529, 1362, 470, 843, 1720, 1300, 1895 };	  // coordinates in original image
float out[8] = { 859, 529, 1380, 529, 859, 1720, 1380, 1720 };  // coordinates in output image

Mat myImg;

/* 1________2
   |        |
   |        |
   |        |
   3________4*/

void getCoord(int event, int x, int y, int flags, void* parameter)  // locate pixel coordinates in a loaded image
{
	
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		cout << "Point: [" << x << ", " << y << "]" << endl;	// display original coordinates
		circle(myImg, Point(x, y), 8, CV_RGB(255, 0, 0), -1);	// mark the selections in red
		imshow("Original", myImg);
	}
	
} 
	
void printM(Mat m)  // display a matrix
{
	int rows = m.rows;
	int cols = m.cols;
	for (int R = 0; R < rows; R++) 
	{
		for (int C = 0; C < cols; C++) 
		{
			cout <<m.at<float>(R, C)<<"\t";
		}
		cout << endl;
	}
}

int main()
{
	
	namedWindow("Original", CV_WINDOW_NORMAL);  // create window 
	
	//resizeWindow("Original", 800, 500);		    // resize the window for image 1
	//myImg = imread("project101.jpg");		    // load image
	
	myImg = imread("project102.jpg");
	resizeWindow("Original", 600, 600);

	imshow("Original", myImg);				    // display the image
	//cvSetMouseCallback("Original", getCoord);
	//waitKey(0);
	
	// build matrix and calculate H, Hinverse
	float matrix[64] = {in[0], in[1], 1, 0, 0, 0,
		-out[0] * in[0], -out[0] * in[1],
		0, 0, 0, in[0], in[1], 1,
		-out[1] * in[0], -out[1] * in[1],
		in[2], in[3], 1, 0, 0, 0,
		-out[2] * in[2], -out[2] * in[3],
		0, 0, 0, in[2], in[3], 1,
		-out[3] * in[2], -out[3] * in[3],
		in[4], in[5], 1, 0, 0, 0,
		-out[4] * in[4], -out[4] * in[5],
		0, 0, 0, in[4], in[5], 1,
		-out[5] * in[4], -out[5] * in[5],
		in[6], in[7], 1, 0, 0, 0,
		-out[6] * in[6], -out[6] * in[7],
		0, 0, 0, in[6], in[7], 1,
		-out[7] * in[6], -out[7] * in[7]};
	
	Mat M = Mat(8, 8, CV_32F, matrix);	// build matrix
	Mat pixel = Mat(8, 1, CV_32F, out);	// matrix for the output pixel coordiantes
	Mat_<float> Hom = M.inv() * pixel;	// calculate the elements in H
				
	printM(M);
	//printM(pixel);
				
	Hom.push_back(float(1));			// add 1 as H33
	Mat H = Hom.reshape(1, 3);			// H matrix formation
	Mat Hinv = H.inv();					// inverse H matric

	
	// display the calculation results
	printf("H= \n");
	printM(H);
	printf("\nHinv= \n");
	printM(Hinv);

	Mat rectify(myImg.rows, myImg.cols, myImg.type()); // create rectified image
	warpPerspective(myImg, rectify, H, myImg.size()*2);  // perform transformation
	namedWindow("Rectified", CV_WINDOW_NORMAL);		   // create output window
	resizeWindow("Rectified", 800, 500);
	//resizeWindow("Rectified", 600, 500);
	imshow("Rectified", rectify);					   // diplayed rectified image
	//imwrite("Four Point Rec 1.jpg", rectify);		   // save image 1
	//imwrite("Four Point Rec 2.jpg", rectify);
	waitKey(0); 
	
	return(0);

}


