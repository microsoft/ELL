#include "stdafx.h"
#include "Tests.h"

double TestForest(double* input0)
{
	int t_0;
	double t_1;
	double t_2;
	int t_3;
	double t_4;
	double t_5;
	int t_6;
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
	int t_17;
	double t_18;
	double t_19;
	int t_20;
	double t_21;
	double t_22;

	t_6 = (input0[0] > 0.20000000000000001);
	/* Block10 */
	if (t_6)
	{
		t_0 = (input0[1] > 0.22);
		/* Block4 */
		t_1 = (t_0) ? 3.2000000000000002 : -3.2000000000000002;
		t_2 = 3 + t_1;
		t_7 = t_2;
	}
	else
	{
		t_3 = (input0[1] > 0.20999999999999999);
		/* Block7 */
		t_4 = (t_3) ? 3.1000000000000001 : -3.1000000000000001;
		t_5 = -3 + t_4;
		t_7 = t_5;
	}
	t_20 = (input0[0] > 0.29999999999999999);
	/* Block24 */
	if (t_20)
	{
		t_8 = (input0[2] > 0.90000000000000002);
		/* Block12 */
		t_9 = (t_8) ? 4 : -4;
		t_10 = 1 + t_9;
		t_21 = t_10;
	}
	else
	{
		t_17 = (input0[1] > 0.59999999999999998);
		/* Block21 */
		if (t_17)
		{
			t_11 = (input0[1] > 0.69999999999999996);
			/* Block15 */
			t_12 = (t_11) ? 2.2000000000000002 : -2.2000000000000002;
			t_13 = 2 + t_12;
			t_18 = t_13;
		}
		else
		{
			t_14 = (input0[1] > 0.40000000000000002);
			/* Block18 */
			t_15 = (t_14) ? 2.1000000000000001 : -2.1000000000000001;
			t_16 = -2 + t_15;
			t_18 = t_16;
		}
		t_19 = -1 + t_18;
		t_21 = t_19;
	}
	t_22 = 0;
	t_22 += t_21;
	t_22 += t_7;
	t_22 += 0;
	/* Block26 */
	return t_22;
}
