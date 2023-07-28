const fs = require('fs');

const arg = process.argv.slice(2);

// A list of files or directories to return.
let list = [];

switch (arg[0]) {
  case 'sources':
    const directory = './src/nstool';

    fs.readdirSync(directory).filter((file) => file.endsWith('.c') || file.endsWith('.cpp')).forEach((file) => list.push(`${directory}/${file}`));
    break;
  case 'include_dirs':
    list = [
      './src/nstool',
      './src/deps/libfmt/include',
      './src/deps/liblz4/include',
      './src/deps/libmbedtls/include',
      './src/deps/libpietendo/include',
      './src/deps/libtoolchain/include',
      './src/deps/nlohmann',
    ];
    break;
  case 'libraries':
    list = [
      '<(module_root_dir)/library/libfmt/build/Release/fmt.lib',
      '<(module_root_dir)/library/liblz4/build/Release/liblz4.lib',
      '<(module_root_dir)/library/libmbedtls/build/Release/libmbedtls.lib',
      '<(module_root_dir)/library/libtoolchain/build/Release/tc.lib',
      '<(module_root_dir)/library/libpietendo/build/Release/pietendo.lib',
    ];
    break;
  default:
    console.log('[Error] Invalid binding key.');
}

console.log(list.join(' '));
