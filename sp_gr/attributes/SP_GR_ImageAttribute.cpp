//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Comment Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_ImageAttribute.h"
#include "sp_gr/base/SP_GR_BaseBitmap.h"
#include "sp_gr/eventhandler/SP_GRM_UpwardHandler.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "snoopy.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
    #include "bitmaps/csquery.xpm"
#endif

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ImageAttribute::SP_GR_ImageAttribute(SP_DS_Attribute* p_pcParent)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseBitmap();

    // image not resizeable for now
    m_pcPrimitive->SetHideDimensions(true);

    wxShapeEvtHandler* l_pcEventHandler = m_pcPrimitive->GetEventHandler();
    wxDELETE(l_pcEventHandler);

    m_pcPrimitive->SetEventHandler(new SP_GRM_UpwardHandler(m_pcPrimitive, m_pcPrimitive));

    m_pcPrimitive->SetSensitivityFilter(OP_ALL &~ OP_DRAG_LEFT);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);
}

SP_GR_ImageAttribute::~SP_GR_ImageAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ImageAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_ImageAttribute* l_pcNew = new SP_GR_ImageAttribute(l_pcParent);

    CloneBase(l_pcNew);

    return l_pcNew;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_ImageAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    m_pcPrimitive->Show(m_bShow);

    wxString l_sVal = dynamic_cast<SP_DS_Attribute*>(m_pcParent)->GetValueString();
    wxBitmap l_Bitmap;

    if(l_sVal.IsEmpty())
    {
   #if defined(__WIN32__)
    	l_Bitmap = wxBitmap( wxT("csquery"), wxBITMAP_TYPE_RESOURCE);
   #elif defined(__WXGTK__) || defined(__WXMAC__) // !__WIN32__
    	l_Bitmap = wxBitmap(bitmap_csquery_xpm);
   #else
   	   #error wxT("No icons for plattform.")
   #endif
    }
    else
    {
    	wxImage l_Image(l_sVal);
    	l_Bitmap = wxBitmap(l_Image);
    }

	if(l_Bitmap.IsOk())
	{
		m_pcPrimitive->SetBitmap(l_Bitmap);

        if (GetGraphicParent())
        {
            GetGraphicParent()->SetWidth(GetWidth());
            GetGraphicParent()->SetHeight(GetHeight());
        }
	}

    return TRUE;
}

wxShape*
SP_GR_ImageAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

