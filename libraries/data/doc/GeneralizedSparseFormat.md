# Generalized Sparse Data Format

The Generalized Sparse (GS) data format is a textual representation of real-valued data vectors. Vector elements are separated by whitespace and can be formatted in three different ways:

* Absolute index value format
* Relative index value format
* Dense format

## Absolute index value format

Each element is an index-value pair, where the index is a zero-based integer, the value is a double-precision floating point number, and the two numbers are separated by a colon (`:`). For example, `1:-12` and `4:0.278` are valid elements. 

Indices must be monotonically increasing, such as

    0:3.14    1:-12    4:0.278    12:1.0e-3

## Relative-index value format

The index of an element can be given relative to the index of the previous element, by adding the prefix `+`. Namely,

    0:3.14    +1:-12    +3:0.278    +8:1.0e-3

which encodes the same vector as the one in the previous section. Any combination of absolute and relative indexing is valid, provided that indices are always monotonically increasing. For example,

    0:3.14    1:-12    4:0.278    +8:1.0e-3

Relative indices must be positive, and an index of `+0` is incorrect. Specifically, if the first element has a relative index of `+1`, it is the same as having an absolute index of `0`. Therefore, the example vector above can also be written as

    +1:3.14    +1:-12    +3:0.278    +8:1.0e-3

## Dense format

If the index is omitted, it is assumed to be `+1`. Namely, the element `3.14` is understood to be `+1:3.14`. Therefore, the example vector above can be written as

    3.14    -12    4:0.278    12:1.0e-3

or as

    3.14    -12    0    0    0.278    0    0    0    0    0    0    0    1.0e-3

## Comments
Comments can appear at the end of each line after either `#` or `//`.

## Examples of incorrectly formatted strings

* index is not an integer:`1.0:10 2:20`
* index is not an integer: `1X:10 2:20`
* value is not a valid double: `1:10X 2:20`
* extra whitespace: `+ 1:10 2:20`
* extra whitespace: `1 :10 2:20`
* extra whitespace: `1: 10 2:20`
* missing whitespace: `1:102:20`
* nonincreasing index: `2:10 2:20`
* nonincreasing index: `10 +0:20`
* nonincreasing index: `10 10 1:20`
* bad comment: `10 10 1:20 / comment`

