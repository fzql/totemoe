
#include "stdafx.hpp"
#include "network.hpp"
#include <openssl/ssl.h>

size_t writefunction(void *ptr, size_t size, size_t nmemb, void *stream)
{
    fwrite(ptr, size, nmemb, (FILE *)stream);
    return (nmemb*size);
}