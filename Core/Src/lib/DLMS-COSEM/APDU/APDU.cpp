
#include "APDU/APDU.h"

namespace EPRI {
    namespace GLO {
        ASN_BEGIN_SCHEMA(Ciphered_APDU_Schema)
            ASN_BASE_TYPE(ASN::DT_Unsigned8)        // Security Control Byte
            ASN_BASE_TYPE(ASN::DT_Unsigned32)       // Invocation Counter
            ASN_BASE_TYPE(ASN::DT_Data)             // Ciphered Data and Authentication Tag
        ASN_END_SCHEMA
    }
}