#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

#include "amf0.h"

/* prototype */
int amf0_encode(amf0_t* amf, char* buf);

static amf0_data_t* _amf0_data_rv(SV* sv);

static amf0_data_t* _amf0_data(SV* sv) {
    amf0_data_t* d;

    if (NULL == sv || !SvOK(sv)) {
        d = (amf0_data_t*)amf0_null_init();
    }
    else if (SvPOKp(sv)) {
        STRLEN len;
        char*  c = SvPV(sv, len);

        d = (amf0_data_t*)amf0_string_init_len(c, len);
    }
    else if (SvNOKp(sv)) {
        d = (amf0_data_t*)amf0_number_init((double)SvNVX(sv));
    }
    else if (SvIOK_UV(sv)) {
        d = (amf0_data_t*)amf0_number_init((double)SvUV(sv));
    }
    else if (SvIOKp(sv)) {
        d = (amf0_data_t*)amf0_number_init((double)SvIV(sv));
    }
    else if (SvROK(sv)) {
        d = _amf0_data_rv(SvRV(sv));
    }
    else {
        Perl_croak(aTHX_ "Data::AMF::XS doesn't support SvTYPE: %d\n", SvTYPE(sv));
    }

    return d;
}

static amf0_data_t* _amf0_data_rv(SV* sv) {
    svtype svt = SvTYPE(sv);
    int i, len, count;
    amf0_data_t* d;
    SV** svp;
    SV* k;
    AV* ary;
    HV* hval;
    HE* he;
    STRLEN strlen;
    char* key;

    if (SVt_PVAV == svt) {
        d = (amf0_data_t*)amf0_strictarray_init();

        ary = (AV*)sv;
        len = av_len(ary) + 1;

        for (i = 0; i < len; ++i) {
            svp = av_fetch(ary, i, 0);
            if (svp) {
                amf0_strictarray_add((amf0_strictarray_t*)d, _amf0_data(*svp));
            }
            else {
                amf0_strictarray_add((amf0_strictarray_t*)d, _amf0_data(NULL));
            }
        }
    }
    else if (SVt_PVHV == svt) {
        d = (amf0_data_t*)amf0_object_init();

        hval  = (HV*)sv;
        count = hv_iterinit(hval);

        while ( (he = hv_iternext(hval)) ) {
            k = hv_iterkeysv(he);
            key = SvPV(k, strlen);

            amf0_object_add((amf0_object_t*)d, key, _amf0_data(HeVAL(he)));
        }
    }
    else {
        Perl_croak(aTHX_ "Data::AMF::XS doesn't support references except ARRAY and HASH");
    }

    return d;
}

XS(encode_amf0) {
    dXSARGS;

    amf0_t* amf0;
    amf0_data_t* d;
    int i, r, len;
    char* b;
    SV* ret;

    if (items < 1) {
        Perl_croak(aTHX_ "Usage: encode_amf0(@objs)");
    }

    amf0 = amf0_init();

    for (i = 0; i < items; ++i) {
        d = _amf0_data((SV*)ST(i));
        if (NULL != d) amf0_append(amf0, d);
    }

    len = amf0_encode(amf0, NULL);
    b = (char*)calloc(1, len);

    r = amf0_encode(amf0, b);

    ret = (SV*)0;
    if (r >= 0) {
        ret = sv_2mortal(newSVpvn(b, len));
    }

    amf0_free(amf0);
    free(b);

    ST(0) = ret;
    XSRETURN(1);
}

static SV* _amf0_sv(amf0_data_t* data) {
    SV* sv = NULL;
    SV* svh;
    SV* sva;
    HV* hv;
    AV* av;
    int i;
    amf0_object_t* obj;
    const char* key;
    amf0_data_t* value;

    switch (data->type) {
        case AMF0_NUMBER:
            sv = newSVnv(((amf0_number_t*)data)->value);
            break;
        case AMF0_BOOLEAN:
            sv = newSViv(((amf0_boolean_t*)data)->value);
            break;
        case AMF0_STRING:
            sv = newSV(0);
            sv_setpv(sv, ((amf0_string_t*)data)->value);
            break;
        case AMF0_OBJECT:
            hv = newHV();
            obj = (amf0_object_t*)data;

            for (i = 0; i < obj->used; ++i) {
                key   = obj->data[i]->key;
                value = obj->data[i]->value;

                svh = _amf0_sv(value);
                hv_store(hv, key, strlen(key), svh, 0);
            }

            sv = newRV(sv_2mortal((SV*)hv));

            break;
        case AMF0_NULL:
        case AMF0_UNDEFINED:
            sv = newSV(0);
            break;
        case AMF0_STRICTARRAY:
            av = newAV();

            for (i = 0; i < ((amf0_strictarray_t*)data)->used; ++i) {
                sva = _amf0_sv(((amf0_strictarray_t*)data)->data[i]);
                av_push(av, sva);
            }

            sv = newRV(sv_2mortal((SV *)av));

            break;
        default:
            Perl_croak(aTHX_ "Unsupported datatype: %d\n", data->type);
            break;
    }

    return sv;
}

XS(decode_amf0) {
    dXSARGS;

    STRLEN len;
    char*  data;
    amf0_t* amf0;
    int i;
    SV* sv;

    if (items != 1) {
        Perl_croak(aTHX_ "Usage: decode_amf0($amf_data)");
    }

    data = SvPV((SV*)ST(0), len);

    amf0 = amf0_decode(data, len);
    if (NULL == amf0) {
        Perl_croak(aTHX_ "Failed to decode AMF data.");
    }

    for (i = 0; i < amf0->used; ++i) {
        sv = _amf0_sv(amf0->data[i]);
        ST(i) = sv_2mortal(sv);
    }

    amf0_free(amf0);

    XSRETURN(i);
}

XS(boot_Data__AMF__XS) {
    newXS("Data::AMF::XS::encode_amf0", encode_amf0, __FILE__);
    newXS("Data::AMF::XS::decode_amf0", decode_amf0, __FILE__);
}

