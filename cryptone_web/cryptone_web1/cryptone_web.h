#include "stdafx.h"
int bestType = 0;
unsigned char * iv128 = NULL;
unsigned char* strUsername = NULL;
char* gServername = NULL;

FILE * gLogFile = NULL;

unsigned char * gAESkey = NULL;
unsigned char * gAESVector = NULL;
unsigned char * gClientPublic = NULL;
unsigned char * gClientPrivate = NULL;
unsigned char * gClientCert = NULL;
unsigned char * gServerPublic = NULL;
unsigned char * gUseridhash = NULL;
unsigned char * gUsername = NULL;
unsigned char * gServerPassword = NULL;
int gUpdateKeys = 0;

char* BeginPrivateKey = "-----BEGIN PRIVATE KEY-----";
char* EndPrivateKey = "-----END PRIVATE KEY-----";
char* BeginPublicKey = "-----BEGIN PUBLIC KEY-----";
char* EndPublicKey = "-----END PUBLIC KEY-----";
char* BeginCertificate = "-----BEGIN CERTIFICATE-----";
char* EndCertificate =   "-----END CERTIFICATE-----";
char* BeginServerPublicKey = "-----BEGIN SERVER PUBLIC KEY-----\n";
char* EndServerPublicKey = "\n-----END SERVER PUBLIC KEY-----\n";
char* BeginAESKey = "-----BEGIN AES KEY-----\n";
char* EndAESKey = "\n-----END AES KEY-----\n";
char* BeginUsername = "-----BEGIN USERNAME-----\n";
char* EndUsername = "\n-----END USERNAME-----\n";
char* BeginClientId = "-----BEGIN CLIENTID-----\n";
char* EndClientId = "\n-----END CLIENTID-----\n";
char* BeginAESVector = "-----BEGIN AESVECTOR-----\n";
char* EndAESVector = "\n-----END AESVECTOR-----\n";
char* BeginContainerPass = "-----BEGIN CONTAINER PASS-----\n";
char* EndContainerPass = "-----END CONTAINER PASS-----\n";

typedef bool ( *GenRSAKeysToMemExp )( unsigned char** PrivateKeyStr, unsigned char** PublicKeyStr, unsigned char** CertificateKeyStr );
typedef unsigned char* ( *aes256_encryptCExp )(  unsigned char* data, int dataLen, unsigned char * key, unsigned char * iv, int &cryptedlen );
typedef unsigned char* ( *aes256_decryptCExp )( unsigned char* cipher, int cipherLen, unsigned char * key, unsigned char * iv, int &decryptLen );
typedef char* ( *base64EncodeExp )( const unsigned char *message, const size_t length );
typedef int   ( *base64DecodeExp )( const char *b64message, const size_t length, unsigned char **buffer );
typedef char* ( *charHex2charExp )( unsigned char* data, int dataLen );
typedef char* ( *char2charHexExp )( unsigned char* data, int dataLen );
typedef char* ( *compress_stringCExp )( char* strData, int strDataLen, int &zippedLen );
typedef char* ( *decompress_stringCExp )( char* strData, int strDataLen, int &unzippedLen );
typedef int ( *public_encryptExp )( unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted );
typedef int ( *private_decryptExp )( unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted );
typedef int ( *private_encryptExp )( unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted );
typedef int ( *public_decryptExp )(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted );


aes256_encryptCExp aes256_encryptC = 0;
aes256_decryptCExp aes256_decryptC = 0;
GenRSAKeysToMemExp GenRSAKeysToMem = 0;
base64EncodeExp base64Encode = 0;
base64DecodeExp base64Decode = 0;
charHex2charExp charHex2char = 0;
char2charHexExp char2charHex = 0;
compress_stringCExp compress_stringC = 0;
decompress_stringCExp decompress_stringC = 0;
public_encryptExp public_encrypt = 0;
private_decryptExp private_decrypt = 0;
private_encryptExp private_encrypt = 0;
public_decryptExp public_decrypt = 0;

HMODULE hModuleCRYPT;
char GlobUserName_buf[64];


