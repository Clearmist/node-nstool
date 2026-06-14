# node-nstool

A Node.js native addon wrapping [nstool](https://github.com/jakcron/nstool) for reading and extracting Nintendo Switch package files (`.nsp`, `.xci`, `.nca`, and more).

## Requirements

- Node.js >= 22
- Prebuilt binaries are included for the following platforms:
  - `linux-x64`
  - `darwin-x64`
  - `darwin-arm64`
  - `win32-x64`

If no prebuild matches your platform the module will compile from source, which requires a C++20 toolchain and CMake.

## Installation

```sh
npm install node-nstool
```

## Usage

```js
import nstool from 'node-nstool';
```

### `nstool.information(options)`

Reads metadata and the filesystem tree from a Nintendo Switch package file.

```js
const result = nstool.information({ source: '/path/to/file.nsp' });

if (result.error) {
  console.error(result.errorMessage);
} else {
  console.log(result.data);   // parsed JSON from nstool
  console.log(result.events); // array of log events
}
```

### `nstool.extract(options)`

Extracts the contents of a package file into a directory.

```js
const result = nstool.extract({
  source: '/path/to/file.nsp',
  outputDirectory: '/path/to/output',
});

if (result.error) {
  console.error(result.errorMessage);
}
```

To extract a single file by name:

```js
const result = nstool.extract({
  source: '/path/to/file.nsp',
  outputDirectory: '/path/to/output',
  fileName: 'program.nca',
});
```

### Options

| Option            | Type    | Methods              | Description                                      |
|-------------------|---------|----------------------|--------------------------------------------------|
| `source`          | string  | all                  | Path to the input file. Required.                |
| `outputDirectory` | string  | `extract`            | Path to the output directory. Required.          |
| `fileName`        | string  | `extract`            | Extract only this file from the package.         |
| `showKeys`        | any     | all                  | Include key information in the output.           |
| `showLayout`      | any     | all                  | Include layout information in the output.        |
| `verbose`         | any     | all                  | Enable verbose output.                           |
| `type`            | string  | all                  | Override the file type detection.                |

### Return value

All methods return a plain object. On success:

```js
{
  data: { /* parsed nstool JSON output */ },
  events: [ /* array of log/event strings */ ],
  parameters: [ /* the argument array passed to nstool */ ]
}
```

On failure:

```js
{
  error: true,
  errorMessage: 'Human-readable description of the problem.'
}
```

## Testing

```sh
npm test
```

The test suite requires `test.nsp` and `test.xci` test files to be present in the project root.

## Building from source

```sh
npm run build
```

This compiles the bundled C++ dependencies (libfmt, liblz4, libmbedtls, libtoolchain, libpietendo) and then the native addon.
