
/*
 * Copyright [2024] <Macx Buddhi Chaturanga>
 */

#ifndef IMCL_LIB_INCLUDE_IMCL_TYPES_H_
#define IMCL_LIB_INCLUDE_IMCL_TYPES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************/
typedef int8_t imcl_int8;
typedef uint8_t imcl_uint8;

typedef int32_t imcl_int32;
typedef uint32_t imcl_uint32;

typedef int64_t imcl_int64;
typedef uint64_t imcl_uint64;
/****************************************/

typedef enum imcl_status_t {
    IMCL_FAILURE = 0,
    IMCL_SUCCESS = 1,
}IMCL_STATUS;

#ifdef __cplusplus
}
#endif
#endif  // IMCL_LIB_INCLUDE_IMCL_TYPES_H_
