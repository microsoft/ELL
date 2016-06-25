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

/*
double Predict(const double* features)
{
	double retVal = 0.0;

	retVal += -0.325 * features[0];
	retVal += 0.5775 * features[1];
	retVal += -0.935 * features[2];
	retVal += 3.325 * features[3];

	return retVal;
}
*/
// New Compiler 
// Input dimension: 3
// Output dimension: 1
// Output coordinates: (4,0)
/*
void PredictUmesh2(const double* input, double* output)
{
	double tmp0 = 8 * input[0]; // coordinate (3,0), allocating new temporary variable
	double tmp1 = input[1]; // coordinate (0,1), allocating new temporary variable
	tmp0 += 18 * tmp1; // coordinate (3,0)
	tmp0 += 100 * tmp1; // coordinate (3,0)
	tmp1 = input[2]; // coordinate (0,2), reassigning temporary variable
	tmp0 += 32 * tmp1; // coordinate (3,0)
	tmp0 += 200 * tmp1; // coordinate (3,0)
	output[0] = tmp0 + 500; // coordinate (5,0)
}
*/
/*
// New Compiler 
// Input dimension: 124
// Output dimension: 1
// Output coordinates: (6,0)
void PredictTest2(const double* input, double* output)
{
	double tmp0 = -2575783.0679999995 * input[1]; // coordinate (5,0), allocating new temporary variable
	tmp0 += -226076.33907999998 * input[2]; // coordinate (5,0)
	tmp0 += 62.452174527000004 * input[3]; // coordinate (5,0)
	tmp0 += 675463.10124999995 * input[4]; // coordinate (5,0)
	tmp0 += 283254.13232999999 * input[5]; // coordinate (5,0)
	tmp0 += 7541.5490500000005 * input[6]; // coordinate (5,0)
	tmp0 += -539589.43978999997 * input[7]; // coordinate (5,0)
	tmp0 += 472746.83736 * input[8]; // coordinate (5,0)
	tmp0 += 354709.21733999997 * input[9]; // coordinate (5,0)
	tmp0 += -25462.840841699999 * input[10]; // coordinate (5,0)
	tmp0 += -70796.581432499996 * input[11]; // coordinate (5,0)
	tmp0 += -181.08065400200002 * input[12]; // coordinate (5,0)
	tmp0 += -146603.94829599999 * input[14]; // coordinate (5,0)
	tmp0 += 46573.329633599998 * input[15]; // coordinate (5,0)
	tmp0 += -14011.519384800002 * input[16]; // coordinate (5,0)
	tmp0 += 1641.4233264 * input[17]; // coordinate (5,0)
	tmp0 += -6497.4068120000002 * input[18]; // coordinate (5,0)
	tmp0 += -10342.2631676 * input[19]; // coordinate (5,0)
	tmp0 += -282.39053116999997 * input[20]; // coordinate (5,0)
	tmp0 += 25917.145206000001 * input[21]; // coordinate (5,0)
	tmp0 += -455409.25482000003 * input[22]; // coordinate (5,0)
	tmp0 += 90828.968431500005 * input[23]; // coordinate (5,0)
	tmp0 += 11716.401600000001 * input[24]; // coordinate (5,0)
	tmp0 += 2814.6985665999996 * input[25]; // coordinate (5,0)
	tmp0 += -105704.444281 * input[26]; // coordinate (5,0)
	tmp0 += -146949.633218 * input[27]; // coordinate (5,0)
	tmp0 += 39108.596526000001 * input[28]; // coordinate (5,0)
	tmp0 += 218949.45852000001 * input[29]; // coordinate (5,0)
	tmp0 += -4839.6153343999995 * input[30]; // coordinate (5,0)
	tmp0 += -30751.168876 * input[31]; // coordinate (5,0)
	tmp0 += 489834.42351999995 * input[32]; // coordinate (5,0)
	tmp0 += 6807.8343255 * input[33]; // coordinate (5,0)
	tmp0 += -21853.47236 * input[34]; // coordinate (5,0)
	tmp0 += -1674561.8482799998 * input[35]; // coordinate (5,0)
	tmp0 += -455409.25482000003 * input[36]; // coordinate (5,0)
	tmp0 += -282.39053116999997 * input[37]; // coordinate (5,0)
	tmp0 += 16068.831975000001 * input[38]; // coordinate (5,0)
	tmp0 += 869503.0237299999 * input[39]; // coordinate (5,0)
	tmp0 += 1269106.65741 * input[40]; // coordinate (5,0)
	tmp0 += -17997.675905999997 * input[41]; // coordinate (5,0)
	tmp0 += -953319.90915000008 * input[42]; // coordinate (5,0)
	tmp0 += -66856.996249000003 * input[43]; // coordinate (5,0)
	tmp0 += 81990.529066000003 * input[44]; // coordinate (5,0)
	tmp0 += 355.51968984000001 * input[45]; // coordinate (5,0)
	tmp0 += 7281.6958430000004 * input[46]; // coordinate (5,0)
	tmp0 += 455548.06803000002 * input[47]; // coordinate (5,0)
	tmp0 += -1842.6890629399998 * input[48]; // coordinate (5,0)
	tmp0 += -1098118.9411199999 * input[49]; // coordinate (5,0)
	tmp0 += 10259.572600199999 * input[50]; // coordinate (5,0)
	tmp0 += 2049649.5222 * input[51]; // coordinate (5,0)
	tmp0 += 676232.17610000004 * input[52]; // coordinate (5,0)
	tmp0 += -552786.19808999996 * input[53]; // coordinate (5,0)
	tmp0 += -8359.0726878000005 * input[54]; // coordinate (5,0)
	tmp0 += -192289.46798400002 * input[55]; // coordinate (5,0)
	tmp0 += -704289.54121000005 * input[56]; // coordinate (5,0)
	tmp0 += -270302.75434799999 * input[57]; // coordinate (5,0)
	tmp0 += -54436.350914999995 * input[58]; // coordinate (5,0)
	tmp0 += 275550.19753399998 * input[59]; // coordinate (5,0)
	tmp0 += -661.48827885000003 * input[60]; // coordinate (5,0)
	tmp0 += 1283175.8654 * input[61]; // coordinate (5,0)
	tmp0 += -1126183.7807499999 * input[62]; // coordinate (5,0)
	tmp0 += 30467.985887999999 * input[63]; // coordinate (5,0)
	tmp0 += 53501.043103999997 * input[64]; // coordinate (5,0)
	tmp0 += -41294.123780000002 * input[65]; // coordinate (5,0)
	tmp0 += -476096.88114000007 * input[66]; // coordinate (5,0)
	tmp0 += 43694.333490000005 * input[67]; // coordinate (5,0)
	tmp0 += 107855.10676200001 * input[68]; // coordinate (5,0)
	tmp0 += -12762.729391500001 * input[69]; // coordinate (5,0)
	tmp0 += -273885.57928000001 * input[70]; // coordinate (5,0)
	tmp0 += -34817.308825599997 * input[71]; // coordinate (5,0)
	tmp0 += -1115114.5223400001 * input[72]; // coordinate (5,0)
	tmp0 += 43032.810167999996 * input[73]; // coordinate (5,0)
	tmp0 += -1163925.29645 * input[74]; // coordinate (5,0)
	tmp0 += 159422.04921599998 * input[75]; // coordinate (5,0)
	tmp0 += -1742412.0667800002 * input[76]; // coordinate (5,0)
	tmp0 += 410568.03736200003 * input[77]; // coordinate (5,0)
	tmp0 += -1887854.2719000001 * input[78]; // coordinate (5,0)
	tmp0 += -34035.262552 * input[79]; // coordinate (5,0)
	tmp0 += -6796.9693134000008 * input[80]; // coordinate (5,0)
	tmp0 += 212786.57232000001 * input[81]; // coordinate (5,0)
	tmp0 += 458410.14559999999 * input[82]; // coordinate (5,0)
	tmp0 += 225933.13615999999 * input[83]; // coordinate (5,0)
	tmp0 += 3220.4750408 * input[84]; // coordinate (5,0)
	tmp0 += 51164.597685000001 * input[85]; // coordinate (5,0)
	tmp0 += -2659.7824198399999 * input[86]; // coordinate (5,0)
	tmp0 += -4150.0608941999999 * input[87]; // coordinate (5,0)
	tmp0 += 163.47169535999998 * input[88]; // coordinate (5,0)
	tmp0 += -77437.076920000007 * input[90]; // coordinate (5,0)
	tmp0 += -33591.213627999998 * input[91]; // coordinate (5,0)
	tmp0 += -114933.726348 * input[92]; // coordinate (5,0)
	tmp0 += -237503.77883699996 * input[93]; // coordinate (5,0)
	tmp0 += -22008.181764000001 * input[94]; // coordinate (5,0)
	tmp0 += -2217.8723442800001 * input[95]; // coordinate (5,0)
	tmp0 += -2604.4322176800001 * input[96]; // coordinate (5,0)
	tmp0 += 146310.91250199999 * input[98]; // coordinate (5,0)
	tmp0 += 23258.824260000001 * input[99]; // coordinate (5,0)
	tmp0 += 2898.7976114999997 * input[100]; // coordinate (5,0)
	tmp0 += 1833.2566799999997 * input[101]; // coordinate (5,0)
	tmp0 += -5454.1419660000001 * input[102]; // coordinate (5,0)
	tmp0 += -233999.90208000003 * input[103]; // coordinate (5,0)
	tmp0 += -3.0594842817400001 * input[104]; // coordinate (5,0)
	tmp0 += -1521.04131809 * input[105]; // coordinate (5,0)
	tmp0 += -10594.8291498 * input[106]; // coordinate (5,0)
	tmp0 += -7283.6702435999996 * input[107]; // coordinate (5,0)
	tmp0 += -2305.7834950799997 * input[108]; // coordinate (5,0)
	tmp0 += -3544.9538957 * input[109]; // coordinate (5,0)
	tmp0 += 74514.834311999992 * input[110]; // coordinate (5,0)
	tmp0 += 2039.5678825000002 * input[111]; // coordinate (5,0)
	tmp0 += -281.56984846 * input[112]; // coordinate (5,0)
	tmp0 += 15409.107600000001 * input[113]; // coordinate (5,0)
	tmp0 += 294.44903299999999 * input[114]; // coordinate (5,0)
	tmp0 += -44.643877955999997 * input[115]; // coordinate (5,0)
	tmp0 += -21.817789466199997 * input[116]; // coordinate (5,0)
	tmp0 += -139.9348937 * input[117]; // coordinate (5,0)
	tmp0 += 7570.2596619999995 * input[118]; // coordinate (5,0)
	tmp0 += -9.5757161328000002 * input[119]; // coordinate (5,0)
	tmp0 += 2443.3592857199997 * input[120]; // coordinate (5,0)
	tmp0 += -519.38359785000011 * input[121]; // coordinate (5,0)
	tmp0 += 2118.1113190000001 * input[122]; // coordinate (5,0)
	output[0] = tmp0 + -0.00085348599999999998; // coordinate (7,0)
}

const double g_features[] = {
	5.0,
	10.0,
	15.0
};
*/
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

const double g_data[] = {3.3, 4.4, 5.5};
void Loop(int startAt, int maxValue, int step)
{
	for (int i = startAt; i < maxValue; ++step)
	{
		printf("%d, %f\n", i, g_data[i]);
	}
}
int main()
{
	Loop(0, 10, 1);
	//HelloData h;
	//h.XValue = 1;
	//h.YValue = 32;

	//double* features = (double *) malloc(sizeof(double) * 4);
	//features[0] = 1.1;
	//features[1] = 9.9;
	//printf("Data %f", features[1]);
	//features[2] = 2.2;
	//features[3] = 3.3;

	//double prediction = Predict(features);

	/*
	double prediction = 0.0;
	double features[124];
	for (int i= 0; i < 124; ++i)
	{
		features[i] = (5 * (i + 1));
	}
	printf("Starting\n");
	PredictTest2(features, &prediction);
	printf("Result %f\n", prediction);
	*/
	
	//free(features);
	//delete[] features;
	//printf("Prediction is %f!\n", prediction);

	/*
	std::vector<double> featuresV({9.3, 4.7, 88.33, 99.3});
	prediction = PredictV(featuresV);
	printf("PredictionV is %f!\n", prediction);
	*/
}

