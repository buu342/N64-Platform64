/***************************************************************
                         serializer.cpp

A collection of helper functions to serialize/deseralize P64 
assets to/from binary files.
***************************************************************/

#include "serializer.h"


/*=============================================================
                            Macros
=============================================================*/

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
        defined(__BIG_ENDIAN__) || \
        defined(__ARMEB__) || \
        defined(__THUMBEB__) || \
        defined(__AARCH64EB__) || \
        defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
    #define IS_BIG_ENDIAN
#endif


/*=============================================================
                           Functions
=============================================================*/

/*==============================
    serialize_pad
    Pads the buffer to the nearest 4 bytes
    @param The buffer to pad
==============================*/

static void serialize_pad(std::vector<uint8_t>* buff)
{
    int len = buff->size();
    if ((len%4) != 0)
        for (int i=0; i<(4-(len%4)); i++)
            buff->push_back('\0');
}


/*==============================
    serialize_header
    Adds a file header string to the buffer
    @param The buffer to add to
    @param The header string
    @param The version of this file
==============================*/

void serialize_header(std::vector<uint8_t>* buff, const char* header, uint32_t version)
{
    int strb_len = strlen(header)+1;
    for (int i=0; i<strb_len; i++)
        buff->push_back(header[i]);
    serialize_pad(buff);
    serialize_u32(buff, version);
}


/*==============================
    serialize_u8
    Adds a byte to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

void serialize_u8(std::vector<uint8_t>* buff, uint8_t val)
{
    buff->push_back(val);
}


/*==============================
    serialize_u32
    Adds a 4 byte value to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

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


/*==============================
    serialize_f32
    Adds a float to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

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


/*==============================
    serialize_wxstring
    Adds a wxString to the buffer
    @param The buffer to add to
    @param The string to add
==============================*/

void serialize_wxstring(std::vector<uint8_t>* buff, wxString str)
{
    const char* strb = str.mb_str(wxConvUTF8);
    int len = strlen(strb)+1; // TODO: NULL check might not work for UTF-8

    // Write the string length and then the data itself
    serialize_u32(buff, len);
    for (int i=0; i<len; i++)
        buff->push_back(strb[i]);
}


/*==============================
    serialize_buffer
    Adds a buffer to the buffer
    @param The buffer to add to
    @param The buffer to insert
    @param The size of the buffer to insert
==============================*/

void serialize_buffer(std::vector<uint8_t>* buff, uint8_t* data, uint32_t size)
{
    for (uint32_t i=0; i<size; i++)
        buff->push_back(data[i]);
    serialize_pad(buff);
}


/*==============================
    deserialize_header
    Deseralizes a header from a buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A buffer to store the header data into
    @param  A pointer to a integer to store the version into
    @return The new position in the buffer
==============================*/

uint32_t deserialize_header(uint8_t* buff, uint32_t pos, char* header, uint32_t* version)
{
    int len = strlen((char*)buff)+1;
    memcpy(header, buff+pos, len);
    pos += len;
    pos = deserialize_u32(buff, pos, version);
    return pos;
}


/*==============================
    deserialize_u8
    Deseralizes a byte value from a buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A pointer to a byte to store the value into
    @return The new position in the buffer
==============================*/

uint32_t deserialize_u8(uint8_t* buff, uint32_t pos, uint8_t* val)
{
    memcpy(val, buff+pos, sizeof(uint8_t));
    pos += sizeof(uint8_t);
    return pos;
}


/*==============================
    deserialize_u32
    Deseralizes a 4 byte value from a buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A pointer to a 4 byte to store the value into
    @return The new position in the buffer
==============================*/

uint32_t deserialize_u32(uint8_t* buff, uint32_t pos, uint32_t* val)
{
    if (pos%4 != 0)
        pos += 4 - (pos%4);
    memcpy(val, buff+pos, sizeof(uint32_t));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>24)&0xFF) | ((*val<<8)&0xFF0000) | ((*val>>8)&0xFF00) | ((*val<<24)&0xFF000000);
    #endif
    pos += sizeof(uint32_t);
    return pos;
}


/*==============================
    deserialize_f32
    Deseralizes a float value from a buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A pointer to a byte to store the float into
    @return The new position in the buffer
==============================*/

uint32_t deserialize_f32(uint8_t* buff, uint32_t pos, float* val)
{
    if (pos%4 != 0)
        pos += 4 - (pos%4);
    memcpy(val, buff+pos, sizeof(float));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>24)&0xFF) | ((*val<<8)&0xFF0000) | ((*val>>8)&0xFF00) | ((*val<<24)&0xFF000000);
    #endif
    pos += sizeof(float);
    return pos;
}


/*==============================
    deserialize_wxstring
    Deseralizes a wxString from a buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A pointer to the wxString to store the value into
    @return The new position in the buffer
==============================*/

uint32_t deserialize_wxstring(uint8_t* buff, uint32_t pos, wxString* str)
{
    wxString temp;
    uint32_t len;
    pos = deserialize_u32(buff, pos, &len);
    *str = wxString::FromUTF8((char*)buff+pos);
    pos += len;
    return pos; 
}


/*==============================
    deserialize_buffer
    Deseralizes a buffer from another buffer
    @param  The buffer to deserialize from
    @param  The current position in the buffer
    @param  A pointer to the buffer to store the value into
    @param  The size of the destination buffer
    @return The new position in the buffer
==============================*/

uint32_t deserialize_buffer(uint8_t* buff, uint32_t pos, uint8_t* data, uint32_t size)
{
    memcpy(data, buff + pos, size);
    pos += size;
    if (pos % 4 != 0)
        pos += 4 - (pos % 4);
    return pos;
}