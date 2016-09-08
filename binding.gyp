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
                    'swig_include_path_prefix%': '../..',
                    'include_path_prefix%': '..',
                    'library_path_prefix%': '..',
                    'library_path_suffix%': '/Release'
                },
            }
        ],
        [
            'OS=="mac"', 
            {
                'variables': {
                    'swig_include_path_prefix%':'..',
                    'include_path_prefix%':'..',
                    'library_path_prefix%':'../..',
                    'library_path_suffix%': ''
                },
            }
        ],
        [
            'OS=="linux"', 
            {
                'variables': {
                    'swig_include_path_prefix%':'..',
                    'include_path_prefix%':'..',
                    'library_path_prefix%':'../..',
                    'library_path_suffix%': ''
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
                        '<(include_path_prefix)/EMLL/build/interfaces/xml/EMLLXML_wrap.xml'
                    ]    
                }                
            ],
            'dependencies': [],
            'include_dirs': [
                "<!(node -e \"require('nan')\")",
                '<(include_path_prefix)/EMLL/libraries/common/include',
                '<(include_path_prefix)/EMLL/libraries/dataset/include',
                '<(include_path_prefix)/EMLL/libraries/evaluators/include',
                '<(include_path_prefix)/EMLL/libraries/features/include',
                '<(include_path_prefix)/EMLL/libraries/layers/include',
                '<(include_path_prefix)/EMLL/libraries/linear/include',
                '<(include_path_prefix)/EMLL/libraries/model/include',
                '<(include_path_prefix)/EMLL/libraries/nodes/include',
                '<(include_path_prefix)/EMLL/libraries/lossFunctions/include',
                '<(include_path_prefix)/EMLL/libraries/predictors/include',
                '<(include_path_prefix)/EMLL/libraries/trainers/include',
                '<(include_path_prefix)/EMLL/libraries/utilities/include',
                '<(include_path_prefix)/EMLL/interfaces/common/include'
            ],
            'sources': [
                '<(include_path_prefix)/EMLL/interfaces/common/src/DataLoadersInterface.cpp',                
                '<(include_path_prefix)/EMLL/interfaces/common/src/DataLoadersInterface.cpp',
                '<(include_path_prefix)/EMLL/interfaces/common/src/RowDatasetInterface.cpp',
                '<(include_path_prefix)/EMLL/build/interfaces/javascript/EMLLJAVASCRIPT_wrap.cxx'
            ],
            'link_settings': {
                'library_dirs': [
                    '<(library_path_prefix)/EMLL/build/libraries/common<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/dataset<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/evaluators<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/features<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/layers<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/linear<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/model<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/nodes<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/lossFunctions<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/predictors<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/trainers<(library_path_suffix)',
                    '<(library_path_prefix)/EMLL/build/libraries/utilities<(library_path_suffix)',
                ]
            },
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
                                '-lmodel.lib',
                                '-lnodes.lib',
                                '-llossFunctions.lib',
                                '-lpredictors.lib',
                                '-ltrainers.lib',
                                '-lutilities.lib'
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
                                'libmodel.a',
                                'libnodes.a',
                                'liblossFunctions.a',
                                'libpredictors.a',
                                'libtrainers.a',
                                'libutilities.a',
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
                                'libcommon.a',
                                'libdataset.a',
                                'libevaluators.a',
                                'libfeatures.a',
                                'liblayers.a',
                                'liblinear.a',
                                'libmodel.a',
                                'libnodes.a',
                                'liblossFunctions.a',
                                'libpredictors.a',
                                'libtrainers.a',
                                'libutilities.a',
                            ]
                        }
                    }
                ]
            ],
        }
    ]
}
