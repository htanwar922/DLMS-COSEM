
#include "APDU/APDU.h"

namespace EPRI {
    namespace GLO {
        ASN_BEGIN_SCHEMA(Ciphered_APDU_Schema)
            ASN_BASE_TYPE(ASN::DT_Length)           // Length of the Encrypted Data and Security Control Byte and Invocation Counter
            ASN_BASE_TYPE(ASN::DT_Unsigned8)        // Security Control Byte
            ASN_BASE_TYPE(ASN::DT_Unsigned32)       // Invocation Counter
            ASN_DATA_TYPE                           // Ciphered Data and Authentication Tag
        ASN_END_SCHEMA

        ASN_BEGIN_SCHEMA(General_Glo_Ciphering::General_Glo_Ciphering_Schema)
            ASN_OCTET_STRING_TYPE(ASN::CONSTRUCTED)     // System Title
            ASN_BASE_TYPE(ASN::DT_Length)               // Length of the Encrypted Data and Security Control Byte and Invocation Counter
            ASN_BASE_TYPE(ASN::DT_Unsigned8)            // Security Control Byte
            ASN_BASE_TYPE(ASN::DT_Unsigned32)           // Invocation Counter
            ASN_DATA_TYPE                               // Encrypted Data
        ASN_END_SCHEMA

        //
        // General_Glo_Ciphering
        //
        General_Glo_Ciphering::General_Glo_Ciphering()
            : APDUSingleType(General_Glo_Ciphering::General_Glo_Ciphering_Schema)
        {
        }
        General_Glo_Ciphering::~General_Glo_Ciphering()
        {
        }

        bool General_Glo_Ciphering::IsValid() const
        {
            return true;
        }

        bool General_Glo_Ciphering::Parse(DLMSVector* pData,
            COSEMAddressType SourceAddress,
            COSEMAddressType DestinationAddress)
        {
            // Perform the base parse, which just loads
            // the stream.
            //
            if (APDUSingleType::Parse(pData, SourceAddress, DestinationAddress))
            {
                if (Deserialize())
                {
                    return (m_Initialized = true);
                }
            }
            return false;
        }

        std::vector<uint8_t> General_Glo_Ciphering::GetBytes()
        {
            if (Serialize())
            {
                return APDUSingleType::GetBytes();
            }
            return std::vector<uint8_t>();
        }

        bool General_Glo_Ciphering::Deserialize()
        {
            DLMSValue    Value;
            ASNType::GetNextResult Result;

            m_Type.Rewind();
            Result = m_Type.GetNextValue(&Value);
            if (Result != ASNType::GetNextResult::VALUE_RETRIEVED)
                return false;
            m_SystemTitle = DLMSValueGet<DLMSVector>(Value);
            Result = m_Type.GetNextValue(&Value);
            if (Result != ASNType::GetNextResult::VALUE_RETRIEVED)
                return false;
            size_t Length = DLMSValueGet<size_t>(Value);

            DLMSVector Data;
            size_t BytesToSkip = m_SystemTitle.Size() + ASNType::CalculateLengthBytes(m_SystemTitle.Size());
            size_t LengthBytes = ASNType::CalculateLengthBytes(Length);
            Data.Clear();
            Data.Append<uint8_t>(Tag);
            Data.Append(m_Type.GetBytes(), BytesToSkip, Length + LengthBytes);

            return m_Ciphered.Parse(&Data, m_SourceAddress, m_DestinationAddress);
        }

        bool General_Glo_Ciphering::Serialize()
        {
            if (not m_Initialized)
                return false;
            m_Type.Clear();
            m_Type.Append(m_SystemTitle);
            m_Type.Append(m_Ciphered.GetCipheredDataAndAuthenticationTag().Size() + sizeof(uint8_t) + sizeof(uint32_t));
            m_Type.Append(m_Ciphered.GetSecurityControlByte());
            m_Type.Append(m_Ciphered.GetInvocationCounter());
            m_Type.Append(m_Ciphered.GetCipheredDataAndAuthenticationTag());
            return true;
        }

        DLMSVector General_Glo_Ciphering::Encrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions, const DLMSVector& Plaintext)
        {
            m_SystemTitle = METER_SYSTEM_TITLE;
            DLMSVector Ciphertext = m_Ciphered.Encrypt(pSuite, SecurityOptions, Plaintext);
            m_Initialized = true;
            return GetBytes();
        }

        DLMSVector General_Glo_Ciphering::Decrypt(const std::shared_ptr<ISecuritySuite> pSuite, const COSEMSecurityOptions& SecurityOptions) const
        {
            return m_Ciphered.Decrypt(pSuite, SecurityOptions);
        }

    }
}