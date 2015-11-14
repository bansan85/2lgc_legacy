#include <stdio.h>
#include <openssl/md5.h>

bool md5sumfile (const char * file, unsigned char* c)
{
  FILE *inFile = fopen (file, "rb");
  MD5_CTX mdContext;
  size_t bytes;
  unsigned char data[1024];

  if (inFile == NULL) {
    return false;
  }

  MD5_Init (&mdContext);
  while ((bytes = fread (data, 1, 1024, inFile)) != 0)
    MD5_Update (&mdContext, data, bytes);
  MD5_Final (c,&mdContext);
  fclose (inFile);
  return true;
}

bool md5sumfiles (const char * file1, const char * file2)
{
  unsigned char c1[MD5_DIGEST_LENGTH], c2[MD5_DIGEST_LENGTH];
  bool retour = true;
  retour = retour && md5sumfile(file1, c1);
  retour = retour && md5sumfile(file2, c2);
  return retour && (c1 == c2);
}
