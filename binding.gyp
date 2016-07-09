# Examples of binding.gyp files for real projects:
# https://github.com/nodejs/node-gyp/wiki/%22binding.gyp%22-files-out-in-the-wild
# Very helpful discussion showing how to enable RTTI on OS X:
# https://github.com/nodejs/node-gyp/issues/26
{
    'conditions': [
        [
            'OS=="win"', 
            {
                'variables': {
                    'swig_include_path%': '../..',
                    'include_path%': '../../..',
                    'library_path%': '../..',
                    'library_suffix%': '/Release'
                },
            }
        ],
        [
            'OS!="win"', 
            {
                'variables': {
                    'swig_include_path%':'../..',
                    'include_path%':'../..',
                    'library_path%':'../../..',
                    'library_suffix%': ''
                },
            }
        ]
    ],
    'variables': {
        'include_path%':'../../..'
    },
    'targets': [
        {
            'target_name': 'swigInterfaces',
            'actions': [
                {
                    'action_name': 'swig',
                    'action': [
                        'swig',
                        '-c++',
                        '-javascript',
                        '-node',
                        '-DV8_VERSION=0x032530',
                        '-I<(swig_include_path)/EMLL/interfaces/Common/include',
                        '-I<(swig_include_path)/EMLL/libraries/common/include',
                        '-I<(swig_include_path)/EMLL/libraries/dataset/include',
                        '-I<(swig_include_path)/EMLL/libraries/evaluators/include',
                        '-I<(swig_include_path)/EMLL/libraries/features/include',
                        '-I<(swig_include_path)/EMLL/libraries/layers/include',
                        '-I<(swig_include_path)/EMLL/libraries/linear/include',
                        '-I<(swig_include_path)/EMLL/libraries/lossFunctions/include',
                        '-I<(swig_include_path)/EMLL/libraries/predictors/include',
                        '-I<(swig_include_path)/EMLL/libraries/trainers/include',
                        '-I<(swig_include_path)/EMLL/libraries/utilities/include',
                        '-o', 'swig/EMLL_wrap.cxx',
                        '<(swig_include_path)/EMLL/interfaces/common/EMLL.i'
                    ],
                    'inputs': [],
                    'outputs': [
                        'swig/EMLL_wrap.cxx'
                    ]
                },
                {
                    'action_name': 'swig_xml',
                    'action': [
                        'swig',
                        '-c++',
                        '-xml',
                        '-I<(swig_include_path)/EMLL/interfaces/Common/include',
                        '-I<(swig_include_path)/EMLL/libraries/common/include',
                        '-I<(swig_include_path)/EMLL/libraries/dataset/include',
                        '-I<(swig_include_path)/EMLL/libraries/evaluators/include',
                        '-I<(swig_include_path)/EMLL/libraries/features/include',
                        '-I<(swig_include_path)/EMLL/libraries/layers/include',
                        '-I<(swig_include_path)/EMLL/libraries/linear/include',
                        '-I<(swig_include_path)/EMLL/libraries/lossFunctions/include',
                        '-I<(swig_include_path)/EMLL/libraries/predictors/include',
                        '-I<(swig_include_path)/EMLL/libraries/trainers/include',
                        '-I<(swig_include_path)/EMLL/libraries/utilities/include',
                        '-o', 'swig/EMLL_wrap.xml',
                        '../../EMLL/interfaces/common/EMLL.i'
                    ],
                    'inputs': [],
                    'outputs': [
                        'swig/EMLL_wrap.xml'
                    ]
                }
            ]
        },
        {
            'target_name': 'emll',
            'dependencies': [
                'swigInterfaces'
            ],
            'include_dirs': [
                '<(include_path)/EMLL/libraries/common/include',
                '<(include_path)/EMLL/libraries/dataset/include',
                '<(include_path)/EMLL/libraries/evaluators/include',
                '<(include_path)/EMLL/libraries/features/include',
                '<(include_path)/EMLL/libraries/layers/include',
                '<(include_path)/EMLL/libraries/linear/include',
                '<(include_path)/EMLL/libraries/lossFunctions/include',
                '<(include_path)/EMLL/libraries/predictors/include',
                '<(include_path)/EMLL/libraries/trainers/include',
                '<(include_path)/EMLL/libraries/utilities/include',
                '<(include_path)/EMLL/interfaces/common/include'
            ],
            'sources': [
                '<(include_path)/EMLL/interfaces/common/src/DataLoadersInterface.cpp',
                '<(include_path)/EMLL/interfaces/common/src/LoadModelInterface.cpp',
                '<(include_path)/EMLL/interfaces/common/src/MapInterface.cpp',
                '<(include_path)/EMLL/interfaces/common/src/ModelInterface.cpp',
                '<(include_path)/EMLL/interfaces/common/src/RowDatasetInterface.cpp',
                './swig/EMLL_wrap.cxx'
            ],
            'conditions': [
                [
                    'OS=="win"',
                    {
                        'link_settings': {
                            'libraries': [
                                '-lcommon.lib',
                                '-ldataset.lib',
                                '-levaluators.lib',
                                '-lfeatures.lib',
                                '-llayers.lib',
                                '-llinear.lib',
                                '-llossFunctions.lib',
                                '-lpredictors.lib',
                                '-ltrainers.lib',
                                '-lutilities.lib'
                            ],
                            'library_dirs': [
                                '<(library_path)/EMLL/build/libraries/common<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/dataset<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/evaluators<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/features<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/layers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/linear<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/lossFunctions<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/predictors<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/trainers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/utilities<(library_suffix)',
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
                                'libcommon.a',
                                'libdataset.a',
                                'libevaluators.a',
                                'libfeatures.a',
                                'liblayers.a',
                                'liblinear.a',
                                'liblossFunctions.a',
                                'libpredictors.a',
                                'libtrainers.a',
                                'libutilities.a',
                            ],
                            'library_dirs': [
                                '<(library_path)/EMLL/build/libraries/common<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/dataset<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/evaluators<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/features<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/layers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/linear<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/lossFunctions<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/predictors<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/trainers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/utilities<(library_suffix)',
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
                                '-std=c++1y',
                                '-stdlib=libc++',
                                '-v'
                            ],
                            'OTHER_LDFLAGS': [
                                '-stdlib=libc++'
                            ],
                            'MACOSX_DEPLOYMENT_TARGET': '10.11',
                            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                            'GCC_ENABLE_CPP_RTTI': 'YES'
                        }
                    }
                ],
                [
                    'OS=="linux"',
                    {
                        'link_settings': {
                            'libraries': [
                                'libcommon.a',
                                'libdataset.a',
                                'libevaluators.a',
                                'libfeatures.a',
                                'liblayers.a',
                                'liblinear.a',
                                'liblossFunctions.a',
                                'libpredictors.a',
                                'libtrainers.a',
                                'libutilities.a',
                            ],
                            'library_dirs': [
                                '<(library_path)/EMLL/build/libraries/common<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/dataset<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/evaluators<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/features<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/layers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/linear<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/lossFunctions<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/predictors<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/trainers<(library_suffix)',
                                '<(library_path)/EMLL/build/libraries/utilities<(library_suffix)',
                            ]
                        }
                    }
                ]
            ],
        }
    ]
}
