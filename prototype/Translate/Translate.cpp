// Translate.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <malloc.h>
//#include <tchar.h>
//#include <vector>
//#include <uchar.h>

/*
struct HelloData
{
	int XValue;
	int YValue;
};
*/

double Predict(double* features)
{
	double retVal = 0.0;

	retVal += -0.325 * features[0];
	retVal += 0.5775 * features[1];
	retVal += -0.935 * features[2];
	retVal += 3.325 * features[3];

	return retVal;
}

/*
double PredictV(std::vector<double>& features)
{
	double retVal = 0.0;

	retVal += -0.325 * features[0];
	retVal += 0.5775 * features[1];
	retVal += -0.935 * features[2];
	retVal += 3.325 * features[3];

	return retVal;
}
*/

int main()
{
	//HelloData h;
	//h.XValue = 1;
	//h.YValue = 32;

	double* features = (double *) malloc(sizeof(double) * 4);
	//features[0] = 1.1;
	features[1] = 9.9;
	printf("Data %f", features[1]);
	//features[2] = 2.2;
	//features[3] = 3.3;

	//double prediction = Predict(features);
	free(features);
	//delete[] features;
	//printf("Prediction is %f!\n", prediction);

	/*
	std::vector<double> featuresV({9.3, 4.7, 88.33, 99.3});
	prediction = PredictV(featuresV);
	printf("PredictionV is %f!\n", prediction);
	*/
}

