#!/usr/bin/env node
'use strict';

const { execSync } = require('child_process');
const path = require('path');

// Arguments: <relative-lib-dir> <lib-name>
const [libDir, libName] = process.argv.slice(2);

if (!libDir || !libName) {
  console.error('Usage: cmake-build.cjs <lib-dir> <lib-name>');
  process.exit(1);
}

const repoRoot = path.join(__dirname, '..');
const libPath = path.join(repoRoot, libDir);

// libfmt requires MSVC to be invoked with /utf-8 (defines __STDC_ISO_10646__).
// Pass the flag during configure only; forwarding it to cmake --build causes
// "Unknown argument -DCMAKE_CXX_FLAGS=/utf-8" on Windows.
const configureFlags = process.platform === 'win32' ? ' -- -DCMAKE_CXX_FLAGS=/utf-8' : '';
const copyCmd = `node "${path.join(__dirname, 'copy-library.cjs')}" ${libName}`;

execSync('cmake-js clean', { cwd: libPath, stdio: 'inherit' });
execSync(`cmake-js configure${configureFlags}`, { cwd: libPath, stdio: 'inherit' });
execSync('cmake-js build', { cwd: libPath, stdio: 'inherit' });
execSync(copyCmd, { cwd: libPath, stdio: 'inherit' });
