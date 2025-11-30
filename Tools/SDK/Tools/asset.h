#pragma once

typedef struct IUnknown IUnknown;

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <vector>
#include <utility>
#include <stdint.h>
#include "asset_thumbnail.h"


/*=============================================================
                             Types
=============================================================*/

typedef enum {
    DF_U8,
    DF_U16,
    DF_U32,
    DF_U64,
    DF_F32,
    DF_F64,
    DF_BUFFER,
    DF_BUFFERSIZE,
    DF_STDVECTOR,
    DF_WXSTRING,
    DF_RGB,
    DF_RGBA,
    DF_FILENAME,
    DF_XY,
    DF_THUMBNAIL,
} DataType;

typedef struct {
    const char* header;
    uint32_t version;
} FileHeader;


/*=============================================================
                            Classes
=============================================================*/

class P64Asset
{
    private:
        FileHeader m_Header;

    protected:
        P64AssetThumbnail m_Thumbnail;
        std::vector<std::pair<DataType, void*>> m_Format;

        void SetUpFileFormat(FileHeader header, std::vector<std::pair<DataType, void*>> fmt);
        void GenerateThumbnail();

    public:
        bool ValidSmallThumbnail();
        bool ValidLargeThumbnail();
        wxIcon GetLargeThumbnail();
        wxIcon GetSmallThumbnail();
        bool IsOk();

        std::vector<uint8_t> Serialize();
        bool Deserialize(std::vector<uint8_t> bytes);

        static bool FileInAssetsFolder(wxFileName file, wxFileName basepath);
        static wxFileName CopyFileToAssetPath(wxFileName file, wxFileName assetdir);
        static wxFileName GetRelativeAssetPath(wxFileName file, wxFileName assetdir);
        static wxFileName GetFullAssetPath(wxFileName file, wxFileName assetdir);

        void operator=(const P64Asset& rhs);

        P64Asset();
        ~P64Asset();
};