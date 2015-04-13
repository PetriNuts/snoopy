//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2009/03/31 11:55:00 $
// Short Description: Graphical Metadata-Base Implementation
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Metadata.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gr/SP_GR_Metadata.h"
#include "sp_gr/SP_GR_Edge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_core/SP_Core.h"
#include "snoopy.h"

SP_GR_Metadata::SP_GR_Metadata(SP_DS_Metadata* p_pcDataParent)
    :SP_Graphic(SP_GRAPHIC_METADATA),
    m_bFixedSize(FALSE)
{
    SetDataParent(p_pcDataParent);
}

SP_GR_Metadata::~SP_GR_Metadata()
{
}

bool
SP_GR_Metadata::CloneBase(SP_Graphic* p_pcCopy)
{
    if (!p_pcCopy)
        return FALSE;

    dynamic_cast<SP_GR_Metadata*>(p_pcCopy)->SetFixedSize(m_bFixedSize);

    return SP_Graphic::CloneBase(p_pcCopy);
}

bool
SP_GR_Metadata::Update(bool p_bLocalOnly)
{
    CHECK_POINTER(GetPrimitive(), return FALSE);
    CHECK_POINTER(GetParent(), return FALSE);

    GetPrimitive()->SetBrush(GetBrush());
    GetPrimitive()->SetPen(GetPen());

	ToggleHide(GetHide());

	return TRUE;
}

bool
SP_GR_Metadata::ResetGraphicState()
{
    if (!GetPrimitive())
        return FALSE;

    if (GetGraphicState() == SP_STATE_NORMAL)
        return TRUE;

    return SetGraphicState(SP_STATE_NORMAL);
}

bool
SP_GR_Metadata::OnDoubleClick(int p_nKeys)
{
    if (SP_Core::Instance()->GetAnimRunning())
        return FALSE;

    if (GetParent() && GetParent()->OnDoubleClick(this, p_nKeys))
    {
        return TRUE;
    }

    return EditProperties();
}

bool
SP_GR_Metadata::EditProperties()
{
    // map of list of attributes
    SP_MapString2ListAttribute l_tmAttributes;

    // get the attributes to show in the dialog
    // implemented in base class 'SP_Graphic'
    CollectDialogAttributes(&l_tmAttributes);

    // construct the dialog
    SP_DLG_ShapeProperties l_cDialog(NULL, wxT("Edit Properties"));
    // add this to the list of affected nodes
    l_cDialog.AddDialogGraphic(this);

    if (l_cDialog.ShowDialogAttributes(&l_tmAttributes))
    {
        // show and forget about the dlg
        // the dialog manages the events (OK, CANCEL) by itself
        l_cDialog.ShowModal();

        if (GetPrimitive())
        {
			dynamic_cast<SP_GUI_Canvas*>(GetPrimitive()->GetCanvas())->Modify(TRUE);
        }
    }
    else
    {
        SP_LOGDEBUG(wxT("No attributes on this node."));
    }

    l_cDialog.Destroy();

    if (GetParent())
    {
        return GetParent()->Update(TRUE);
    }

    return Update(TRUE);
}

bool
SP_GR_Metadata::AddToCanvas(SP_GUI_Canvas* p_pcCanvas, double p_nX, double p_nY, int p_nKeys)
{
    CHECK_POINTER(p_pcCanvas, return FALSE);

    // just wouldn't draw me, but that's no BAD state at all
    if (p_pcCanvas->GetNetnumber() != GetNetnumber())
        return TRUE;

    CHECK_POINTER(GetPrimitive(), return FALSE);

    if (GetPrimitive()->GetCanvas())
        return TRUE;

    if ((p_nX != -1) && (p_nY != -1))
        SetPosXY(p_nX, p_nY);

    //p_pcCanvas->AddShape(GetPrimitive());
    //insert shape of metadata at beginning of shape list
    p_pcCanvas->InsertShape(GetPrimitive());
    GetPrimitive()->Show(GetShow());

    ShowAttributeGraphic();

    return TRUE;
}

bool
SP_GR_Metadata::AddToDeleteQueue(SP_GRAPHIC_STATE p_eFilter)
{
    if (!GetPrimitive() || !GetParent())
        return FALSE;

    if (GetDelete())
        return TRUE;

    if (((p_eFilter & SP_STATE_COARSEBORDERDOWN) == SP_STATE_COARSEBORDERDOWN) &&
        (GetGraphicState() == SP_STATE_COARSEBORDERDOWN))
        return FALSE;

    wxNode* l_pcNode = GetPrimitive()->GetLines().GetFirst();
    while (l_pcNode)
    {
        wxLineShape* l_pcLine = dynamic_cast<wxLineShape*>(l_pcNode->GetData());
        SP_Graphic* l_pcGraphic = SP_Core::Instance()->ResolveExtern(l_pcLine);
        if (l_pcGraphic)
        {
            // enqueue the lines, too
            if ((p_eFilter != SP_STATE_SINGLE))
                l_pcGraphic->AddAllToDeleteQueue(p_eFilter);
            // the edge AND either end will be removed! we can't ensure, that
            // the edge isn't removed before any of their nodes, so we just
            // tell source and target that there are'nt any more lines connected
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseSource();
            dynamic_cast<SP_GR_Edge*>(l_pcGraphic)->ReleaseTarget();

            l_pcNode = GetPrimitive()->GetLines().GetFirst();
        }
        else
            l_pcNode = l_pcNode->GetNext();
    }

    return SP_Graphic::AddToDeleteQueue(p_eFilter);
}

bool
SP_GR_Metadata::ShowAttributeGraphic()
{
    SP_Data* l_plParent = this->GetParent();
    if (l_plParent)
    {
        return dynamic_cast<SP_DS_Metadata*>(l_plParent)->ShowAttributeGraphic(this);
    }
    return FALSE;
}

void
SP_GR_Metadata::SetFixedSize(bool p_bVal)
{
    m_bFixedSize = p_bVal;

    if (GetPrimitive())
        GetPrimitive()->SetFixedSize(p_bVal, p_bVal);
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_Metadata::SetWidth(double p_nVal)
{
    double l_nHeight = GetHeight();

    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(p_nVal, l_nHeight);
    return TRUE;
}

double
SP_GR_Metadata::GetWidth()
{
    CHECK_POINTER(GetPrimitive(), return 0.0);

    double l_nWidth, l_nHeight;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nWidth;
}

bool
SP_GR_Metadata::SetHeight(double p_nVal)
{
    double l_nWidth = GetWidth();

    if (!GetPrimitive())
        return FALSE;

    GetPrimitive()->SetSize(l_nWidth, p_nVal);
    return TRUE;
}

double
SP_GR_Metadata::GetHeight()
{
    CHECK_POINTER(GetPrimitive(), return 0.0);

    double l_nWidth, l_nHeight;
    GetPrimitive()->GetBoundingBoxMin(&l_nWidth, &l_nHeight);

    return l_nHeight;
}
