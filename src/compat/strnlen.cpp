// Copyright (c) 2017 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/straks-config.h"
#endif

#include <cstring>
// OSX 10.6 is missing strnlen at runtime, but builds targetting it will still
// succeed. Define our own version here to avoid a crash.
//#if HAVE_DECL_STRNLEN == 0//TODO--
size_t strnlen_int( const char *start, size_t max_len) //TODO-- added _int
{
    const char *end = (const char *)memchr(start, '\0', max_len);

    return end ? (size_t)(end - start) : max_len;
}
//#endif // HAVE_DECL_STRNLEN
