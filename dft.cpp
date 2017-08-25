#include "dft.hpp"

Mat get_dft(Mat image)
{
	Mat resizedImage;
	Mat complexI;

	int m = getOptimalDFTSize(image.rows);
	int n = getOptimalDFTSize(image.cols);

	copyMakeBorder(image, resizedImage, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(resizedImage), Mat::zeros(resizedImage.size(), CV_32F) };
	merge(planes, 2, complexI);

	dft(complexI, complexI);          

	return complexI;
}

void get_magnitude(Mat complexI)
{
	Mat magnitudeImage;
	Mat planes[] = { Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };

	split(complexI, planes);
	magnitude(planes[0], planes[1], magnitudeImage);

	magnitudeImage += Scalar::all(1);
	log(magnitudeImage, magnitudeImage);

	magnitudeImage = crop_and_rearrange(magnitudeImage);

	normalize(magnitudeImage, magnitudeImage, 0, 1, NORM_MINMAX);
	imshow("Magnitude spectrum", magnitudeImage);
}

void get_idft(Mat complexI) 
{
	Mat outputImage;
	idft(complexI, outputImage);
	Mat planes[] = { Mat::zeros(complexI.size(), CV_32F), Mat::zeros(complexI.size(), CV_32F) };
	split(outputImage, planes);

	magnitude(planes[0], planes[1], outputImage);

	normalize(outputImage, outputImage, 0, 1, NORM_MINMAX);
	imshow("Output Image", outputImage);
}

void get_convolution(Mat image) 
{

	Point anchor = Point(-1, -1);
	double delta = 0;
	int ddepth = -1;
	Mat kernel;
	int kernel_size;
	Mat convolvedImage;

	kernel_size = 3 + 2 * (4 % 5);
	kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);
	imshow("Kernel", kernel);
		
	filter2D(image, convolvedImage, ddepth, kernel, anchor, delta, BORDER_DEFAULT);
	imshow("Output Image", convolvedImage);
}

void get_gaussian(Mat complexI, bool invert)
{
	Mat gaussianMask;
	Mat outputImage;
	Mat kernelSpec;

	gaussianMask = createGaussianHighPassFilter(complexI.size(),15, invert);
	imshow("Kernel", gaussianMask);
	gaussianMask = crop_and_rearrange(gaussianMask);

	Mat planes[] = { Mat_<float>(gaussianMask), Mat::zeros(gaussianMask.size(), CV_32F) };
	merge(planes, 2, kernelSpec);

	mulSpectrums(complexI, kernelSpec, complexI, DFT_ROWS);

	get_magnitude(complexI);
	get_idft(complexI);
}

Mat crop_and_rearrange(Mat image)
{
	image = image(Rect(0, 0, image.cols & -2, image.rows & -2));

	int cx = image.cols / 2;
	int cy = image.rows / 2;

	Mat q0(image, Rect(0, 0, cx, cy));   
	Mat q1(image, Rect(cx, 0, cx, cy)); 
	Mat q2(image, Rect(0, cy, cx, cy)); 
	Mat q3(image, Rect(cx, cy, cx, cy));

	Mat temporary;    
	q0.copyTo(temporary);
	q3.copyTo(q0);
	temporary.copyTo(q3);

	q1.copyTo(temporary);
	q2.copyTo(q1);
	temporary.copyTo(q2);

	return image;
}

double gaussianCoeff(double u, double v, double d0)
{
	double d = sqrt(u*u + v*v);
	return 1.0 - cv::exp((-d*d) / (2 * d0*d0));
}

Mat createGaussianHighPassFilter(Size size, double cutoffInPixels, bool invert)
{
	Mat image(size, CV_64F);

	Point center(size.width / 2, size.height / 2);

	for (int u = 0; u < image.rows; u++)
	{
		for (int v = 0; v < image.cols; v++)
		{
		    image.at<double>(u, v) = gaussianCoeff(u - center.y, v - center.x, cutoffInPixels);
			if (invert) image.at<double>(u, v) = 1 - image.at<double>(u, v);
		}
	}
	return image;
}

void action_switch(int action, Mat inputImage)
{
	int i;
	Mat outputImage;
	Mat complexI;

	destroyAllWindows();
	imshow("Input Image", inputImage);

	switch (action)
	{
	case(1):
		complexI = get_dft(inputImage);
		get_magnitude(complexI);
		break;
	case(2):
		complexI = get_dft(inputImage);
		get_magnitude(complexI);
		get_gaussian(complexI, false);
		break;
	case(3):
		complexI = get_dft(inputImage);
		get_magnitude(complexI);
		get_gaussian(complexI, true);
		break;
	case(4):
		get_convolution(inputImage);
		break;
	case(5):
		for (int i = 1; i < MAX_KERNEL_LENGTH - 15; i = i + 2)
		{
			bilateralFilter(inputImage, outputImage, i, i * 2, i / 2);
		}
		imshow("Output Image", outputImage);
		break;

	}

	printf("Showing images.....\n");
	cvWaitKey(2000);
}

int main(int argc, char ** argv)
{
	int action = 10;

	const char* inputFileName = argc >= 2 ? argv[1] : "Examples/lena.jpg";

	Mat inputImage;
	Mat outputImage;

	printf("Discrete Fourier Transform -- File name: %s\n\n", inputFileName );

	inputImage = imread(inputFileName, CV_LOAD_IMAGE_GRAYSCALE);

	if (inputImage.empty()) {
		printf("Image not found, please restart the program with different image path! \n");
		return EXIT_FAILURE;
	}
	printf("Image found... Starting program\n");

	while (action != 0) {
		printf("\nSelect what do you want to do with the picture: \n"
			   "1 - Get frequency magnitude spectrum\n"
		   	   "2 - Apply high pass filter on an image\n"
			   "3 - Apply low pass filter on an image\n"
			   "4 - Apply convolution on an image\n"
			   "5 - Remove noise from an image\n\n");
	
		printf("Enter a number from the given range: ");
		scanf("%d", &action);
		printf("\n");

		action_switch(action, inputImage);
		printf("\n");

		if (action <= 0 || action >= 6) 
		{
			printf("Number you inserted is lower than 1 or higher than 6 \n "
				   "Ending Program...\n");
			return EXIT_FAILURE;
		}

	}
	return 0;
}
