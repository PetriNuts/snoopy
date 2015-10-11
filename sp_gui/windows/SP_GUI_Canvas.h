//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GUI_CANVAS_H__
#define __SP_GUI_CANVAS_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>
#include <wx/overlay.h>

#include "sp_core/SP_List.h"

//#include "sp_gui/mdi/SP_MDI_View.h"
//#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_core/base/SP_Data.h"

//! include not possible
class SP_GUI_Childframe;
class SP_MDI_View;
class SP_DS_Graph;
class SP_Graphic;

class SP_GUI_Canvas: public wxShapeCanvas
{
private:
    DECLARE_CLASS(SP_GUI_Canvas)
    DECLARE_EVENT_TABLE()

    wxOverlay m_Overlay;
    double g_nStartX = -10.0;
    double g_nStartY = -10.0;

protected:
    bool m_bStartup;
    SP_MDI_View* m_pcView;
    SP_GUI_Childframe* m_pcParentframe;
    SP_Type* m_pcEditElement;

    //! for segmented line drawing
    //! temporary holds the points, the user clicks on the canvas,
    //! while drawing a line
    wxList* m_pcControlPoints;

    int m_nSizeX = 0;
    int m_nSizeY = 0;

    double m_nLastPosX = 0;
    double m_nLastPosY = 0;

    bool UnSelectAll(int p_nKeys = 0);

    bool OnEndDragLeftMerge(SP_Graphic* p_pcShape);
public:
    SP_GUI_Canvas(
        SP_MDI_View* p_pcView,
        SP_GUI_Childframe *p_pcFrame,
        wxWindowID p_nId = -1,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = wxScrolledWindowStyle | wxRETAINED );
    ~SP_GUI_Canvas();

    bool OnStartLine(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnDrawLine(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnEndLine(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnBeginDragLeftShape(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnDragLeftShape(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnEndDragLeftShape(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);
    bool OnClickOnShape(wxShape* p_pcShape, double p_nX, double p_nY, int p_nKeys, int p_nAttach);

    inline SP_GUI_Childframe* GetParentframe() { return m_pcParentframe; }
    inline SP_MDI_View*  GetView() const { return m_pcView; }
    inline void SetView(SP_MDI_View* p_pcView) { m_pcView = p_pcView; }

    SP_Type* SetEditElement(SP_Type* p_pcElement);
    inline SP_Type* GetEditElement() const { return m_pcEditElement; }

    virtual void OnLeftClick(double x, double y, int keys = 0);
    virtual void OnRightClick(double x, double y, int keys = 0);

    virtual void OnDragLeft(bool draw, double x, double y, int keys=0); // Erase if draw false
    virtual void OnBeginDragLeft(double x, double y, int keys=0);
    virtual void OnEndDragLeft(double x, double y, int keys=0);

    virtual void OnDragRight(bool draw, double x, double y, int keys=0); // Erase if draw false
    virtual void OnBeginDragRight(double x, double y, int keys=0);
    virtual void OnEndDragRight(double x, double y, int keys=0);

    bool MergeAll(SP_ListGraphic* p_plShapes, SP_Graphic* p_pcTarget);

    void AddShape(wxShape* p_pcShape, wxShape *addAfter);
    void AddShape(wxShape* p_pcShape);
    void InsertShape(wxShape* p_pcShape);
    void RemoveShape(wxShape *p_pcShape);

    void OnPaint( wxPaintEvent& p_cEvent);
    void OnMouseEvent(wxMouseEvent& p_cEvent );
    void OnScroll(wxScrollWinEvent& p_cEvent);
    void OnKeyEvent(wxKeyEvent& p_cEvent);

    void RefreshRects(bool p_bErase = TRUE);
    void ReleaseMouse() { if (GetCapture() == this) wxShapeCanvas::ReleaseMouse(); }

    void DrawOutline(double x1, double y1, double x2, double y2);
    bool DrawOutlineShapes(double p_nOffsetX, double p_nOffsetY);

    unsigned int GetNetnumber();
    bool DrawAllElements(SP_DS_Graph* p_pcGraph, bool p_bLocalOnly = FALSE);

    //! new point for the actual segmented line
    void AddControlPoint(double p_nX, double p_nY);
    //! accessor
    wxList* GetControlPoints() const { return m_pcControlPoints; }
    bool DrawTempEdge(double p_nX, double p_nY);

    void Modify(bool p_bVal);

    bool MoveShape(wxShape* p_pcShape, double p_nOffsetX, double p_nOffsetY);
    bool MoveLinePoints(double p_nOffsetX, double p_nOffsetY);
    bool MoveShapes(double p_nOffsetX, double p_nOffsetY);

    wxOverlay& GetOverlay() { return m_Overlay; }

    int GetVirtualSizeX() { return m_nSizeX; }
    int GetVirtualSizeY() { return m_nSizeY; }
    void SetVirtualSizeX(int x) { m_nSizeX = x; }
    void SetVirtualSizeY(int y) { m_nSizeY = y; }
    bool UpdateVirtualSize(int x, int y);

};

#endif // __SP_GUI_CANVAS_H__
