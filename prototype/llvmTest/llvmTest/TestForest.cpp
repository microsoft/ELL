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
	int t_11;
	double t_12;
	double t_13;
	int t_14;
	double t_15;
	double t_16;

	t_0 = (input0[0] > 0.20000000000000001);
	/* Block4 */
	t_1 = (t_0) ? 3 : -3;
	t_14 = (input0[0] > 0.29999999999999999);
	/* Block18 */
	if (t_14)
	{
		t_2 = (input0[2] > 0.90000000000000002);
		/* Block6 */
		t_3 = (t_2) ? 4 : -4;
		t_4 = 1 + t_3;
		t_15 = t_4;
	}
	else
	{
		t_11 = (input0[1] > 0.59999999999999998);
		/* Block15 */
		if (t_11)
		{
			t_5 = (input0[1] > 0.69999999999999996);
			/* Block9 */
			t_6 = (t_5) ? 2.2000000000000002 : -2.2000000000000002;
			t_7 = 2 + t_6;
			t_12 = t_7;
		}
		else
		{
			t_8 = (input0[1] > 0.40000000000000002);
			/* Block12 */
			t_9 = (t_8) ? 2.1000000000000001 : -2.1000000000000001;
			t_10 = -2 + t_9;
			t_12 = t_10;
		}
		t_13 = -1 + t_12;
		t_15 = t_13;
	}
	t_16 = 0;
	t_16 = t_16 + t_15;
	t_16 = t_16 + t_1;
	t_16 = t_16 + 0;
	return t_16;
}
