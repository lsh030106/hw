#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
 
int euckr2utf8(char *source, char *dest, int dest_size) {
    iconv_t it;
    char *pout;
    size_t in_size, out_size;
 
    it = iconv_open("UTF-8", "EUC-KR");
    in_size = strlen(source);
    out_size = dest_size;
    pout = dest;
    if (iconv(it, &source, &in_size, &pout, &out_size) < 0)
        return(-1);
    iconv_close(it);
    return(pout - dest);
}
 
int utf82euckr(char *source, char *dest, int dest_size) {
    iconv_t it;
    char *pout;
    size_t in_size, out_size;
 
    it = iconv_open("EUC-KR", "UTF-8");
    in_size = strlen(source);
    out_size = dest_size;
    pout = dest;
    if (iconv(it, &source, &in_size, &pout, &out_size) < 0)
        return(-1);
    iconv_close(it);
    return(pout - dest);
}
