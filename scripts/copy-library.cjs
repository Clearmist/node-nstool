#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// Get library name from command line argument
const libName = process.argv[2];

if (!libName) {
  console.error('Usage: copy-library.cjs <library-name>');
  process.exit(1);
}

// Determine target platform directory.
const isWin = process.platform === 'win32';
const platformDir = process.platform === 'darwin' ? 'macos' : (isWin ? 'win32' : 'linux');

// Resolve source artifact names and destination names per platform.
const unixArtifacts = {
  libfmt: { source: 'libfmt.a', dest: 'libfmt.a' },
  liblz4: { source: 'liblz4.a', dest: 'liblz4.a' },
  libmbedtls: { source: 'libmbedtls.a', dest: 'libmbedtls.a' },
  libtoolchain: { source: 'libtoolchain.a', dest: 'libtoolchain.a' },
  libpietendo: { source: 'libpietendo.a', dest: 'libpietendo.a' },
};

const winArtifacts = {
  libfmt: { sources: ['fmt.lib', 'libfmt.lib'], dest: 'fmt.lib' },
  liblz4: { sources: ['lz4.lib', 'liblz4.lib'], dest: 'liblz4.lib' },
  libmbedtls: { sources: ['mbedtls.lib', 'libmbedtls.lib'], dest: 'libmbedtls.lib' },
  libtoolchain: { sources: ['toolchain.lib', 'libtoolchain.lib', 'tc.lib'], dest: 'tc.lib' },
  libpietendo: { sources: ['pietendo.lib', 'libpietendo.lib'], dest: 'pietendo.lib' },
};

const artifact = isWin ? winArtifacts[libName] : unixArtifacts[libName];

if (!artifact) {
  console.error(`Unsupported library name: ${libName}`);
  process.exit(1);
}

const destDir = path.join(__dirname, '..', 'library');
const destFile = path.join(destDir, artifact.dest);

// Create library directory if it doesn't exist
if (!fs.existsSync(destDir)) {
  fs.mkdirSync(destDir, { recursive: true });
}

const libBinDir = path.join(__dirname, '..', 'deps', 'nstool', 'deps', libName, 'bin', platformDir);

let sourceFile = null;

if (isWin) {
  const configDirs = ['Release', 'RelWithDebInfo', 'MinSizeRel', 'Debug', ''];
  for (const configDir of configDirs) {
    for (const sourceName of artifact.sources) {
      const candidate = configDir
        ? path.join(libBinDir, configDir, sourceName)
        : path.join(libBinDir, sourceName);
      if (fs.existsSync(candidate)) {
        sourceFile = candidate;
        break;
      }
    }
    if (sourceFile) {
      break;
    }
  }
} else {
  const candidate = path.join(libBinDir, artifact.source);
  if (fs.existsSync(candidate)) {
    sourceFile = candidate;
  }
}

// Check if source file exists
if (!sourceFile) {
  const attempted = isWin ? artifact.sources.join(', ') : artifact.source;
  console.warn(`Warning: Source file not found in ${libBinDir} (attempted: ${attempted})`);
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
