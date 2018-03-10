#!/usr/bin/env python3
#
# Requires python 3.5 or greater
#

"""
Script for generating the code for transforming input and output windows in for Winograd convolution. 
"""

import argparse
from string import Template

import symbolic


"""
Y = A' * (GgG' .* B'dB) * A
g = filter ([g0 g1 g2]')
d = signal ([d0 d1 d2 d3]')

"""

# For F(2,3)
B_t_2_3 = [[1,   0,  -1,   0],
           [0,   1,   1,   0],
           [0,  -1,   1,   0],
           [0,   1,   0,  -1]]

G_2_3 = [[    1,      0,      0],
         [ 1./2,   1./2,   1./2],
         [ 1./2,  -1./2,   1./2],
         [    0,      0,      1]]

A_t_2_3 =[[1,   1,   1,   0],
          [0,   1,  -1,  -1]]


# For F(4,3)
B_t_4_3 = [[4,   0,  -5,   0,   1,   0],
           [0,  -4,  -4,   1,   1,   0],
           [0,   4,  -4,  -1,   1,   0],
           [0,  -2,  -1,   2,   1,   0],
           [0,   2,  -1,  -2,   1,   0],
           [0,   4,   0,  -5,   0,   1]]

G_4_3 = [[  1./4,       0,       0],
         [ -1./6,   -1./6,   -1./6],
         [ -1./6,    1./6,   -1./6],
         [ 1./24,   1./12,    1./6],
         [ 1./24,  -1./12,    1./6],
         [     0,       0,       1]]


A_t_4_3 = [[1,   1,   1,   1,   1,   0],
           [0,   1,  -1,   2,  -2,   0],
           [0,   1,   1,   4,   4,   0],
           [0,   1,  -1,   8,  -8,   1]]


def get_data_transform_matrix(tile_size, filter_size):
    if tile_size == 2 and filter_size == 3:
        return symbolic.MatrixLiteral(B_t_2_3)
    elif tile_size == 4 and filter_size == 3:
        return symbolic.MatrixLiteral(B_t_4_3)
    else:
        raise Exception("Invalid parameters for Winograd matrices")

def get_filter_transform_matrix(tile_size, filter_size):
    if tile_size == 2 and filter_size == 3:
        return symbolic.MatrixLiteral(G_2_3)
    elif tile_size == 4 and filter_size == 3:
        return symbolic.MatrixLiteral(G_4_3)
    else:
        raise Exception("Invalid parameters for Winograd matrices")

def get_tile_transform_matrix(tile_size, filter_size):
    if tile_size == 2 and filter_size == 3:
        return symbolic.MatrixLiteral(A_t_2_3)
    elif tile_size == 4 and filter_size == 3:
        return symbolic.MatrixLiteral(A_t_4_3)
    else:
        raise Exception("Invalid parameters for Winograd matrices")


# 1D:
# # Y = A' * (Gg .* B'd)
#
# 2D:
# Y = A' * (GgG' .* B'dB) * A

def transpose(A):
    m = len(A)
    n = len(A[0])
    result = [[[] for j in range(m)] for i in range(n)] # An n x m  list of empty lists
    for i in range(m):
        for j in range(n):
            result[j][i] = A[i][j]
    return result

def get_name_mat(name, rows, cols, format_str="{}_{}_{}"):
    return [[format_str.format(name, r, c) for c in range(cols)] for r in range(rows)]

def generate_winograd_mat_expr(A, X):
    """ Computes A @ X or A @ X @ A', depending on dimensions of X """
    assert(isinstance(A, symbolic.MatrixLiteral))
    if X.columns == 1:
        return A @ X
    else:
        A_t = symbolic.MatrixLiteral(transpose(A.value))
        return A @ X @ A_t

def get_temp_var(name, rows, ndim):
    cols = 1 if ndim == 1 else rows
    return symbolic.MatrixVariable(name, rows, cols) # vector or square
    
def get_column_vector(values):
    return symbolic.MatrixLiteral([[x] for x in values])

def generate_winograd_B_expr(tile_size, filter_size, data_var_name, ndim):
    B_t = get_data_transform_matrix(tile_size, filter_size)
    return generate_winograd_mat_expr(B_t, get_temp_var(data_var_name, B_t.columns, ndim))

def generate_winograd_A_expr(tile_size, filter_size, temp_var_name, ndim):
    A_t = get_tile_transform_matrix(tile_size, filter_size)
    return generate_winograd_mat_expr(A_t, get_temp_var(temp_var_name, A_t.columns, ndim))

# TODO: get rid of this, but keep the filter-transform part
def generate_winograd_exprs(tile_size, filter_size, ndim, filter_values=None):
    B_t = get_data_transform_matrix(tile_size, filter_size)
    B_expr = generate_winograd_mat_expr(B_t, get_temp_var("d", B_t.columns, ndim))

    G = get_filter_transform_matrix(tile_size, filter_size)
    if filter_values is not None:
        if ndim == 1:
            g = symbolic.MatrixLiteral([[x] for x in filter_values])
        else:
            g = symbolic.MatrixLiteral(filter_values)
    else:
        g = get_temp_var("g", G.columns, ndim)
    G_expr = generate_winograd_mat_expr(G, g)
    
    A_t = get_tile_transform_matrix(tile_size, filter_size)
    A_expr = generate_winograd_mat_expr(A_t, get_temp_var("X", A_t.columns, ndim))
    return B_expr, G_expr, A_expr

def print_indented(s, indent):
    lines = s.splitlines()
    indent_str = " " * indent
    for line in lines:
        print(indent_str + line)

def print_expr_code(var_type, var_name, expr, unroll_assignment, assignment_offset=(0,0), start_indent=12):
    offset_expr = [symbolic.expr(i) for i in assignment_offset]
    if isinstance(expr, symbolic.MatrixExpr) or isinstance(expr, symbolic.MatrixLiteral):
        if unroll_assignment:
            var = symbolic.MatrixVariable(var_name, expr.rows, expr.columns)
            for i in range(expr.rows):
                for j in range(expr.columns):
                    assign_row = symbolic.simplify(offset_expr[0] + i)
                    assign_column = symbolic.simplify(offset_expr[1] + j)
                    print_indented("{} = {};".format(symbolic.cstr(var[assign_row, assign_column]), symbolic.cstr(expr[i,j])), start_indent)
            print()
        else:
            print_indented("{} {} = {};".format(var_type, var_name, symbolic.cstr(expr)), start_indent)
    
## TODO: emit just a single loop for 1D convolutions
def print_ell_inner_loop(ndim, tile_size, filter_size, output_rows, output_columns, filter_matrix_name, start_indent, indent_width):
    use_symbolic_var_names = True # for tile_size, filter_size, window_size
    output_var_name = "result"
    row_index_var = "rowIndex"
    column_index_var = "columnIndex"
    window_size = tile_size + filter_size - 1
    tile_size_str = "tileSize" if use_symbolic_var_names else str(tile_size)
    filter_size_str = "filterSize" if use_symbolic_var_names else str(filter_size)
    window_size_str = "windowSize" if use_symbolic_var_names else str(window_size)
    inner_indent = start_indent + 2*indent_width

    # A bunch of template strings for generating code

    temp_var_name = "X"
    data_var_name = "d"
    vector_type = "math::RowVector<ValueType>"
    matrix_type = "math::RowMatrix<ValueType>"
    index_var = "index"
    multiply_code_1D = Template("math::ElementwiseMultiplySet(${filter_matrix_name}, ${temp_var_name}, ${temp_var_name});")

    # 1D case
    begin_loop_1D = Template("""
for (int ${index} = 0; ${index} < ${output_size}; ${index} += ${tile_size})
{
""")
    temp_var_declaration_1D = Template("${var_type} ${temp_var_name}(${window_size});")
    data_var_declaration_1D = Template("auto ${data_var_name} = signal.GetSubVector(${index}, ${window_size});")
    end_loop_1D = """
    }
}
"""

    # 2D case
    begin_loop_2D = Template("""
for (int ${row_index} = 0; ${row_index} < ${output_rows}; ${row_index} += ${tile_size})
{
    for (int ${column_index} = 0; ${column_index} < ${output_columns}; ${column_index} += ${tile_size})
    {
""")
    temp_var_declaration_2D = Template("${var_type} ${temp_var_name}(${window_size}, ${window_size});")
    data_var_declaration_2D = Template("auto ${data_var_name} = signal.GetSubMatrix(${row_index}, ${column_index}, ${window_size}, ${window_size});")
    end_loop_2D = """
    }
}
"""

    # Choose appropriate template strings:
    if ndim == 1:
        indices = {"index": "index"}
        output_sizes = {"output_size": "outputSize"}

        begin_loop = begin_loop_1D
        temp_var_declaration = temp_var_declaration_1D
        data_var_declaration = data_var_declaration_1D
        end_loop = end_loop_1D
        var_type = vector_type
        multiply_code = multiply_code_1D
    else:
        indices = {"row_index": "rowIndex", "column_index": "columnIndex"}
        output_sizes = {"output_rows": "outputRows", "output_columns": "outputColumns"}
        begin_loop = begin_loop_2D
        temp_var_declaration = temp_var_declaration_2D
        data_var_declaration = data_var_declaration_2D
        end_loop = end_loop_2D
        var_type = matrix_type
        multiply_code = multiply_code_2D

    # Get expressions to generate code from
    B = generate_winograd_B_expr(tile_size, filter_size, data_var_name, ndim)
    A = generate_winograd_A_expr(tile_size, filter_size, temp_var_name, ndim)

    # Emit temporary variable declaration
    print_indented(temp_var_declaration.substitute(var_type=var_type, temp_var_name=temp_var_name, window_size=window_size_str), start_indent)

    # Start the loop
    print_indented(begin_loop.substitute(tile_size=tile_size, **indices, **output_sizes), start_indent)
    
    print_indented(data_var_declaration.substitute(data_var_name=data_var_name, window_size=window_size_str, **indices), inner_indent)
    print()

    print_indented("// Transform input data", inner_indent)
    print_expr_code(matrix_type, temp_var_name, symbolic.simplify(B), True, start_indent=inner_indent)

    # Compute elementwise product
    print_indented("// Compute elementwise product of transformed data and transformed filter", inner_indent)
    print_indented(multiply_code.substitute(filter_matrix_name=filter_matrix_name, temp_var_name=temp_var_name), inner_indent)

    # Transform result to tile
    print_indented("// Transform result to a tile", inner_indent)
    print_expr_code(matrix_type, output_var_name, symbolic.simplify(A), True, assignment_offset=(row_index_var, column_index_var), start_indent=inner_indent)

    # Start the loop
    print_indented(end_loop_2D, start_indent)

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Generate inner loop of Winograd convolution algorithm")
    arg_parser.add_argument("--ndim", "-n", help="Number of dimensions for the convolution (1 or 2)", type=int, default=2)
    arg_parser.add_argument("--tile_size", "-t", help="Size of the tiles to use (currently either 2 or 4)", type=int, default=4)
    arg_parser.add_argument("--filter_size", "-f", help="Size of filter to use (currently must be 3)", type=int, default=3)
    args = arg_parser.parse_args()
    output_rows = "outputRows"
    output_columns = "outputColumns"
    filter_matrix_name = "GgGt"
    indent_width = 4
    start_indent = 4 * indent_width
    print_ell_inner_loop(args.ndim, args.tile_size, args.filter_size, output_rows, output_columns, filter_matrix_name, start_indent, indent_width)
