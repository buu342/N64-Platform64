#include "panel_imgview.h"
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include "../resource.h"

Panel_ImgView::Panel_ImgView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle) : wxScrolledWindow(parent, id, pos, size, style)
{
    this->Connect(wxEVT_PAINT, wxPaintEventHandler(Panel_ImgView::OnPaint), NULL, this);
    this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Panel_ImgView::OnMouse), NULL, this);
    this->LoadImageFromPath(Tex_Missing);
}

Panel_ImgView::~Panel_ImgView()
{

}

void Panel_ImgView::Create(wxWindow* parent, wxWindowID id)
{
    wxScrolledWindow::Create(parent, id);
}

void Panel_ImgView::LoadImageFromPath(wxBitmap& image)
{
    this->m_Bitmap = image;
    //SetVirtualSize(this->m_Bitmap.GetWidth(), this->m_Bitmap.GetHeight());
    this->Refresh();
}

void Panel_ImgView::OnMouse(wxMouseEvent& event)
{

}

void Panel_ImgView::OnPaint(wxPaintEvent& event)
{
    int w, h;
    wxPaintDC dc(this);
    PrepareDC(dc);
    dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWFRAME)));
    dc.Clear();
    dc.GetSize(&w, &h);
    dc.DrawBitmap(this->m_Bitmap, w/2 - this->m_Bitmap.GetWidth()/2, h/2 - this->m_Bitmap.GetHeight()/2, false);
}