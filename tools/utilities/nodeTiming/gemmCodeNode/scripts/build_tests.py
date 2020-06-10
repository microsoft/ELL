#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     build_tests.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import os
import sys
import platform
import argparse
import shutil
from special_model_args import special_model_args
from make_default_models import default_model_dir

script_path = os.path.dirname(os.path.realpath(__file__))
sys.path += [os.path.join(script_path, "../../pythonlibs")]

import find_ell

win_script_header = "@echo off\n\n"

def make_cmakelists(srcdir, outdir, dir_name, model_name, run_count=1000, warmup_count=100, additional_libraries=[]):
    add_subdirectory_str = "add_subdirectory({})"

    add_subdirectory_tag = "@ADD_SUBDIRECTORIES@"
    link_libraries_tag = "@LINK_LIBRARIES@"
    run_count_tag = "@RUN_COUNT@"
    warmup_count_tag = "@WARMUP_COUNT@"

    cmake_template_file = os.path.join(srcdir, "CMakeLists.txt.in")
    output_dir = os.path.join(outdir, dir_name)
    os.makedirs(output_dir, exist_ok=True)
    cmake_outfile = os.path.join(outdir, "{}/CMakeLists.txt".format(dir_name))
    with open(cmake_template_file) as f:
        template = f.read()

    link_libraries = [model_name] + additional_libraries
    template = template.replace(add_subdirectory_tag, add_subdirectory_str.format(dir_name))
    template = template.replace(link_libraries_tag, "{}".format(" ".join(link_libraries)))
    template = template.replace(run_count_tag, str(run_count))
    template = template.replace(warmup_count_tag, str(warmup_count))
    with open(cmake_outfile, 'w', newline='\n') as f:
        f.write(template)

def make_runner_cpp(srcdir, outdir, dir_name, model_name, data_type):
    capitalized_model_name = model_name[0].upper() + model_name[1:]

    model_name_tag = "@MODEL_NAME@"
    model_dir_tag = "@MODEL_DIR@"
    allcaps_model_name_tag = "@ALLCAPS_MODEL_NAME@"
    data_type_tag = "@DATA_TYPE@"

    cpp_template_file = os.path.join(srcdir, "Runner.cpp.in")
    cpp_outfile = os.path.join(outdir, "{}/Runner.cpp".format(dir_name))
    with open(cpp_template_file) as f:
        template = f.read()
    tags_list = [
        (model_name_tag, model_name),
        (model_dir_tag, dir_name),
        (allcaps_model_name_tag, capitalized_model_name),
        (data_type_tag, data_type)
    ]
    for tag, replacement in tags_list:
        template = template.replace(tag, replacement)

    with open(cpp_outfile, 'w', newline='\n') as f:
        f.write(template)

def make_build_script(outdir, dir_name, model_name, use_mkl):
    win_build_script_str = ('mkdir build_{}\n'
                        'cd build_{}\n'
                        'cmake -G "Visual Studio 15 2017 Win64" -Thost=x64 {} .. && cmake --build . --config release -- /m /verbosity:minimal\n'
                        'cd ..')
    unix_build_script_str = ('mkdir build_{}\n'
                        'cd build_{}\n'
                        'cmake .. -DCMAKE_BUILD_TYPE=Release {}\n'
                        'make -j\n'
                        'if [[ "$OSTYPE" == "darwin"* ]]; then\n'
                        '  objdump -d --no-show-raw-insn ./Runner > Runner.s\n'
                        'else\n'
                        '  objdump -d -w --no-show-raw-insn -rRSC ./Runner > Runner.s\n'
                        'fi\n'
                        'cd ..')
    use_mkl_str = '-DUSE_MKL=1' if use_mkl else ''
    win_build_str = win_build_script_str.format(model_name, model_name, use_mkl_str)
    unix_build_str = unix_build_script_str.format(model_name, model_name, use_mkl_str)
    win_outfile = os.path.join(outdir, "{}/build.cmd".format(dir_name))
    unix_outfile = os.path.join(outdir, "{}/build.sh".format(dir_name))
    with open(win_outfile, 'w') as f:
        f.write(win_build_str)
    with open(unix_outfile, 'w', newline='\n') as f:
        f.write(unix_build_str)

def make_build_all_script(ell_models, outdir):
    win_build_script_str = ('cd {}\n'
                            'call build.cmd\n'
                            'cd ..\n\n')
    unix_build_script_str = ('cd {}\n'
                            'chmod +x build.sh\n'
                            './build.sh\n'
                            'cd ..\n\n')
    win_str_list = [win_build_script_str.format(model_name) for (model_name, model_filename, model_relpath, model_path) in ell_models]
    unix_str_list = [unix_build_script_str.format(model_name) for (model_name, model_filename, model_relpath, model_path) in ell_models]
    win_outfile = os.path.join(outdir, "build_all.cmd")
    unix_outfile = os.path.join(outdir, "build_all.sh")
    with open(win_outfile, 'w') as f:
        f.write(win_script_header)
        f.write("\n".join(win_str_list))
    with open(unix_outfile, 'w', newline='\n') as f:
        f.write("\n".join(unix_str_list))

def make_run_all_script(ell_models, outdir, target):
    win_build_script_str = ('cd {}\\build_{}\\Release\n'
                            'call Runner.exe\n'
                            'cd ..\\..\\..\n')
    unix_build_script_str = ('cd {}/build_{}\n'
                            '{}./Runner\n'
                            'cd ../..\n')
    win_str_list = [win_build_script_str.format(model_name, model_name) for (model_name, model_filename, model_relpath, model_path) in ell_models]

    preload_str = ""
    if target == "pi3":
        preload_str = "LD_PRELOAD=~/miniconda3/envs/py34/lib/libopenblas.so "
    unix_str_list = [unix_build_script_str.format(model_name, model_name, preload_str) for (model_name, model_filename, model_relpath, model_path) in ell_models]

    win_outfile = os.path.join(outdir, "run_all.cmd")
    unix_outfile = os.path.join(outdir, "run_all.sh")
    with open(win_outfile, 'w') as f:
        f.write(win_script_header)
        f.write("\n".join(win_str_list))
    with open(unix_outfile, 'w', newline='\n') as f:
        f.write("\n".join(unix_str_list))

def make_clean_all_script(ell_models, outdir):
    win_clean_str = "rd /s /q {}"
    unix_clean_str = "rm -rf {}"
    win_str_list = [win_clean_str.format(model_name) for (model_name, model_filename, model_relpath, model_path) in ell_models]
    unix_str_list = [unix_clean_str.format(model_name) for (model_name, model_filename, model_relpath, model_path) in ell_models]
    win_outfile = os.path.join(outdir, "clean_all.cmd")
    unix_outfile = os.path.join(outdir, "clean_all.sh")
    with open(win_outfile, 'w') as f:
        f.write(win_script_header)
        f.write("\n".join(win_str_list))
    with open(unix_outfile, 'w', newline='\n') as f:
        f.write("\n".join(unix_str_list))

def make_import_all_script(ell_models, outdir, language="cpp", target="host", profile=False):
    mkdir_str = "mkdir {}"
    ell_root = find_ell.get_ell_root()
    wrap_path = os.path.abspath(os.path.join(ell_root, "tools", "wrap", "wrap.py"))
    wrap_str = "python {} -t {} -l {} --llvm_format ir -od {}/{} {} -f {} \n"
    profile_str =  "--profile" if profile else ""
    str_list = []
    for (model_name, model_filename, model_relpath, model_path) in ell_models:
        str_list.append(mkdir_str.format(model_name))
        last_str = model_path
        if model_filename in special_model_args:
            last_str += " " + " ".join(special_model_args[model_filename])
        str_list.append(wrap_str.format(wrap_path, target, language, model_name, model_name, profile_str, last_str))

    win_outfile = os.path.join(outdir, "import_all.cmd")
    unix_outfile = os.path.join(outdir, "import_all.sh")
    with open(win_outfile, 'w') as f:
        f.write(win_script_header)
        f.write("\n".join(str_list))
    with open(unix_outfile, 'w', newline='\n') as f:
        f.write("\n".join(str_list))

def find_ell_models_under_path(path_to_walk, suffix=".ell"):
    found_ell_models = [] # List of tuples (name, filename, fullpath)
    for root, dirs, files in os.walk(path_to_walk):
        for filename in files:
            if filename.endswith(suffix):
                relpath = os.path.relpath(root, start=path_to_walk)
                name = filename.rstrip(suffix)
                found_ell_models.append((name, filename, relpath, os.path.abspath(os.path.join(root, filename))))
    return found_ell_models

def copy_static_files(input_path, output_path):
    for file_name in os.listdir(input_path):
        full_path = os.path.join(input_path, file_name)
        if os.path.isfile(full_path):
            shutil.copy(full_path, output_path)

def get_first_path_element(path):
    first_path_element = None
    while len(path) > 0:
        split_relpath = os.path.split(path)
        path = split_relpath[0]
        first_path_element = split_relpath[1]
    return first_path_element

def create_gitignore(path, ignore_contents=["*"]):
    output_file = os.path.join(path, ".gitignore")
    with open(output_file, 'w', newline='\n') as f:
        f.writelines(ignore_contents)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--model_path", "-m", default=default_model_dir, help="Path to search under for ELL models")
    parser.add_argument("--variants", "-v", nargs="*", help="Which implementation variants of GEMM to build. Default is to run all", default=None)
    parser.add_argument("--target", "-t", default="host")
    parser.add_argument("--language", "-l", default="cpp")
    parser.add_argument("--outdir", "-o", default="test_output")
    parser.add_argument("--profile", "-p", action="store_true")
    parser.add_argument("--run_count", "-r", type=int, default=1000)
    parser.add_argument("--warmup_count", "-w", type=int, default=100)
    parser.add_argument("--include_mkl", "-mkl", action="store_true")
    parser.add_argument("--include_for_loops", action="store_true")
    parser.add_argument("--additional_libraries", nargs="*", default=[])
    parser.add_argument("--data_type", "-dt", choices=["float", "double"], default=None)
    args = parser.parse_args()

    mkl_name = "gemmMKL"
    blas_name = "gemmBLAS"
    naive_for_loops_name = "gemmELL"
    models_using_mkl = [mkl_name]
    order_precedence = [mkl_name, blas_name, naive_for_loops_name]

    ell_models = find_ell_models_under_path(args.model_path)
    if len(ell_models) == 0:
        print("No ELL models found at {}".format(args.model_path))
        sys.exit()

    # Re-order ell models for better result printing
    # Sort by the order_precedence list, then sort by N as an integer from the "gemmN" model name
    ell_models.sort(key=lambda x: order_precedence.index(x[0]) if x[0] in order_precedence else (len(order_precedence) + int(x[0].lstrip("gemm"))))

    if args.variants and len(args.variants) > 0:
        models_to_include = []
        if args.include_mkl:
            # Add MKL first so other scripts can default to comparing against the first results
            models_to_include.append(mkl_name)
        models_to_include.append(blas_name)
        if args.include_for_loops:
            models_to_include.append(naive_for_loops_name)
        additional_models_to_include = ["gemm{}".format(value) for value in args.variants]
        models_to_include.extend(additional_models_to_include)
        ell_models = list(filter(lambda ell_model_info: ell_model_info[0] in models_to_include, ell_models))
    else:
        if not args.include_mkl:
            ell_models = list(filter(lambda ell_model_info: ell_model_info[0] != mkl_name, ell_models))
        if not args.include_for_loops:
            ell_models = list(filter(lambda ell_model_info: ell_model_info[0] != naive_for_loops_name, ell_models))


    # Make and the testing output directory and add a .gitignore file to it
    os.makedirs(args.outdir, exist_ok=True)
    create_gitignore(args.outdir)

    # Group ELL models by their relative paths to separate the same implementation for different sizes/types
    rel_path_to_ell_models = {}
    for (model_name, model_filename, model_relpath, model_path) in ell_models:
        if model_relpath not in rel_path_to_ell_models:
            rel_path_to_ell_models[model_relpath] = []
        rel_path_to_ell_models[model_relpath].append((model_name, model_filename, model_relpath, model_path))

    for rel_path in rel_path_to_ell_models:
        data_type = args.data_type
        if data_type is None:
            # Try to get the data type from the name of the first directory in the model relpath
            first_path_elt = get_first_path_element(rel_path)
            if first_path_elt in ["float", "double"]:
                data_type = first_path_elt
            else:
                data_type = "double" # Default

        outdir = os.path.join(args.outdir, rel_path)
        current_ell_models = rel_path_to_ell_models[rel_path]
        os.makedirs(outdir, exist_ok=True)

        copy_static_files(os.path.join(script_path, "..", "deploy"), outdir)
        make_import_all_script(current_ell_models, outdir, args.language, args.target, args.profile)
        make_build_all_script(current_ell_models, outdir)
        make_run_all_script(current_ell_models, outdir, args.target)
        make_clean_all_script(current_ell_models, outdir)
        src_dir = os.path.join(script_path, "..", "src")
        for (model_name, model_filename, model_relpath, model_path) in current_ell_models:
            os.makedirs(os.path.join(outdir, model_name), exist_ok=True)
            make_cmakelists(src_dir, outdir, model_name, model_name, args.run_count, args.warmup_count, additional_libraries=args.additional_libraries)
            make_runner_cpp(src_dir, outdir, model_name, model_name, data_type)
            make_build_script(outdir, model_name, model_name, model_name in models_using_mkl)

    print("Created testing utilities at {}".format(args.outdir))
