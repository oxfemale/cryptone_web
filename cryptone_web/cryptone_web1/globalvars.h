#include "stdafx.h"
#define ALG_CLASS_DATA_ENCRYPT          (3 << 13)
#define ALG_TYPE_STREAM                 (4 << 9)
#define ALG_SID_RC4                     1
#define CALG_RC4                (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_STREAM|ALG_SID_RC4)
#define CRYPT_MODE_ECB          2


using namespace std;
extern HMODULE hModuleCRYPT;
extern char GlobUserName_buf[64];
extern int bestType;
extern unsigned char * iv128;
extern unsigned char* strUsername;
extern char* gServername;
//Global keys
extern unsigned char * gAESkey;
extern unsigned char * gAESVector;
extern unsigned char * gClientPublic;
extern unsigned char * gClientPrivate;
extern unsigned char * gClientCert;
extern unsigned char * gServerPublic;
extern unsigned char * gUseridhash;
extern unsigned char * gUsername;
extern unsigned char * gServerPassword;


extern int gUpdateKeys;

extern char* BeginPrivateKey;
extern char* EndPrivateKey;
extern char* BeginPublicKey;
extern char* EndPublicKey;
extern char* BeginCertificate;
extern char* EndCertificate;
extern char* BeginServerPublicKey;
extern char* EndServerPublicKey;
extern char* BeginAESKey;
extern char* EndAESKey;
extern char* BeginUsername;
extern char* EndUsername;
extern char* BeginClientId;
extern char* EndClientId;
extern char* BeginAESVector;
extern char* EndAESVector;
extern char* BeginContainerPass;
extern char* EndContainerPass;

typedef bool(*GenRSAKeysToMemExp)(unsigned char** PrivateKeyStr, unsigned char** PublicKeyStr, unsigned char** CertificateKeyStr);
typedef unsigned char* (*aes256_encryptCExp)(unsigned char* data, int dataLen, unsigned char * key, unsigned char * iv, int &cryptedlen);
typedef unsigned char* (*aes256_decryptCExp)(unsigned char* cipher, int cipherLen, unsigned char * key, unsigned char * iv, int &decryptLen);
typedef char* (*base64EncodeExp)(const unsigned char *message, const size_t length);
typedef int(*base64DecodeExp)(const char *b64message, const size_t length, unsigned char **buffer);
typedef char* (*charHex2charExp)(unsigned char* data, int dataLen);
typedef char* (*char2charHexExp)(unsigned char* data, int dataLen);
typedef char* (*compress_stringCExp)(char* strData, int strDataLen, int &zippedLen);
typedef char* (*decompress_stringCExp)(char* strData, int strDataLen, int &unzippedLen);
typedef int(*public_encryptExp)(unsigned char * data, int data_len, unsigned char * key, unsigned char *encrypted);
typedef int(*private_decryptExp)(unsigned char * enc_data, int data_len, unsigned char * key, unsigned char *decrypted);
typedef int(*private_encryptExp)(unsigned char * data, int data_len, unsigned char * key, unsigned char *encrypted);
typedef int(*public_decryptExp)(unsigned char * enc_data, int data_len, unsigned char * key, unsigned char *decrypted);

extern aes256_encryptCExp aes256_encryptC;
extern aes256_decryptCExp aes256_decryptC;
extern GenRSAKeysToMemExp GenRSAKeysToMem;
extern base64EncodeExp base64Encode;
extern base64DecodeExp base64Decode;
extern charHex2charExp charHex2char;
extern char2charHexExp char2charHex;
extern compress_stringCExp compress_stringC;
extern decompress_stringCExp decompress_stringC;
extern public_encryptExp public_encrypt;
extern private_decryptExp private_decrypt;
extern private_encryptExp private_encrypt;
extern public_decryptExp public_decrypt;
