/***************************************************************
                           asset.cpp

This is the base class for all editor assets.
***************************************************************/

#include "asset.h"
#include "../serializer.h"


/*==============================
    P64Asset (Constructor)
    Initializes the class
==============================*/

P64Asset::P64Asset()
{

}


/*==============================
    P64Asset (Destructor)
    Cleans up the class before deletion
==============================*/

P64Asset::~P64Asset()
{

}


/*==============================
    P64Asset::operator=
    Assigns this object to another.
==============================*/

void P64Asset::operator=(const P64Asset& rhs)
{
    this->m_Header = rhs.m_Header;
    this->m_Thumbnail = rhs.m_Thumbnail;
    // Do not mess with m_Format as it is specific to each object's addresses
}


/*==============================
    P64Asset::SetUpFileFormat
    Sets up a file format for serialization
    @param  The header format
    @param  The data format
==============================*/

void P64Asset::SetUpFileFormat(FileHeader header, std::vector<std::pair<DataType, void*>> fmt)
{
    this->m_Header = header;
    this->m_Format = fmt;
}


/*==============================
    P64Asset::FileInAssetsFolder
    Checks if a given file is inside the assets folder
    @param  The file to check
    @param  The assets folder path
    @return Whether the file is inside the assets folder
==============================*/

bool P64Asset::FileInAssetsFolder(wxFileName file, wxFileName basepath)
{
    wxString fullpath;
    file.Normalize(wxPATH_NORM_ABSOLUTE | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, basepath.GetPathWithSep());
    fullpath = file.GetFullPath();
    if (fullpath.Contains(basepath.GetFullPath()))
        return wxFileExists(fullpath);
    return false;
}


/*==============================
    P64Asset::CopyFileToAssetPath
    Copies a file to be inside an asset's folder, and then
    returns the new filename
    @param  The file to check
    @param  The asset's folder path
    @param  (optional) The window to attach dialogs to.
            Default is NULL.
    @return The new filename, or an invalid filename if an
            error ocurrs
==============================*/

wxFileName P64Asset::CopyFileToAssetPath(wxFileName file, wxFileName assetdir, wxWindow* win)
{
    wxMessageDialog dialog(win, "The file you have selected is not inside the asset folder, so it will be copied into it.\nContinue?", "File outside assets folder", wxCENTER | wxYES_NO | wxYES_DEFAULT | wxICON_WARNING);
    if (dialog.ShowModal() == wxID_YES)
    {
        wxString newpath = assetdir.GetPathWithSep() + file.GetFullName();
        if (wxFileExists(newpath))
        {
            wxMessageDialog dialog(win, wxString::Format("The file '%s' already exists in the asset folder.\nHow to proceed?", (const char*)file.GetFullName().c_str()), "Duplicate file", wxCENTER | wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_WARNING);
            dialog.SetYesNoLabels("Overwrite it", "Rename it");
            switch (dialog.ShowModal())
            {
                case wxID_YES: 
                    wxCopyFile(file.GetFullPath(), newpath, true); 
                    return newpath;
                case wxID_NO:
                {
                    int newnum = 2;
                    wxString newnewpath;
                    do
                    {
                        newnewpath = assetdir.GetPathWithSep() + file.GetName() + wxString::Format(" (%d).", newnum) + file.GetExt();
                        newnum++;
                    }
                    while (wxFileExists(newnewpath));
                    wxCopyFile(file.GetFullPath(), newnewpath); 
                    return newnewpath;
                }
                default: 
                    return wxFileName("");
            }
        }
        else
        {
            wxCopyFile(file.GetFullPath(), newpath);
            return newpath;
        }
    }
    else
        return wxFileName("");
}


/*==============================
    P64Asset::CopyFileToAssetPath
    Returns a file's path relative to an asset's folder
    @param  The file who's path we want to relativize
    @param  The asset's folder path
    @return The relative filename, or an invalid filename if an
            error ocurrs
==============================*/

wxFileName P64Asset::GetRelativeAssetPath(wxFileName file, wxFileName assetdir)
{
    if (file.MakeRelativeTo(assetdir.GetPathWithSep()))
        return file;
    return wxFileName("");
}


/*==============================
    P64Asset::CopyFileToAssetPath
    Returns a file's path absolute path from a relative one to
    an asset's folder
    @param  The file who's path we want to absolutize
    @param  The asset's folder path
    @return The absolute filename, or an invalid filename if an
            error ocurrs
==============================*/

wxFileName P64Asset::GetFullAssetPath(wxFileName file, wxFileName assetdir)
{
    if (file.Normalize(wxPATH_NORM_ABSOLUTE | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, assetdir.GetPathWithSep()))
        return file;
    return wxFileName("");
}


/*==============================
    P64Asset::Serialize
    Serializes the object
    @return The seralized data
==============================*/

std::vector<uint8_t> P64Asset::Serialize()
{
    uint32_t buffersize;
    std::vector<uint8_t> data;

    serialize_header(&data, this->m_Header.header, this->m_Header.version);
    for (std::pair<DataType, void*>& dat : this->m_Format)
    {
        switch (dat.first)
        {
            case DF_U8: serialize_u8(&data, *((uint8_t*)(dat.second))); break;
            case DF_U16: serialize_u16(&data, *((uint16_t*)(dat.second))); break;
            case DF_U32: serialize_u32(&data, *((uint32_t*)(dat.second))); break;
            case DF_U64: serialize_u64(&data, *((uint64_t*)(dat.second))); break;
            case DF_F32: serialize_f32(&data, *((float*)(dat.second))); break;
            case DF_F64: serialize_f64(&data, *((double*)(dat.second))); break;
            case DF_BUFFERSIZE: 
                serialize_u32(&data, *((uint32_t*)(dat.second)));
                buffersize = *((uint32_t*)(dat.second));
                break;
            case DF_BUFFER: 
            {
                if (buffersize != 0)
                    serialize_buffer(&data, *((uint8_t**)dat.second), buffersize); break;
                break;
            }
            case DF_STDVECTOR: serialize_stdvector(&data, ((std::vector<uint8_t>*)(dat.second))); break;
            case DF_WXSTRING: serialize_wxstring(&data, *((wxString*)(dat.second))); break;
            case DF_FILENAME: serialize_wxstring(&data, ((wxFileName*)(dat.second))->GetFullPath()); break;
            case DF_RGB:
                serialize_u8(&data, ((wxColor*)(dat.second))->Red());
                serialize_u8(&data, ((wxColor*)(dat.second))->Green());
                serialize_u8(&data, ((wxColor*)(dat.second))->Blue());
                break;
            case DF_RGBA:
                serialize_u8(&data, ((wxColor*)(dat.second))->Red());
                serialize_u8(&data, ((wxColor*)(dat.second))->Green());
                serialize_u8(&data, ((wxColor*)(dat.second))->Blue());
                serialize_u8(&data, ((wxColor*)(dat.second))->Alpha());
                break;
            case DF_XY:
                serialize_s32(&data, ((wxPoint*)(dat.second))->x);
                serialize_s32(&data, ((wxPoint*)(dat.second))->y);
                break;
            case DF_THUMBNAIL:
            {
                std::vector<uint8_t> tdata = ((P64AssetThumbnail*)dat.second)->Serialize();
                serialize_buffer(&data, &tdata[0], tdata.size()); break;
                break;
            }
        }
    }
    return data;
}


/*==============================
    P64Asset::Deserialize
    Deserializes the object
    @param  The serialized data
    @param  (optional) The window to attach dialogs to.
            Default is NULL.
    @return Whether the deserialization succeeded
==============================*/

bool P64Asset::Deserialize(std::vector<uint8_t> bytes, wxWindow* win)
{
    uint32_t buffersize;
    uint32_t version;
    char header[64];
    uint8_t* buff = &(bytes[0]);
    uint32_t size = bytes.size();
    uint32_t left = size;

    // A valid asset needs to be at least 16 bytes
    if (bytes.size() < 16)
    {
        if (win != NULL)
        {
            wxMessageDialog dialog(win, "File is not an asset", "Error deserializing", wxCENTER | wxOK | wxOK_DEFAULT | wxICON_ERROR);
            dialog.ShowModal();
        }
        return false;
    }

    // Check the header
    deserialize_header(&buff, header, &version);
    if (strcmp(header, this->m_Header.header) != 0)
    {
        if (win != NULL)
        {
            wxMessageDialog dialog(win, "File is not of the correct type", "Error deserializing", wxCENTER | wxOK |  wxOK_DEFAULT | wxICON_ERROR);
            dialog.ShowModal();
        }
        return false;
    }
    if (version > this->m_Header.version)
    {
        if (win != NULL)
        {
            wxMessageDialog dialog(win, "File is a more recent, unsupported version", "Error deserializing", wxCENTER | wxOK |  wxOK_DEFAULT | wxICON_ERROR);
            dialog.ShowModal();
        }
        return false;
    }

    // Iterate over the data format
    for (std::pair<DataType, void*>& dat : this->m_Format)
    {
        int before = size - (((uint64_t)buff) - ((uint64_t)(&(bytes[0]))));
        switch (dat.first)
        {
            case DF_U8: if (left >= 1) deserialize_u8(&buff, ((uint8_t*)dat.second)); break;
            case DF_U16: if (left >= 2) deserialize_u16(&buff, ((uint16_t*)dat.second)); break;
            case DF_U32: if (left >= 4) deserialize_u32(&buff, ((uint32_t*)dat.second)); break;
            case DF_U64: if (left >= 8) deserialize_u64(&buff, ((uint64_t*)dat.second)); break;
            case DF_F32: if (left >= 4) deserialize_f32(&buff, ((float*)dat.second)); break;
            case DF_F64: if (left >= 8) deserialize_f64(&buff, ((double*)dat.second)); break;
            case DF_BUFFERSIZE:
                if (left >= 4)
                {
                    deserialize_u32(&buff, ((uint32_t*)dat.second)); 
                    buffersize = *((uint32_t*)dat.second); 
                }
                break;
            case DF_BUFFER: 
            {
                if (buffersize != 0 && left >= buffersize)
                {
                    if (*((uint8_t**)dat.second) != NULL)
                        free(*((uint8_t**)dat.second));
                    *((uint8_t**)dat.second) = (uint8_t*)malloc(buffersize);
                    deserialize_buffer(&buff, *((uint8_t**)dat.second), buffersize); 
                }
                else
                    *((uint8_t**)dat.second) = NULL;
                break;
            }
            case DF_STDVECTOR: deserialize_stdvector(&buff, ((std::vector<uint8_t>*)(dat.second))); break;
            case DF_WXSTRING: if (left >= 1) deserialize_wxstring(&buff, ((wxString*)(dat.second))); break;
            case DF_FILENAME:
                if (left >= 1)
                {
                    wxString temp;
                    deserialize_wxstring(&buff, &temp);
                    *((wxFileName*)(dat.second)) = wxFileName(temp);
                }
                break;
            case DF_RGB:
            {
                if (left >= 3)
                {
                    uint8_t r, g, b;
                    deserialize_u8(&buff, &r);
                    deserialize_u8(&buff, &g);
                    deserialize_u8(&buff, &b);
                    *((wxColor*)(dat.second)) = wxColor(r, g, b);
                }
                break;
            }
            case DF_RGBA:
            {
                if (left >= 4)
                {
                    uint8_t r, g, b, a;
                    deserialize_u8(&buff, &r);
                    deserialize_u8(&buff, &g);
                    deserialize_u8(&buff, &b);
                    deserialize_u8(&buff, &a);
                    *((wxColor*)(dat.second)) = wxColor(r, g, b, a);
                }
                break;
            }
            case DF_XY:
                if (left >= 8)
                {
                    deserialize_s32(&buff, &((wxPoint*)(dat.second))->x);
                    deserialize_s32(&buff, &((wxPoint*)(dat.second))->y);
                }
                break;
            case DF_THUMBNAIL: ((P64AssetThumbnail*)dat.second)->Deserialize(&buff); break;
        }
        left = size - (((uint64_t)buff) - ((uint64_t)(&(bytes[0]))));
    }
    return true;
}


/*==============================
    P64Asset::ValidSmallThumbnail
    Checks if we have a valid small thumbnail
    @return Whether a valid small thumbnail has been generated
==============================*/

bool P64Asset::ValidSmallThumbnail()
{
    return this->m_Thumbnail.IsValidSmall();
}


/*==============================
    P64Asset::ValidLargeThumbnail
    Checks if we have a valid large thumbnail
    @return Whether a valid large thumbnail has been generated
==============================*/

bool P64Asset::ValidLargeThumbnail()
{
    return this->m_Thumbnail.IsValidLarge();
}


/*==============================
    P64Asset::GetSmallThumbnail
    Gets the small thumbnail icon
    @return The small thumbnail, or wxNullIcon
==============================*/

wxIcon P64Asset::GetSmallThumbnail()
{
    return this->m_Thumbnail.GetSmallIcon();
}


/*==============================
    P64Asset::GetLargeThumbnail
    Gets the large thumbnail icon
    @return The large thumbnail, or wxNullIcon
==============================*/

wxIcon P64Asset::GetLargeThumbnail()
{
    return this->m_Thumbnail.GetLargeIcon();
}


/*==============================
    P64Asset::IsOk
    Checks if the asset is properly initialized
    @return Whether the asset is properly initialized
==============================*/

bool P64Asset::IsOk()
{
    return true;
}