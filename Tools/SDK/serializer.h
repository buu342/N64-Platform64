#include <vector>
#include <stdint.h>
#include <wx/string.h>

void serialize_header(std::vector<uint8_t>* buff, const char* header, uint32_t version);
void serialize_u8(std::vector<uint8_t>* buff, uint8_t val);
void serialize_u32(std::vector<uint8_t>* buff, uint32_t val);
void serialize_f32(std::vector<uint8_t>* buff, float val);
void serialize_string(std::vector<uint8_t>* buff, const char* str);
void serialize_wxstring(std::vector<uint8_t>* buff, wxString str);