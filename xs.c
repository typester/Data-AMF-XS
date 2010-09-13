#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#include "amf0.h"

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

    amf0_data_t* d = NULL;;

    if (SVt_PVAV == svt) {
        d = (amf0_data_t*)amf0_strictarray_init();

        AV* ary = (AV*)sv;
        int len = av_len(ary) + 1;

        int i;
        for (i = 0; i < len; ++i) {
            SV** svp = av_fetch(ary, i, 0);
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

        HV* hval  = (HV*)sv;
        int count = hv_iterinit(hval);
        HE* he;

        while (he = hv_iternext(hval)) {
            SV* k = hv_iterkeysv(he);
            STRLEN len;
            char*  key = SvPV(k, len);

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

    if (items < 1) {
        Perl_croak(aTHX_ "Usage: encode_amf0(@objs)");
    }

    amf0_t* amf0 = amf0_init();

    int i = 0;
    for (i = 0; i < items; ++i) {
        amf0_data_t* d = _amf0_data((SV*)ST(i));
        if (NULL != d) amf0_append(amf0, d);
    }

    int len = amf0_encode(amf0, NULL);
    char* b = calloc(1, len);

    int r = amf0_encode(amf0, b);

    SV* ret = (SV*)0;
    if (r >= 0) {
        ret = sv_2mortal(newSVpvn(b, len));
    }
    ST(0) = ret;
    XSRETURN(1);

    free(b);
    amf0_free(amf0);
}

static SV* _amf0_sv(amf0_data_t* data) {
    SV* sv = NULL;

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
        case AMF0_OBJECT: {
            HV* hv = newHV();

            int i;
            for (i = 0; i < ((amf0_object_t*)data)->used; ++i) {
                SV* svh = _amf0_sv(((amf0_object_t*)data)->data[i]->value);
                hv_store(hv, ((amf0_object_t*)data)->data[i]->key, 0, svh, 0);
            }

            sv = newRV((SV*)hv);

            break;
        }
        case AMF0_NULL:
        case AMF0_UNDEFINED:
            sv = newSV(0);
            break;
        case AMF0_STRICTARRAY: {
            AV* av = newAV();

            int i;
            for (i = 0; i < ((amf0_strictarray_t*)data)->used; ++i) {
                SV* sva = _amf0_sv(((amf0_strictarray_t*)data)->data[i]);
                av_push(av, sva);
            }

            sv = newRV((SV *)av);

            break;
        }
    }

    return sv;
}

XS(decode_amf0) {
    dXSARGS;

    if (items != 1) {
        Perl_croak(aTHX_ "Usage: decode_amf0($amf_data)");
    }

    STRLEN len;
    char*  data = SvPV((SV*)ST(0), len);

    amf0_t* amf0 = amf0_decode(data, len);

    int i;
    for (i = 0; i < amf0->used; ++i) {
        SV* sv = _amf0_sv(amf0->data[i]);
        ST(i) = sv_2mortal(sv);
    }
    XSRETURN(++i);

    amf0_free(amf0);
}

XS(boot_Data__AMF__XS) {
    newXS("Data::AMF::XS::encode_amf0", encode_amf0, __FILE__);
    newXS("Data::AMF::XS::decode_amf0", decode_amf0, __FILE__);
}

