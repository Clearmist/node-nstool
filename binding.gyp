{
    'targets': [
        {
            'target_name': 'node-nstool',
            'sources': [
                'src/node-nstool.cpp',
                "<!@(node binding.js sources)"
            ],
            'cflags': [
                '/std:c++17',
                '-std=c++17'
            ],
            'include_dirs': [
                "<!(node -p \"require('node-addon-api').include_dir\")",
                "<!@(node binding.js include_dirs)"
            ],
            'libraries': [
                '<!@(node binding.js libraries)'
            ],
            'dependencies': [
                "<!(node -p \"require('node-addon-api').gyp\")",
            ],
            'copies': [
                {
                    'destination': '<(module_root_dir)/build/Release',
                    'files': [
                        '<(module_root_dir)/library/libfmt/build/Release/fmt.dll',
                        '<(module_root_dir)/library/liblz4/build/Release/liblz4.dll',
                        '<(module_root_dir)/library/libmbedtls/build/Release/libmbedtls.dll',
                        '<(module_root_dir)/library/libtoolchain/build/Release/tc.dll',
                        '<(module_root_dir)/library/libpietendo/build/Release/pietendo.dll'
                    ]
                }
            ],
            'defines': [
                'NODE_ADDON_API',
            ],
            'conditions': [
                [
                    "OS=='win'",
                    {
                        'defines': [
                            '_HAS_EXCEPTIONS=1',
                            '_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS=1'
                        ],
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'AdditionalOptions': ['-std:c++17', '-permissive'],
                                'ExceptionHandling': 1
                            },
                        }
                    }
                ],
                [
                    "OS=='mac'",
                    {
                        'cflags+': ['-fvisibility=hidden'],
                        'xcode_settings': {
                            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                            'CLANG_CXX_LIBRARY': 'libc++',
                            'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
                            'MACOSX_DEPLOYMENT_TARGET': '10.9',
                            'OTHER_CFLAGS': [
                                '-arch x86_64',
                                '-arch arm64',
                            ],
                            'OTHER_LDFLAGS': [
                                '-arch x86_64',
                                '-arch arm64',
                            ],
                        },
                    }
                ],
            ],
        }
    ]
}
