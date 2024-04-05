/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_RESOURCE_PB_H_INCLUDED
#define PB_RESOURCE_PB_H_INCLUDED
#include "pb.h"
#include "common.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* Resource information. */
typedef struct _Resource {
    /* Set of attributes that describe the resource.
 Attribute keys MUST be unique (it is not allowed to have more than one
 attribute with the same key). */
    pb_callback_t attributes;
    /* dropped_attributes_count is the number of dropped attributes. If the value is 0, then
 no attributes were dropped. */
    uint32_t dropped_attributes_count;
} Resource;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Resource_init_default                    {{{NULL}, NULL}, 0}
#define Resource_init_zero                       {{{NULL}, NULL}, 0}

/* Field tags (for use in manual encoding/decoding) */
#define Resource_attributes_tag                  1
#define Resource_dropped_attributes_count_tag    2

/* Struct field encoding specification for nanopb */
#define Resource_FIELDLIST(X, a) \
X(a, CALLBACK, REPEATED, MESSAGE,  attributes,        1) \
X(a, STATIC,   SINGULAR, UINT32,   dropped_attributes_count,   2)
#define Resource_CALLBACK pb_default_field_callback
#define Resource_DEFAULT NULL
#define Resource_attributes_MSGTYPE KeyValue

extern const pb_msgdesc_t Resource_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Resource_fields &Resource_msg

/* Maximum encoded size of messages (where known) */
/* Resource_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif