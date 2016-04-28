Feature file used by microbit:

    # Note: For data sampled every 18ms
    f_in	Input
    f_raw_accel	ColumnSubset	f_in	2	3	4
    f_lowpass	IirFilter	f_raw_accel	0.8	1,-0.2
    f_gravity	IirFilter	f_lowpass	0.001	1,-0.999
    f_accel	Subtract	f_lowpass	f_gravity

    # Delay lines
    f_delay_4	Delay	f_accel	4
    f_delay_5	Delay	f_accel	5
    f_delay_6	Delay	f_accel	6
    f_delay_8	Delay	f_accel	8
    f_delay_10	Delay	f_accel	10
    f_delay_12	Delay	f_accel	12
    f_delay_16	Delay	f_accel	16
    f_delay_20	Delay	f_accel	20

    # dot2 features at various wavelengths
    # wavelengths: 5, 6, 7, 4 -- but mostly just using 6

    f_dot2_8_norm	DotProduct2	f_accel	f_delay_4	f_delay_8
    f_dot2_8_mean_1	Mean	f_dot2_8_norm	2
    f_dot2_8_mean_2	Mean	f_dot2_8_norm	4

    f_dot2_10_norm	DotProduct2	f_accel	f_delay_5	f_delay_10
    f_dot2_10_mean_1	Mean	f_dot2_10_norm	3
    f_dot2_10_mean_2	Mean	f_dot2_10_norm	5
    f_dot2_10_var_2	Variance	f_dot2_10_norm	f_dot2_10_mean_2	5

    f_dot2_12_norm	DotProduct2	f_accel	f_delay_6	f_delay_12
    f_dot2_12_mean_1	Mean	f_dot2_12_norm	3
    f_dot2_12_mean_2	Mean	f_dot2_12_norm	6
    f_dot2_12_var_2	Variance	f_dot2_12_norm	f_dot2_12_mean_2	6

    f_dot2_16_norm	DotProduct2	f_accel	f_delay_8	f_delay_16
    f_dot2_16_mean_1	Mean	f_dot2_16_norm	4
    f_dot2_16_mean_2	Mean	f_dot2_16_norm	8

    f_dot2_20_norm	DotProduct2	f_accel	f_delay_10	f_delay_20
    f_dot2_20_mean_1	Mean	f_dot2_20_norm	5
    f_dot2_20_mean_2	Mean	f_dot2_20_norm	10

    f_out	Concat	f_dot2_10_mean_2

Features used:
* IIR filter
* Elementwise subtract
* Delay
* dot2
* Mean over window
* Variance over window

Non-feature concepts needed:
* Delay line / history
* Running total?
* Elementwise x^2

