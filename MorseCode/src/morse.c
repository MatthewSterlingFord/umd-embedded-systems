#include "morse.h"

// Breadth first traversal of a morse code tree where a left branch is a dot
// and a right branch is a dash.
// NULL bytes (\x00) represent unused entries.
// See morse_decode for how this can be indexed into in O(1) time
const char *TABLE =
    "\x00"
        "et"
        "ianm"
        "surwdkgo"
        "hvf\x00l\x00pjbxcyzq\x00\x00"
        "54\x003\x00\x00\x002\x00\x00\x00\x00\x00\x00\x0016\x00\x00\x00\x00\x00\x00\x007\x00\x00\x008\x009\x00";

/* Decode `code`, whose last `len` bits represent a morse code where a dot
 * is a 0 and a dash is a 1.
 *
 * `len` should be <= 5.
 */
char morse_decode(uint_fast8_t code, uint_fast8_t len) {
  // The table is a binary tree, but that is equivalent to a series of
  // lookup tables of size 2^1, 2^2, etc, which we can index into when given the
  // morse code as a binary value.
  //
  // Note that this really only moves the O(n) tree traversal (for n = len) into
  // the caller's loop which builds the `code` value. However, since that loop
  // has to be done anyways its still a decent savings.
  return (TABLE + ((1 << (len) - 1)))[code];
}
