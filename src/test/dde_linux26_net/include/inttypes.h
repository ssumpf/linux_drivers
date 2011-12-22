/*
 * \brief  Fixed-size integer definitions used by uIP
 * \author Christian Helmuth
 * \date   2009-02-21
 */

#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <base/fixed_stdint.h>

typedef genode_int8_t     int8_t;
typedef genode_uint8_t   uint8_t;
typedef genode_int16_t   int16_t;
typedef genode_uint16_t uint16_t;
typedef genode_int32_t   int32_t;
typedef genode_uint32_t uint32_t;
typedef genode_int64_t   int64_t;
typedef genode_uint64_t uint64_t;

typedef __SIZE_TYPE__     size_t;

#endif /* _INTTYPES_H_ */
