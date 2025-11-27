#include <vector>
#include <stdint.h>
#include <wx/string.h>


/*=============================================================
                      Function Prototypes
=============================================================*/

void serialize_header(std::vector<uint8_t>* buff, const char* header, uint32_t version);
void serialize_u8(std::vector<uint8_t>* buff, uint8_t val);
void serialize_u16(std::vector<uint8_t>* buff, uint16_t val);
void serialize_u32(std::vector<uint8_t>* buff, uint32_t val);
void serialize_s32(std::vector<uint8_t>* buff, int32_t val);
void serialize_u64(std::vector<uint8_t>* buff, uint64_t val);
void serialize_f32(std::vector<uint8_t>* buff, float val);
void serialize_f64(std::vector<uint8_t>* buff, double val);
void serialize_wxstring(std::vector<uint8_t>* buff, wxString str);
void serialize_buffer(std::vector<uint8_t>* buff, uint8_t* data, uint32_t size);
void serialize_stdvector(std::vector<uint8_t>* buff, std::vector<uint8_t>* data);

void deserialize_header(uint8_t** buff, char* header, uint32_t* version);
void deserialize_u8(uint8_t** buff, uint8_t* val);
void deserialize_u16(uint8_t** buff, uint16_t* val);
void deserialize_u32(uint8_t** buff, uint32_t* val);
void deserialize_s32(uint8_t** buff, int32_t* val);
void deserialize_u64(uint8_t** buff, uint64_t* val);
void deserialize_f32(uint8_t** buff, float* val);
void deserialize_f64(uint8_t** buff, double* val);
void deserialize_wxstring(uint8_t** buff, wxString* str);
void deserialize_buffer(uint8_t** buff, uint8_t* data, uint32_t size);
void deserialize_stdvector(uint8_t** buff, std::vector<uint8_t>* data);