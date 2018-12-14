// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// ysp+ { AES DES and SMS4 crypt }

#include "crypto/ysp_crypto_encryption.h"

#include <openssl/aes.h>
#include <map>
#include <set>

#include "base/base64.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"
#include "crypto/sha2.h"

#define ENCRYPT 1
#define DECRYPT 0

#define DES_BLOCK_SIZE 8
#define SMS4_BLOCK_SIZE 16

namespace cryptokey {
char flags = '0';
char pwd[17] = "";
char iv[17] = "";
}  // namespace cryptokey
namespace crypto_sign {
const char cAES = 'A';
const char caes = 'a';
const char cDES = 'D';
const char cdes = 'd';
const char cSM4 = 'S';
const char csm4 = 's';
}  // namespace crypto_sign

namespace {

std::string uuid = "";
const char* crypto_impl_key = "d5A+JDyiBfIcC+8q";
const char* crypto_impl_iv = "WFMxTBb4EODsRgyM";
class cryptokeyimpl {
 public:
  cryptokeyimpl();
  ~cryptokeyimpl();
  bool EncryptString(const std::string& plain_text, std::string* cipher_text);
  bool DecryptString(const std::string& cipher_text, std::string* plain_text);
};

cryptokeyimpl::cryptokeyimpl() {}
cryptokeyimpl::~cryptokeyimpl() {}

bool cryptokeyimpl::EncryptString(const std::string& plain_text,
                                  std::string* cipher_text) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, crypto_impl_key, AES_BLOCK_SIZE);
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = plain_text.size();
  std::string pla_padding = plain_text;
  int offset = AES_BLOCK_SIZE - (out_size % AES_BLOCK_SIZE);
  size_t out_size_new = out_size + offset;
  char padding[AES_BLOCK_SIZE];
  memset(padding, offset, AES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, crypto_impl_iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
                  pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
  cipher_text->swap(result);
  return true;
}

bool cryptokeyimpl::DecryptString(const std::string& cipher_text,
                                  std::string* plain_text) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, crypto_impl_key, AES_BLOCK_SIZE);
  if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = cipher_text.size();
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, crypto_impl_iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(cipher_text.data()), out_ptr,
                  cipher_text.size(), &aes_key, ivec, AES_DECRYPT);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  return true;
}

}  // namespace

static void Des_Enc(std::string* enc, std::string ptr, int length);
static void Des_Dec(std::string* dec, std::string ptr, int length);
static void Des_SetKey(const char Key[8]);                //生成子密钥
static void Des_Run(char Out[8], char In[8], bool Type);  // DES算法
// static void printHex (const char *cmd, int len );
// static void printArray ( const char *In, int len );
static void F_func(bool In[32], const bool Ki[48]);   // f函数
static void S_func(bool Out[32], const bool In[48]);  // S盒代替
static void Transform(bool* Out, bool* In, const char* Table, int len);  // 变换
static void Xor(bool* InA, const bool* InB, int len);  // 异或
static void RotateL(bool* In, int len, int loop);      // 循环左移
static void ByteToBit(bool* Out, const char* In, int bits);  // 字节组转换成位组
static void BitToByte(char* Out, const bool* In, int bits);  // 位组转换成字节组

// 16位子密钥
static bool SubKey[16][48];

// 64位经过PC1转换为56位 (PC-1)
const static char PC1_Table[56] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43,
    35, 27, 19, 11, 3,  60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7,  62, 54,
    46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4};

// 左移
const static char LOOP_Table[16] = {1, 1, 2, 2, 2, 2, 2, 2,
                                    1, 2, 2, 2, 2, 2, 2, 1};

// 排列选择 2 (PC-2)
const static char PC2_Table[48] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10, 23, 19, 12, 4,
    26, 8,  16, 7,  27, 20, 13, 2,  41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

// Ri_1（32位）经过变换E后膨胀为48位 (E)  void F_func
static const char E_Table[48] = {32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
                                 8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
                                 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
                                 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

// 8个4比特合并为32比特的排列 P
const static char P_Table[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25,
};

// 经过S盒 S-boxes
const static char S_Box[8][4][16] = {
    {// S1
     {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    {// S2
     {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    {// S3
     {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    {// S4
     {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    {// S5
     {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    {// S6
     {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    {// S7
     {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    {// S8
     {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

// 初始排列 (IP)
const static char IP_Table[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

// L16与R16合并后经过IP_1的最终排列 (IP**-1)
const static char IPR_Table[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9,  49, 17, 57, 25};

static void Des_SetKey(const char Key[8]) {
  int i;
  static bool K[64], *KL = &K[0], *KR = &K[28];

  ByteToBit(K, Key, 64);  //转换为二进制

  Transform(K, K, PC1_Table, 56);  // 64比特的密钥K，经过PC-1后，生成56比特的串。

  //生成16个子密钥
  for (i = 0; i < 16; i++) {
    //循环左移，合并
    RotateL(KL, 28, LOOP_Table[i]);
    RotateL(KR, 28, LOOP_Table[i]);
    Transform(SubKey[i], K, PC2_Table, 48);
  }
}

static void Des_Run(char Out[8], char In[8], bool Type) {
  int i;
  static bool M[64], tmp[32], *Li = &M[0], *Ri = &M[32];

  //转换为64位的数据块
  ByteToBit(M, In, 64);

  // IP置换 （初始）
  Transform(M, M, IP_Table, 64);

  //该比特串被分为32位的L0和32位的R0两部分。

  if (Type == ENCRYPT) {
    // 16轮置换
    for (i = 0; i < 16; i++) {
      memcpy(tmp, Ri, 32);

      // R[i] = L[i-1] xor f(R[i-1], K[i])
      F_func(Ri, SubKey[i]);

      // 2.4.6 Exclusive-or the resulting value with L[i-1].
      // R[I]=P XOR L[I-1]
      Xor(Ri, Li, 32);

      // L[i] = R[i-1]
      memcpy(Li, tmp, 32);
    }
  } else {
    // 如果解密则反转子密钥顺序
    for (i = 15; i >= 0; i--) {
      memcpy(tmp, Li, 32);
      F_func(Li, SubKey[i]);
      Xor(Li, Ri, 32);
      memcpy(Ri, tmp, 32);
    }
  }

  // R16与L16合并成64位的比特串。R16一定要排在L16前面。R16与L16合并后成的比特串，经过置换IP-1后所得的比特串就是密文。
  Transform(M, M, IPR_Table, 64);

  BitToByte(Out, M, 64);
}

//将32比特的输入再转化为32比特的输出
static void F_func(bool In[32], const bool Ki[48]) {
  static bool MR[48];

  //输入Ri-1(32比特)经过变换E后，膨胀为48比特
  Transform(MR, In, E_Table, 48);

  //异或
  Xor(MR, Ki, 48);

  //膨胀后的比特串分为8组，每组6比特。各组经过各自的S盒后，又变为4比特(具体过程见后)，合并后又成为32比特。
  S_func(In, MR);

  //该32比特经过P变换后，输出的比特串才是32比特的f (Ri-1,Ki)。
  Transform(In, In, P_Table, 32);
}

static void S_func(bool Out[32], const bool In[48]) {
  char j, m, n;

  //膨胀后的比特串分为8组，每组6比特。
  for (j = 0; j < 8; j++, In += 6, Out += 4) {
    //在其输入In[0],In[1],In[2],In[3],In[4],In[5]中，计算出m=In[0]*2+In[5],
    //n=In[4]+In[3]*2+In[2]*4+In[1]*8，再从Sj表中查出m行，n列的值Smn。将Smn化为二进制，即得Si盒的输出。
    m = (In[0] << 1) + In[5];
    n = (In[1] << 3) + (In[2] << 2) + (In[3] << 1) + In[4];

    ByteToBit(Out, &S_Box[(int)j][(int)m][(int)n], 4);
  }
}
/*
// 打印指定位置指定长度HEX值
static void printHex (const char *cmd, int len )
{
    int i;

    for ( i=0; i<len; i++ )
    {
        printf ( "[%02X]", ( unsigned char ) cmd[i] );
    }
    printf ( "\n" );
}

// 打印数组测试用
static void printArray ( const char *In, int len )
{
    int   i;
    char tmp[256];

    memset ( tmp, 0, sizeof ( tmp ) );

    for ( i=0; i<len; i++ )
    {
        tmp[ ( int ) In[i]]=In[i];
    }

    for ( i=0; i<len; i++ )
    {
        printf ( "[%02d]", ( unsigned char ) tmp[i] );
    }
    printf ( "\n" );
}
*/
static void Transform(bool* Out, bool* In, const char* Table, int len) {
  int i;
  static bool tmp[256];

  for (i = 0; i < len; i++) {
    tmp[i] = In[Table[i] - 1];
  }
  memcpy(Out, tmp, len);
}

static void Xor(bool* InA, const bool* InB, int len) {
  int i;

  for (i = 0; i < len; i++) {
    InA[i] ^= InB[i];
  }
}

static void RotateL(bool* In, int len, int loop) {
  bool tmp[2];
  int i = 0;
  for (i = 0; i < loop; i++) {
    tmp[i] = In[i];
  }
  for (; i < len; i++) {
    In[i - loop] = In[i];
  }

  for (i = 0; i < loop; i++) {
    In[i + len - loop] = tmp[i];
  }
}

// Sample:
// In = [0x01]
// Out = [0x01] [0x00] [0x00] [0x00] [0x00] [0x00] [0x00] [0x00]
static void ByteToBit(bool* Out, const char* In, int bits) {
  int i;

  for (i = 0; i < bits; i++) {
    // In[i]的第N位右移N位并和0x01按位"与"运算(N=1~8)
    Out[i] = (In[i / 8] >> (i % 8)) & 1;
  }
}

static void BitToByte(char* Out, const bool* In, int bits) {
  int i;

  memset(Out, 0, (bits + 7) / 8);
  for (i = 0; i < bits; i++) {
    Out[i / 8] |= In[i] << (i % 8);
  }
}

void Des_EncryptString(std::string* cipher_text, const std::string plain_text) {
  char key[DES_BLOCK_SIZE] = "";
  size_t out_size = plain_text.length();
  char padding[DES_BLOCK_SIZE] = "";
  std::string result;

  std::string pla_padding = plain_text;
  int offset = DES_BLOCK_SIZE - (out_size % DES_BLOCK_SIZE);
  out_size += offset;
  memset(padding, offset, DES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  memcpy(key, cryptokey::pwd, DES_BLOCK_SIZE);
  Des_SetKey(key);
  Des_Enc(&result, pla_padding, out_size);
  cipher_text->swap(result);
}
void Des_DecryptString(std::string* plain_text, const std::string cipher_text) {
  char key[DES_BLOCK_SIZE] = "";
  size_t out_size = cipher_text.size();
  std::string result;

  memcpy(key, cryptokey::pwd, DES_BLOCK_SIZE);
  Des_SetKey(key);
  Des_Dec(&result, cipher_text, out_size);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  DLOG(INFO) << "result: [" << result << "] [" << plain_text << "]";
}

static void Des_Enc(std::string* enc, std::string ptr, int length) {
  const char* tmp = ptr.data();
  char str1[9] = {0};
  char str2[9] = {0};
  int i = 0, j = 0, offset = 0;
  while (i < length) {
    str1[j] = tmp[i];
    if (j == 7 || i == (length - 1)) {
      memset(str2, 0, sizeof(str2));
      Des_Run(str2, str1, ENCRYPT);
      enc->append(str2, 8);
      memset(str1, 0, sizeof(str1));
      offset += (sizeof(str2) - 1);
      j = -1;
    }

    i++;
    j++;
  }
}

static void Des_Dec(std::string* dec, std::string ptr, int length) {
  const char* tmp = ptr.data();
  char* str1 = (char*)calloc(length, sizeof(char));
  char str2[9] = {0};
  int i = 0, j = 0, offset = 0;
  memset(str1, 0, length);
  while (i < length) {
    str1[j] = tmp[i];
    if (j == 7 || i == (length - 1)) {
      memset(str2, 0, sizeof(str2));
      Des_Run(str2, str1, DECRYPT);
      dec->append(str2, 8);
      // memset(str1, 0, sizeof(str1));
      memset(str1, 0, length);
      offset += (sizeof(str2) - 1);
      j = -1;
    }

    i++;
    j++;
  }
  free(str1);
}

/********************begin SMS4 crypt***************************/

#define SMS4_NUM_ROUNDS 32
typedef struct {
  uint32_t rk[SMS4_NUM_ROUNDS];
} sms4_key_t;

void sms4_set_encrypt_key(sms4_key_t* key, const unsigned char* user_key);
void sms4_set_decrypt_key(sms4_key_t* key, const unsigned char* user_key);
void sms4_encrypt(const unsigned char* in,
                  unsigned char* out,
                  const sms4_key_t* key);
#define sms4_decrypt(in, out, key) sms4_encrypt(in, out, key)
void sms4_crypt_ecb(sms4_key_t* key,
                    int mode,
                    int length,
                    std::string input,
                    std::string* output);

#define FK0 0xa3b1bac6
#define FK1 0x56aa3350
#define FK2 0x677d9197
#define FK3 0xb27022dc

#define CK0 0x00070e15
#define CK1 0x1c232a31
#define CK2 0x383f464d
#define CK3 0x545b6269
#define CK4 0x70777e85
#define CK5 0x8c939aa1
#define CK6 0xa8afb6bd
#define CK7 0xc4cbd2d9
#define CK8 0xe0e7eef5
#define CK9 0xfc030a11
#define CK10 0x181f262d
#define CK11 0x343b4249
#define CK12 0x50575e65
#define CK13 0x6c737a81
#define CK14 0x888f969d
#define CK15 0xa4abb2b9
#define CK16 0xc0c7ced5
#define CK17 0xdce3eaf1
#define CK18 0xf8ff060d
#define CK19 0x141b2229
#define CK20 0x30373e45
#define CK21 0x4c535a61
#define CK22 0x686f767d
#define CK23 0x848b9299
#define CK24 0xa0a7aeb5
#define CK25 0xbcc3cad1
#define CK26 0xd8dfe6ed
#define CK27 0xf4fb0209
#define CK28 0x10171e25
#define CK29 0x2c333a41
#define CK30 0x484f565d
#define CK31 0x646b7279

static const uint8_t SBOX[256] = {
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2,
    0x28, 0xfb, 0x2c, 0x05, 0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
    0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99, 0x9c, 0x42, 0x50, 0xf4,
    0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa,
    0x75, 0x8f, 0x3f, 0xa6, 0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
    0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8, 0x68, 0x6b, 0x81, 0xb2,
    0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b,
    0x01, 0x21, 0x78, 0x87, 0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
    0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e, 0xea, 0xbf, 0x8a, 0xd2,
    0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30,
    0xf5, 0x8c, 0xb1, 0xe3, 0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
    0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f, 0xd5, 0xdb, 0x37, 0x45,
    0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41,
    0x1f, 0x10, 0x5a, 0xd8, 0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
    0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0, 0x89, 0x69, 0x97, 0x4a,
    0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e,
    0xd7, 0xcb, 0x39, 0x48};

#define GETU32(pc)                                         \
  (((uint32_t)(pc)[0] << 24) ^ ((uint32_t)(pc)[1] << 16) ^ \
   ((uint32_t)(pc)[2] << 8) ^ ((uint32_t)(pc)[3]))

#define PUTU32(st, ct)               \
  {                                  \
    (ct)[0] = (uint8_t)((st) >> 24); \
    (ct)[1] = (uint8_t)((st) >> 16); \
    (ct)[2] = (uint8_t)((st) >> 8);  \
    (ct)[3] = (uint8_t)(st);         \
  }

#define ROT(A, i) (((A) << i) | ((A) >> (32 - i)))

#define S(A)                                                      \
  ((SBOX[((A) >> 24)] << 24) ^ (SBOX[((A) >> 16) & 0xff] << 16) ^ \
   (SBOX[((A) >> 8) & 0xff] << 8) ^ (SBOX[((A)) & 0xff]))

#define L(B) ((B) ^ ROT((B), 2) ^ ROT((B), 10) ^ ROT((B), 18) ^ ROT((B), 24))
#define L_(B) ((B) ^ ROT((B), 13) ^ ROT((B), 23))

#define ROUND(X0, X1, X2, X3, X4, RK) \
  X4 = (X1) ^ (X2) ^ (X3) ^ (RK);     \
  X4 = S(X4);                         \
  X4 = (X0) ^ L(X4)
#define ROUND_(X0, X1, X2, X3, X4, CK, RK) \
  X4 = (X1) ^ (X2) ^ (X3) ^ (CK);          \
  X4 = S(X4);                              \
  X4 = (X0) ^ L_(X4);                      \
  RK = X4

void sms4_set_encrypt_key(sms4_key_t* key, const unsigned char* user_key) {
  uint32_t* rk = key->rk;
  uint32_t X0, X1, X2, X3, X4;

  X0 = GETU32(user_key) ^ FK0;
  X1 = GETU32(user_key + 4) ^ FK1;
  X2 = GETU32(user_key + 8) ^ FK2;
  X3 = GETU32(user_key + 12) ^ FK3;

  ROUND_(X0, X1, X2, X3, X4, CK0, rk[0]);
  ROUND_(X1, X2, X3, X4, X0, CK1, rk[1]);
  ROUND_(X2, X3, X4, X0, X1, CK2, rk[2]);
  ROUND_(X3, X4, X0, X1, X2, CK3, rk[3]);
  ROUND_(X4, X0, X1, X2, X3, CK4, rk[4]);
  ROUND_(X0, X1, X2, X3, X4, CK5, rk[5]);
  ROUND_(X1, X2, X3, X4, X0, CK6, rk[6]);
  ROUND_(X2, X3, X4, X0, X1, CK7, rk[7]);
  ROUND_(X3, X4, X0, X1, X2, CK8, rk[8]);
  ROUND_(X4, X0, X1, X2, X3, CK9, rk[9]);
  ROUND_(X0, X1, X2, X3, X4, CK10, rk[10]);
  ROUND_(X1, X2, X3, X4, X0, CK11, rk[11]);
  ROUND_(X2, X3, X4, X0, X1, CK12, rk[12]);
  ROUND_(X3, X4, X0, X1, X2, CK13, rk[13]);
  ROUND_(X4, X0, X1, X2, X3, CK14, rk[14]);
  ROUND_(X0, X1, X2, X3, X4, CK15, rk[15]);
  ROUND_(X1, X2, X3, X4, X0, CK16, rk[16]);
  ROUND_(X2, X3, X4, X0, X1, CK17, rk[17]);
  ROUND_(X3, X4, X0, X1, X2, CK18, rk[18]);
  ROUND_(X4, X0, X1, X2, X3, CK19, rk[19]);
  ROUND_(X0, X1, X2, X3, X4, CK20, rk[20]);
  ROUND_(X1, X2, X3, X4, X0, CK21, rk[21]);
  ROUND_(X2, X3, X4, X0, X1, CK22, rk[22]);
  ROUND_(X3, X4, X0, X1, X2, CK23, rk[23]);
  ROUND_(X4, X0, X1, X2, X3, CK24, rk[24]);
  ROUND_(X0, X1, X2, X3, X4, CK25, rk[25]);
  ROUND_(X1, X2, X3, X4, X0, CK26, rk[26]);
  ROUND_(X2, X3, X4, X0, X1, CK27, rk[27]);
  ROUND_(X3, X4, X0, X1, X2, CK28, rk[28]);
  ROUND_(X4, X0, X1, X2, X3, CK29, rk[29]);
  ROUND_(X0, X1, X2, X3, X4, CK30, rk[30]);
  ROUND_(X1, X2, X3, X4, X0, CK31, rk[31]);
}

void sms4_set_decrypt_key(sms4_key_t* key, const unsigned char* user_key) {
  uint32_t* rk = key->rk;
  uint32_t X0, X1, X2, X3, X4;

  X0 = GETU32(user_key) ^ FK0;
  X1 = GETU32(user_key + 4) ^ FK1;
  X2 = GETU32(user_key + 8) ^ FK2;
  X3 = GETU32(user_key + 12) ^ FK3;

  ROUND_(X0, X1, X2, X3, X4, CK0, rk[31]);
  ROUND_(X1, X2, X3, X4, X0, CK1, rk[30]);
  ROUND_(X2, X3, X4, X0, X1, CK2, rk[29]);
  ROUND_(X3, X4, X0, X1, X2, CK3, rk[28]);
  ROUND_(X4, X0, X1, X2, X3, CK4, rk[27]);
  ROUND_(X0, X1, X2, X3, X4, CK5, rk[26]);
  ROUND_(X1, X2, X3, X4, X0, CK6, rk[25]);
  ROUND_(X2, X3, X4, X0, X1, CK7, rk[24]);
  ROUND_(X3, X4, X0, X1, X2, CK8, rk[23]);
  ROUND_(X4, X0, X1, X2, X3, CK9, rk[22]);
  ROUND_(X0, X1, X2, X3, X4, CK10, rk[21]);
  ROUND_(X1, X2, X3, X4, X0, CK11, rk[20]);
  ROUND_(X2, X3, X4, X0, X1, CK12, rk[19]);
  ROUND_(X3, X4, X0, X1, X2, CK13, rk[18]);
  ROUND_(X4, X0, X1, X2, X3, CK14, rk[17]);
  ROUND_(X0, X1, X2, X3, X4, CK15, rk[16]);
  ROUND_(X1, X2, X3, X4, X0, CK16, rk[15]);
  ROUND_(X2, X3, X4, X0, X1, CK17, rk[14]);
  ROUND_(X3, X4, X0, X1, X2, CK18, rk[13]);
  ROUND_(X4, X0, X1, X2, X3, CK19, rk[12]);
  ROUND_(X0, X1, X2, X3, X4, CK20, rk[11]);
  ROUND_(X1, X2, X3, X4, X0, CK21, rk[10]);
  ROUND_(X2, X3, X4, X0, X1, CK22, rk[9]);
  ROUND_(X3, X4, X0, X1, X2, CK23, rk[8]);
  ROUND_(X4, X0, X1, X2, X3, CK24, rk[7]);
  ROUND_(X0, X1, X2, X3, X4, CK25, rk[6]);
  ROUND_(X1, X2, X3, X4, X0, CK26, rk[5]);
  ROUND_(X2, X3, X4, X0, X1, CK27, rk[4]);
  ROUND_(X3, X4, X0, X1, X2, CK28, rk[3]);
  ROUND_(X4, X0, X1, X2, X3, CK29, rk[2]);
  ROUND_(X0, X1, X2, X3, X4, CK30, rk[1]);
  ROUND_(X1, X2, X3, X4, X0, CK31, rk[0]);
}

void sms4_encrypt(const unsigned char* in,
                  unsigned char* out,
                  const sms4_key_t* key) {
  const uint32_t* rk = key->rk;
  uint32_t X0, X1, X2, X3, X4;

  X0 = GETU32(in);
  X1 = GETU32(in + 4);
  X2 = GETU32(in + 8);
  X3 = GETU32(in + 12);

  ROUND(X0, X1, X2, X3, X4, rk[0]);
  ROUND(X1, X2, X3, X4, X0, rk[1]);
  ROUND(X2, X3, X4, X0, X1, rk[2]);
  ROUND(X3, X4, X0, X1, X2, rk[3]);
  ROUND(X4, X0, X1, X2, X3, rk[4]);
  ROUND(X0, X1, X2, X3, X4, rk[5]);
  ROUND(X1, X2, X3, X4, X0, rk[6]);
  ROUND(X2, X3, X4, X0, X1, rk[7]);
  ROUND(X3, X4, X0, X1, X2, rk[8]);
  ROUND(X4, X0, X1, X2, X3, rk[9]);
  ROUND(X0, X1, X2, X3, X4, rk[10]);
  ROUND(X1, X2, X3, X4, X0, rk[11]);
  ROUND(X2, X3, X4, X0, X1, rk[12]);
  ROUND(X3, X4, X0, X1, X2, rk[13]);
  ROUND(X4, X0, X1, X2, X3, rk[14]);
  ROUND(X0, X1, X2, X3, X4, rk[15]);
  ROUND(X1, X2, X3, X4, X0, rk[16]);
  ROUND(X2, X3, X4, X0, X1, rk[17]);
  ROUND(X3, X4, X0, X1, X2, rk[18]);
  ROUND(X4, X0, X1, X2, X3, rk[19]);
  ROUND(X0, X1, X2, X3, X4, rk[20]);
  ROUND(X1, X2, X3, X4, X0, rk[21]);
  ROUND(X2, X3, X4, X0, X1, rk[22]);
  ROUND(X3, X4, X0, X1, X2, rk[23]);
  ROUND(X4, X0, X1, X2, X3, rk[24]);
  ROUND(X0, X1, X2, X3, X4, rk[25]);
  ROUND(X1, X2, X3, X4, X0, rk[26]);
  ROUND(X2, X3, X4, X0, X1, rk[27]);
  ROUND(X3, X4, X0, X1, X2, rk[28]);
  ROUND(X4, X0, X1, X2, X3, rk[29]);
  ROUND(X0, X1, X2, X3, X4, rk[30]);
  ROUND(X1, X2, X3, X4, X0, rk[31]);

  PUTU32(X0, out);
  PUTU32(X4, out + 4);
  PUTU32(X3, out + 8);
  PUTU32(X2, out + 12);
}

void sms4_crypt_ecb(sms4_key_t* key,
                    int mode,
                    int length,
                    std::string input,
                    std::string* output) {
  std::string outtmp;
  for (int i = 0; length > 0; length -= SMS4_BLOCK_SIZE) {
    sms4_encrypt((unsigned char*)input.c_str() + i,
                 (unsigned char*)outtmp.c_str(), key);
    i += SMS4_BLOCK_SIZE;
    output->append(outtmp.c_str());
  }
}
/*
 * SMS4-ECB block encryption
 */
void Sms4_EncryptString(std::string* cipher_text, const std::string plain_text) {
  unsigned char key[SMS4_BLOCK_SIZE] = "";
  sms4_key_t ctx;
  std::string result;
  char padding[SMS4_BLOCK_SIZE] = "";
  size_t sms4_len = plain_text.length();

  std::string pla_padding = plain_text;
  int offset = SMS4_BLOCK_SIZE - (sms4_len % SMS4_BLOCK_SIZE);
  sms4_len += offset;
  memset(padding, offset, SMS4_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  memcpy(key, cryptokey::pwd, SMS4_BLOCK_SIZE);
  sms4_set_encrypt_key(&ctx, key);
  sms4_crypt_ecb(&ctx, ENCRYPT, sms4_len, pla_padding, &result);
  cipher_text->swap(result);
}

/*
 * SMS4-ECB block decryption
 */
void Sms4_DecryptString(std::string* plain_text, const std::string cipher_text) {
  unsigned char key[SMS4_BLOCK_SIZE] = "";
  sms4_key_t ctx;
  std::string result;
  if (cipher_text.empty()) {
    DLOG(INFO) << "cipher_text is NULL !";
    return;
  }
  size_t sms4_len = cipher_text.length();

  memcpy(key, cryptokey::pwd, SMS4_BLOCK_SIZE);
  sms4_set_decrypt_key(&ctx, key);
  sms4_crypt_ecb(&ctx, DECRYPT, sms4_len, cipher_text, &result);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  DLOG(INFO) << "result: [" << result << "] [" << plain_text << "]";
}
/********************end SMS4 crypt***************************/
namespace {
class CookieCryptor {
 public:
  CookieCryptor();
  ~CookieCryptor();
  bool EncryptString(const std::string& plain_text, std::string* cipher_text);
  bool DecryptString(const std::string& cipher_text, std::string* plain_text);
};

CookieCryptor::CookieCryptor() {}

CookieCryptor::~CookieCryptor() {}

bool CookieCryptor::EncryptString(const std::string& plain_text,
                                  std::string* cipher_text) {
  AES_KEY aes_key;
  char key_data[AES_BLOCK_SIZE] = {0};
  memcpy(key_data, cryptokey::pwd, AES_BLOCK_SIZE);
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = plain_text.size();
  std::string pla_padding = plain_text;
  int offset = AES_BLOCK_SIZE - (out_size % AES_BLOCK_SIZE);
  size_t out_size_new = out_size + offset;
  char padding[AES_BLOCK_SIZE];
  memset(padding, offset, AES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, cryptokey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
                  pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
  cipher_text->swap(result);
  return true;
}

bool CookieCryptor::DecryptString(const std::string& cipher_text,
                                  std::string* plain_text) {
  AES_KEY aes_key;
  char key_data[AES_BLOCK_SIZE] = {0};
  memcpy(key_data, cryptokey::pwd, AES_BLOCK_SIZE);
  if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
                          AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = cipher_text.size();
  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = {0};
  memcpy(ivec, cryptokey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(cipher_text.data()), out_ptr,
                  cipher_text.size(), &aes_key, ivec, AES_DECRYPT);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  return true;
}
}  // namespace

inline YspCryptoSingleton::YspCryptoSingleton():should_encrypt_(false) {}
YspCryptoSingleton* YspCryptoSingleton::instance_ = nullptr;
YspCryptoSingleton* YspCryptoSingleton::GetInstance() {
  if (instance_ == nullptr)  // 判断是否第一次调用
    instance_ = new YspCryptoSingleton();
  return instance_;
}

void YspCryptoSingleton::SetShouldEncrypt() {
  should_encrypt_ = true;
}
bool YspCryptoSingleton::GetShouldEncrypt() {
  return should_encrypt_;
}

void YspCryptoSingleton::Init(const std::string& crypto_key) {
  if (crypto_key.empty())
    return;
  cryptokey::flags = crypto_key.data()[0];
  if (cryptokey::flags != '0')
    SetShouldEncrypt();
  if (crypto_key.length() >= 17) {
    memcpy(cryptokey::pwd, crypto_key.data() + 1, 16);
    if (crypto_key.length() >= 33) {
      memcpy(cryptokey::iv, crypto_key.data() + 17, 16);
    } else {
      memcpy(cryptokey::iv, "the iv: 16 bytes", 16);
    }
  } else {
    memcpy(cryptokey::pwd, "allmobilize.com.", 16);
    memcpy(cryptokey::iv, "the iv: 16 bytes", 16);
  }
  // DLOG(INFO) << "cryptokey::pwd:" << cryptokey::pwd << " cryptokey::iv" <<
  // cryptokey::iv;
}

std::string YspCryptoSingleton::EncryptString(const std::string& plain_text) {
  std::string cipher_text, result, key = "";
  if (plain_text.empty())
    return result;
  key = cryptokey::flags;
  key.append(cryptokey::pwd);
  key.append(cryptokey::iv);
  if (cryptokey::flags == '1') {
    return plain_text;
  } else if (cryptokey::flags == crypto_sign::cAES ||
             cryptokey::flags == crypto_sign::caes) {
    CookieCryptor Aes_Crypted_;
    Aes_Crypted_.EncryptString(plain_text, &result);
  } else if (cryptokey::flags == crypto_sign::cDES ||
             cryptokey::flags == crypto_sign::cdes) {
    std::string enc_value = plain_text;
    Des_EncryptString(&result, enc_value);
  } else if (cryptokey::flags == crypto_sign::cSM4 ||
             cryptokey::flags == crypto_sign::csm4) {
    Sms4_EncryptString(&result, plain_text);
  } else {
    result.assign(plain_text);
  }
  cryptokeyimpl AES_crypto;
  AES_crypto.EncryptString(key, &cipher_text);
  std::string base64_result = "", base64_cipher_text = "";
  base::Base64Encode(result, &base64_result);
  base::Base64Encode(cipher_text, &base64_cipher_text);
  base64_cipher_text += "[[]]" + base64_result;
  return base64_cipher_text;
}
std::string YspCryptoSingleton::DecryptString(const std::string& cipher_text) {
  std::string plain_text, cip_result, base64_cip_result;
  if (cipher_text.empty())
    return plain_text;
  size_t offset = cipher_text.find("[[]]");
  if (cipher_text.find("[[]]") != std::string::npos) {
    std::string enc_Key, pla_enc_Key, base64_enc_key;
    enc_Key.assign(cipher_text, 0, offset);
    cryptokeyimpl AES_crypto;
    base::Base64Decode(enc_Key, &base64_enc_key);
    AES_crypto.DecryptString(base64_enc_key, &pla_enc_Key);
    Init(pla_enc_Key);
    cip_result.assign(cipher_text, offset + 4, cipher_text.length() - offset - 4);
    base::Base64Decode(cip_result, &base64_cip_result);
  }
  if (cryptokey::flags == '1') {
    return cipher_text;
  } else if (cryptokey::flags == crypto_sign::cAES ||
             cryptokey::flags == crypto_sign::caes) {
    CookieCryptor Aes_Crypted_;
    Aes_Crypted_.DecryptString(base64_cip_result, &plain_text);
  } else if (cryptokey::flags == crypto_sign::cDES ||
             cryptokey::flags == crypto_sign::cdes) {
    Des_DecryptString(&plain_text, base64_cip_result);
  } else if (cryptokey::flags == crypto_sign::cSM4 ||
             cryptokey::flags == crypto_sign::csm4) {
    Sms4_DecryptString(&plain_text, base64_cip_result);
  } else {
    plain_text.assign(base64_cip_result);
  }
  return plain_text;
}

void YspCryptoSingleton::SetUserId(const std::string& id) {
  uuid = id;
}

std::string YspCryptoSingleton::GetUserId() {
  return uuid;
}

void YspCryptoSingleton::SetPinKeys(const std::vector<std::string>& pin_keys) {
  if (pin_keys_.empty())
    pin_keys_ = pin_keys;
}

const std::vector<std::string>& YspCryptoSingleton::GetPinKeys(){
  return pin_keys_;
}

std::string YspCryptoSingleton::GetPinKey(int index) {
  if (index >= 0 && index < (int)pin_keys_.size())
    return pin_keys_[index];
  return "";
}

std::string YspCryptoSingleton::GetCurrentPinKey() {
  if (pin_keys_.empty())
    return "";
  return pin_keys_[pin_keys_.size() - 1];
}

void YspCryptoSingleton::UpdateCurrentPinKey(const std::string& pin_key) {
  if (pin_key.empty())
    return;
  std::string sha_key = crypto::SHA256HashString(pin_key);
  sha_key = base::HexEncode(sha_key.data(), sha_key.length());
  pin_keys_.push_back(sha_key);
}

int YspCryptoSingleton::GetCurrentPinKeyIndex() {
  return pin_keys_.size() - 1;
}
