#ifndef BE_OBJECT_H
#define BE_OBJECT_H

#include "berry.h"

/* basic types, do not change value */
#define BE_NONE         (-1)    /* unknow type */
#define BE_COMPTR       (-2)    /* common pointer */
#define BE_NIL          0
#define BE_INT          1
#define BE_REAL         2
#define BE_BOOL         3
#define BE_FUNCTION     4
#define BE_STRING       5
#define BE_CLASS        6
#define BE_INSTANCE     7
#define BE_PROTO        8
#define BE_LIST         9
#define BE_MAP          10
#define BE_MODULE       11
#define BE_NTVFUNC      ((0 << 5) | BE_FUNCTION)
#define BE_CLOSURE      ((1 << 5) | BE_FUNCTION)
#define BE_NTVCLOS      ((2 << 5) | BE_FUNCTION)

#define array_count(a)   (sizeof(a) / sizeof((a)[0]))

#define bcommon_header          \
    struct bgcobject *next;     \
    bbyte type;                 \
    bbyte marked

#define bstring_header          \
    bcommon_header;             \
    bbyte extra;                \
    bbyte slen

typedef struct bgcobject {
    bcommon_header;
} bgcobject;

typedef struct bclosure bclosure;
typedef struct bntvclos bntvclos;
typedef struct bclass bclass;
typedef struct binstance binstance;
typedef struct blist blist;
typedef struct bmap bmap;

typedef uint32_t binstruction;

typedef struct bstring {
    bstring_header;
} bstring;

/* the definition of the vector and stack data structures.
 * in fact, the stack is implemented by vector. */
typedef struct bvector {
    int capacity;   /* the number of elements that the vector can store */
    int size;       /* the size of each element (bytes) */
    void *data;     /* the data block pointer, if vector is empty,
                       it will point to the first element */
    void *end;      /* pointer to the last element, if the vector is empty,
                       the end pointer will be smaller than the data pointer */
} bvector, bstack;

/* berry value data union, a berry value is always described
 * by the data structure contained in the bvaldata union. */
union bvaldata {
    bbool b;        /* boolean */
    breal r;        /* real number */
    bint i;         /* integer number */
    void *p;        /* object pointer */
    bstring *s;     /* string pointer */
    bgcobject *gc;  /* GC object */
    bntvfunc nf;    /* native C function */
};

/* berry value. for simple types, the value of the data is stored,
 * while the complex type stores a reference to the data. */
typedef struct bvalue {
    union bvaldata v; /* the value data */
    int type;         /* the value type */
} bvalue;

typedef struct {
    bstring *name; /* the name of variable */
    int startpc;   /* first point where variable is active */
    int endpc;     /* first point where variable is dead */
} blocalval;

typedef struct {
    bstring *name;
    int index;
} bvaldesc;

typedef struct {
    bbyte instack;
    bbyte idx;
} bupvaldesc;

typedef struct {
#if BE_DEBUG_RUNTIME_INFO > 1
    uint16_t linenumber;
    uint16_t endpc;
#else
    int linenumber;
    int endpc;
#endif
} blineinfo;

typedef struct bupval {
    bvalue *value;
    union {
        bvalue value;
        struct bupval *next;
    } u;
    int refcnt;
} bupval;

typedef struct bproto {
    bcommon_header;
    bbyte nlocal; /* local variable count */
    bbyte nstack; /* number of stack size by this function */
    bbyte nupvals; /* upvalue count */
    bbyte argc; /* argument count */
    bgcobject *gray; /* for gc gray list */
    bupvaldesc *upvals;
    bvalue *ktab; /* constants table */
    struct bproto **ptab; /* proto table */
    binstruction *code; /* instructions sequence */
    bstring *name; /* function name */
    int codesize; /* code size */
    int nconst; /* constants count */
    int nproto; /* proto count */
#if BE_DEBUG_RUNTIME_INFO /* debug information */
    bstring *source;
    blineinfo *lineinfo;
    int nlineinfo;
#endif
} bproto;

struct bclosure {
    bcommon_header;
    bbyte nupvals;
    bgcobject *gray; /* for gc gray list */
    bproto *proto;
    bupval *upvals[1];
};

/* C native function or closure */
struct bntvclos {
    bcommon_header;
    bbyte nupvals;
    bgcobject *gray; /* for gc gray list */
    bntvfunc f;
};

typedef const char* (*breader)(void*, size_t*);

#define cast(_T, _v)            ((_T)(_v))
#define cast_int(_v)            cast(int, _v)
#define cast_bool(_v)           cast(bbool, _v)
#define basetype(_t)            ((_t) & 0x1F)

#define var_type(_v)            ((_v)->type)
#define var_basetype(_v)        basetype((_v)->type)
#define var_istype(_v, _t)      (var_type(_v) == _t)
#define var_settype(_v, _t)     ((_v)->type = _t)
#define var_setobj(_v, _t, _o)  { (_v)->v.p = _o; var_settype(_v, _t); }

#define var_isnil(_v)           var_istype(_v, BE_NIL)
#define var_isbool(_v)          var_istype(_v, BE_BOOL)
#define var_isint(_v)           var_istype(_v, BE_INT)
#define var_isreal(_v)          var_istype(_v, BE_REAL)
#define var_isstr(_v)           var_istype(_v, BE_STRING)
#define var_isclosure(_v)       var_istype(_v, BE_CLOSURE)
#define var_isntvclos(_v)       var_istype(_v, BE_NTVCLOS)
#define var_isntvfunc(_v)       var_istype(_v, BE_NTVFUNC)
#define var_isfunction(_v)      (var_basetype(_v) == BE_FUNCTION)
#define var_isproto(_v)         var_istype(_v, BE_PROTO)
#define var_isclass(_v)         var_istype(_v, BE_CLASS)
#define var_isinstance(_v)      var_istype(_v, BE_INSTANCE)
#define var_islist(_v)          var_istype(_v, BE_LIST)
#define var_ismap(_v)           var_istype(_v, BE_MAP)
#define var_ismodule(_v)        var_istype(_v, BE_MODULE)
#define var_isnumber(_v)        (var_isint(_v) || var_isreal(_v))

#define var_setnil(_v)          var_settype(_v, BE_NIL)
#define var_setval(_v, _s)      (*(_v) = *(_s))
#define var_setbool(_v, _b)     { var_settype(_v, BE_BOOL); (_v)->v.b = (bbool)(_b); }
#define var_setint(_v, _i)      { var_settype(_v, BE_INT); (_v)->v.i = (_i); }
#define var_setreal(_v, _r)     { var_settype(_v, BE_REAL); (_v)->v.r = (_r); }
#define var_setstr(_v, _s)      var_setobj(_v, BE_STRING, _s)
#define var_setinstance(_v, _o) var_setobj(_v, BE_INSTANCE, _o)
#define var_setclass(_v, _o)    var_setobj(_v, BE_CLASS, _o)
#define var_setclosure(_v, _o)  var_setobj(_v, BE_CLOSURE, _o)
#define var_setntvclos(_v, _o)  var_setobj(_v, BE_NTVCLOS, _o)
#define var_setntvfunc(_v, _o)  { (_v)->v.nf = (_o); var_settype(_v, BE_NTVFUNC); }
#define var_setlist(_v, _o)     var_setobj(_v, BE_LIST, _o)
#define var_setmap(_v, _o)      var_setobj(_v, BE_MAP, _o)
#define var_setmodule(_v, _o)   var_setobj(_v, BE_MODULE, _o)
#define var_setproto(_v, _o)    var_setobj(_v, BE_PROTO, _o)

#define var_tobool(_v)          ((_v)->v.b)
#define var_toint(_v)           ((_v)->v.i)
#define var_toreal(_v)          ((_v)->v.r)
#define var_tostr(_v)           ((_v)->v.s)
#define var_togc(_v)            ((_v)->v.gc)
#define var_toobj(_v)           ((_v)->v.p)
#define var_tontvfunc(_v)       ((_v)->v.nf)
#define var_toidx(_v)           cast_int(var_toint(_v))

const char* be_vtype2str(bvalue *v);

#endif
