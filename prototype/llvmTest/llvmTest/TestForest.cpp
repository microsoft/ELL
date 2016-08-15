#include "stdafx.h"
#include "Tests.h"

double TestForest(double* input0)
{
	int t_0;
	double t_1;
	int t_2;
	double t_3;
	double t_4;
	int t_5;
	double t_6;
	double t_7;
	int t_8;
	double t_9;
	double t_10;

	t_0 = (input0[0] > 0.20000000000000001);
	/* Block4 */
	t_1 = (t_0) ? 3 : -3;
	t_8 = (input0[0] > 0.29999999999999999);
	/* Block12 */
	if (t_8)
	{
		t_2 = (input0[2] > 0.90000000000000002);
		/* Block6 */
		t_3 = (t_2) ? 4 : -4;
		t_4 = 1 + t_3;
		t_9 = t_4;
	}
	else
	{
		t_5 = (input0[1] > 0.59999999999999998);
		/* Block9 */
		t_6 = (t_5) ? 2 : -2;
		t_7 = -1 + t_6;
		t_9 = t_7;
	}
	t_10 = 0;
	t_10 = t_10 + t_9;
	t_10 = t_10 + t_1;
	t_10 = t_10 + 0;
	return t_10;
}
