// Himanshu

#pragma once

#include <random>

#include "DLMSValue.h"
#include "COSEM/COSEMObjectInstanceID.h"

namespace LibOpenSSL {

	class AES
	{
	protected:
		const char* ciphername = NULL;
		const char algo[4] = "";
		const int bitsize = 0;
		const char mode[4] = "";

        uint8_t* key = NULL;
		int key_len = 0, tag_len = 0;
	public:
        AES(const char* ciphername, const uint8_t* key, int key_len, int tag_len = 0);

        int Encrypt(const uint8_t* plaintext, int len, const uint8_t* iv, int iv_len, uint8_t* ciphertext, uint8_t* tag = NULL, uint8_t* aad = NULL, int aad_len = 0) const;
		int Decrypt(const uint8_t* ciphertext, int len, const uint8_t* iv, int iv_len, uint8_t* plaintext, uint8_t* tag = NULL, uint8_t* aad = NULL, int aad_len = 0) const;
		void PrintCiphertext(const uint8_t* ciphertext, int len);

		EPRI::DLMSVector GetKey() const;
		void SetKey(const uint8_t* key, int key_len);
        int GetTagLength() const;

		virtual ~AES();
	};

	class AES_128_GCM : public AES
	{
	public:
		AES_128_GCM(const uint8_t* key)
            : AES("AES-128-GCM", key, 16, 12)
        {
        }
	};
}


namespace EPRI
{
    class ISecuritySuite
    {
    public:
        enum SecurityControlBitMask : uint8_t {
            security_suit_id    = 0x0f,
            authentication      = 0x10,
            encryption          = 0x20,
            compression         = 0x40,
        };

        virtual ~ISecuritySuite() = default;

        virtual bool Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const = 0;
        virtual bool Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const = 0;
        virtual bool GenerateGMAC(const DLMSVector& iv, const DLMSVector& CtoS, DLMSVector& tag) const = 0;
        virtual int GetTagLength() const = 0;

        const COSEMObjectInstanceID& GetSecuritySetupObjectID() const
        {
            return m_SecuritySetupObjectID;
        }
        void SetSecuritySetupObjectID(const COSEMObjectInstanceID& SecuritySetupObjectID)
        {
            m_SecuritySetupObjectID = SecuritySetupObjectID;
        }
        uint8_t GetSecurityControlByte() const
        {
            return m_SecurityControlByte;
        }
        void ClearSecurityControlByte()
        {
            m_SecurityControlByte = 0;
        }
        void SetSecurityControlBits(SecurityControlBitMask BitMask)
        {
            m_SecurityControlByte |= BitMask;
        }
        uint32_t GetRandom()
        {
            return (std::mt19937(m_RandomDevice()))();
        }
    protected:
        COSEMObjectInstanceID m_SecuritySetupObjectID;
        uint8_t m_SecurityControlByte;
        std::random_device m_RandomDevice;
    };

    class SecuritySuite_None : public ISecuritySuite
    {
    public:
        SecuritySuite_None() = default;
        virtual ~SecuritySuite_None() = default;

        bool Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const
        {
            ciphertext = plaintext;
            return true;
        }
        bool Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const
        {
            plaintext = ciphertext;
            return true;
        }
        bool GenerateGMAC(const DLMSVector& iv, const DLMSVector& CtoS, DLMSVector& tag) const
        {
            return false;
        }
        int GetTagLength() const
        {
            return 0;
        }
    };

    class SecuritySuite_0 : public ISecuritySuite
    {
    public:
        SecuritySuite_0(const uint8_t* key, const uint8_t* aad_wo_placeholder);
        virtual ~SecuritySuite_0();

        DLMSVector GetKey() const;
        void SetKey(const DLMSVector& key);
        DLMSVector GetAAD() const;
        void SetAAD(const DLMSVector& aad);

        bool Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const;
        bool Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const;
        bool GenerateGMAC(const DLMSVector& iv, const DLMSVector& CtoS, DLMSVector& tag) const;
        int GetTagLength() const;

    protected:
        LibOpenSSL::AES_128_GCM m_AES;
        DLMSVector m_AAD;
    };

    class SecuritySuite_1 : public ISecuritySuite
    {
    public:
        SecuritySuite_1() = default;
        virtual ~SecuritySuite_1() = default;

        bool Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const
        {
            return false;
        }
        bool Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const
        {
            return false;
        }
        bool GenerateGMAC(const DLMSVector& iv, const DLMSVector& CtoS, DLMSVector& tag) const
        {
            return false;
        }
        int GetTagLength() const
        {
            return 0;
        }
    };

    class SecuritySuite_2 : public ISecuritySuite
    {
    public:
        SecuritySuite_2() = default;
        virtual ~SecuritySuite_2() = default;

        bool Encrypt(const DLMSVector& plaintext, const DLMSVector& iv, DLMSVector& ciphertext, DLMSVector& tag) const
        {
            return false;
        }
        bool Decrypt(const DLMSVector& ciphertext, const DLMSVector& iv, DLMSVector& plaintext, const DLMSVector& tag) const
        {
            return false;
        }
        bool GenerateGMAC(const DLMSVector& iv, const DLMSVector& CtoS, DLMSVector& tag) const
        {
            return false;
        }
        int GetTagLength() const
        {
            return 0;
        }
    };
}