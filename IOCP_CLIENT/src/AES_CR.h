#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/* must free() *ct;

 filename : secretkey .key file path
 flag 0 : encrypt
 flag 1 : decrypt
 txt    : if flag 0, txt would be plain text or 1 for cipher text
 ct : buffer of function, it has return value if then success function

 return ct for success and NULL for faulure
 */

unsigned char *symkey(const char *filename, int flag, unsigned char *txt, unsigned char *ct);

