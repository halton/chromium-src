#ifdef REDCORE
//ysp+ { proxy }

#include "net/socket/ysp_spa_packet.h"

#ifdef WIN32
#include <sys/timeb.h>
#endif


#include <string>
#include <openssl/aes.h>
#include <openssl/md5.h>
#include "base/base64.h"
#include "base/files/file_util.h"
#include "base/md5.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "components/version_info/version_info.h"
#include "crypto/hmac.h"
#include "crypto/sha2.h"
#include "net/base/io_buffer.h"
#include "net/base/rand_callback.h"
#include "net/socket/udp_socket.h"

#define RAND_VALUE_SIZE				16
#define SHA256_DIGEST_LENGTH		32
#define AES_MAX_SIZE				32
#define AES_SALT_LEN				8
#define MD5_DIGEST_LEN				16

#ifndef WIN32
#ifndef RAND_FILE
#define RAND_FILE "/dev/urandom"
#endif
#endif



//#define TEST_REDCORE

namespace {
#ifdef WIN32
size_t strlcat(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;
	size_t dlen;

	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));
}

size_t strlcpy(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	if (n == 0) {
		if (siz != 0)
			*d = '\0';
		while (*s++)
			;
	}

	return(s - src - 1);
}
#else
#define strnlen(s,l) (strlen(s)<l ? strlen(s):l)
#endif

void
get_random_data(unsigned char *data, const size_t len)
{
	uint32_t    i;
#ifdef WIN32
	int				rnum;
	struct _timeb	tb;

	_ftime_s(&tb);

	srand((uint32_t)(tb.time * 1000) + tb.millitm);

	for (i = 0; i<len; i++)
	{
		rnum = rand();
		*(data + i) = rnum % 0xff;
	}
#else
	FILE           *rfd;
	struct timeval  tv;
	int             do_time = 0;
	size_t          amt_read;

	if ((rfd = fopen(RAND_FILE, "r")) == NULL)
	{
		do_time = 1;
	}
	else
	{
		amt_read = fread(data, len, 1, rfd);
		fclose(rfd);

		if (amt_read != 1)
			do_time = 1;
	}

	if (do_time)
	{
		gettimeofday(&tv, NULL);
		srand(tv.tv_usec);

		for (i = 0; i<len; i++)
			*(data + i) = rand() % 0xff;
	}

#endif
	memset(data, 1, len);

}


std::string set_key_and_iv(const std::string key, const unsigned char *value)
{
	char            pw_buf[AES_MAX_SIZE] = { 0 };
	unsigned char   tmp_buf[MD5_DIGEST_LEN + AES_MAX_SIZE + AES_BLOCK_SIZE] = { 0 };
	unsigned char   kiv_buf[AES_MAX_SIZE + AES_BLOCK_SIZE] = { 0 }; //Key and IV buffer
	unsigned char	salt_buf[AES_SALT_LEN] = { 0 }; //salt buffer
	unsigned char   md5_buf[MD5_DIGEST_LEN] = { 0 }; //md5 buffer
	char			key_iv_buf[AES_MAX_SIZE + AES_BLOCK_SIZE] = { 0 };

	int             final_key_len = 0;
	size_t          kiv_len = 0;
	int				key_len = key.length();

		if (key_len < AES_BLOCK_SIZE)
		{
			memcpy(pw_buf, key.c_str(), key_len);
			memset(pw_buf + key_len, '0', AES_BLOCK_SIZE - key_len);
			final_key_len = AES_BLOCK_SIZE;
		}
		else
		{
			memcpy(pw_buf, key.c_str(), key_len);
			final_key_len = key_len;
		}

	if (value != NULL)
	{
		memcpy(salt_buf, (value + AES_SALT_LEN), AES_SALT_LEN);
	}
	else
	{
		get_random_data(salt_buf, AES_SALT_LEN);
	}

	memcpy(tmp_buf + MD5_DIGEST_LEN, pw_buf, final_key_len);
	memcpy(tmp_buf + MD5_DIGEST_LEN + final_key_len, salt_buf, AES_SALT_LEN);
	std::string md5_tmp;
	while (kiv_len < sizeof(kiv_buf))
	{
		if (kiv_len == 0)
			MD5(tmp_buf + MD5_DIGEST_LEN, final_key_len + AES_SALT_LEN, md5_buf);
		else
			MD5(tmp_buf, MD5_DIGEST_LEN + final_key_len + AES_SALT_LEN, md5_buf);

		memcpy(tmp_buf, md5_buf, MD5_DIGEST_LEN);

		memcpy(kiv_buf + kiv_len, md5_buf, MD5_DIGEST_LEN);

		kiv_len += MD5_DIGEST_LEN;
	}
	char str_buf[1024] = { 0 };
	for (size_t i = 0; i < (AES_MAX_SIZE + AES_BLOCK_SIZE); i++) {
		sprintf(str_buf + (i * 3), "%02x ", kiv_buf[i]);
		key_iv_buf[i] = kiv_buf[i];
	}
	DLOG(INFO) << "key: " << str_buf;
	return std::string(key_iv_buf, 0, AES_MAX_SIZE + AES_BLOCK_SIZE);
}

void InitHmacKey(unsigned char* inner_key, unsigned char* outer_key, const std::string hmac_key)
{
	size_t i = 0;
	for (i = 0; i < 136 && i < hmac_key.length(); i++)
	{
		inner_key[i] = hmac_key.c_str()[i] ^ 0x36;
		outer_key[i] = hmac_key.c_str()[i] ^ 0x5c;
	}
	if (i < 136)
	{
		while (i < 136)
		{
			inner_key[i] = 0x36;
			outer_key[i] = 0x5c;
			i++;
		}
	}
	return;
}

} //namespace

YSPRedcoreSpaPacket* g_instance = nullptr;

YSPRedcoreSpaPacket::YSPRedcoreSpaPacket()
	: rand_value_(""),
	  b64_device_id_(""),
	  b64_username_(""),
	  time_stamp_(0),
	  version_(""),
	  b64_access_cmd_(""),
	  server_ip_(""),
	  block_key_(""),
	  hmac_key_(""),
	  encoded_string_(""),
	  sha256_encoded_string_(""),
	  encrypted_string_(""),
	  hmac_sha256_encrypted_string_("")
{
}

YSPRedcoreSpaPacket::~YSPRedcoreSpaPacket()
{
}

//static
YSPRedcoreSpaPacket* YSPRedcoreSpaPacket::GetInstance() {
	if (!g_instance) {
		g_instance = new YSPRedcoreSpaPacket;
	}
	return g_instance;
}

int YSPRedcoreSpaPacket::InitValues(const std::string & device_id, const std::string & username,
							  const std::string & access_cmd, const std::string & server_ip,
							  const std::string & key, const std::string & hmac_key, const int timeDiff)
{
	int ret = 0;
	if (device_id.empty() || username.empty() || access_cmd.empty() || server_ip.empty() || key.empty() || hmac_key.empty())
	{
		DLOG(INFO) << "parameter is empty !";
		return -1;
	}
	server_ip_ = server_ip;
	std::string b64_tmp = "";
	base::Base64Encode(device_id, &b64_tmp);
	b64_device_id_.assign(b64_tmp, 0, b64_tmp.find('='));
	DLOG(INFO) << "device_id: " << device_id << " b64_tmp: " << b64_tmp << " b64_device_id: " << b64_device_id_;
	b64_tmp = "";
	base::Base64Encode(username, &b64_tmp);
	b64_username_.assign(b64_tmp, 0, b64_tmp.find('='));
	b64_tmp = "";
	base::Base64Encode(access_cmd, &b64_tmp);
	b64_access_cmd_.assign(b64_tmp, 0, b64_tmp.find('='));
	DLOG(INFO) << "access_cmd: " << access_cmd << " b64_tmp: " << b64_tmp << " b64_access_cmd_: " << b64_access_cmd_;
	std::string block_key = "";
	if (!base::Base64Decode(key, &block_key))
	{
		DLOG(INFO) << "block key base64 decode failure !";
		return -1;
	}
	block_key_ = set_key_and_iv(block_key, NULL);
#ifdef TEST_REDCORE
	char Hex_block_key[1024] = { 0 };
	for (size_t i = 0; i < block_key_.length(); i++) {
		sprintf(Hex_block_key + (i * 3), "%02x ", (uint8_t)(block_key_.c_str()[i]));
	}
	DLOG(INFO) << "HEX block_key: " << Hex_block_key;
#endif //TEST_REDCORE
	if (!base::Base64Decode(hmac_key, &hmac_key_))
	{
		DLOG(INFO) << "hmac key base64 decode failure !";
		return -1;
	}
	unsigned char inner_key[136] = { 0 };
	unsigned char outer_key[136] = { 0 };
	InitHmacKey(inner_key, outer_key, hmac_key_.c_str());
#ifdef TEST_REDCORE
	char inner_tmp[1024] = { 0 };
	char outer_tmp[1024] = { 0 };
	for (size_t i = 0; i < 136; i++) {
		sprintf(inner_tmp + (i * 3), "%02x ", inner_key[i]);
		sprintf(outer_tmp + (i * 3), "%02x ", outer_key[i]);
	}
	DLOG(INFO) << "inner key: " << inner_tmp;
	DLOG(INFO) << "outer key: " << outer_tmp;
#endif //TEST_REDCORE
	if ((ret = CreateRandAndTimeStamp(timeDiff)) < 0)
	{
		DLOG(INFO) << "create rand and timestamp failure !";
		return ret;
	}
	version_ = "2.0.2";

	{	//验证字符串拼接
		char enc_msg[1024] = { 0 };
		strlcpy(const_cast<char*>(enc_msg), rand_value_.c_str(), 1024);
#ifdef TEST_REDCORE
		strlcpy(enc_msg, "8205236612517342", 1024);
#endif //TEST_REDCORE
		strlcat(const_cast<char*>(enc_msg), ":", 1024);
		strlcat(const_cast<char*>(enc_msg), b64_device_id_.c_str(), 1024);
		strlcat(const_cast<char*>(enc_msg), ":", 1024);
		strlcat(const_cast<char*>(enc_msg), b64_username_.c_str(), 1024);
		int offset = strlen(enc_msg);
		snprintf(const_cast<char*>(enc_msg + offset), 1024 - offset, ":%u:"
#ifdef WIN32
                 , (unsigned long)
#else
                 , (unsigned int)
#endif
                 time_stamp_);
#ifdef TEST_REDCORE
		strlcat(enc_msg, ":", 1024);
		strlcat(enc_msg, "1535273604", 1024);
		strlcat(enc_msg, ":", 1024);
#endif //TEST_REDCORE
		strlcat(enc_msg, version_.c_str(), 1024);
		offset = strlen(enc_msg);
		snprintf(enc_msg + offset, 1024 - offset, ":%i:", 1); //message type: Access message
		strlcat(enc_msg, b64_access_cmd_.c_str(), 1024);
		encoded_string_.assign(enc_msg, strlen(enc_msg));
		DLOG(INFO) << "encoded_string: " << encoded_string_;
#ifdef TEST_REDCORE
		std::string cip_test = "";
		EncryptString(encoded_string_, &cip_test);
		char enc_str[2048] = { 0 };
		for (size_t i = 0; i < cip_test.length(); i++) {
			sprintf(enc_str + (i * 3), "%02x ", (uint8_t)(cip_test.c_str()[i]));
		}
		DLOG(INFO) << "encrypt string: " << enc_str;
#endif //TEST_REDCORE

	}
	{	//计算验证字符串SHA256的值
		std::string sha256_tmp = "";
		sha256_tmp = crypto::SHA256HashString(encoded_string_);
		std::string b64_sha256_tmp = "";
		base::Base64Encode(sha256_tmp, &b64_sha256_tmp);
		sha256_encoded_string_.assign(b64_sha256_tmp, 0, b64_sha256_tmp.find('='));
		DLOG(INFO) << "sha256_encoded_string_: " << sha256_encoded_string_;
	}
	{	//"encoded_string_:sha256_encoded_string_" AES-CBC加密
		std::string plaintext = "";
		plaintext += encoded_string_ + ":" + sha256_encoded_string_;
		std::string ciphertext = "";
		std::string ciphertmp = "";
		if (!EncryptString(plaintext, &ciphertmp))
		{
			DLOG(INFO) << "enctypt failure !";
			return -1;
		}
		char value_tmp[9] = { 0 };
		get_random_data((unsigned char*)value_tmp, 8);
		ciphertext.assign("Salted__");
		ciphertext.append(value_tmp);
		ciphertext.append(ciphertmp);
		std::string b64_ciphertext = "";
		base::Base64Encode(ciphertext, &b64_ciphertext);
		encrypted_string_.assign(b64_ciphertext, 0, b64_ciphertext.find('='));
		DLOG(INFO) << "encrypted_string_: " << encrypted_string_;
	}
	{ //AES-CBC加密后的字符串再进行HMAC-SHA256加密
		std::string message_padd = "";
		char message_tmp[1024] = { 0 };
		memcpy(message_tmp, inner_key, 64);
		memcpy(message_tmp + 64, encrypted_string_.c_str(), encrypted_string_.length());
		message_padd.assign(message_tmp, 0, encrypted_string_.length() + 64);
		std::string hash_str = crypto::SHA256HashString(message_padd);
#ifdef TEST_REDCORE
		char Hex_hash_str[2048] = { 0 };
		for (size_t i = 0; i < hash_str.length(); i++) {
			sprintf(Hex_hash_str + (i * 3), "%02x ", (uint8_t)(hash_str.c_str()[i]));
		}
		DLOG(INFO) << "HEX hash_str: " << Hex_hash_str;
#endif //TEST_REDCORE
		char hash_tmp[1024] = { 0 };
		memcpy(hash_tmp, outer_key, 64);
		memcpy(hash_tmp + 64, hash_str.c_str(), 32);
		std::string hash_str_tmp = "";
		for (size_t i = 0; i < 96; i++) {
			hash_str_tmp += hash_tmp[i];
		}
		std::string hmac_tmp = crypto::SHA256HashString(hash_str_tmp);
#ifdef TEST_REDCORE
		char Hex_hmac_tmp[2048] = { 0 };
		for (size_t i = 0; i < hmac_tmp.length(); i++) {
			sprintf(Hex_hmac_tmp + (i * 3), "%02x ", (uint8_t)(hmac_tmp.c_str()[i]));
		}
		DLOG(INFO) << "HEX hmac_tmp: " << Hex_hmac_tmp;
#endif //TEST_REDCORE
		std::string b64_hmac_tmp = "";
		base::Base64Encode(hmac_tmp, &b64_hmac_tmp);
		hmac_sha256_encrypted_string_.assign(b64_hmac_tmp, 0, b64_hmac_tmp.find('='));
		DLOG(INFO) << "hmac_sha256_encrypted_string_: " << hmac_sha256_encrypted_string_;

	}

	return ret;
}

// net::IPAddressNumber -> net::IPAddress
// https://codereview.chromium.org/1932363003
int YSPRedcoreSpaPacket::SendUdpPacket(net::CompletionCallback callback)
{
	// net::UDPSocket socket(net::DatagramSocket::DEFAULT_BIND, 
	// 					  net::RandIntCallback(),
	// 					  NULL, net::NetLogSource());
	// socket.Open(net::AddressFamily::ADDRESS_FAMILY_IPV4);
	// const std::vector<uint8_t>& address
	// net::IPAddressNumber ip_number;
	// net::ParseURLHostnameToNumber(server_ip_, &ip_number);
	// socket.Connect(net::IPEndPoint(ip_number, 62201));
	// net::IPEndPoint endpoint;
	// socket.GetLocalAddress(&endpoint);
	// std::string request_data = "";
	// request_data.assign(encrypted_string_, 10, encrypted_string_.length() - 10);
	// request_data += hmac_sha256_encrypted_string_;
	// scoped_refptr<net::StringIOBuffer> io_buffer(new net::StringIOBuffer(request_data));
	// scoped_refptr<net::DrainableIOBuffer> buffer(
	// 	new net::DrainableIOBuffer(io_buffer.get(), request_data.length()));
	// socket.Write(buffer.get(), buffer->BytesRemaining(), callback);
	// socket.Close();
	return 0;
}

int YSPRedcoreSpaPacket::CreateRandAndTimeStamp(const int timeDiff)
{
	unsigned long seed;
	char rand_tmp[RAND_VALUE_SIZE + 1] = { 0 };
	char tmp_buf[RAND_VALUE_SIZE + 1] = { 0 };
#ifdef WIN32
	struct _timeb timeb;
	_ftime_s(&timeb);
	seed = 0xFFFFFFFF & ((timeb.time * 1000) + timeb.millitm);
#else
	char rand_file_buf[5] = { 0 };
	struct timeval  tv;
	base::FilePath rand_file("/dev/urandom");
	int length = base::ReadFile(rand_file, rand_file_buf, 4);
	if (length == 4)
	{
		memcpy(&seed, rand_file_buf, 4);
	}
	else if (length == -1)
	{
		gettimeofday(&tv, NULL);
		seed = tv.tv_usec;
	}
	else
	{
		DLOG(INFO) << "read rand file failure !";
		return -1;
	}
#endif	

	srand(seed);
	snprintf(rand_tmp, RAND_VALUE_SIZE, "%u", rand());

	while (strnlen(rand_tmp, RAND_VALUE_SIZE + 1) < RAND_VALUE_SIZE)
	{
		snprintf(tmp_buf, RAND_VALUE_SIZE, "%u", rand());
		strlcat(rand_tmp, tmp_buf, RAND_VALUE_SIZE + 1);
	}
	rand_value_.assign(rand_tmp, RAND_VALUE_SIZE);

	time_stamp_ = base::Time::Now().ToTimeT() + timeDiff;

	return 0;
}

bool YSPRedcoreSpaPacket::EncryptString(const std::string& plaintext,
	std::string* ciphertext) {
	AES_KEY aes_key;
	char key_data[33] = { 0 };
	memcpy(key_data, block_key_.c_str(), 16);
	memcpy(key_data + 16, block_key_.c_str() + 16, 16);
	if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(key_data),
		AES_BLOCK_SIZE * 16, &aes_key) != 0) {
		return false;
	}
	const size_t out_size = plaintext.size();
	std::string pla_padding = plaintext;
	int offset = AES_BLOCK_SIZE - out_size % AES_BLOCK_SIZE;
	size_t out_size_new = out_size + offset;
	char padding[AES_BLOCK_SIZE];
	memset(padding, offset, AES_BLOCK_SIZE);
	pla_padding.append(padding, offset);
	std::string result;
	uint8_t* out_ptr =
		reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size_new + 1));

	uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
	memcpy(ivec, block_key_.c_str() + 32, AES_BLOCK_SIZE);

	AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(pla_padding.data()), out_ptr,
		pla_padding.size(), &aes_key, ivec, AES_ENCRYPT);
	ciphertext->swap(result);
	return true;
}

bool YSPRedcoreSpaPacket::DecryptString(const std::string& ciphertext,
	std::string* plaintext) {
	AES_KEY aes_key;
	char key_data[33] = { 0 };
	memcpy(key_data, block_key_.c_str(), 16);
	memcpy(key_data + 16, block_key_.c_str() + 16, 16);
	if (AES_set_decrypt_key(reinterpret_cast<const uint8_t*>(key_data),
		AES_BLOCK_SIZE * 16, &aes_key) != 0) {
		return false;
	}
	const size_t out_size = ciphertext.size();
	std::string result;
	uint8_t* out_ptr =
		reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

	uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
	memcpy(ivec, block_key_.c_str() + 32, AES_BLOCK_SIZE);

	AES_cbc_encrypt(reinterpret_cast<const uint8_t*>(ciphertext.data()), out_ptr,
		ciphertext.size(), &aes_key, ivec, AES_DECRYPT);
	size_t length = result.length();
	int offset = result.c_str()[length - 1];
	plaintext->clear();
	plaintext->assign(result, 0, length - offset);
	return true;
}

bool YSPRedcoreSpaPacket::HMACEncryptString(const std::string& plaintext, std::string * ciphertext)
{
	if (plaintext.empty())
		return false;
	unsigned char digestSha256[SHA256_DIGEST_LENGTH] = { '\0' };
	crypto::HMAC hmacSha256(crypto::HMAC::SHA256);
    
    if(hmacSha256.Init(hmac_key_))
    {
        if(hmacSha256.Sign(plaintext, digestSha256, SHA256_DIGEST_LENGTH))
        {
            std::string digest = "", digest_base64 = "";
            for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
                digest += digestSha256[i];
            base::Base64Encode(digest, &digest_base64);
            ciphertext->append(digest_base64);
            return true;
        }
    }
    
	return false;
}

#endif