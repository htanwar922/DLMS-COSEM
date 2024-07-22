// Himanshu

#include "COSEM/COSEMSecuritySuite.h"

#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/aes.h"
#include "openssl/rand.h"

namespace LibOpenSSL {

	AES::AES(const char* ciphername, const uint8_t* key, int key_len, int tag_len /*= 0*/)
		: ciphername(ciphername)
		, key_len(key_len)
		, tag_len(tag_len)
	{
		this->key = new uint8_t[key_len];
		if (key)
			memcpy((void*)this->key, (void*)key, key_len);
		sscanf(ciphername, "%[^-]-%d-%s", (char*)algo, (int*)&bitsize, (char*)mode);
		int bytesize = bitsize / 8;
		if (key_len != bytesize) {
            LOG_ERROR("Key length %d is not equal to %d\n", key_len, bytesize);
        }
	}

	int AES::Encrypt(const uint8_t* plaintext, int len, const uint8_t* iv, int iv_len, uint8_t* ciphertext, uint8_t* tag /*= NULL*/, uint8_t* aad /*= NULL*/, int aad_len /*= 0*/) const
	{
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (ERR_LIB_NONE != EVP_EncryptInit_ex(ctx, EVP_get_cipherbyname(ciphername), NULL, NULL, NULL)) {
			ERROR("EVP_EncryptInit error\n");
			return -1;
		}
		if (0 == strcmp(mode, "GCM")) {
			if (ERR_LIB_NONE != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
				ERROR("EVP_CIPHER_CTX_ctrl error in setting the IV Length\n");
				return -1;
			}
		}
		if (ERR_LIB_NONE != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) {
			ERROR("EVP_EncryptInit error\n");
			return -1;
		}

		if (aad_len and aad) {
			if (ERR_LIB_NONE != EVP_EncryptUpdate(ctx, NULL, &aad_len, aad, aad_len)) {
				ERROR("EVP_EncryptUpdate error in setting the AAD\n");
				return -1;
			}
		}

		int retLength1 = 0;
		// Repeat for more plaintext blocks before finishing.
		if (ERR_LIB_NONE != EVP_EncryptUpdate(ctx, ciphertext, &retLength1, plaintext, len)) {
			ERROR("EVP_EncryptUpdate error\n");
			return -1;
		}
		int retLength2 = 0;
		if (ERR_LIB_NONE != EVP_EncryptFinal_ex(ctx, ciphertext + retLength1, &retLength2)) {
			ERROR("EVP_EncryptFinal error\n");
			return -1;
		}

		if (0 == strcmp(mode, "GCM")) {
			if (tag_len and tag) {
				if (ERR_LIB_NONE != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, tag_len, tag)) {
					ERROR("EVP_CIPHER_CTX_ctrl error in getting the tag\n");
					return -1;
				}
			}
			else {
				ERROR("Tag length is zero or tag is NULL\n");
			}
		}

		EVP_CIPHER_CTX_free(ctx);
		return retLength1 + retLength2;
	}

	int AES::Decrypt(const uint8_t* ciphertext, int len, const uint8_t* iv, int iv_len, uint8_t* plaintext, uint8_t* tag /*= NULL*/, uint8_t* aad /*= NULL*/, int aad_len /*= 0*/) const
	{
		EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
		if (ERR_LIB_NONE != EVP_DecryptInit_ex(ctx, EVP_get_cipherbyname(ciphername), NULL, NULL, NULL)) {
			ERROR("EVP_DecryptInit error\n");
			return -1;
		}

		if (0 == strcmp(mode, "GCM")) {
			if (ERR_LIB_NONE != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv_len, NULL)) {
				ERROR("EVP_CIPHER_CTX_ctrl error in setting the IV Length\n");
				return -1;
			}
			if (tag_len and tag) {
				if (ERR_LIB_NONE != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag_len, tag)) {
					ERROR("EVP_CIPHER_CTX_ctrl error in setting the tag\n");
					return -1;
				}
			}
			else {
				ERROR("Tag length is zero or tag is NULL\n");
			}
		}

		if (ERR_LIB_NONE != EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
			ERROR("EVP_DecryptInit error\n");
			return -1;
		}

		if (aad_len and aad) {
			if (ERR_LIB_NONE != EVP_DecryptUpdate(ctx, NULL, &aad_len, aad, aad_len)) {
				ERROR("EVP_DecryptUpdate error in setting the AAD\n");
				return -1;
			}
		}

		int retLength1 = 0;
		// Repeat for more ciphertext blocks before finishing.
		if (ERR_LIB_NONE != EVP_DecryptUpdate(ctx, plaintext, &retLength1, ciphertext, len)) {
			ERROR("EVP_DecryptUpdate error\n");
			return -1;
		}
		int retLength2 = 0;
		if (ERR_LIB_NONE != EVP_DecryptFinal_ex(ctx, plaintext + retLength1, &retLength2)) {
			ERROR("EVP_DecryptFinal error\n");
			return -1;
		}

		EVP_CIPHER_CTX_free(ctx);
		return retLength1 + retLength2;
	}

	void AES::PrintCiphertext(const uint8_t* ciphertext, int len)
	{
		BIO_dump_fp(stdout, (const char*)ciphertext, len);
	}

	EPRI::DLMSVector AES::GetKey() const
	{
		return EPRI::DLMSVector(key, key_len);
	}

	void AES::SetKey(const uint8_t* key, int key_len)
    {
        if (this->key)
			delete[] this->key;
        this->key = new uint8_t[key_len];
        memcpy((void*)this->key, (void*)key, key_len);
        this->key_len = key_len;
    }

	int AES::GetTagLength() const
	{
        return tag_len;
    }

	AES::~AES()
	{
		if (key)
			delete[] key;
	}
}


namespace EPRI
{
    SecuritySuite_0::SecuritySuite_0(const uint8_t* key, const uint8_t* aad_wo_placeholder)
        : ISecuritySuite()
		, m_AES(key)
    {
		m_AAD.Append<uint8_t>(0x00);	// Placeholder for Security Control Byte
		m_AAD.AppendBuffer(aad_wo_placeholder, 16);
    }

    SecuritySuite_0::~SecuritySuite_0()
    {
    }

    DLMSVector SecuritySuite_0::GetKey() const
    {
        return m_AES.GetKey();
    }

    void SecuritySuite_0::SetKey(const DLMSVector& key)
    {
		m_AES.SetKey(key.GetData(), key.Size());
    }

	DLMSVector SecuritySuite_0::GetAAD() const
	{
		return DLMSVector(m_AAD.GetData() + 1, m_AAD.Size() - 1);
	}

	void SecuritySuite_0::SetAAD(const DLMSVector& aad)
	{
		m_AAD.Clear();
		m_AAD.Append<uint8_t>(m_SecurityControlByte);
		m_AAD.Append(aad);
	}

    bool SecuritySuite_0::Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const
    {
		//m_AAD[0] = m_SecurityControlByte;
		DLMSVector AAD = m_AAD;
		AAD[0] = m_SecurityControlByte;
		if (m_SecurityControlByte & authentication)
		{
			tag.Clear();
			tag.Resize(m_AES.GetTagLength());
		}
        ciphertext.AppendExtra(plaintext.Size());
        int len = m_AES.Encrypt(plaintext.GetData()
			, plaintext.Size()
			, iv.GetData()
			, iv.Size()
			, (uint8_t *)ciphertext.GetData()
			, (uint8_t *)tag.GetData()
			, (uint8_t *)AAD.GetData()
			, AAD.Size()
		);
        if (len < 0) {
            LOG_ERROR("Encryption failed\n");
			return false;
		}
		return true;
	}

    bool SecuritySuite_0::Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const
    {
		//m_AAD[0] = m_SecurityControlByte;
		DLMSVector AAD = m_AAD;
		AAD[0] = m_SecurityControlByte;
		plaintext.AppendExtra(ciphertext.Size());
        int len = m_AES.Decrypt(ciphertext.GetData()
			, ciphertext.Size()
			, iv.GetData()
			, iv.Size()
			, (uint8_t *)plaintext.GetData()
			, (uint8_t *)tag.GetData()
			, (uint8_t *)AAD.GetData()
			, AAD.Size()
		);
        if (len < 0) {
            LOG_ERROR("Decryption failed\n");
			return false;
		}
		return true;
	}

	int SecuritySuite_0::GetTagLength() const
    {
        return m_AES.GetTagLength();
    }
}