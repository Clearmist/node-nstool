const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const test = require('node:test');

const addon = require('../index.js');

const fixtureNames = ['test.nsp', 'test.xci'];
const fixturePaths = Object.fromEntries(
  fixtureNames.map((fixtureName) => [fixtureName, path.resolve(fixtureName)])
);

const prebuildFilePaths = [
  path.resolve('prebuilds/linux-x64/node-nstool.node'),
  path.resolve('prebuilds/win32-x64/node-nstool.node'),
];

const currentPrebuildPath = path.resolve(
  'prebuilds',
  `${process.platform}-${process.arch}`,
  'node-nstool.node'
);

for (const fixturePath of Object.values(fixturePaths)) {
  if (!fs.existsSync(fixturePath)) {
    throw new Error(`Required testing file is missing: ${fixturePath}`);
  }
}

test('prebuild files are present in the repository', () => {
  for (const prebuildFilePath of prebuildFilePaths) {
    assert.ok(fs.existsSync(prebuildFilePath), `Missing prebuild file: ${prebuildFilePath}`);
    assert.ok(fs.statSync(prebuildFilePath).isFile(), `Prebuild path is not a file: ${prebuildFilePath}`);
  }
});

test('current platform prebuild loads as a native addon', () => {
  assert.ok(fs.existsSync(currentPrebuildPath), `Missing current platform prebuild: ${currentPrebuildPath}`);

  const prebuildAddon = require(currentPrebuildPath);

  assert.equal(typeof prebuildAddon, 'object');
  assert.equal(typeof prebuildAddon.run, 'function');
});

test('information returns data, events and parameters for test.nsp', () => {
  const source = fixturePaths['test.nsp'];
  const result = addon.information({ source });

  assert.equal(result.error, undefined);
  assert.equal(typeof result.data, 'object');
  assert.ok(Array.isArray(result.events));
  assert.deepEqual(result.parameters, ['nstool', '--json', '--fstree', source]);
  assert.equal(result.data.format, 'PartitionFs');
  assert.ok(Array.isArray(result.data.tree));
  assert.ok(result.data.tree.length > 0);
});

test('information returns data, events and parameters for test.xci', () => {
  const source = fixturePaths['test.xci'];
  const result = addon.information({ source });

  assert.equal(result.error, undefined);
  assert.equal(typeof result.data, 'object');
  assert.ok(Array.isArray(result.events));
  assert.deepEqual(result.parameters, ['nstool', '--json', '--fstree', source]);
  assert.equal(result.data.format, 'PartitionFs');
  assert.equal(typeof result.data.gameCardHeader, 'object');
});

test('extract writes files to the output directory', () => {
  const outputDirectory = fs.mkdtempSync(path.join(os.tmpdir(), 'node-nstool-'));
  const source = fixturePaths['test.nsp'];

  const result = addon.extract({ source, outputDirectory });

  assert.equal(result.error, undefined);
  assert.equal(typeof result.data, 'object');
  assert.deepEqual(result.parameters, [
    'nstool', '--json', '--fstree', '--extract', outputDirectory, source,
  ]);
  assert.ok(fs.readdirSync(outputDirectory).length > 0, 'output directory should contain extracted files');
});

test('wrapper returns an error shape when source is missing', () => {
  assert.deepEqual(addon.information({}), {
    error: true,
    errorMessage: 'Provide a source file using the "source" option.',
  });
});

test('wrapper returns an error shape when outputDirectory is missing', () => {
  assert.deepEqual(addon.extract({ source: fixturePaths['test.nsp'] }), {
    error: true,
    errorMessage: 'Provide a full path to an output directory using the "outputDirectory " option.',
  });
});
