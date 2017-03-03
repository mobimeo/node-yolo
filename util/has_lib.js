/**
taken from https://github.com/Automattic/node-canvas/blob/master/util/has_lib.js
License

(The MIT License)

Copyright (c) 2010 LearnBoost, and contributors <dev@learnboost.com>

Copyright (c) 2014 Automattic, Inc and contributors <dev@automattic.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
var query = process.argv[2]
var fs = require('fs')
var childProcess = require('child_process')

var SYSTEM_PATHS = [
  '/lib',
  '/usr/lib',
  '/usr/local/lib',
  '/opt/local/lib',
  '/usr/lib/x86_64-linux-gnu',
  '/usr/lib/i386-linux-gnu'
]

/**
 * Checks for lib using ldconfig if present, or searching SYSTEM_PATHS
 * otherwise.
 * @param String library name, e.g. 'jpeg' in 'libjpeg64.so' (see first line)
 * @return Boolean exists
 */
function hasSystemLib (lib) {
  var libName = 'lib' + lib + '.+(so|dylib)'
  var libNameRegex = new RegExp(libName)

    // Try using ldconfig on linux systems
  if (hasLdconfig()) {
    try {
      if (childProcess.execSync('ldconfig -p 2>/dev/null | grep -E "' + libName + '"').length) {
        return true
      }
    } catch (err) {
      // noop -- proceed to other search methods
    }
  }

    // Try checking common library locations
  return SYSTEM_PATHS.some(function (systemPath) {
    try {
      var dirListing = fs.readdirSync(systemPath)
      return dirListing.some(function (file) {
        return libNameRegex.test(file)
      })
    } catch (err) {
      return false
    }
  })
}

/**
 * Checks for ldconfig on the path and /sbin
 * @return Boolean exists
 */
function hasLdconfig () {
  try {
    // Add /sbin to path as ldconfig is located there on some systems -- e.g.
    // Debian (and it can still be used by unprivileged users):
    childProcess.execSync('export PATH="$PATH:/sbin"')
    process.env.PATH = '...'
    // execSync throws on nonzero exit
    childProcess.execSync('hash ldconfig 2>/dev/null')
    return true
  } catch (err) {
    return false
  }
}

/**
* Check if cuda is present
* @return Boolean exists
*/
function hasCuda () {
  try {
    return childProcess.execSync('[ -d /usr/local/cuda ] && echo "Yes"').length > 0;
  } catch (err) {
    return false
  }
}

function main (query) {
  switch (query) {
    case 'opencv':
      return hasSystemLib(query)
    case 'cuda':
      return hasCuda();
    default:
      throw new Error('Unknown library: ' + query)
  }
}

process.stdout.write(main(query).toString())