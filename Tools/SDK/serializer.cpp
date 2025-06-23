#include "serializer.h"

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
        defined(__BIG_ENDIAN__) || \
        defined(__ARMEB__) || \
        defined(__THUMBEB__) || \
        defined(__AARCH64EB__) || \
        defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
    #define IS_BIG_ENDIAN
#endif

static void serialize_pad(std::vector<uint8_t>* buff)
{
    int len = buff->size();
    if ((len%4) != 0)
        for (int i=0; i<(4-(len%4)); i++)
            buff->push_back('\0');
}

void serialize_header(std::vector<uint8_t>* buff, const char* header, uint32_t version)
{
    int strb_len = strlen(header)+1;
    for (int i=0; i<strb_len; i++)
        buff->push_back(header[i]);
    serialize_pad(buff);
    serialize_u32(buff, version);
}

void serialize_u8(std::vector<uint8_t>* buff, uint8_t val)
{
    buff->push_back(val);
}

void serialize_u32(std::vector<uint8_t>* buff, uint32_t val)
{
    unsigned char temp[4];
    memcpy(temp, &val, 4);

    // Ensure 32-bit alignment
    serialize_pad(buff);
    
    // Write the int
    #if (IS_BIG_ENDIAN)
        for (int i=4; i>=0; i--)
            buff->push_back(temp[i]);
    #else
        for (int i=0; i<4; i++)
            buff->push_back(temp[i]);
    #endif
}

void serialize_f32(std::vector<uint8_t>* buff, float val)
{
    unsigned char temp[4];
    memcpy(temp, &val, 4);

    // Ensure 32-bit alignment
    serialize_pad(buff);
    
    // Write the float
    #if (IS_BIG_ENDIAN)
        for (int i=4; i>=0; i--)
            buff->push_back(temp[i]);
    #else
        for (int i=0; i<4; i++)
            buff->push_back(temp[i]);
    #endif
}

void serialize_string(std::vector<uint8_t>* buff, const char* str)
{
    int strb_len = strlen(str)+1;

    // Write the string length and then the data itself
    serialize_u32(buff, strb_len);
    for (int i=0; i<strb_len; i++)
        buff->push_back(str[i]);

    // Zero pad to the nearest 4 bytes
    if ((strb_len%4) != 0)
        for (int i=0; i<(4-(strb_len%4)); i++)
            buff->push_back('\0');
}

void serialize_wxstring(std::vector<uint8_t>* buff, wxString str)
{
    const char* strb = str.mb_str(wxConvUTF8);
    int strb_len = strlen(strb)+1;

    // Write the string length and then the data itself
    serialize_u32(buff, strb_len);
    for (int i=0; i<strb_len; i++)
        buff->push_back(strb[i]);

    // Zero pad to the nearest 4 bytes
    if ((strb_len%4) != 0)
        for (int i=0; i<(4-(strb_len%4)); i++)
            buff->push_back('\0');
}