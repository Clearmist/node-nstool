{
    'targets': [
        {
            'target_name': 'node-nstool',
            'sources': [
                'src/node-nstool.cpp',
                "<!@(node binding.js sources)"
            ],
            'cflags_cc': [],
            'include_dirs': [
                "<!(node -p \"require('node-addon-api').include_dir\")",
                "<!@(node binding.js include_dirs)"
            ],
            'libraries': [],
            'dependencies': [
                "<!(node -p \"require('node-addon-api').gyp\")",
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