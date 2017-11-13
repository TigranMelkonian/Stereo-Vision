// BasicBlockMatchingAlg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;


void tintImageBlue(Mat& image);

int main()
{
	Mat grayImageLeft, grayImageRight;
	grayImageLeft = imread("left.png", 0);
	grayImageRight = imread("right.png", 0);

	Mat1f Dbasic(grayImageLeft.rows, grayImageLeft.cols); //initialize a matirx of zeros

	int disparityRange = 50;
	int halfBlockSize = 3;
	int blocksize = 2 * halfBlockSize + 1;

	for (int i = 0; i < grayImageLeft.rows; i++)
	{
		cout << "at row: " << i << endl;
		//upper and lower bound
		int minr = max(0, i - halfBlockSize);
		int maxr = min(grayImageLeft.rows - 1, i + halfBlockSize);

		for (int j = 0; j < grayImageLeft.cols; j++)
		{
			//left and right bound
			int minc = max(0, j - halfBlockSize);
			int maxc = min(grayImageLeft.cols - 1, j + halfBlockSize);

			//search range
			int mind = 0;
			int maxd = min(disparityRange, grayImageLeft.cols - 1 - maxc);

			Mat1i template_(maxr - minr + 1, maxc - minc + 1);
			for (int k = minr; k <= maxr; k++)
			{
				for (int l = minc; l <= maxc; l++)
				{
					template_.at<int>(k - minr, l - minc) = grayImageRight.at<uchar>(k, l);
				}
			}

			int numBlocks = maxd - mind + 1;

			float* blockDiffs = new float[numBlocks];
			for (int k = mind; k <= maxd; k++)
			{
				Mat1i block(maxr - minr + 1, maxc - minc + 1);
				for (int l = minr; l <= maxr; l++)
				{
					for (int n = minc + k; n <= maxc + k; n++)
					{
						/*if (n == 450)
						{
						cout << l << "+" << n << endl;
						cout << minr << "+" << maxr << "+" << minc << "+" << maxc << "+" << maxd << endl;
						}*/
						block.at<int>(l - minr, n - minc - k) = grayImageLeft.at<uchar>(l, n);
					}
				}

				int blockIndex = k - mind;
				Mat1i difference(maxr - minr + 1, maxc - minc + 1);
				absdiff(template_, block, difference);
				blockDiffs[blockIndex] = sum(difference)[0];
			}

			int smallestIndex = 0;
			float smallestValue = FLT_MAX;
			for (int k = 0; k < numBlocks; k++)
			{
				if (blockDiffs[k] < smallestValue)
				{
					smallestValue = blockDiffs[k];
					smallestIndex = k;
				}
			}
			float disparity = smallestIndex + mind;
			Dbasic.at<float>(i, j) = disparity;
		}
	}

	Mat jetcolor;
	//applyColorMap(Dbasic, jetcolor, COLORMAP_JET);
	normalize(Dbasic, jetcolor, 0, 1, cv::NORM_MINMAX);
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", jetcolor);                // Show our image inside it.

	waitKey(0); // Wait for a keystroke in the window
	return 0;
}