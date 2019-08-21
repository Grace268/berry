#include "be_regex.h"
#include "be_mem.h"

typedef struct {
    int temp;
} bregex;

bregex* be_regex_compile(const char *pattern)
{
    bregex *re = be_os_malloc(sizeof(bregex));
    return re;
}

void be_regex_delete(bregex *re)
{
    be_os_free(re);
}
