#ifndef _AMF0_H_
#define _AMF0_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

typedef enum {
    AMF0_NUMBER      = 0x00,
    AMF0_BOOLEAN     = 0x01,
    AMF0_STRING      = 0x02,
    AMF0_OBJECT      = 0x03,
    AMF0_MOVIECLIP   = 0x04,
    AMF0_NULL        = 0x05,
    AMF0_UNDEFINED   = 0x06,
    AMF0_REFERENCE   = 0x07,
    AMF0_ECMAARRAY   = 0x08,
    AMF0_OBJECTEND   = 0x09,
    AMF0_STRICTARRAY = 0x0a,
    AMF0_DATE        = 0x0b,
    AMF0_LONGSTRING  = 0x0c,
    AMF0_UNSUPPORTED = 0x0d,
    AMF0_RECORDSET   = 0x0e,
    AMF0_XMLDOCUMENT = 0x0f,
    AMF0_TYPEDOBJECT = 0x10,
} amf0_type_t;

typedef struct {
    amf0_type_t type;
} amf0_data_t;

typedef struct {
    amf0_type_t type;
    double value;
} amf0_number_t;

typedef struct {
    amf0_type_t type;
    int value;
} amf0_boolean_t;

typedef struct {
    amf0_type_t type;
    char* value;
} amf0_string_t;

typedef struct {
    char* key;
    amf0_data_t* value;
} amf0_object_keyvalue_t;

typedef struct {
    amf0_type_t type;
    int used;
    amf0_object_keyvalue_t** data;
} amf0_object_t;

typedef struct {
    amf0_type_t type;
} amf0_null_t;

typedef struct {
    amf0_type_t type;
    int value;
} amf0_undefined_t;

typedef struct {
    amf0_type_t type;
    int used;
    amf0_data_t** data;
} amf0_strictarray_t;


/* todo */

typedef struct {
    int used;
    amf0_data_t** data;
} amf0_t;

amf0_t* amf0_init();
void amf0_free(amf0_t* amf);

amf0_t* amf0_decode(const char* buf, int len);

void amf0_append(amf0_t* amf, amf0_data_t* data);

int amf0_encode_data(amf0_data_t* data, char* buf);
void amf0_data_free(amf0_data_t* data);

amf0_number_t* amf0_number_init(double n);
void amf0_number_free(amf0_number_t* number);

amf0_boolean_t* amf0_boolean_init(int bool_);
void amf0_boolean_free(amf0_boolean_t* bool_);

amf0_string_t* amf0_string_init(const char* buf);
amf0_string_t* amf0_string_init_len(const char* buf, int len);
void amf0_string_free(amf0_string_t* string);

amf0_object_t* amf0_object_init();
void amf0_object_free(amf0_object_t* obj);
void amf0_object_add(amf0_object_t* obj, const char* key, amf0_data_t* value);

amf0_null_t* amf0_null_init();
void amf0_null_free(amf0_null_t* null);

amf0_undefined_t* amf0_undefined_init();
void amf0_undefined_free(amf0_undefined_t* undef);

amf0_strictarray_t* amf0_strictarray_init();
void amf0_strictarray_add(amf0_strictarray_t* array, amf0_data_t* data);
void amf0_strictarray_free(amf0_strictarray_t* array);

#ifndef UNUSED
#define UNUSED(x) ( (void)(x) )
#endif

#endif

