#ifndef _ROPE_UTILS_H_
#define _ROPE_UTILS_H_

/*
 * Rope utilities
 */

#define DECLARE_ROPE_DATA_UCS(name, type, len, first, next)                    \
    type name[len];                                                            \
    name[0] = first;                                                           \
    for (int i = 1; i < len; i++) {                                            \
        name[i] = next;                                                        \
    }
#define NEW_ROPE_UCS(type, format, len, first, next)                           \
    {                                                                          \
        DECLARE_ROPE_DATA_UCS(data, type, len, first, next);                   \
        return Col_NewRope(format, data, sizeof(data));                        \
    }
#define DECLARE_ROPE_DATA_UTF8(name, len, first, next)                         \
    Col_Char1 name[len * COL_UTF8_MAX_WIDTH], *p = data;                       \
    p = Col_Utf8Set(p, first);                                                 \
    for (int i = 1; i < len; i++) {                                            \
        p = Col_Utf8Set(p, next);                                              \
    }
#define NEW_ROPE_UTF8(len, first, next)                                        \
    {                                                                          \
        DECLARE_ROPE_DATA_UTF8(data, len, first, next);                        \
        return Col_NewRope(COL_UTF8, data, (p - data) * sizeof(*data));        \
    }
#define DECLARE_ROPE_DATA_UTF16(name, len, first, next)                        \
    Col_Char2 name[len * COL_UTF16_MAX_WIDTH], *p = data;                      \
    p = Col_Utf16Set(p, first);                                                \
    for (int i = 1; i < len; i++) {                                            \
        p = Col_Utf16Set(p, next);                                             \
    }
#define NEW_ROPE_UTF16(len, first, next)                                       \
    {                                                                          \
        DECLARE_ROPE_DATA_UTF16(data, len, first, next);                       \
        return Col_NewRope(COL_UTF16, data, (p - data) * sizeof(*data));       \
    }
#define NEW_ROPE_STRING(len, first, next)                                      \
    {                                                                          \
        DECLARE_ROPE_DATA_UCS(data, char, len + 1, first, next);               \
        data[len] = 0;                                                         \
        return Col_NewRopeFromString(data);                                    \
    }

#endif /* _ROPE_UTILS_H_ */
