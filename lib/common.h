#ifndef LIB_COMMON_H_
#define LIB_COMMON_H_

/*
 * Some common macros
 */

// set bit
#define sbi(var, bit) { var |= _BV (bit); }
// clear bit
#define cbi(var, bit) { var &= ~_BV (bit); }
// bit is set
#define bis(var, bit) (var & _BV (bit))

#endif /* LIB_COMMON_H_ */
