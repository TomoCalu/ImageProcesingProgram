#include "dft.h"

/*
Functions called in main
*/

Mat get_dft(Mat inputImage)
{
	Mat outputImage;
	Mat resizedImage;  //expand input image to optimal size

	resizedImage = expand_to_optimal_size(inputImage);

	cout << "Making place for both the complex and the real values" << endl;
	Mat planes[] = { Mat_<float>(resizedImage), Mat::zeros(resizedImage.size(), CV_32F) };
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	cout << "Making the Discrete Fourier Transform" << endl;
	dft(complexI, complexI);          // this way the result may fit in the source matrix
									  // compute the magnitude and switch to logarithmic scale
									  // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))	
	cout << "Transforming the real and complex values to magnitude" << endl;
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	outputImage = planes[0];

	cout << "Switching to a logarithmic scale" << endl;
	outputImage += Scalar::all(1);                    // switch to logarithmic scale
	log(outputImage, outputImage);

	crop_and_rearrange(outputImage);

	return outputImage;
}

Mat get_idft(Mat inputImage)
{
	Mat complexI;

	//ucitaj complexI od originalne slike


	Mat outputImage;
	Mat resizedImage;

	resizedImage = expand_to_optimal_size(inputImage);

	cout << "Making place for both the complex and the real values" << endl;
	Mat planes[] = { Mat_<float>(resizedImage), Mat::zeros(resizedImage.size(), CV_32F) };

	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	//dft(tempA, tempA, 0, A.rows);
	idft(complexI, outputImage, cv::DFT_REAL_OUTPUT, complexI.rows);

	//idft(complexI, outputImage, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

	imshow("Outpuasdt", outputImage);
	outputImage.convertTo(outputImage, CV_8U);

	normalize(outputImage, outputImage, 0, 1, CV_MINMAX);
	imshow("Image", outputImage);

	crop_and_rearrange(outputImage);

	return outputImage;
}

Mat action_switch(int action, Mat inputImage)
{
	int i;
	char* outputFile;

	Mat outputImage;

	switch (action)
	{
	case(1):
		outputImage = get_dft(inputImage);
		normalize(outputImage, outputImage, 0, 1, CV_MINMAX);
		outputFile = "Examples/spectrum.jpg";
		break;
	case(2):
		outputImage = get_idft(inputImage);
		//idft(complexI, outputImage, cv::DFT_REAL_OUTPUT);
		//normalize(outputImage, outputImage, 0, 1, CV_MINMAX);
		outputFile = "Examples/inverse.jpg";
		break;
	case(3):
		for (i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
		{
			blur(inputImage, outputImage, Size(i, i));
		}
		outputFile = "Examples/high-pass-filter.jpg";
		break;
	case(4):
		Sobel(inputImage, outputImage, outputImage.type(), 1, 0, 3, 1, 0, BORDER_REFLECT_101);
		outputFile = "Examples/low-pass-filter.jpg";
		break;
	case(5):
		for (int i = 1; i < MAX_KERNEL_LENGTH - 15; i = i + 2)
		{
			bilateralFilter(inputImage, outputImage, i, i * 2, i / 2);
		}
		outputFile = "Examples/removed-peaks.jpg";
		break;
	case(6):
		for (i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
		{
			GaussianBlur(inputImage, outputImage, Size(i + 30, i + 30), 0, 0);
		}
		outputFile = "Examples/gaussian-blur.jpg";
		break;
	}
	//write_image_to_file(outputImage, outputFile);

	return outputImage;
}

void show_images(Mat inputImage, Mat outputImage)
{
	cout << "Showing program results" << endl;

	imshow("Input Image", inputImage);    // Show the result
	imshow("Output Image", outputImage);
}

/*
End of Functions called in main
*/

/*
Helper Functions
*/

void write_image_to_file(Mat outputImage, char* outputFile)
{
	normalize(outputImage, outputImage, 0, 1, CV_MINMAX);
	Mat gray;
	outputImage.convertTo(gray, CV_8U, 255); // upscale to [0..255]
	imwrite(outputFile, gray);
	printf("Output file location: %s\n", outputFile);
}

Mat expand_to_optimal_size(Mat image)
{
	Mat resizedImage;

	int m = getOptimalDFTSize(image.rows);
	int n = getOptimalDFTSize(image.cols); // on the border add zero values

	cout << "Expanding the image to an optimal size" << endl;
	copyMakeBorder(image, resizedImage, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));

	return resizedImage;
}

Mat crop_and_rearrange(Mat magnitudeImage)
{
	cout << "Transforming imatrix into a viewable image form" << endl;
	// crop the spectrum, if it has an odd number of rows or columns
	cout << "Croping and rearranging" << endl;
	magnitudeImage = magnitudeImage(Rect(0, 0, magnitudeImage.cols & -2, magnitudeImage.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magnitudeImage.cols / 2;
	int cy = magnitudeImage.rows / 2;

	Mat q0(magnitudeImage, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magnitudeImage, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magnitudeImage, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magnitudeImage, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat temporary;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(temporary);
	q3.copyTo(q0);
	temporary.copyTo(q3);

	q1.copyTo(temporary);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	temporary.copyTo(q2);

	return magnitudeImage;
}

/*
End of Helper Function
*/

int main(int argc, char ** argv)
{
	int action;

	const char* inputFileName = argc >= 2 ? argv[1] : "Examples/lena.jpg";

	Mat inputImage;
	Mat outputImage;

	cout << "Discrete Fourier Transform --" << " File name: " << inputFileName << " " << endl << endl;

	inputImage = imread(inputFileName, CV_LOAD_IMAGE_GRAYSCALE);
	//inputImage = imread(inputFileName, CV_LOAD_IMAGE_COLOR);

	if (inputImage.empty()) {
		cout << "Image not found, please restart the program with different image path! " << endl;
		return EXIT_FAILURE;
	}


	cout << "Image found... Starting program" << endl;
	cout << "\nSelect what do you want to do with the picture: \n"
		"1 - Get frequency magnitude spectrum\n"
		"2 - Inverse discrete function\n"
		"3 - Apply low pass filter on an image\n"
		"4 - Apply high pass filter on an image\n"
		"5 - Remove noise from an image\n"
		"6 - Apply Gaussian filter on an image\n" << endl;
	cout << "Enter a number from the given range: ";
	cin >> action;
	cout << "\n";

	outputImage = action_switch(action, inputImage);

	show_images(inputImage, outputImage);

	waitKey();

	return 0;
}
