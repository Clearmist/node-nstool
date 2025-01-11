# node-nstool
Node native module to access nstool.

Needs c++17 in binding.gyp.

Needs openssl_fips declared in these files:
    `node_modules\node-addon-api\node_api.gyp`
    `node_modules\node-addon-api\node_addon_api.gyp`
  
```python
"variables": {
  'openssl_fips': ''
},
```
