const fs = require('fs');
const nstool = require('./build/Release/node-nstool.node');

const nodeNSTool = {
  error: function(errorMessage) {
    return {
      error: true,
      errorMessage,
    };
  },
  run: function(options, parameters) {
    if (typeof options?.showKeys !== 'undefined') {
      parameters.push('--showkeys');
    }

    if (typeof options?.showLayout !== 'undefined') {
      parameters.push('--showlayout');
    }

    if (typeof options?.verbose !== 'undefined') {
      parameters.push('--verbose');
    }

    // Make sure that the user provided a source file to process.
    if (typeof options?.source === 'undefined' || typeof options.source !== 'string') {
      return this.error('Provide a source file using the "source" option.');
    }

    // Make sure that the source file is readable.
    try {
      fs.accessSync(options.source, fs.constants.R_OK);
    } catch {
      return this.error(`The source file is not readable. Given: ${options.source}`);
    }

    const passing = [...parameters, options.source];

    try {
      const results = JSON.parse(nstool.run(...passing));

      results.parameters = passing;

      return results;
    } catch (error) {
      // Convert Napi::Error exceptions.
      return this.error(error.message);
    }
  },
  information: function(options) {
    const parameters = [
      'nstool',
      '--json',
      '--fstree',
    ];

    return this.run(options, parameters);
  },
  extract: function(options) {
    // Make sure that the user provided a source file to process.
    if (typeof options?.outputDirectory === 'undefined') {
      return this.error('Provide a full path to an output directory using the "outputDirectory " option.');
    }

    if (typeof options.outputDirectory !== 'string') {
      return this.error('The path of the output directory must be a string.');
    }

    // Make sure that the output directory is writable.
    try {
      fs.accessSync(options.outputDirectory, fs.constants.W_OK);
    } catch {
      return this.error(`The output directory is not writable. Given: ${options.outputDirectory}`);
    }

    const parameters = [
      'nstool',
      '--json',
      '--type',
      'nca',
      '--extract',
      options.outputDirectory,
    ];

    if (typeof options?.fileName !== 'undefined') {
      if (typeof options.fileName !== 'string') {
        return this.error('The file name of the file you want to extract must be a string.');
      }

      // The user wants to extract a single file.
      parameters.push('--file');
      parameters.push(options.fileName);
    }

    return this.run(options, parameters);
  },
};

module.exports = nodeNSTool;
