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
    serialize_pad16
    Pads the buffer to the nearest 4 bytes
    @param The buffer to pad
==============================*/

static void serialize_pad16(std::vector<uint8_t>* buff)
{
    int len = buff->size();
    if ((len%2) != 0)
        for (int i=0; i<(2-(len%2)); i++)
            buff->push_back('\0');
}


/*==============================
    serialize_pad32
    Pads the buffer to the nearest 4 bytes
    @param The buffer to pad
==============================*/

static void serialize_pad32(std::vector<uint8_t>* buff)
{
    int len = buff->size();
    if ((len%4) != 0)
        for (int i=0; i<(4-(len%4)); i++)
            buff->push_back('\0');
}


/*==============================
    serialize_pad64
    Pads the buffer to the nearest 8 bytes
    @param The buffer to pad
==============================*/

static void serialize_pad64(std::vector<uint8_t>* buff)
{
    int len = buff->size();
    if ((len%8) != 0)
        for (int i=0; i<(8-(len%8)); i++)
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
    serialize_pad32(buff);
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
    serialize_u16
    Adds a 2 byte value to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

void serialize_u16(std::vector<uint8_t>* buff, uint16_t val)
{
    unsigned char temp[2];
    memcpy(temp, &val, 2);

    // Ensure 16-bit alignment
    serialize_pad16(buff);
    
    // Write the int
    #if (IS_BIG_ENDIAN)
        for (int i=2; i>=0; i--)
            buff->push_back(temp[i]);
    #else
        for (int i=0; i<2; i++)
            buff->push_back(temp[i]);
    #endif
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
    serialize_pad32(buff);
    
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
    serialize_s32
    Adds a signed 4 byte value to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

void serialize_s32(std::vector<uint8_t>* buff, int32_t val)
{
    unsigned char temp[4];
    memcpy(temp, &val, 4);

    // Ensure 32-bit alignment
    serialize_pad32(buff);
    
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
    serialize_u64
    Adds an 8 byte value to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

void serialize_u64(std::vector<uint8_t>* buff, uint64_t val)
{
    unsigned char temp[8];
    memcpy(temp, &val, 8);

    // Ensure 64-bit alignment
    serialize_pad64(buff);
    
    // Write the int
    #if (IS_BIG_ENDIAN)
        for (int i=8; i>=0; i--)
            buff->push_back(temp[i]);
    #else
        for (int i=0; i<8; i++)
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
    serialize_pad32(buff);
    
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
    serialize_f64
    Adds a double to the buffer
    @param The buffer to add to
    @param The value to add
==============================*/

void serialize_f64(std::vector<uint8_t>* buff, double val)
{
    unsigned char temp[8];
    memcpy(temp, &val, 8);

    // Ensure 64-bit alignment
    serialize_pad64(buff);
    
    // Write the float
    #if (IS_BIG_ENDIAN)
        for (int i=8; i>=0; i--)
            buff->push_back(temp[i]);
    #else
        for (int i=0; i<8; i++)
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
}


/*==============================
    serialize_stdvector
    Adds a std::vector to the buffer
    @param The buffer to add to
    @param The std::vector to insert
==============================*/

void serialize_stdvector(std::vector<uint8_t>* buff, std::vector<uint8_t>* data)
{
    uint32_t size = data->size();

    // Write the buffer length and then the data itself
    serialize_u32(buff, size);
    for (uint32_t i=0; i<size; i++)
        buff->push_back((*data)[i]);
}


/*==============================
    deserialize_header
    Deserializes a header from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A buffer to store the header data into
    @param  A pointer to a integer to store the version into
==============================*/

void deserialize_header(uint8_t** buff, char* header, uint32_t* version)
{
    int len = strlen((char*)(*buff))+1;
    memcpy(header, (*buff), len);
    (*buff) += len; 
    deserialize_u32(buff, version);
}


/*==============================
    deserialize_u8
    Deserializes a byte value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a byte to store the value into
==============================*/

void deserialize_u8(uint8_t** buff, uint8_t* val)
{
    memcpy(val, (*buff), sizeof(uint8_t));
    (*buff) += sizeof(uint8_t);
}


/*==============================
    deserialize_u16
    Deserializes a 2 byte value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a word to store the value into
==============================*/

void deserialize_u16(uint8_t** buff, uint16_t* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%2 != 0)
    {
        aligned += 2 - (aligned%2);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(uint16_t));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>8)&0x00FF) | ((*val<<8)&0xFF00);
    #endif
    (*buff) += sizeof(uint16_t);
}


/*==============================
    deserialize_u32
    Deserializes a 4 byte value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a dword to store the value into
==============================*/

void deserialize_u32(uint8_t** buff, uint32_t* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%4 != 0)
    {
        aligned += 4 - (aligned%4);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(uint32_t));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>24)&0x000000FF) |
               ((*val>>8 )&0x0000FF00) | 
               ((*val<<8 )&0x00FF0000) | 
               ((*val<<24)&0xFF000000);
    #endif
    (*buff) += sizeof(uint32_t);
}


/*==============================
    deserialize_s32
    Deserializes a signed 4 byte value from a buffer and
    increments the buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a dword to store the value into
==============================*/

void deserialize_s32(uint8_t** buff, int32_t* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%4 != 0)
    {
        aligned += 4 - (aligned%4);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(int32_t));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>24)&0x000000FF) |
               ((*val>>8 )&0x0000FF00) | 
               ((*val<<8 )&0x00FF0000) | 
               ((*val<<24)&0xFF000000);
    #endif
    (*buff) += sizeof(int32_t);
}


/*==============================
    deserialize_u64
    Deserializes an 8 byte value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a qword to store the value into
==============================*/

void deserialize_u64(uint8_t** buff, uint64_t* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%8 != 0)
    {
        aligned += 8 - (aligned%8);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(uint64_t));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>56)&0x00000000000000FF) | 
               ((*val>>40)&0x000000000000FF00) | 
               ((*val>>24)&0x0000000000FF0000) | 
               ((*val>>8 )&0x00000000FF000000) | 
               ((*val<<8 )&0x000000FF00000000) | 
               ((*val<<24)&0x0000FF0000000000) | 
               ((*val<<40)&0x00FF000000000000) | 
               ((*val<<56)&0xFF00000000000000) | 
    #endif
    (*buff) += sizeof(uint64_t);
}


/*==============================
    deserialize_f32
    Deserializes a float value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a float to store the value into
==============================*/

void deserialize_f32(uint8_t** buff, float* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%4 != 0)
    {
        aligned += 4 - (aligned%4);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(float));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>24)&0x000000FF) |
               ((*val>>8 )&0x0000FF00) | 
               ((*val<<8 )&0x00FF0000) | 
               ((*val<<24)&0xFF000000);
    #endif
    (*buff) += sizeof(float);
}


/*==============================
    deserialize_f64
    Deserializes a double value from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a double to store the value into
==============================*/

void deserialize_f64(uint8_t** buff, double* val)
{
    intptr_t aligned = (intptr_t)(*buff);
    if (aligned%8 != 0)
    {
        aligned += 8 - (aligned%8);
        (*buff) = (uint8_t*)aligned;
    }
    memcpy(val, (*buff), sizeof(double));
    #if (IS_BIG_ENDIAN)
        *val = ((*val>>56)&0x00000000000000FF) | 
               ((*val>>40)&0x000000000000FF00) | 
               ((*val>>24)&0x0000000000FF0000) | 
               ((*val>>8 )&0x00000000FF000000) | 
               ((*val<<8 )&0x000000FF00000000) | 
               ((*val<<24)&0x0000FF0000000000) | 
               ((*val<<40)&0x00FF000000000000) | 
               ((*val<<56)&0xFF00000000000000) | 
    #endif
    (*buff) += sizeof(double);
}


/*==============================
    deserialize_wxstring
    Deserializes a wxString from a buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to a wxString to store the value into
==============================*/

void deserialize_wxstring(uint8_t** buff, wxString* str)
{
    wxString temp;
    uint32_t len;
    deserialize_u32(buff, &len);
    *str = wxString::FromUTF8((char*)(*buff));
    (*buff) += len;
}


/*==============================
    deserialize_buffer
    Deserializes a buffer from another buffer and increments the
    buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to the buffer to store the data into
    @param  The size of the destination buffer
==============================*/

void deserialize_buffer(uint8_t** buff, uint8_t* data, uint32_t size)
{
    memcpy(data, (*buff), size);
    (*buff) += size;
}


/*==============================
    deserialize_stdvector
    Deserializes an std::vector from a buffer and increments
    the buffer pointer to the next position
    @param  A pointer to the buffer to deserialize from
    @param  A pointer to the std::vector to store the data 
            into
==============================*/

void deserialize_stdvector(uint8_t** buff, std::vector<uint8_t>* data)
{
    uint32_t count;
    deserialize_u32(buff, &count);
    if (count > 0)
    {
        for (uint32_t i=0; i<count; i++)
            data->push_back((*buff)[i]);
        (*buff) += count;
    }
}