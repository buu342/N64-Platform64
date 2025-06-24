#include <vector>
#include <stdint.h>
#include <wx/string.h>

void serialize_header(std::vector<uint8_t>* buff, const char* header, uint32_t version);
void serialize_u8(std::vector<uint8_t>* buff, uint8_t val);
void serialize_u32(std::vector<uint8_t>* buff, uint32_t val);
void serialize_f32(std::vector<uint8_t>* buff, float val);
void serialize_wxstring(std::vector<uint8_t>* buff, wxString str);

uint32_t deserialize_header(uint8_t* buff, uint32_t pos, char* header, uint32_t* version);
uint32_t deserialize_u8(uint8_t* buff, uint32_t pos, uint8_t* val);
uint32_t deserialize_u32(uint8_t* buff, uint32_t pos, uint32_t* val);
uint32_t deserialize_f32(uint8_t* buff, uint32_t pos, float* val);
uint32_t deserialize_wxstring(uint8_t* buff, uint32_t pos, wxString* str);