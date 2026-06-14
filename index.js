import { createRequire } from 'node:module';

const require = createRequire(import.meta.url);
const nodeNSTool = require('./index.cjs');

export default nodeNSTool;
