const fs = require('fs');
const path = require('path');

const isWin = process.platform === 'win32';
const platformDir = process.platform === 'darwin' ? 'macos' : (isWin ? 'win32' : 'linux');

const arg = process.argv.slice(2);

// A list of files or directories to return.
let list = [];

// Helper function to check if library directory has prebuilt libraries
function usePrebuiltLibraries() {
  const libDir = './library';
  if (!fs.existsSync(libDir)) {
    return false;
  }

  if (isWin) {
    // Check for Windows .lib files
    return fs.existsSync(path.join(libDir, 'fmt.lib')) &&
           fs.existsSync(path.join(libDir, 'liblz4.lib')) &&
           fs.existsSync(path.join(libDir, 'libmbedtls.lib')) &&
           fs.existsSync(path.join(libDir, 'tc.lib')) &&
           fs.existsSync(path.join(libDir, 'pietendo.lib'));
  } else {
    // Check for Unix .a files
    return fs.existsSync(path.join(libDir, 'libfmt.a')) &&
           fs.existsSync(path.join(libDir, 'liblz4.a')) &&
           fs.existsSync(path.join(libDir, 'libmbedtls.a')) &&
           fs.existsSync(path.join(libDir, 'libtoolchain.a')) &&
           fs.existsSync(path.join(libDir, 'libpietendo.a'));
  }
}

switch (arg[0]) {
  case 'sources':
    const directory = './deps/nstool/src';

    fs.readdirSync(directory).filter((file) => file.endsWith('.c') || file.endsWith('.cpp')).forEach((file) => list.push(`${directory}/${file}`));
    break;
  case 'include_dirs':
    list = [
      './deps/nstool/src',
      './src/include',
      './deps/nstool/deps/libfmt/include',
      './deps/nstool/deps/liblz4/include',
      './deps/nstool/deps/libmbedtls/include',
      './deps/nstool/deps/libpietendo/include',
      './deps/nstool/deps/libtoolchain/include',
      './node_modules/node-api-headers/include',
    ];
    break;
  case 'libraries':
    if (usePrebuiltLibraries()) {
      // Use prebuilt libraries from library directory
      if (isWin) {
        list = [
          '<(module_root_dir)/library/fmt.lib',
          '<(module_root_dir)/library/liblz4.lib',
          '<(module_root_dir)/library/libmbedtls.lib',
          '<(module_root_dir)/library/tc.lib',
          '<(module_root_dir)/library/pietendo.lib',
        ];
      } else {
        list = [
          '<(module_root_dir)/library/libpietendo.a',
          '<(module_root_dir)/library/libtoolchain.a',
          '<(module_root_dir)/library/libmbedtls.a',
          '<(module_root_dir)/library/libfmt.a',
          '<(module_root_dir)/library/liblz4.a',
        ];
      }
    } else {
      // Fall back to built libraries in deps
      if (isWin) {
        list = [
          '<(module_root_dir)/library/fmt.lib',
          '<(module_root_dir)/library/liblz4.lib',
          '<(module_root_dir)/library/libmbedtls.lib',
          '<(module_root_dir)/library/tc.lib',
          '<(module_root_dir)/library/pietendo.lib',
        ];
      } else {
        list = [
          `<(module_root_dir)/deps/nstool/deps/libpietendo/bin/${platformDir}/libpietendo.a`,
          `<(module_root_dir)/deps/nstool/deps/libtoolchain/bin/${platformDir}/libtoolchain.a`,
          `<(module_root_dir)/deps/nstool/deps/libmbedtls/bin/${platformDir}/libmbedtls.a`,
          `<(module_root_dir)/deps/nstool/deps/libfmt/bin/${platformDir}/libfmt.a`,
          `<(module_root_dir)/deps/nstool/deps/liblz4/bin/${platformDir}/liblz4.a`,
        ];
      }
    }
    break;
  default:
    console.log('[Error] Invalid binding key.');
}

console.log(list.join(' '));
