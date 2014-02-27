#include <check.h>

void assert_str(const char *a, const char *b)
{
    if (!b || !a) {
        fail_if(a != b);
    }
    else {
        fail_if(strcmp(a, b));
    }
}

void assert_str_msg(const char *a, const char *b, const char *diag)
{
    if (!b || !a) { fail_if(a != b, diag); }
    else { fail_if(strcmp(a, b), diag); }
}
