# The default python on my OS X machine isn't the same as the one that was used for linking
# with SWIG, apparently. Thus the explicit /usr/bin. (TODO: find a better solution than this one)
/usr/bin/python -i sgd.py ../../../examples/data/test_data.txt

