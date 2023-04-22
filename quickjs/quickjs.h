#pragma once

#include "quickjs_mc.h"

struct list_head {
    struct list_head* prev;
    struct list_head* next;
};

typedef enum JSGCObjectTypeEnum JSGCObjectTypeEnum;

/* header for GC objects. GC objects are C data structures with a
   reference count that can reference other GC objects. JS Objects are
   a particular type of GC object. */
struct JSGCObjectHeader {
    int ref_count; /* must come first, 32-bit */
    JSGCObjectTypeEnum gc_obj_type : 4;
    uint8_t mark : 4; /* used by the GC */
    uint8_t dummy1; /* not used by the GC */
    uint16_t dummy2; /* not used by the GC */
    struct list_head link;
};

typedef struct JSShape JSShape;
typedef struct JSProperty JSProperty;

struct JSObject {
    union {
        JSGCObjectHeader header;
        struct {
            int __gc_ref_count; /* corresponds to header.ref_count */
            uint8_t __gc_mark; /* corresponds to header.mark/gc_obj_type */

            uint8_t extensible : 1;
            uint8_t free_mark : 1; /* only used when freeing objects with cycles */
            uint8_t is_exotic : 1; /* TRUE if object has exotic property handlers */
            uint8_t fast_array : 1; /* TRUE if u.array is used for get/put (for JS_CLASS_ARRAY, JS_CLASS_ARGUMENTS and typed arrays) */
            uint8_t is_constructor : 1; /* TRUE if object is a constructor function */
            uint8_t is_uncatchable_error : 1; /* if TRUE, error is not catchable */
            uint8_t tmp_mark : 1; /* used in JS_WriteObjectRec() */
            uint8_t is_HTMLDDA : 1; /* specific annex B IsHtmlDDA behavior */
            uint16_t class_id; /* see JS_CLASS_x */
        };
    };
    /* byte offsets: 16/24 */
    JSShape* shape; /* prototype and property names + flag */
    JSProperty* prop; /* array of properties */
    /* byte offsets: 24/40 */
    struct JSMapRecord* first_weak_ref; /* XXX: use a bit and an external hash table? */
    /* byte offsets: 28/48 */
    union {
        void* opaque;
    } u;
    /* byte sizes: 40/48/72 */
};

/* return NULL if not an object of class class_id */
inline void* JS_GetOpaque(JSValueConst obj, JSClassID class_id)
{
    JSObject* p;
    if (JS_VALUE_GET_TAG(obj) != JS_TAG_OBJECT)
        return NULL;
    p = JS_VALUE_GET_OBJ(obj);
    if (p->class_id != class_id)
        return NULL;
    return p->u.opaque;
}

inline JSValue JS_NewArrayBuffer(JSContext* ctx, uint8_t* buf, size_t len,
    JSFreeArrayBufferDataFunc* free_func, void* opaque,
    BOOL is_shared)
{
    return JS_EXCEPTION;
}

inline BOOL JS_IsConstructor(JSContext* ctx, JSValueConst val)
{
    JSObject* p;
    if (JS_VALUE_GET_TAG(val) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(val);
    return p->is_constructor;
}

inline BOOL JS_SetConstructorBit(JSContext* ctx, JSValueConst func_obj, BOOL val)
{
    JSObject* p;
    if (JS_VALUE_GET_TAG(func_obj) != JS_TAG_OBJECT)
        return FALSE;
    p = JS_VALUE_GET_OBJ(func_obj);
    p->is_constructor = val;
    return TRUE;
}

/* Note: OS and CPU dependent */
/*static inline uintptr_t js_get_stack_pointer(void)
{
    return (uintptr_t)__builtin_frame_address(0);
}*/

//static void update_stack_limit(JSRuntime *rt)
//{
//    if (rt->stack_size == 0) {
//        rt->stack_limit = 0; /* no limit */
//    } else {
//        rt->stack_limit = rt->stack_top - rt->stack_size;
//    }
//}

inline void JS_UpdateStackTop(JSRuntime* rt)
{
    //rt->stack_top = js_get_stack_pointer();
    //update_stack_limit(rt);
}