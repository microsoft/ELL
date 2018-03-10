#!/usr/bin/env python3
"""
Very simple symbolic math library. Used for generating code for implementing Winograd transformation matrices.
"""

import numbers
import operator

# Helpful guide to overloadable python operators:
#
# https://www.python-course.eu/python3_magic_methods.php

def num_rows(matrix_as_list):
    return len(matrix_as_list)

def num_columns(matrix_as_list):
    return len(matrix_as_list[0])

def expr(obj):
    """Convert a number or string into an expression"""
    if isinstance(obj, Expr):
        return obj
    if isinstance(obj, numbers.Number):
        return ScalarLiteral(obj)
    if isinstance(obj, str):
        return ScalarVariable(obj)
    raise Exception("ERROR converting object to expression: {}".format(obj))

def cstr(x):
    try:
        return x.cstr()
    except AttributeError:
        return str(x)

def expand(x):
    try:
        return x.expand()
    except AttributeError:
        return x

def simplify(x):
    try:
        return x.simplify()
    except AttributeError:
        return x

# Base class
class Expr(object):
    def expand(self):
        return self

    def simplify(self):
        return self

    def cstr(self):
        return str(self)

    def __add__(self, other):
        return Plus(self, other)

    def __sub__(self, other):
        return Minus(self, other)

    def __mul__(self, other):
        return Times(self, other)

    def __truediv__(self, other):
        return Divide(self, other)

    def __radd__(self, other):
        return Plus(other, self)

    def __rsub__(self, other):
        return Minus(other, self)

    def __rmul__(self, other):
        return Times(other, self)

    def __rtruediv__(self, other):
        return Divide(other, self)

    def __matmul__(self, other):
        return MatrixMultiply(self, other)

    def __neg__(self):
        return Negate(self)

    def __pos__(self):
        return self

class Function(Expr):
    def __init__(self, args, fn_name, fn):
        self.args = args
        self.fn_name = fn_name
        self.fn_fn = fn_fn

    def expand(self):
        args = (expand(a) for a in self.args)
        return Function(args, self.fn_name, self.fn)

    def simplify(self):
        args = (simplify(a) for a in self.args)
        return Function(args, self.fn_name, self.fn)

    def __str__(self):
        return "{}({})".format(self.fn_name, (", ".join(self.args)))

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return "{}({})".format(self.fn_name, (", ".join((cstr(arg) for arg in self.args))))

class UnaryOp(Expr):
    def __init__(self, a, op_name, op_fn):
        self.a = a
        self.op_name = op_name
        self.op_fn = op_fn

    def expand(self):
        a = expand(self.a)
        return UnaryOp(a, self.op_name, self.op_fn)

    def simplify(self):
        a = simplify(self.a)

        if isinstance(a, ScalarLiteral):
            return ScalarLiteral(self.op_fn(a.value))
        else:
            return UnaryOp(a, self.op_name, self.op_fn)

    def __str__(self):
        return "({} {})".format(self.op_name, self.a)

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return "({} {})".format(self.op_name, cstr(self.a))

class BinaryOp(Expr):
    def __init__(self, a, b, op_name, op_fn):
        if isinstance(a, numbers.Number):
            a = ScalarLiteral(a)
        if isinstance(b, numbers.Number):
            b = ScalarLiteral(b)
        self.a = a
        self.b = b
        self.op_name = op_name
        self.op_fn = op_fn
    
    def expand(self):
        a = expand(self.a)
        b = expand(self.b)
        return BinaryOp(a, b, self.op_name, self.op_fn)

    def simplify(self):
        a = simplify(self.a)
        b = simplify(self.b)

        if isinstance(a, ScalarLiteral) and isinstance(b, ScalarLiteral):
            return ScalarLiteral(self.op_fn(a.value, b.value))
        else:
            return BinaryOp(a, b, self.op_name, self.op_fn)
    
    def __str__(self):
        return "({} {} {})".format(self.a, self.op_name, self.b)

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return "({} {} {})".format(cstr(self.a), self.op_name, cstr(self.b))

class Plus(BinaryOp):
    def __init__(self, a, b):
        super().__init__(a, b, "+", operator.add)

    def simplify(self):
        s = super().simplify()
        if isinstance(s, ScalarLiteral):
            return s
        if isinstance(s.a, ScalarLiteral) and s.a.value == 0:
            return s.b
        if isinstance(s.b, ScalarLiteral) and s.b.value == 0:
            return s.a
        if isinstance(s.a, Negate):
            return Minus(s.b, s.a.a)
        if isinstance(s.b, Negate):
            return Minus(s.a, s.b.a)
        return s

class Minus(BinaryOp):
    def __init__(self, a, b):
        super().__init__(a, b, "-", operator.sub)

    def simplify(self):
        s = super().simplify()
        if isinstance(s, ScalarLiteral):
            return s
        if isinstance(s.a, ScalarLiteral) and s.a.value == 0:
            return Negate(s.b)
        if isinstance(s.b, ScalarLiteral) and s.b.value == 0:
            return s.a
        if isinstance(s.b, Negate):
            return Plus(s.a, s.b.a)
        return s

class Times(BinaryOp):
    def __init__(self, a, b):
        super().__init__(a, b, "*", operator.mul)

    def simplify(self):
        s = super().simplify()
        if isinstance(s, ScalarLiteral):
            return s
        if (isinstance(s.a, ScalarLiteral) and s.a.value == 0) or (isinstance(s.b, ScalarLiteral) and s.b.value == 0):
            return 0
        if isinstance(s.a, ScalarLiteral) and s.a.value == 1:
            return s.b
        if isinstance(s.b, ScalarLiteral) and s.b.value == 1:
            return s.a
        if isinstance(s.a, ScalarLiteral) and s.a.value == -1:
            return Negate(s.b)
        if isinstance(s.b, ScalarLiteral) and s.b.value == -1:
            return Negate(s.a)
        return s

class Divide(BinaryOp):
    def __init__(self, a, b):
        super().__init__(a, b, "/", operator.truediv)

    def simplify(self):
        s = super().simplify()
        if isinstance(s, ScalarLiteral):
            return s
        if isinstance(s.b, ScalarLiteral) and s.b.value == 1:
            return s.a
        if isinstance(s.b, ScalarLiteral) and s.b.value == -1:
            return Negate(s.a)
        return s

class MatrixTranspose(Function):
    def __init__(self, m):
        super().__init__((m,), "transpose", transpose)

    ## ??        

class MatrixMultiply(BinaryOp):
    def __init__(self, a, b):
        super().__init__(a, b, "@", operator.matmul)

    def expand(self):
        a = expand(self.a)
        b = expand(self.b)
        return matmult(a, b)

    def simplify(self):
        a = simplify(self.a)
        b = simplify(self.b)
        return simplify(matmult(a, b))

class Negate(UnaryOp):
    def __init__(self, a):
        super().__init__(a, "-", operator.neg)

class ArrayAccess(Expr):
    def __init__(self, a, index):
        self.a = a
        indices = index if len(index) > 1 else (index,)
        self.indices = [expr(i) for i in indices]

    def __str__(self):
        return "{}[{}]".format(self.a, (", ".join(str(i) for i in self.indices)))

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return "{}({})".format(self.a, (", ".join(cstr(i) for i in self.indices)))

class ScalarLiteral(Expr):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return str(self.value)

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

class ScalarVariable(Expr):
    def __init__(self, name):
        self.name = name
    
    def __str__(self):
        return self.name

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

temp_count = 0
def get_next_temp_name():
    global temp_count
    return "t_{}".format(temp_count)
    temp_count += 1

class MatrixExpr(Expr):
    def __init__(self, rows, columns, contents=None): 
        self.name = get_next_temp_name()
        self.rows = rows
        self.columns = columns
        self.contents = contents if contents is not None else [[[] for j in range(columns)] for i in range(rows)]
    
    def simplify(self):
        result = MatrixExpr(self.rows, self.columns)
        for row in range(self.rows):
            for column in range(self.columns):
                entry = self.contents[row][column]
                result[row, column] = simplify(entry)
        return result

    def __getitem__(self, index):
        return self.contents[index[0]][index[1]]

    def __setitem__(self, index, value):
        self.contents[index[0]][index[1]] = value

    def _to_string(self, lbracket, rbracket, str_fn):
        elemstrings = [[str_fn(x) for x in row] for row in self.contents]
        rowstrings = [lbracket + ", ".join(c for c in row) + rbracket for row in elemstrings]
        return lbracket + ",\n".join(rowstrings) + rbracket

    def __str__(self):
        return self._to_string("[", "]", str)

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return self._to_string("{", "}", cstr)


# TODO: combine MatrixLiteral with MatrixExpr
class MatrixLiteral(MatrixExpr):
    def __init__(self, value):
        self.value = value
        self.rows = len(value)
        self.columns = len(value[0])
    
    def __getitem__(self, index):
        return ScalarLiteral(self.value[index[0]][index[1]])

    def _to_string(self, lbracket, rbracket, str_fn):
        elemstrings = [[str_fn(x) for x in row] for row in self.value]
        rowstrings = [lbracket + ", ".join(c for c in row) + rbracket for row in elemstrings]
        return lbracket + ",\n".join(rowstrings) + rbracket

    def __str__(self):
        return self._to_string("[", "]", str)

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

    def cstr(self):
        return self._to_string("{", "}", cstr)

class MatrixVariable(Expr):
    def __init__(self, name, rows, columns):
        self.name = name
        self.rows = rows
        self.columns = columns

    def __getitem__(self, index):
        return ArrayAccess(self, index)

    def __str__(self):
        return self.name

    def __repr__(self):
        return "{}({})".format(self.__class__.__name__, self.__str__())

def matmult(A, B):
    # A: m x k
    # B: k x n
    # result: m x n
    m = A.rows
    k = A.columns
    assert(k == B.rows)
    n = B.columns
    O = MatrixExpr(m, n)
    for i in range(m):
        for j in range(n):
            O[i,j] = 0
            for l in range(k):
                O[i,j] += A[i,l] * B[l,j]
    return O

# Test
if __name__ == "__main__":
    a = 3
    b = ScalarLiteral(4)
    c = ScalarVariable('x')
    d = ScalarLiteral(0)
    ex = (a + b) * c + d
    print("a: {}".format(a))
    print("b: {}".format(b))
    print("c: {}".format(c))
    print("a+b: {}".format(a+b))
    print("Expr: {}".format(ex))
    print("Simplified expr: {}".format(simplify(ex)))

    print()
    print("Matrix example")
    A = MatrixLiteral([[1,2,3],[4,5,6]])
    B = MatrixLiteral([[5,6],[7,8],[9,10]])
    m = MatrixVariable("m", 3, 1)
    print("A:", A)
    print("B:", B)
    print("m:", m)
    print()
    print("A@m: ", A@m)
    print()
    print("A@m, expanded: ", (A@m).expand())
    print()
    print("A@m, simplified: ", simplify(A@m))
    print()
    print("A@B: ", A@B)
    print()
    print("A@B, expanded: ", expand(A@B))
    print()
    print("A@B, simplified: ", simplify(A@B))
    print()


##
## TODO:
##
## matrix transpose
## arbitrary functions (just called by name)
##