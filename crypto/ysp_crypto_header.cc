// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//ysp+ { crypto http header }

#include "crypto/ysp_crypto_header.h"

#include <map>
#include <set>
#include <openssl/aes.h>

#include "base/json/json_reader.h"
#include "base/values.h"
#include "base/md5.h"
#include "base/base64.h"
#include "base/time/time.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"

#include "crypto/hmac.h"

#define SHA1_DIGEST_LENGTH	20
#define SHA256_DIGEST_LENGTH	32
#define SM3_DIGEST_LENGTH	32
#define SM3_HMAC_SIZE		(SM3_DIGEST_LENGTH)
void sm3_hmac(const unsigned char *data, size_t data_len,
	const unsigned char *key, size_t key_len, unsigned char mac[SM3_HMAC_SIZE]);

namespace cryptoHeaderkey{
    char flags = '0';
    char pwd[9] = "";
    char sal[9] = "";
    char iv[17] = "";
} //namespace cryptoHeaderkey
namespace {
class HeaderCryptor{
 public:
   HeaderCryptor();
  ~HeaderCryptor();
  bool EncryptString(const std::string& plain_text,
                     std::string* cipher_text);
  bool DecryptString(const std::string& cipher_text,
                     std::string* plain_text);
};

HeaderCryptor::HeaderCryptor() {
}

HeaderCryptor::~HeaderCryptor() {}

bool HeaderCryptor::EncryptString(const std::string& plain_text,
  std::string* cipher_text) {
  AES_KEY aes_key;
  char key_data[17] = {0};
  memcpy(key_data, cryptoHeaderkey::pwd, 8);
  memcpy(key_data + 8, cryptoHeaderkey::sal, 8);
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
    AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = plain_text.size();
  std::string pla_padding = plain_text;
  int offset = AES_BLOCK_SIZE - out_size % AES_BLOCK_SIZE;
  size_t out_size_new = out_size + offset;
  char padding[AES_BLOCK_SIZE];
  memset(padding, offset, AES_BLOCK_SIZE);
  pla_padding.append(padding, offset);
  std::string result;
  uint8_t* out_ptr =
    reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
  memcpy(ivec, cryptoHeaderkey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
    pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
  cipher_text->swap(result);
  return true;
}

bool HeaderCryptor::DecryptString(const std::string& cipher_text,
  std::string* plain_text) {
  AES_KEY aes_key;
  char key_data[17] = { 0 };
  memcpy(key_data, cryptoHeaderkey::pwd, 8);
  memcpy(key_data + 8, cryptoHeaderkey::sal, 8);
  if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
    AES_BLOCK_SIZE * 8, &aes_key) != 0) {
    return false;
  }
  const size_t out_size = cipher_text.size();
  std::string result;
  uint8_t* out_ptr =
    reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
  memcpy(ivec, cryptoHeaderkey::iv, AES_BLOCK_SIZE);

  AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(cipher_text.data()), out_ptr,
    cipher_text.size(), &aes_key, ivec, AES_DECRYPT);
  size_t length = result.length();
  int offset = result.c_str()[length - 1];
  plain_text->clear();
  plain_text->assign(result, 0, length - offset);
  return true;
}
} //namespace

YspCryptoHeader* YspCryptoHeader::instance_ = nullptr;
YspCryptoHeader * YspCryptoHeader::GetInstance() {
    if(instance_ == nullptr)  //判断是否第一次调用
        instance_ = new YspCryptoHeader();
    return instance_;
}

void YspCryptoHeader::Init(const std::string& crypto_key) {
  if (crypto_key.empty())
    return;
  cryptoHeaderkey::flags = crypto_key.data()[0];
  if (crypto_key.length() >= 9) {
    memcpy(cryptoHeaderkey::pwd, crypto_key.data() + 1, 8);
    if (crypto_key.length() >= 17) {
      memcpy(cryptoHeaderkey::sal, crypto_key.data() + 9, 8);
      if (crypto_key.length() >= 33) {
        memcpy(cryptoHeaderkey::iv, crypto_key.data() + 17, 16);
      } else {
        memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
      }
    } else {
      memcpy(cryptoHeaderkey::sal, "ize.com.", 8);
      memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
    }
  } else {
    memcpy(cryptoHeaderkey::pwd, "allmobil", 8);
    memcpy(cryptoHeaderkey::sal, "ize.com.", 8);
    memcpy(cryptoHeaderkey::iv, "the iv: 16 bytes", 16);
  }
  //DLOG(INFO) << "cryptokey::pwd:" << cryptokey::pwd << " cryptokey::sal:" << cryptokey::sal << " cryptokey::iv" << cryptokey::iv;
}

std::string YspCryptoHeader::EncryptString(const std::string& plain_text) {
    std::string cipher_text;
  if (!plain_text.empty()) {
    HeaderCryptor Aes_Crypted_;
    Aes_Crypted_.EncryptString(plain_text, &cipher_text);
  }
    return cipher_text;
}
std::string YspCryptoHeader::DecryptString(const std::string& cipher_text) {
    std::string plain_text;
  if (!cipher_text.empty()) {
    HeaderCryptor Aes_Crypted_;
    Aes_Crypted_.DecryptString(cipher_text, &plain_text);
  }
    return plain_text;
}

bool YspCryptoHeader::isAddHeaders()
{
  if (cryptoHeaderkey::flags == '3')
    return true;
  return false;
}

std::string YspCryptoHeader::GetEncString()
{
  std::string base64_enc_timeStamp;
	std::string timeStamp = base::Int64ToString((base::Time::Now().ToTimeT() + time_diff_));
  std::string enc_timeStamp = EncryptString(timeStamp);

  //DLOG(INFO) << "enc_timeStamp: " << base::HexEncode(enc_timeStamp.c_str(), enc_timeStamp.length());
  base::Base64Encode(enc_timeStamp, &base64_enc_timeStamp);
  DLOG(INFO) << "timeStamp:[" << timeStamp << "] base64_enc_timeStamp: [" << base64_enc_timeStamp << "]";

  //std::string base64_dec_tmp;
  //base::Base64Decode(base64_enc_timeStamp, &base64_dec_tmp);
  //std::string tmp_dec = DecryptString(base64_dec_tmp);
  //DLOG(INFO) << "dec_tmp: [" << tmp_dec << "]";
  return " (" + base64_enc_timeStamp + ")";
}

std::string YspCryptoHeader::GetHmacEncString(const std::string& message_type,
                                              const std::string& uri,
                                              const std::string& version) {
  int type = 1;  // 1:sha1, 2:sha256, 3:sm3
  // std::string method = "HTTP" + message_type;

  int clientID = 352234625;
  char client_id[64] = {0};
  std::string client_id_hex = "";
  sprintf(client_id, "%x", clientID);
  client_id_hex.assign(client_id);
  char time_stamp[64] = {0};
#ifdef WIN32
	sprintf(time_stamp, "%I64x", (base::Time::Now().ToTimeT() + time_diff_));
#else
	sprintf(time_stamp, "%lx", (base::Time::Now().ToTimeT() + time_diff_));
#endif
        std::string time_stamp_hex;
        time_stamp_hex.assign(time_stamp);
        // std::string key = "hIxw20i48TZV3bHB6hPjkujMLYa5gsAj";
        std::string key = "";
        key.assign(cryptoHeaderkey::pwd);
        key.append(cryptoHeaderkey::sal);
	key.append(cryptoHeaderkey::iv);
	//memcpy(key, cryptoHeaderkey::pwd, 8);
	//memcpy(key, cryptoHeaderkey::sal, 8);
	//memcpy(key, cryptoHeaderkey::iv, 16);
	key = "uHc2NRU4OZ6pvquna7ba6GMwWVRdoxgH";
        std::string value = version + "#" + base::IntToString(type) + "#" +
                            client_id_hex + "#" + time_stamp_hex + "#" +
                            message_type + "#" + uri;
        std::string cipher_text =
            version + base::IntToString(type) + client_id_hex + time_stamp_hex;
        if (type == 1) {
          unsigned char digestSha1[SHA1_DIGEST_LENGTH] = {'\0'};

          crypto::HMAC hmacSha1(crypto::HMAC::SHA1);

          if (hmacSha1.Init(key)) {
            if(hmacSha1.Sign(value, digestSha1, SHA1_DIGEST_LENGTH))
            {
                std::string digest = "", digest_base64 = "";
                for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
                    digest += digestSha1[i];
                }
                base::Base64Encode(digest, &digest_base64);
                cipher_text.append(digest_base64);
                
                DLOG(INFO) << "HMAC SHA1 hex : " << base::HexEncode(digestSha1, SHA1_DIGEST_LENGTH);
            }
        }
	}
	else if (type == 2) {
		unsigned char digestSha256[SHA256_DIGEST_LENGTH] = { '\0' };
		crypto::HMAC hmacSha256(crypto::HMAC::SHA256);
        if(hmacSha256.Init(key))
        {
            if(hmacSha256.Sign(value, digestSha256, SHA256_DIGEST_LENGTH))
            {
                std::string digest = "", digest_base64 = "";
                for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
                    digest += digestSha256[i];
                base::Base64Encode(digest, &digest_base64);
                cipher_text.append(digest_base64);
                
                DLOG(INFO) << "HMAC SHA256 hex : " << base::HexEncode(digestSha256, SHA256_DIGEST_LENGTH);
            }
        }
	}
	else if (type == 3) {
		unsigned char digestSm3[SM3_HMAC_SIZE] = { '\0' };
		sm3_hmac(reinterpret_cast<const unsigned char*>(value.data()), value.length(),
			reinterpret_cast<const unsigned char*>(key.data()), key.length(), digestSm3);
		std::string digest = "", digest_base64 = "";
		for (size_t i = 0; i < SM3_HMAC_SIZE; i++)
			digest += digestSm3[i];
		base::Base64Encode(digest, &digest_base64);
		cipher_text.append(digest_base64);

		DLOG(INFO) << "HMAC SM3 hex : " << base::HexEncode(digestSm3, SM3_HMAC_SIZE);
	}
	DLOG(INFO) << "HMAC cipher_text: " << " (" + cipher_text + ")";
	DLOG(INFO) << "value: " << value;
	return " (" + cipher_text + ")";
}

//YSP+ { SM3
#define SM3_BLOCK_SIZE		64
#define SM3_CBLOCK		(SM3_BLOCK_SIZE)

#define IPAD	0x36
#define OPAD	0x5C

typedef struct {
	uint32_t digest[8];
	int nblocks;
	unsigned char block[64];
	int num;
} sm3_ctx_t;

typedef struct {
	sm3_ctx_t sm3_ctx;
	unsigned char key[SM3_BLOCK_SIZE];
} sm3_hmac_ctx_t;

void sm3_hmac_init(sm3_hmac_ctx_t *ctx, const unsigned char *key, size_t key_len);
void sm3_hmac_update(sm3_hmac_ctx_t *ctx, const unsigned char *data, size_t data_len);
void sm3_hmac_final(sm3_hmac_ctx_t *ctx, unsigned char mac[SM3_HMAC_SIZE]);

#define ROTATELEFT(X,n)  (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )

static uint32_t cpu_to_be32(uint32_t value)
{
#ifndef __BIG_ENDIAN__

	uint8_t lolo = (value >> 24) & 0xFF;
	uint8_t lohi = (value >> 16) & 0xFF;
	uint8_t hilo = (value >> 8) & 0xFF;
	uint8_t hihi = (value >> 0) & 0xFF;

	uint32_t value2 = (hihi << 24)
		| (hilo << 16)
		| (lohi << 8)
		| (lolo << 0);
	return value2;
#else
	return digest;
#endif
}

void sm3_compress(uint32_t digest[8], const unsigned char block[64])
{
	int j;
	uint32_t W[68], W1[64];
	const uint32_t *pblock = (const uint32_t *)block;

	uint32_t A = digest[0];
	uint32_t B = digest[1];
	uint32_t C = digest[2];
	uint32_t D = digest[3];
	uint32_t E = digest[4];
	uint32_t F = digest[5];
	uint32_t G = digest[6];
	uint32_t H = digest[7];
	uint32_t SS1, SS2, TT1, TT2, T[64];

	for (j = 0; j < 16; j++) {
		W[j] = cpu_to_be32(pblock[j]);
	}
	for (j = 16; j < 68; j++) {
		W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTATELEFT(W[j - 3], 15)) ^ ROTATELEFT(W[j - 13], 7) ^ W[j - 6];;
	}
	for (j = 0; j < 64; j++) {
		W1[j] = W[j] ^ W[j + 4];
	}

	for (j = 0; j < 16; j++) {

		T[j] = 0x79CC4519;
		SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j)), 7);
		SS2 = SS1 ^ ROTATELEFT(A, 12);
		TT1 = FF0(A, B, C) + D + SS2 + W1[j];
		TT2 = GG0(E, F, G) + H + SS1 + W[j];
		D = C;
		C = ROTATELEFT(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = ROTATELEFT(F, 19);
		F = E;
		E = P0(TT2);
	}

	for (j = 16; j < 64; j++) {

		T[j] = 0x7A879D8A;
		SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(T[j], j % 32)), 7);
		SS2 = SS1 ^ ROTATELEFT(A, 12);
		TT1 = FF1(A, B, C) + D + SS2 + W1[j];
		TT2 = GG1(E, F, G) + H + SS1 + W[j];
		D = C;
		C = ROTATELEFT(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = ROTATELEFT(F, 19);
		F = E;
		E = P0(TT2);
	}

	digest[0] ^= A;
	digest[1] ^= B;
	digest[2] ^= C;
	digest[3] ^= D;
	digest[4] ^= E;
	digest[5] ^= F;
	digest[6] ^= G;
	digest[7] ^= H;
}

void sm3_init(sm3_ctx_t *ctx)
{
	ctx->digest[0] = 0x7380166F;
	ctx->digest[1] = 0x4914B2B9;
	ctx->digest[2] = 0x172442D7;
	ctx->digest[3] = 0xDA8A0600;
	ctx->digest[4] = 0xA96F30BC;
	ctx->digest[5] = 0x163138AA;
	ctx->digest[6] = 0xE38DEE4D;
	ctx->digest[7] = 0xB0FB0E4E;

	ctx->nblocks = 0;
	ctx->num = 0;
}

void sm3_update(sm3_ctx_t *ctx, const unsigned char* data, size_t data_len)
{
	if (ctx->num) {
		unsigned int left = SM3_BLOCK_SIZE - ctx->num;
		if (data_len < left) {
			memcpy(ctx->block + ctx->num, data, data_len);
			ctx->num += data_len;
			return;
		}
		else {
			memcpy(ctx->block + ctx->num, data, left);
			sm3_compress(ctx->digest, ctx->block);
			ctx->nblocks++;
			data += left;
			data_len -= left;
		}
	}
	while (data_len >= SM3_BLOCK_SIZE) {
		sm3_compress(ctx->digest, data);
		ctx->nblocks++;
		data += SM3_BLOCK_SIZE;
		data_len -= SM3_BLOCK_SIZE;
	}
	ctx->num = data_len;
	if (data_len) {
		memcpy(ctx->block, data, data_len);
	}
}

void sm3_final(sm3_ctx_t *ctx, unsigned char *digest)
{
	int i;
	uint32_t *pdigest = (uint32_t *)digest;
	uint32_t *count = (uint32_t *)(ctx->block + SM3_BLOCK_SIZE - 8);

	ctx->block[ctx->num] = 0x80;

	if (ctx->num + 9 <= SM3_BLOCK_SIZE) {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 9);
	}
	else {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 1);
		sm3_compress(ctx->digest, ctx->block);
		memset(ctx->block, 0, SM3_BLOCK_SIZE - 8);
	}

	count[0] = cpu_to_be32((ctx->nblocks) >> 23);
	count[1] = cpu_to_be32((ctx->nblocks << 9) + (ctx->num << 3));

	sm3_compress(ctx->digest, ctx->block);
	for (i = 0; i < (int)(sizeof(ctx->digest) / sizeof(ctx->digest[0])); i++) {
		pdigest[i] = cpu_to_be32(ctx->digest[i]);
	}
}

void sm3(const unsigned char *msg, size_t msglen,
	unsigned char dgst[SM3_DIGEST_LENGTH])
{
	sm3_ctx_t ctx;

	sm3_init(&ctx);
	sm3_update(&ctx, msg, msglen);
	sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(sm3_ctx_t));
}

void sm3_hmac_init(sm3_hmac_ctx_t *ctx, const unsigned char *key, size_t key_len)
{
	int i;

	if (key_len <= SM3_BLOCK_SIZE) {
		memcpy(ctx->key, key, key_len);
		memset(ctx->key + key_len, 0, SM3_BLOCK_SIZE - key_len);
	}
	else {
		sm3_init(&ctx->sm3_ctx);
		sm3_update(&ctx->sm3_ctx, key, key_len);
		sm3_final(&ctx->sm3_ctx, ctx->key);
		memset(ctx->key + SM3_DIGEST_LENGTH, 0,
			SM3_BLOCK_SIZE - SM3_DIGEST_LENGTH);
	}
	for (i = 0; i < SM3_BLOCK_SIZE; i++) {
		ctx->key[i] ^= IPAD;
	}

	sm3_init(&ctx->sm3_ctx);
	sm3_update(&ctx->sm3_ctx, ctx->key, SM3_BLOCK_SIZE);
}

void sm3_hmac_update(sm3_hmac_ctx_t *ctx,
	const unsigned char *data, size_t data_len)
{
	sm3_update(&ctx->sm3_ctx, data, data_len);
}

void sm3_hmac_final(sm3_hmac_ctx_t *ctx, unsigned char mac[SM3_HMAC_SIZE])
{
	int i;
	for (i = 0; i < SM3_BLOCK_SIZE; i++) {
		ctx->key[i] ^= (IPAD ^ OPAD);
	}
	sm3_final(&ctx->sm3_ctx, mac);
	sm3_init(&ctx->sm3_ctx);
	sm3_update(&ctx->sm3_ctx, ctx->key, SM3_BLOCK_SIZE);
	sm3_update(&ctx->sm3_ctx, mac, SM3_DIGEST_LENGTH);
	sm3_final(&ctx->sm3_ctx, mac);
}

void sm3_hmac(const unsigned char *data, size_t data_len,
	const unsigned char *key, size_t key_len,
	unsigned char mac[SM3_HMAC_SIZE])
{
	sm3_hmac_ctx_t ctx;
	sm3_hmac_init(&ctx, key, key_len);
	sm3_hmac_update(&ctx, data, data_len);
	sm3_hmac_final(&ctx, mac);
	memset(&ctx, 0, sizeof(ctx));
}
//YSP+ }