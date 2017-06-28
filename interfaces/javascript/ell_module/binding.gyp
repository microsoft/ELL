{
    'variables': {
        'blas_include_path': '@BLAS_INCLUDE_DIRS@',
        'llvm_include_path': '@LLVM_INCLUDE_DIRS@',
        'llvm_library_path': '@LLVM_LIBRARY_DIRS@',
    },
    'conditions': [
        [
            'OS=="win"', 
            {
                'variables': {
                    'include_path_prefix%': '../../../..',
                    'library_path_prefix%': '../../../..',
                    'library_path_suffix%': '/Release',
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
                },
            }
        ],
        [
            'OS=="linux"', 
            {
                'variables': {
                    'include_path_prefix%':'../../../..',
                    'library_path_prefix%':'../../../../..',
                    'library_path_suffix%': '',
                },
            }
        ]
    ],
    'targets': [
        {
            'target_name': 'ell',
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
                '<(include_path_prefix)/libraries/functions/include',
                '<(include_path_prefix)/libraries/math/include',
                '<(include_path_prefix)/libraries/model/include',
                '<(include_path_prefix)/libraries/nodes/include',
                '<(include_path_prefix)/libraries/predictors/include',
                '<(include_path_prefix)/libraries/predictors/neural/include',
                '<(include_path_prefix)/libraries/trainers/include',
                '<(include_path_prefix)/libraries/utilities/include',
                '<(include_path_prefix)/libraries/emitters/include',

                '<(llvm_include_path)',
                '<(blas_include_path)'
            ],
            'sources': [
                '../ELLJAVASCRIPT_wrap.cxx',
                @MODULE_INTERFACE_SRC@
            ],
            'link_settings': {
                'library_dirs': [
                    '<(library_path_prefix)/build/libraries/common<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/data<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/evaluators<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/functions<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/math<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/model<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/nodes<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/predictors<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/trainers<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/utilities<(library_path_suffix)',
                    '<(library_path_prefix)/build/libraries/emitters<(library_path_suffix)',
                    '<(llvm_library_path)',
                ],
                'libraries': [
                    '-lcommon',
                    '-ldata',
                    '-levaluators',                                                                
                    '-lfunctions',
                    '-lmath',
                    '-lmodel',
                    '-lnodes',
                    '-lpredictors',
                    '-ltrainers',
                    '-lutilities',
                    '-lemitters',
                    @MODULE_BLAS_LIBS@
                    @MODULE_LLVM_LIBS@
                ]
            },
            'conditions': [
                [
                    'OS=="win"',
                    {
                        'link_settings': {
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
                        },
                        'cflags': [
                            '-Wno-missing-braces'
                        ],
                        'cflags_cc': [
                            '-Wno-missing-braces'
                        ],
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
                                '-Wno-missing-braces'
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
                            '-fexceptions',
                            '-fPIC'
                        ],
                        'cflags_cc!': [
                            '-fno-rtti',
                            '-fno-exceptions'
                        ],
                        'link_settings': {
                        }
                    }
                ]
            ],
        }
    ]
}
