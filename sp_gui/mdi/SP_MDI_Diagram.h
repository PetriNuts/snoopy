//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_MDI_DIAGRAM_H__
#define __SP_MDI_DIAGRAM_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/ogl/ogl.h>

#include "sp_defines.h"

//! include not possible
//#include "sp_gui/mdi/SP_MDI_Doc.h"
class SP_MDI_Doc;

class SP_MDI_Diagram: public wxDiagram
{
private:
    DECLARE_CLASS(SP_MDI_Diagram)
protected:
    SP_MDI_Doc* m_pcDoc;

    bool m_bGridVisible;
public:
    SP_MDI_Diagram(SP_MDI_Doc* p_pcDoc);
    virtual ~SP_MDI_Diagram();

    inline SP_MDI_Doc* GetDocument() const { return m_pcDoc; }

    virtual void Redraw(wxDC& dc);

    void SetGridVisible(bool p_bVal);
//    SP_GRID_STYLES_T GetGridStyle();
};

#endif // __SP_MDI_DIAGRAM_H__

