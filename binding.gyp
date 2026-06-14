{
    'variables': {
        'openssl_fips': ''
    },
    'targets': [
        {
            'target_name': 'node-nstool',
            'sources': [
                'src/node-nstool.cpp',
                "<!@(node binding.cjs sources)"
            ],
            'cflags': [
                '-std=c++17',
            ],
            'cflags_cc': [
                '-std=c++17'
            ],
            'cflags!': [
                '-fno-exceptions',
                '-std=c++17'
            ],
            "cflags_cc!": [
                '-fno-exceptions',
                '-std=c++17'
            ],
            'include_dirs': [
                "<!(node -p \"require('node-api-headers').include_dir\")",
                "<!(node -p \"require('node-addon-api').include_dir\")",
                "<!@(node -p \"require('node-addon-api').include\")",
                "<!@(node binding.cjs include_dirs)"
            ],
            'libraries': [
                '<!@(node binding.cjs libraries)'
            ],
            'dependencies': [
                "<!(node -p \"require('node-addon-api').gyp\")",
                "<!(node -p \"require('node-addon-api').targets\"):node_addon_api",
            ],
            'defines': [
                'NODE_ADDON_API',
            ],
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'AdditionalOptions': [
                        '-std:c++17',
                        '-permissive'
                    ],
                    'ExceptionHandling': 1,
                    'RuntimeLibrary': 2
                },
            },
            'xcode_settings': {
                'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
                'MACOSX_DEPLOYMENT_TARGET': '10.9',
                'OTHER_CFLAGS': [
                    '-arch x86_64',
                    '-arch arm64',
                    '-std=c++17',
                    '-stdlib=libc++',
                    '-fexceptions'
                ],
                'OTHER_LDFLAGS': [
                    '-arch x86_64',
                    '-arch arm64',
                ],
            },
            'conditions': [
                [
                    "OS=='win'",
                    {
                        'copies': [
                            {
                                'destination': '<(module_root_dir)/build/Release',
                                'files': [
                                    '<(module_root_dir)/library/fmt.lib',
                                    '<(module_root_dir)/library/liblz4.lib',
                                    '<(module_root_dir)/library/libmbedtls.lib',
                                    '<(module_root_dir)/library/tc.lib',
                                    '<(module_root_dir)/library/pietendo.lib'
                                ]
                            }
                        ],
                        'defines': [
                            '_HAS_EXCEPTIONS=1',
                            '_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS=1'
                        ],
                    }
                ],
                [
                    "OS=='mac'",
                    {
                        'cflags+': ['-fvisibility=hidden'],
                    }
                ],
                [
                    "OS=='linux'",
                    {
                        'cflags+': [
                            '-fvisibility=hidden',
                            '-fPIC',
                        ],
                        'cflags_cc+': [
                            '-fvisibility=hidden',
                            '-fPIC',
                        ],
                    }
                ],
            ],
        }
    ]
}
