const fs = require('fs');
const napi = require('node-addon-api');

const arg = process.argv.slice(2);

// Individual files to include.
let list = [];

switch (arg[0]) {
  case 'sources':
    list = [
      'src/node-nstool.cpp',
    ];

    [
      './src/nstool',
      './src/deps/libfmt/src',
      './src/deps/liblz4/src',
      './src/deps/libmbedtls/src',
      './src/deps/libpietendo/src/ctr',
      './src/deps/libpietendo/src/ctr/es',
      './src/deps/libpietendo/src/hac',
      './src/deps/libpietendo/src/hac/es',
      './src/deps/libtoolchain/src',
    ].forEach((directory) => {
      fs.readdirSync(directory).filter((file) => file.endsWith('.c') || file.endsWith('.cpp')).forEach((file) => {
        list.push(`${directory}/${file}`);
      });
    });
    break;
  case 'include_dirs':
    list = [
      './src/nstool',
      './src/deps/libfmt/include',
      './src/deps/liblz4/include',
      './src/deps/libmbedtls/include',
      './src/deps/libpietendo/include',
      './src/deps/libtoolchain/include',
    ];
    break;
  case 'dependencies':
    list = [];
    break;
  default:
    console.log('[Error] Invalid binding key.');
}

console.log(list.join(' '));
