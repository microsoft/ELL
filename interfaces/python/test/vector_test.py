import numpy as np
import find_ell
import ell
from testing import Testing

def test_double():
    testing = Testing()

    # empty vector
    e = ell.math.DoubleVector()
    np.testing.assert_equal(e.size(), 0)

    # vector from list
    l = [1.1,2.2,3.3,4.4,]
    e = ell.math.DoubleVector(l)

    np.testing.assert_equal(list(e), l)

    # vector from numpy array
    a = np.array(range(10), dtype=float)
    e = ell.math.DoubleVector(a)

    np.testing.assert_equal(np.asarray(e), a)

    # conver to numpy using array
    b = np.array(e).ravel()
    np.testing.assert_equal(a, b)

    # copy_from numpy array
    e = ell.math.DoubleVector()
    e.copy_from(a)
    np.testing.assert_equal(np.asarray(e), a)

    # convert data types
    a = a.astype(np.float32)
    e = ell.math.DoubleVector(a)
    np.testing.assert_equal(np.asarray(e), a)

    # enumerating array
    for i in range(a.shape[0]):
        x = a[i]
        y = e[i]
        np.testing.assert_equal(x, y)

    # auto-ravel numpy arrays
    a = np.ones((10,10), dtype=float)
    a *= range(10)
    e = ell.math.DoubleVector(a)
    np.testing.assert_equal(np.asarray(e), a.ravel())

    testing.ProcessTest("DoubleVector test", True)

def assert_compare_floats(a, b):
    c = np.array(list(a), dtype=np.float32)
    d = np.array(list(b), dtype=np.float32)
    np.testing.assert_equal(c, d)

def test_float():
    testing = Testing()

    # empty vector
    e = ell.math.FloatVector()
    np.testing.assert_equal(e.size(), 0)

    # vector from list of floats
    l = [1.1,2.2,3.3,4.4]
    e = ell.math.FloatVector(l)

    assert_compare_floats(e, l)

    # vector from numpy array
    a = np.array(range(10), dtype=np.float32)
    e = ell.math.FloatVector(a)

    np.testing.assert_equal(np.asarray(e), a)

    # convert to numpy using array
    b = np.array(e).ravel()
    np.testing.assert_equal(a, b)

    # copy_from numpy array
    e = ell.math.FloatVector()
    e.copy_from(a)
    np.testing.assert_equal(np.asarray(e), a)

    # convert data types
    a = a.astype(np.float)
    e = ell.math.FloatVector(a)
    np.testing.assert_equal(np.asarray(e), a)

    # enumerating array
    for i in range(a.shape[0]):
        x = a[i]
        y = e[i]
        np.testing.assert_equal(x, y)

    # auto-ravel numpy arrays
    a = np.ones((10,10), dtype=np.float32)
    a *= range(10)
    e = ell.math.FloatVector(a)
    np.testing.assert_equal(np.asarray(e), a.ravel())

    testing.ProcessTest("FloatVector test", True)


def test():
    test_double()
    test_float()
    return 0

if __name__ == "__main__":
    test()
