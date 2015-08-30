#pragma once

#define abort_on_error(where, error) ({ \
    typeof(where) _where = where; \
    typeof(error) _error = error; \
    \
    if(_error) \
    { \
        fprintf(stderr, __FILE__ ":%d: %s error (%#x).\n", __LINE__, _where, _error); \
        abort(); \
    } \
})
