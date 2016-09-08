#include "stdafx.h"
#include "Tests.h"

double TestForestOld(double* input0)
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

double TestForest3(double* input0)
{
	int t_0;
	double t_1;
	double t_2;
	int t_3;
	double t_4;
	double t_5;
	int t_6;
	double t_7;
	double t_8;
	int t_9;
	int t_10;
	int t_11;
	int t_12;
	int t_13;
	int t_14;
	int t_15;
	int t_16;
	int t_17;
	int t_18;
	int t_19;
	int t_20;

	/* BinaryPredicateNode<double> */
	/* Block4 */
	t_0 = (input0[0] > 0.90000000000000002);
	/* BinaryPredicateNode<double> */
	/* Block7 */
	t_3 = (input0[0] > 0.59999999999999998);
	/* BinaryPredicateNode<double> */
	/* Block10 */
	t_6 = (input0[0] > 0.29999999999999999);
	/* MultiplexerNode<double,bool> */
	/* Block11 */
	if (t_6)
	{
		/* Merge3 */
		/* MultiplexerNode<double,bool> */
		/* Block5 */
		t_1 = (t_0) ? 4 : -4;
		/* Merge1 */
		/* BinaryOperationNode<double> */
		/* Block6 */
		t_2 = 1 + t_1;
		t_7 = t_2;
	}
	else
	{
		/* Merge4 */
		/* MultiplexerNode<double,bool> */
		/* Block8 */
		t_4 = (t_3) ? 2 : -2;
		/* Merge2 */
		/* BinaryOperationNode<double> */
		/* Block9 */
		t_5 = -1 + t_4;
		t_7 = t_5;
	}
	/* Merge5 */
	/* SumNode<double> */
	/* Block12 */
	t_8 = 0;
	t_8 += t_7;
	t_8 += 0;
	/* TypeCastNode<bool,int32> */
	/* Block13 */
	/* BinaryPredicateNode<int32> */
	/* Block14 */
	t_9 = (t_6 == 0);
	/* Merge6 */
	/* MultiplexerNode<bool,bool> */
	/* Block15 */
	t_10 = (t_9) ? 1 : 0;
	/* TypeCastNode<bool,int32> */
	/* Block16 */
	/* BinaryPredicateNode<int32> */
	/* Block17 */
	t_11 = (t_3 == 0);
	/* Merge7 */
	/* MultiplexerNode<bool,bool> */
	/* Block18 */
	t_12 = (t_11) ? t_10 : 0;
	/* BinaryPredicateNode<int32> */
	/* Block19 */
	t_13 = (t_3 == 1);
	/* Merge8 */
	/* MultiplexerNode<bool,bool> */
	/* Block20 */
	t_14 = (t_13) ? t_10 : 0;
	/* BinaryPredicateNode<int32> */
	/* Block21 */
	t_15 = (t_6 == 1);
	/* Merge9 */
	/* MultiplexerNode<bool,bool> */
	/* Block22 */
	t_16 = (t_15) ? 1 : 0;
	/* TypeCastNode<bool,int32> */
	/* Block23 */
	/* BinaryPredicateNode<int32> */
	/* Block24 */
	t_17 = (t_0 == 0);
	/* Merge10 */
	/* MultiplexerNode<bool,bool> */
	/* Block25 */
	t_18 = (t_17) ? t_16 : 0;
	/* BinaryPredicateNode<int32> */
	/* Block26 */
	t_19 = (t_0 == 1);
	/* Merge11 */
	/* MultiplexerNode<bool,bool> */
	/* Block27 */
	t_20 = (t_19) ? t_16 : 0;
	return t_8;
}

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
	int t_12;
	int t_13;
	int t_14;
	int t_15;
	int t_16;
	int t_17;
	int t_18;
	int t_19;
	int t_20;
	int t_21;
	int t_22;
	int t_23;
	int t_24;
	int t_25;
	int t_26;

	t_0 = (input0[0] > 0.20000000000000001);
	t_1 = (t_0) ? 3 : -3;
	t_2 = (input0[0] > 0.90000000000000002);
	t_5 = (input0[0] > 0.59999999999999998);
	t_8 = (input0[0] > 0.29999999999999999);
	if (t_8)
	{
		t_3 = (t_2) ? 4 : -4;
		t_4 = 1 + t_3;
		t_9 = t_4;
	}
	else
	{
		t_6 = (t_5) ? 2 : -2;
		t_7 = -1 + t_6;
		t_9 = t_7;
	}
	t_10 = 0;
	t_10 += t_9;
	t_10 += t_1;
	t_10 += 0;
	t_11 = (t_8 == 0);
	t_12 = (t_11) ? 1 : 0;
	t_13 = (t_5 == 0);
	t_14 = (t_13) ? t_12 : 0;
	t_15 = (t_5 == 1);
	t_16 = (t_15) ? t_12 : 0;
	t_17 = (t_8 == 1);
	t_18 = (t_17) ? 1 : 0;
	t_19 = (t_2 == 0);
	t_20 = (t_19) ? t_18 : 0;
	t_21 = (t_2 == 1);
	t_22 = (t_21) ? t_18 : 0;
	t_23 = (t_0 == 0);
	t_24 = (t_23) ? 1 : 0;
	t_25 = (t_0 == 1);
	t_26 = (t_25) ? 1 : 0;
	return t_10;
}