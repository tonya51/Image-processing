#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

IplImage* insertNoise(IplImage* img, CvSize size){
	
	IplImage* imgWithGaussianNoise;
	IplImage* noise;
	CvRNG rng; //randon number genetator
	float mean, deviation;
	
	printf("Insert the mean of the Gaussian noise: ");
	scanf("%f", &mean);
	printf("Insert the deviation of the Gaussian noise: ");
	scanf("%f", &deviation);
	noise = cvCreateImage(size, 8, 1); //create noise
	imgWithGaussianNoise = cvCreateImage(size, 8, 1);
	rng = cvRNG(0xffffffff);
	
	cvRandArr(&rng, noise, CV_RAND_NORMAL, cvScalarAll(mean), cvScalarAll(deviation)); //fill the noise image with random numbers following normal distribution
	cvAdd(img, noise, imgWithGaussianNoise, NULL); //add the original image and the noise

	return imgWithGaussianNoise;

}

int orderValues(int values[], int a, int rank){

	//bubble sort the values of the selected pixel and its neighboring ones
	
	int i, j, temp;
	
	for(i = a-1; i > 0; i--){
		for(j = 1; j <= i; j++){
			if(values[j-1] > values[j]){
				temp = values[j-1];
				values[j-1] = values[j];
				values[j] = temp;
			}
		}
	}

	return values[rank];

}

IplImage* removeNoise(IplImage* imgWithGaussianNoise, CvSize size){

	int row, column, i, j;
	int values[9], a, rank;
	IplImage* smoothedImg;

	do{
		printf("Insert a rank from 0 to 8 for the rank order filter: ");
		scanf("%d", &rank);
	}while(rank < 0 || rank > 8);
	smoothedImg = cvCreateImage(size, 8, 1);

	for(row = 0; row < imgWithGaussianNoise->height; row++){
		for(column = 0; column < imgWithGaussianNoise->width; column++){
			a = 0;
			for(i = -1; i <= 1; i++){
				for(j = -1; j <= 1; j++){
					if((row+i) >= 0 && (row+i) < imgWithGaussianNoise->height && (column+j) >= 0 && (column+j) < imgWithGaussianNoise->width){
						values[a] = CV_IMAGE_ELEM(imgWithGaussianNoise, uchar, row+i, column+j); //fill the array with the values of the selected pixel and its neighboring ones
						a++;
					}
				}
			}
			CV_IMAGE_ELEM(smoothedImg, uchar, row, column) = orderValues(values, a, rank); //change the pixel value according to the rank of the rank order filter
		}
	}

	return smoothedImg;
	
}

IplImage* sobelEdgeDetector(IplImage* img, CvSize size){

	IplImage* sobelEdgedImg;
	int filterSize;
	
	do{
		printf("Insert the size of the Sobel filter: ");
		scanf("%d", &filterSize);
	}while(filterSize%2 != 1);
	sobelEdgedImg = cvCreateImage(size, 8, 1);
	
	cvSobel(img, sobelEdgedImg, 0, 1, filterSize); //Sobel filter

	return sobelEdgedImg;

}

IplImage* imageSegmentation(IplImage* img, CvSize size){

	int i, j, threshold;
	IplImage* segmentedImg;

	do{
		printf("Insert the value of the segmentation threshold: ");
		scanf("%d", &threshold);
	}while(threshold < 0 || threshold > 255);
	segmentedImg = cvCreateImage(size, 8, 1);
	
	for(i = 0; i < img->height; i++){
		for(j = 0; j < img->width; j++){
			if(CV_IMAGE_ELEM(img, uchar, i, j) < threshold)
				CV_IMAGE_ELEM(segmentedImg, uchar, i, j) = 0; //black
			else
				CV_IMAGE_ELEM(segmentedImg, uchar, i, j) = 255; //white
		}
	}

	return segmentedImg;

}

int main(){
	
	IplImage* img;
	IplImage* imgWithGaussianNoise;
	IplImage* smoothedImg;
	IplImage* sobelEdgedImg;
	IplImage* segmentedImg;
	CvSize size;
	char filename[100];

	printf("Insert the name of your image: ");
	scanf("%s", filename);
	
	img = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE); //load image as grayscale
	size = cvGetSize(img);
	cvNamedWindow("Original image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Original image", img);
	printf("Press any key to close the window.\n\n");
	cvWaitKey(0);
	cvDestroyWindow("Original image");
	
	printf("Additive Gaussian noise.\n");
	imgWithGaussianNoise = insertNoise(img, size);
	cvNamedWindow("Image with Gaussian noise", CV_WINDOW_AUTOSIZE);
	cvShowImage("Image with Gaussian noise", imgWithGaussianNoise);
	printf("Press any key to close the window.\n\n");
	cvWaitKey(0);
	cvDestroyWindow("Image with Gaussian noise");

	printf("Noise removal using a rank order filter.\n");
	smoothedImg = removeNoise(imgWithGaussianNoise, size);
	cvNamedWindow("Filtered image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Filtered image", smoothedImg);
	printf("Press any key to close the window.\n\n");
	cvWaitKey(0);
	cvDestroyWindow("Filtered image");

	printf("Detection of edges using a Sobel filter.\n");
	sobelEdgedImg = sobelEdgeDetector(img, size);
	cvNamedWindow("Detected edges", CV_WINDOW_AUTOSIZE);
	cvShowImage("Detected edges", sobelEdgedImg);
	printf("Press any key to close the window.\n\n");
	cvWaitKey(0);
	cvDestroyWindow("Detected edges");

	printf("Segmentation of the image into a binary one with adaptive thresholding.\n");
	segmentedImg = imageSegmentation(img, size);
	cvNamedWindow("Segmented image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Segmented image", segmentedImg);
	printf("Press any key to close the window.\n\n");
	cvWaitKey(0);
	cvDestroyWindow("Segmented image");
	
	system("pause");
	return 0;

}