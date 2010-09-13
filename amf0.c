#include "amf0.h"
#include "byteorderutil.h"

int amf0_encode_data(amf0_data_t* data, char* buf) {
    int r;

    switch (data->type) {
        case AMF0_NUMBER:
            r = amf0_encode_number((amf0_number_t*)data, buf);
            break;
        case AMF0_BOOLEAN:
            r = amf0_encode_boolean((amf0_boolean_t*)data, buf);
            break;
        case AMF0_STRING:
            r = amf0_encode_string((amf0_string_t*)data, buf);
            break;
        case AMF0_OBJECT:
            r = amf0_encode_object((amf0_object_t*)data, buf);
            break;
        case AMF0_NULL:
            r = amf0_encode_null((amf0_null_t*)data, buf);
            break;
        case AMF0_UNDEFINED:
            r = amf0_encode_undefined((amf0_undefined_t*)data, buf);
            break;
        case AMF0_STRICTARRAY:
            r = amf0_encode_strictarray((amf0_strictarray_t*)data, buf);
            break;
    }

    return r;
}

int amf0_decode_data(amf0_data_t** data, const char* buf, int len) {
    int p = 0;
    amf0_type_t type = buf[p++];

    int r = 0;
    switch (type) {
        case AMF0_NUMBER:
            r = amf0_decode_number(data, buf + p, len - p);
            break;
        case AMF0_BOOLEAN:
            r = amf0_decode_boolean(data, buf + p, len - p);
            break;
        case AMF0_STRING:
            r = amf0_decode_string(data, buf + p, len - p);
            break;
        case AMF0_OBJECT:
            r = amf0_decode_object(data, buf + p, len - p);
            break;
        case AMF0_NULL:
            r = amf0_decode_null(data, buf + p, len - p);
            break;
        case AMF0_UNDEFINED:
            r = amf0_decode_undefined(data, buf + p, len - p);
            break;
        case AMF0_STRICTARRAY:
            r = amf0_decode_strictarray(data, buf + p, len - p);
            break;
        default:
            fprintf(stderr, "unknown type %x\n", type);
            r = -1;
    }

    if (r < 0) return r;

    return p + r;
}

void amf0_data_free(amf0_data_t* data) {
    switch (data->type) {
        case AMF0_NUMBER:
            amf0_number_free((amf0_number_t*)data);
            break;
        case AMF0_BOOLEAN:
            amf0_boolean_free((amf0_boolean_t*)data);
            break;
        case AMF0_STRING:
            amf0_string_free((amf0_string_t*)data);
            break;
        case AMF0_OBJECT:
            amf0_object_free((amf0_object_t*)data);
            break;
        case AMF0_NULL:
            amf0_null_free((amf0_null_t*)data);
            break;
        case AMF0_UNDEFINED:
            amf0_undefined_free((amf0_undefined_t*)data);
            break;
        case AMF0_STRICTARRAY:
            amf0_strictarray_free((amf0_strictarray_t*)data);
            break;
    }
}

amf0_number_t* amf0_number_init(double n) {
    amf0_number_t* number = calloc(1, sizeof(amf0_number_t));
    number->type  = AMF0_NUMBER;
    number->value = n;

    return number;
}

void amf0_number_free(amf0_number_t* number) {
    free(number);
}

amf0_boolean_t* amf0_boolean_init(int b) {
    amf0_boolean_t* bool = calloc(1, sizeof(amf0_boolean_t));
    bool->type  = AMF0_BOOLEAN;
    bool->value = b;

    return bool;
}

void amf0_boolean_free(amf0_boolean_t* bool) {
    free(bool);
}

amf0_string_t* amf0_string_init(const char* buf) {
    amf0_string_t* string = calloc(1, sizeof(amf0_string_t));
    string->type  = AMF0_STRING;
    string->value = calloc(1, strlen((char*)buf) + 1);
    memcpy(string->value, buf, strlen(buf));

    return string;
}

amf0_string_t* amf0_string_init_len(const char* buf, int len) {
    amf0_string_t* string = calloc(1, sizeof(amf0_string_t));
    string->type  = AMF0_STRING;
    string->value = calloc(1, len);
    memcpy(string->value, buf, len);

    return string;
}

void amf0_string_free(amf0_string_t* string) {
    free(string->value);
    free(string);
}

amf0_object_t* amf0_object_init() {
    amf0_object_t* obj = calloc(1, sizeof(amf0_object_t));
    obj->type = AMF0_OBJECT;

    return obj;
}

void amf0_object_free(amf0_object_t* obj) {
    int i;
    amf0_object_keyvalue_t* kv;

    for (i = 0; i < obj->used; ++i) {
        kv = obj->data[i];

        amf0_data_free(kv->value);
        free(kv);
    }

    if (NULL != obj->data) free(obj->data);
    free(obj);
}

void amf0_object_add(amf0_object_t* obj, const char* key, amf0_data_t* value) {
    if (NULL == obj->data) {
        obj->data = calloc(1, sizeof(amf0_object_keyvalue_t*));
    }
    else {
        obj->data =
            realloc(obj->data, sizeof(amf0_object_keyvalue_t*) * (obj->used + 1));
    }
    assert(NULL != obj->data);

    amf0_object_keyvalue_t* kv = calloc(1, sizeof(amf0_object_keyvalue_t));
    kv->key   = key;
    kv->value = value;

    obj->data[obj->used] = kv;
    obj->used++;
}

amf0_null_t* amf0_null_init() {
    amf0_null_t* null = calloc(1, sizeof(amf0_null_t));
    null->type = AMF0_NULL;
    return null;
}

void amf0_null_free(amf0_null_t* null) {
    free(null);
}

amf0_undefined_t* amf0_undefined_init() {
    amf0_undefined_t* undef = calloc(1, sizeof(amf0_undefined_t));
    undef->type = AMF0_UNDEFINED;
    return undef;
}

void amf0_undefined_free(amf0_undefined_t* undef) {
    free(undef);
}

amf0_strictarray_t* amf0_strictarray_init() {
    amf0_strictarray_t* array = calloc(1, sizeof(amf0_strictarray_t));
    array->type = AMF0_STRICTARRAY;

    return array;
}

void amf0_strictarray_add(amf0_strictarray_t* array, amf0_data_t* data) {
    if (NULL == array->data) {
        array->data = calloc(1, sizeof(amf0_data_t*));
    }
    else {
        array->data = realloc(array->data, sizeof(amf0_data_t*) * (array->used + 1));
    }

    array->data[array->used] = data;
    array->used++;
}

void amf0_strictarray_free(amf0_strictarray_t* array) {
    int i;
    amf0_data_t* data;

    for (i = 0; i < array->used; ++i) {
        data = array->data[i];
        amf0_data_free(data);
    }

    if (NULL != array->data) free(array->data);
    free(array);
}

amf0_t* amf0_init() {
    amf0_t* data = calloc(1, sizeof(amf0_t));
    return data;
}

void amf0_free(amf0_t* amf) {
    int i;
    amf0_data_t* data;

    for (i = 0; i < amf->used; ++i) {
        data = amf->data[i];
        amf0_data_free(data);
    }

    if (NULL != amf->data) free(amf->data);
    free(amf);
}

amf0_t* amf0_decode(const char* buf, int len) {
    int r, p;
    int offset = 0;

    amf0_t* amf = amf0_init();

    while (offset < len) {
        amf0_data_t* data;
        r = amf0_decode_data(&data, buf + offset, len - offset);

        if (r < 0) break;

        offset += r;

        assert(NULL != data);
        amf0_append(amf, data);
    }

    return amf;
}

void amf0_append(amf0_t* amf, amf0_data_t* data) {
    if (NULL == amf->data) {
        amf->data = calloc(1, sizeof(amf0_data_t*));
    }
    else {
        amf->data = realloc(amf->data, sizeof(amf0_data_t*) * (amf->used + 1));
    }
    assert(NULL != amf->data);

    amf->data[amf->used] = data;
    amf->used++;
}

int amf0_encode(amf0_t* amf, char* buf) {
    int i, r, p = 0;
    char* b;
    amf0_data_t* data;

    for (i = 0; i < amf->used; ++i) {
        data = amf->data[i];
        b = NULL == buf ? NULL : buf + p;

        r = amf0_encode_data(data, b);

        assert(r >= 0);
        p += r;
    }

    return p;
}

int amf0_encode_number(amf0_number_t* number, char* buf) {
    if (NULL != buf) {
        buf[0] = AMF0_NUMBER;

        double value = number->value;
#ifdef __LITTLE_ENDIAN__
        swap_bytes(&value, sizeof(value));
#endif

        memcpy(buf + 1, (void*)&value, 8);
    }
    return 9;
}

int amf0_decode_number(amf0_data_t** data, const char* buf, int len) {
    if (len < 8) return -1;

    double n;
    memcpy(&n, buf, 8);
#ifdef __LITTLE_ENDIAN__
    swap_bytes(&n, 8);
#endif

    amf0_number_t* number = amf0_number_init(n);
    *data = (amf0_data_t*)number;

    return 8;
}

int amf0_encode_boolean(amf0_boolean_t* bool, char* buf) {
    if (NULL != buf) {
        buf[0] = AMF0_BOOLEAN;
        buf[1] = bool->value ? 1 : 0;
    }

    return 2;
}

int amf0_decode_boolean(amf0_data_t** data, const char* buf, int len) {
    if (len < 1) return -1;

    amf0_boolean_t* bool = amf0_boolean_init(buf[0]);
    *data = (amf0_data_t*)bool;

    return 1;
}

int amf0_encode_string(amf0_string_t* string, char* buf) {
    if (NULL != buf) {
        buf[0] = AMF0_STRING;

        unsigned short len = strlen(string->value);
#ifdef __LITTLE_ENDIAN__
        swap_bytes(&len, 2);
#endif

        memcpy(buf + 1, (void*)&len, 2);
        memcpy(buf + 3, (void*)string->value, strlen(string->value));
    }

    return 3 + strlen(string->value);
}

int amf0_decode_string(amf0_data_t** data, const char* buf, int len) {
    if (len < 2) return -1;

    unsigned short s;
    memcpy(&s, buf, 2);
#ifdef __LITTLE_ENDIAN__
    swap_bytes(&s, 2);
#endif
    len -= 2;

    if (len < s) return -1;

    char* b = calloc(1, s+1);
    memcpy(b, buf + 2, s+1);
    b[s] = '\0';

    amf0_string_t* string = amf0_string_init(b);
    *data = (amf0_data_t*)string;
    free(b);

    return s + 2;
}

int amf0_encode_object(amf0_object_t* obj, char* buf) {
    int p = 0;
    int i, r;
    amf0_object_keyvalue_t* kv;
    amf0_string_t* key;

    if (NULL != buf)
        buf[p] = AMF0_OBJECT;
    p += 1;

    for (i = 0; i < obj->used; ++i) {
        kv = obj->data[i];

        if (NULL != buf) {
            unsigned short len = strlen(kv->key);
#ifdef __LITTLE_ENDIAN__
            swap_bytes(&len, 2);
#endif

            memcpy(buf + p, &len, 2);
        }
        p += 2;

        if (NULL != buf) {
            memcpy(buf + p, kv->key, strlen(kv->key));
        }
        p += strlen(kv->key);

        char* b = NULL == buf ? NULL : buf + p;
        r = amf0_encode_data(kv->value, b);

        assert(r);

        p += r;
    }

    if (NULL != buf) {
        buf[p++] = 0x00;
        buf[p++] = 0x00;
        buf[p++] = AMF0_OBJECTEND;
    }
    else {
        p += 3;
    }

    return p;
}

int amf0_decode_object(amf0_data_t** data, const char* buf, int len) {
    int offset = 0;
    int p, r;

    amf0_object_t *obj = amf0_object_init();

    while (offset < len) {
        p = offset;

        if (len < (p + 2)) return -1;

        unsigned short s;
        memcpy(&s, buf + p, 2);
#ifdef __LITTLE_ENDIAN__
        swap_bytes(&s, 2);
#endif
        p += 2;

        if (len < (p + s)) return -1;

        char* b = calloc(1, s);
        memcpy(b, buf + p, s);
        p += s;

        if (b[0] == 0x00 && b[1] == 0x00) {
            /* object end */
            assert(buf[p++] == AMF0_OBJECTEND);
            offset += p;
            free(b);
            break;
        }

        amf0_data_t* data;
        r = amf0_decode_data(&data, buf + p, len - p);
        if (r < 0) break;

        p += r;
        offset = p;

        assert(NULL != data);

        amf0_object_add(obj, b, data);
        free(b);
    }

    *data = (amf0_data_t*)obj;

    return offset;
}

int amf0_encode_null(amf0_null_t* null, char* buf) {
    if (NULL != buf) {
        buf[0] = AMF0_NULL;
    }
    return 1;
}

int amf0_decode_null(amf0_data_t** data, const char* buf, int len) {
    amf0_null_t* null = amf0_null_init();
    *data = (amf0_data_t*)null;
    return 0;
}

int amf0_encode_undefined(amf0_undefined_t* undef, char* buf) {
    if (NULL != buf) {
        buf[0] = AMF0_UNDEFINED;
    }
    return 1;
}

int amf0_decode_undefined(amf0_data_t** data, const char* buf, int len) {
    amf0_undefined_t* undef = amf0_undefined_init();
    *data = (amf0_data_t*)undef;
    return 0;
}

int amf0_encode_strictarray(amf0_strictarray_t* array, char* buf) {
    int i, r;
    int p = 0;

    if (NULL != buf)
        buf[0] = AMF0_STRICTARRAY;
    p++;

    if (NULL != buf) {
        int len = array->used;

#ifdef __LITTLE_ENDIAN__
        swap_bytes(&len, 4);
#endif
        memcpy(buf + p, &len, 4);
    }
    p += 4;

    for (i = 0; i < array->used; ++i) {
        char* b = NULL == buf ? NULL : buf + p;
        amf0_data_t* data = array->data[i];

        r = amf0_encode_data(data, b);
        assert(r);

        p += r;
    }

    return p;
}

int amf0_decode_strictarray(amf0_data_t** data, const char* buf, int len) {
    if (len < 4) return -1;

    int count;
    memcpy(&count, buf, 4);
#ifdef __LITTLE_ENDIAN__
    swap_bytes(&count, 4);
#endif

    amf0_strictarray_t* array = amf0_strictarray_init();

    int offset = 4;
    while (array->used < count && offset < len) {
        amf0_data_t* data;
        int r = amf0_decode_data(&data, buf + offset, len - offset);

        if (r < 0) break;

        offset += r;
        amf0_strictarray_add(array, data);
    }

    *data = (amf0_data_t*)array;
}
