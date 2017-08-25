#include "dft.h"

/*
Functions called in main
*/

Mat get_dft(Mat image)
{
	Mat resizedImage;
	Mat complexI;

	int m = getOptimalDFTSize(image.rows);
	int n = getOptimalDFTSize(image.cols);

	//cout << "Expanding the image to an optimal size" << endl;
	copyMakeBorder(image, resizedImage, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));

	//cout << "Making place for both the complex and the real values" << endl;
	Mat planes[] = { Mat_<float>(resizedImage), Mat::zeros(resizedImage.size(), CV_32F) };
	merge(planes, 2, complexI);

	//cout << "Making the Discrete Fourier Transform" << endl;
	dft(complexI, complexI);          

	return complexI;
}

void get_magnitude(Mat complexI)
{
	Mat magnitudeImage;
	Mat planes[] = { Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };

   // cout << "Transforming the real and complex values to magnitude" << endl;
	split(complexI, planes);
	magnitude(planes[0], planes[1], magnitudeImage);

	//cout << "Switching to a logarithmic scale" << endl;
	magnitudeImage += Scalar::all(1);
	log(magnitudeImage, magnitudeImage);

	crop_and_rearrange(magnitudeImage);

	normalize(magnitudeImage, magnitudeImage, 0, 1, NORM_MINMAX);
	imshow("Magnitude spectrum", magnitudeImage);
}

Mat get_idft(Mat complexI) 
{
	Mat outputImage;
	idft(complexI, outputImage);
	Mat planes[] = { Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };
	split(outputImage, planes);

	magnitude(planes[0], planes[1], outputImage);

	normalize(outputImage, outputImage, 0, 1, NORM_MINMAX);
	imshow("Output Image", outputImage);

	return outputImage;
}

Mat action_switch(int action, Mat inputImage)
{
	//int i;
	char* outputFile;
	Mat outputImage;
	Mat complexI;

   switch (action)
   {
		case(1):
			complexI = get_dft(inputImage);
			get_magnitude(complexI);
			break;
		case(2):
			complexI = get_dft(inputImage);
			get_magnitude(complexI);
			outputImage = get_gaussian(complexI, false);
			break;
		case(3):
			complexI = get_dft(inputImage);
			get_magnitude(complexI);
			outputImage = get_gaussian(complexI, true);
			break;
		/*case(4):
			Sobel(inputImage, outputImage, outputImage.type(), 1, 0, 3, 1, 0, BORDER_REFLECT_101);
			imshow("Output Image", outputImage);
			break;
		case(5):
			for (int i = 1; i < MAX_KERNEL_LENGTH - 15; i = i + 2)
			{
				bilateralFilter(inputImage, outputImage, i, i * 2, i / 2);
			}
			imshow("Output Image", outputImage);
			break;
		case(6):
			for (i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
			{
				GaussianBlur(inputImage, outputImage, Size(i + 30, i + 30), 0, 0);
			}
			imshow("Output Image", outputImage);
			break;*/
	}

   printf("Showing images\n");
   cvWaitKey(2000);
   return outputImage;
}

/*
End of Functions called in main
*/

/*
Helper Functions
*/

Mat get_gaussian(Mat complexI, bool invert)
{
	Mat gaussianMask;
	Mat outputImage;
	gaussianMask = createGaussianHighPassFilter(complexI.size(),50, invert);
	imshow("Gsaus-mask", gaussianMask);
	gaussianMask = crop_and_rearrange(gaussianMask);

	Mat planes[] = { Mat_<float>(gaussianMask), Mat::zeros(gaussianMask.size(), CV_32F) };
	Mat kernelSpec;
	merge(planes, 2, kernelSpec);

	mulSpectrums(complexI, kernelSpec, complexI, DFT_ROWS);

	get_magnitude(complexI);
	outputImage = get_idft(complexI);

	return outputImage;
}

Mat crop_and_rearrange(Mat magnitudeImage)
{
	cout << "Transforming imatrix into a viewable image form" << endl;
	cout << "Croping and rearranging" << endl;
	magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));

	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;

	Mat q0(magnitudeImage, Rect(0, 0, cx, cy));   
	Mat q1(magnitudeImage, Rect(cx, 0, cx, cy)); 
	Mat q2(magnitudeImage, Rect(0, cy, cx, cy)); 
	Mat q3(magnitudeImage, Rect(cx, cy, cx, cy));

	Mat temporary;    
	q0.copyTo(temporary);
	q3.copyTo(q0);
	temporary.copyTo(q3);

	q1.copyTo(temporary);
	q2.copyTo(q1);
	temporary.copyTo(q2);

	return magnitudeImage;
}

double pixelDistance(double u, double v)
{
	return cv::sqrt(u*u + v*v);
}

double gaussianCoeff(double u, double v, double d0)
{
	double d = pixelDistance(u, v);
	return 1.0 - cv::exp((-d*d) / (2 * d0*d0));
}

cv::Mat createGaussianHighPassFilter(cv::Size size, double cutoffInPixels, bool invert)
{
	Mat ghpf(size, CV_64F);

	cv::Point center(size.width / 2, size.height / 2);

	for (int u = 0; u < ghpf.rows; u++)
	{
		for (int v = 0; v < ghpf.cols; v++)
		{
		    ghpf.at<double>(u, v) = gaussianCoeff(u - center.y, v - center.x, cutoffInPixels);
			if (invert) ghpf.at<double>(u, v) = 1 - ghpf.at<double>(u, v);
		}
	}
	return ghpf;
}

/*
End of Helper Function
*/

int main(int argc, char ** argv)
{
	int action = 10;

	const char* inputFileName = argc >= 2 ? argv[1] : "Examples/lena.jpg";

	Mat inputImage;
	Mat outputImage;

	cout << "Discrete Fourier Transform --" << " File name: " << inputFileName << " " << endl << endl;

	inputImage = imread(inputFileName, CV_LOAD_IMAGE_GRAYSCALE);

	if (inputImage.empty()) {
		cout << "Image not found, please restart the program with different image path! " << endl;
		return EXIT_FAILURE;
	}
	cout << "Image found... Starting program" << endl;


	while (action != 0) {
	cout << "\nSelect what do you want to do with the picture: \n"
		"1 - Get frequency magnitude spectrum\n"
		"2 - Apply high pass filter on an image\n"
		"3 - Apply low pass filter on an image\n"
		/*"4 - Apply high pass filter on an image\n"
		"5 - Remove noise from an image\n"
		"6 - Apply Gaussian filter on an image\n" */<< endl;
	
	cout << "Enter a number from the given range: ";
	cin >> action;
	cout << "\n";

	imshow("Input Image", inputImage);

	outputImage = action_switch(action, inputImage);
	cout << "\n";
	}
	return 0;
}
