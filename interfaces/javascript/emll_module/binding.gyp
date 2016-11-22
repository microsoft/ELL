# Examples of binding.gyp files for real projects:
# https://github.com/nodejs/node-gyp/wiki/%22binding.gyp%22-files-out-in-the-wild
# Very helpful discussion showing how to enable RTTI on OS X:
# https://github.com/nodejs/node-gyp/issues/26

# TODO: fill in llvm and blas paths from CMake via configure_file 
# TODO: have CMake generate the .d.ts file from XML
{
    'variables': {
    },
    'conditions': [
        [
            'OS=="win"', 
            {
                'variables': {
                    'include_path_prefix%': '../../../..',
                    'library_path_prefix%': '../../../..',
                    'library_path_suffix%': '/Release',
                    'llvm_include_path': '../../../../private/binaries/llvm/include',
                    'blas_include_path': '../../../../private/dependencies/OpenBLAS/win64/v0.2.19/haswell/include',
                    'llvm_library_path': '../../../../private/binaries/llvm/Rel/lib',
                },
            }
        ],
        [
            'OS=="mac"', 
            {
                'variables': {
                    'include_path_prefix%':'../../../..',
                    'library_path_prefix%':'../../../../..',
                    'library_path_suffix%': '',
                    'llvm_include_path': '/usr/local/include/llvm/include',
                    'blas_include_path': '../../../../private/dependencies/OpenBLAS/win64/v0.2.19/haswell/include',
                    'llvm_library_path': '/usr/local/lib',
                },
            }
        ],
        [
            'OS=="linux"', 
            {
                'variables': {
                    'include_path_prefix%':'../../../..',
                    'library_path_prefix%':'../../../..',
                    'library_path_suffix%': '',
                    'llvm_include_path': '/usr/local/include/llvm/include',
                    'blas_include_path': '../../../../private/dependencies/OpenBLAS/win64/v0.2.19/haswell/include',
                    'llvm_library_path': '/usr/local/lib',
                },
            }
        ]
    ],
    'targets': [
        {
            'target_name': 'emll',
            'copies' : [
                {
                    'destination': 'build',
                    'files': [
                    ]    
                }                
            ],
            'defines': [
                '__STDC_LIMIT_MACROS=1', 
                '__STDC_CONSTANT_MACROS=1'
            ],
            'dependencies': [],
            'include_dirs': [
                "<!(node -e \"require('nan')\")",
                '<(include_path_prefix)/interfaces/common/include',
                '<(include_path_prefix)/libraries/common/include',
                '<(include_path_prefix)/libraries/data/include',
                '<(include_path_prefix)/libraries/evaluators/include',
                '<(include_path_prefix)/libraries/lossFunctions/include',
                '<(include_path_prefix)/libraries/math/include',
                '<(include_path_prefix)/libraries/model/include',
                '<(include_path_prefix)/libraries/nodes/include',
                '<(include_path_prefix)/libraries/predictors/include',
                '<(include_path_prefix)/libraries/trainers/include',
                '<(include_path_prefix)/libraries/utilities/include',
                '<(include_path_prefix)/private/libraries/compiler/include',
                '<(include_path_prefix)/private/libraries/emitters/include',

                '<(llvm_include_path)',
                '<(blas_include_path)'
            ],
            'sources': [
                '../EMLLJAVASCRIPT_wrap.cxx'
            ],
            'link_settings': {
                'library_dirs': [
                    '<(library_path_prefix)/build/libraries/common<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/data<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/evaluators<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/lossFunctions<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/math<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/model<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/nodes<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/predictors<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/trainers<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/utilities<(library_path_suffix)',
                    '<(library_path_prefix)/build/private/libraries/compiler<(library_path_suffix)',
                    '<(library_path_prefix)/build/private/libraries/emitters<(library_path_suffix)',
                    '<(llvm_library_path)',
                ]
            },
            'conditions': [
                [
                    'OS=="win"',
                    {
                        'link_settings': {
                            'libraries': [
                                '-lcommon',
                                '-ldata',
                                '-levaluators',                                                                
                                '-llossFunctions',
                                '-lmath',
                                '-lmodel',
                                '-lnodes',
                                '-lpredictors',
                                '-ltrainers',
                                '-lutilities',
                                '-lcompiler',
                                '-lemitters',

                                '-lLLVMAnalysis',
                                '-lLLVMAsmParser',
                                '-lLLVMAsmPrinter',
                                '-lLLVMBitReader',
                                '-lLLVMBitWriter',
                                '-lLLVMCodeGen',
                                '-lLLVMCore',
                                '-lLLVMDebugInfoCodeView',
                                '-lLLVMExecutionEngine',
                                '-lLLVMInstCombine',
                                '-lLLVMMC',
                                '-lLLVMMCDisassembler',
                                '-lLLVMMCJIT',
                                '-lLLVMMCParser',
                                '-lLLVMObject',
                                '-lLLVMRuntimeDyld',
                                '-lLLVMScalarOpts',
                                '-lLLVMSelectionDag',
                                '-lLLVMSupport',
                                '-lLLVMTarget',
                                '-lLLVMTransformUtils',
                                '-lLLVMX86AsmPrinter',
                                '-lLLVMX86CodeGen',
                                '-lLLVMX86Desc',
                                '-lLLVMX86Info',
                                '-lLLVMX86Utils'                                
                            ]
                        },
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'AdditionalOptions': [
                                    '/MD'
                                ]
                            },
                            'VCLibrarianTool': {},
                            'VCLinkerTool': {}
                        }
                    }
                ],
                [
                    'OS=="mac"',
                    {
                        'link_settings': {
                            'libraries': [
                                '-lcommon',
                                '-ldata',
                                '-levaluators',                                                                
                                '-llossFunctions',
                                '-lmath',
                                '-lmodel',
                                '-lnodes',
                                '-lpredictors',
                                '-ltrainers',
                                '-lutilities',
                                '-lcompiler',
                                '-lemitters',
                                '-lLLVMAnalysis',
                                '-lLLVMAsmParser',
                                '-lLLVMAsmPrinter',
                                '-lLLVMBitReader',
                                '-lLLVMBitWriter',
                                '-lLLVMCodeGen',
                                '-lLLVMCore',
                                '-lLLVMDebugInfoCodeView',
                                '-lLLVMExecutionEngine',
                                '-lLLVMInstCombine',
                                '-lLLVMMC',
                                '-lLLVMMCDisassembler',
                                '-lLLVMMCJIT',
                                '-lLLVMMCParser',
                                '-lLLVMObject',
                                '-lLLVMRuntimeDyld',
                                '-lLLVMScalarOpts',
                                '-lLLVMSelectionDag',
                                '-lLLVMSupport',
                                '-lLLVMTarget',
                                '-lLLVMTransformUtils',
                                '-lLLVMX86AsmPrinter',
                                '-lLLVMX86CodeGen',
                                '-lLLVMX86Desc',
                                '-lLLVMX86Info',
                                '-lLLVMX86Utils'                                
                            ]
                        },
                        'cflags_cc!': [
                            '-fno-rtti',
                            '-fno-exceptions'
                        ],
                        'cflags!': [
                            '-fno-exceptions'
                        ],
                        'xcode_settings': {
                            'OTHER_CPLUSPLUSFLAGS': [
                                '-std=c++14',
                                '-stdlib=libc++',
                                '-v',

                            ],
                            'OTHER_LDFLAGS': [
                                '-stdlib=libc++'
                            ],
                            'MACOSX_DEPLOYMENT_TARGET': '10.12',
                            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                            'GCC_ENABLE_CPP_RTTI': 'YES'
                        }
                    }
                ],
                [
                    'OS=="linux"',
                    {
                        'cflags_cc': [
                            '-std=c++1y',
                            '-fexceptions'
                        ],
                        'cflags_cc!': [
                            '-fno-rtti',
                            '-fno-exceptions'
                        ],
                        'link_settings': {
                            'libraries': [
                                '-lcommon',
                                '-ldata',
                                '-levaluators',                                                                
                                '-llossFunctions',
                                '-lmath',
                                '-lmodel',
                                '-lnodes',
                                '-lpredictors',
                                '-ltrainers',
                                '-lutilities',
                                '-lcompiler',
                                '-lemitters',
                                '-lLLVMAnalysis',
                                '-lLLVMAsmParser',
                                '-lLLVMAsmPrinter',
                                '-lLLVMBitReader',
                                '-lLLVMBitWriter',
                                '-lLLVMCodeGen',
                                '-lLLVMCore',
                                '-lLLVMDebugInfoCodeView',
                                '-lLLVMExecutionEngine',
                                '-lLLVMInstCombine',
                                '-lLLVMMC',
                                '-lLLVMMCDisassembler',
                                '-lLLVMMCJIT',
                                '-lLLVMMCParser',
                                '-lLLVMObject',
                                '-lLLVMRuntimeDyld',
                                '-lLLVMScalarOpts',
                                '-lLLVMSelectionDag',
                                '-lLLVMSupport',
                                '-lLLVMTarget',
                                '-lLLVMTransformUtils',
                                '-lLLVMX86AsmPrinter',
                                '-lLLVMX86CodeGen',
                                '-lLLVMX86Desc',
                                '-lLLVMX86Info',
                                '-lLLVMX86Utils'                                
                            ]
                        }
                    }
                ]
            ],
        }
    ]
}
