#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// Get library name from command line argument
const libName = process.argv[2];

if (!libName) {
  console.error('Usage: copy-library.js <library-name>');
  process.exit(1);
}

// Determine platform directory
const isWin = process.platform === 'win32';
const platformDir = process.platform === 'darwin' ? 'macos' : (isWin ? 'win32' : 'linux');

// Determine file extension
const fileExt = isWin ? '.lib' : '.a';

// Map library names to their file names (may differ from folder name)
const libNameMap = {
  libfmt: 'libfmt',
  liblz4: 'liblz4',
  libmbedtls: 'libmbedtls',
  libtoolchain: 'libtoolchain',
  libpietendo: 'libpietendo',
};

const fileName = libNameMap[libName] || libName;
const fileNameWithExt = `${fileName}${fileExt}`;

// Build source and destination paths
const sourceFile = path.join(__dirname, '..', 'deps', 'nstool', 'deps', libName, 'bin', platformDir, fileNameWithExt);
const destDir = path.join(__dirname, '..', 'library');
const destFile = path.join(destDir, fileNameWithExt);

// Create library directory if it doesn't exist
if (!fs.existsSync(destDir)) {
  fs.mkdirSync(destDir, { recursive: true });
}

// Check if source file exists
if (!fs.existsSync(sourceFile)) {
  console.warn(`Warning: Source file not found: ${sourceFile}`);
  process.exit(0); // Don't fail, just warn
}

// Copy the file
try {
  fs.copyFileSync(sourceFile, destFile);
  console.log(`Copied ${path.basename(sourceFile)} to library directory`);
} catch (error) {
  console.error(`Error copying ${libName}: ${error.message}`);
  process.exit(1);
}
