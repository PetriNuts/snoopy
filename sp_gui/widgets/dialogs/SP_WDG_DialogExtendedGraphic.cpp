//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/04/28 12:27:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/widgets/dialogs/SP_WDG_DialogExtendedGraphic.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"
#include "sp_core/base/SP_Graphic.h"

#if defined(__WXGTK24__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#else
    #define FAMILY_CTRLS GENERIC_CTRLS
#endif

enum
{
    SP_COLOURPEN,
    SP_COLOURBRUSH,
    SP_DEFAULTPEN,
    SP_DEFAULTBRUSH,
	SP_SPINCTRL_WIDTH,
	SP_SPINCTRL_HEIGHT
};

BEGIN_EVENT_TABLE(SP_WDG_DialogExtendedGraphic, SP_WDG_DialogBase)
// intentionally left blank
END_EVENT_TABLE()

SP_WDG_DialogExtendedGraphic::SP_WDG_DialogExtendedGraphic(const wxString& p_sPage, unsigned int p_nOrdering)
:SP_WDG_DialogBase(p_sPage, p_nOrdering),
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
m_pcListBox(NULL),
#endif
m_pcButtonPen(NULL),
m_pcButtonBrush(NULL),
m_bBrushSet(FALSE),
m_bPenSet(FALSE)
{
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
   //Nodestyle
   m_pcListBoxValues.Clear();
/* changed by ckruege2 17.06.2009 */
   m_pcListBoxValues.Add(wxT("solid line"));
   m_pcListBoxValues.Add(wxT("double solid line"));
   m_pcListBoxValues.Add(wxT("dotted line"));
/* 
GTK have problems with both designs.to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic, SP_GR_ExtendedEdge, shapes/SP_GR_Extended*, SP_GPR_Elements
*/
#ifndef linux
   m_pcListBoxValues.Add(wxT("dotted-dashed line"));
   m_pcListBoxValues.Add(wxT("dashed line"));
#endif
/* changing end */

#endif
/* changed by ckruege2 17.06.2009 */
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
   //edgestyle
   m_pcListBoxValues_Edge.Clear();
   m_pcListBoxValues_Edge.Add(wxT("solid line"));
   m_pcListBoxValues_Edge.Add(wxT("dotted line"));
/* 
GTK have problems with both designs. to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic, SP_GR_ExtendedEdge, shapes/SP_GR_Extended*, SP_GPR_Elements
*/
#ifndef linux
   m_pcListBoxValues_Edge.Add(wxT("dotted-dashed line"));
   m_pcListBoxValues_Edge.Add(wxT("dashed line"));
#endif
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
   //arrows
   m_pcArrowListBoxValues.Clear();
   m_pcArrowListBoxValues.Add(wxT("None"));
   m_pcArrowListBoxValues.Add(wxT("Empty Circle"));
   m_pcArrowListBoxValues.Add(wxT("Filled Circle"));
   m_pcArrowListBoxValues.Add(wxT("Arrow"));
   m_pcArrowListBoxValues.Add(wxT("Double Arrow"));
   m_pcArrowListBoxValues.Add(wxT("Double Circle"));
   m_pcArrowListBoxValues.Add(wxT("Circle Arrow"));
#endif
}
/* changing end */


SP_WDG_DialogExtendedGraphic::~SP_WDG_DialogExtendedGraphic()
{
    DisconnectEvents();
    CleanUpAttributes();
}

SP_WDG_DialogBase* SP_WDG_DialogExtendedGraphic::Clone()
{
    return new SP_WDG_DialogExtendedGraphic(GetName(), GetDialogOrdering());
}

bool SP_WDG_DialogExtendedGraphic::AddToDialog(SP_ListGraphic* p_plGraphics, SP_DLG_ShapeProperties* p_pcDlg)
{
    CHECK_POINTER(p_plGraphics, return FALSE);
    CHECK_BOOL((!p_plGraphics->empty()), return TRUE);
    CHECK_POINTER(p_pcDlg, return FALSE);

	//parent element
	SP_Data* l_pctmp = (*(p_plGraphics->begin()))->GetParent();
	//remeber parent for OkDlg
	m_parent_tmp=l_pctmp;

    // remember the graphics
    m_lGraphics = *p_plGraphics;
    m_pcDlg = p_pcDlg;
    m_bMultiple = (p_plGraphics->size() > 1);

    wxString l_sPage = GetName() + wxT(":") + (*m_lGraphics.begin())->GetParent()->GetClassName();
    SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
    CHECK_POINTER(l_pcPage, return FALSE);

    wxBoxSizer* l_pcSizer;

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Pen Colour:")), 0, wxALL | wxALIGN_CENTER, 5);
    m_pcButtonPen = new wxColourPickerCtrl(l_pcPage, m_nDialogID+ SP_COLOURPEN + wxID_HIGHEST , (*m_lGraphics.begin())->GetPen()->GetColour(),
                                            wxDefaultPosition, wxDefaultSize,
                                            wxCLRP_DEFAULT_STYLE);
	l_pcSizer->Add(m_pcButtonPen, 0, wxALL, 5);
    l_pcSizer->Add(new wxButton(l_pcPage, SP_DEFAULTPEN + m_nDialogID+ wxID_HIGHEST, _T("Default")), 0, wxALL | wxALIGN_CENTER, 5);
    l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

    l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
    l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Brush Colour:")), 0, wxALL | wxALIGN_CENTER, 5);

	m_pcButtonBrush = new wxColourPickerCtrl(l_pcPage, SP_COLOURBRUSH + m_nDialogID+ wxID_HIGHEST, (*m_lGraphics.begin())->GetBrush()->GetColour(),
											wxDefaultPosition, wxDefaultSize);

	l_pcSizer->Add(m_pcButtonBrush, 0, wxALL, 5);
    l_pcSizer->Add(new wxButton(l_pcPage, SP_DEFAULTBRUSH + m_nDialogID+ wxID_HIGHEST, _T("Default")), 0, wxALL | wxALIGN_CENTER, 5);
    l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	//only Nodes have high and width
	if (l_pctmp->GetElementType() == SP_ELEMENT_NODE)
	{
		int l_intW = (*m_lGraphics.begin())->GetWidth();
		int l_intH = (*m_lGraphics.begin())->GetHeight();

		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Width:")), 0, wxALL | wxALIGN_CENTER, 5);
		m_scWidth = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_WIDTH + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%d"), l_intW),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intW);
		l_pcSizer->Add(m_scWidth, 0, wxALL, 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Height:")), 0, wxALL | wxALIGN_CENTER, 5);
		m_scHeight = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_HEIGHT + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%d"), l_intH),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intH);
		l_pcSizer->Add(m_scHeight, 0, wxALL, 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	}

	//LineThickness
	size_t l_intT=0;
	SP_Graphic* l_pcGr;
	if (l_pctmp->GetElementType() == SP_ELEMENT_NODE) //node
	{
		l_pcGr = *m_lGraphics.begin();
		l_intT = (dynamic_cast<SP_GR_ExtendedDrawnShape*>(l_pcGr))->GetThickness();
	}
	if (l_pctmp->GetElementType() == SP_ELEMENT_EDGE) //edge
	{
		l_pcGr = *m_lGraphics.begin();
		l_intT = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetThickness();
	}
	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Line Thickness:")), 0, wxALL | wxALIGN_CENTER, 5);
	m_scThickness = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_HEIGHT + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%lu"), l_intT),
					wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intT);
	l_pcSizer->Add(m_scThickness, 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	//Line Design and Old Node Design
	if (l_pctmp->GetElementType() == SP_ELEMENT_NODE) //node
	{
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Pen Design:")), 0, wxALL | wxALIGN_CENTER, 5);
		l_pcGr = *m_lGraphics.begin();
		SP_EXTENDED_TYPE l_nType = (dynamic_cast<SP_GR_ExtendedDrawnShape*>(l_pcGr))->GetType();
		wxString l_pcType;
		l_pcType << l_nType;
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcSpinCtrl = new wxSpinCtrl(l_pcPage, -1, l_pcType);
		m_pcSpinCtrl->SetRange(0, 3);
		l_pcSizer->Add(m_pcSpinCtrl, 1, wxALL, 5);
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcListBox = new wxListBox(l_pcPage, -1);
		m_pcListBox->InsertItems(m_pcListBoxValues, 0);
		if (m_pcListBoxValues.GetCount() > 0) {
		  if (m_pcListBoxValues.GetCount() > (unsigned long) l_nType) {
			SP_LOGDEBUG(wxT("SP_WDG_DialogExtendedGraphic: l_nType=") + (wxString() << l_nType));
			m_pcListBox->SetSelection((int) l_nType);
		  } else {
			m_pcListBox->SetSelection((int) m_pcListBoxValues.GetCount() - 1);
		  }
		}
		l_pcSizer->Add(m_pcListBox, 1, wxALL, 5);
#endif
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticLine( l_pcPage, -1, wxDefaultPosition, wxSize(300,3), wxHORIZONTAL),0, wxALL | wxALIGN_CENTER , 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	}

	//Line Design
	if (l_pctmp->GetElementType() == SP_ELEMENT_EDGE)
	{
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Pen Design:")), 0, wxALL | wxALIGN_CENTER, 5);
		l_pcGr = *m_lGraphics.begin();
		SP_EXTENDED_TYPE_EDGE l_nType = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetType();
		wxString l_pcType;
		l_pcType << l_nType;
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcSpinCtrl = new wxSpinCtrl(l_pcPage, -1, l_pcType);
		m_pcSpinCtrl->SetRange(0, 3);
		l_pcSizer->Add(m_pcSpinCtrl, 1, wxALL, 5);
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcListBox_Edge = new wxListBox(l_pcPage, -1);
		m_pcListBox_Edge->InsertItems(m_pcListBoxValues_Edge, 0);
		if (m_pcListBoxValues_Edge.GetCount() > 0) {
		  if (m_pcListBoxValues_Edge.GetCount() > (unsigned long) l_nType) {
			SP_LOGDEBUG(wxT("SP_WDG_DialogExtendedGraphic: l_nType=") + (wxString() << l_nType));
			m_pcListBox_Edge->SetSelection((int) l_nType);
		} else {
			m_pcListBox_Edge->SetSelection((int) m_pcListBoxValues_Edge.GetCount() - 1);
		}
		}
		l_pcSizer->Add(m_pcListBox_Edge, 1, wxALL, 5);
#endif
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticLine( l_pcPage, -1, wxDefaultPosition, wxSize(300,3), wxHORIZONTAL),0, wxALL | wxALIGN_CENTER , 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	}

	//Arrowedge design
	if (l_pctmp->GetElementType() == SP_ELEMENT_EDGE)
	{
		wxFlexGridSizer* l_gSizer;
		l_gSizer = new wxFlexGridSizer( 3, 3, 0, 0 );
		l_gSizer->Add(new wxStaticText(l_pcPage, -1, _T("Arrows")), 0, wxALL, 5);
		l_gSizer->Add(new wxStaticText(l_pcPage, -1, _T("From")), 0, wxALL, 5);
		l_gSizer->Add(new wxStaticText(l_pcPage, -1, _T("To")), 0, wxALL, 5);
		l_gSizer->Add(new wxStaticText(l_pcPage, -1, _T("Arrowsheads")), 0, wxALL, 5);

		//checkbox1 from Arrow
		l_pcGr = *m_lGraphics.begin();
		SP_EXTENDED_ARROW_TYPE_EDGE l_nAType = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetArrowTypeLeft();
		wxString l_pcAType;
		l_pcAType << l_nAType;
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcArrowLeftSpinCtrl = new wxSpinCtrl(l_pcPage, -1, l_pcAType);
		m_pcArrowLeftSpinCtrl->SetRange(0, 3);
		l_pcSizer->Add(m_pcArrowLeftSpinCtrl, 1, wxALL, 5);
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcArrowLeftListBox = new wxListBox(l_pcPage, -1);
		m_pcArrowLeftListBox->InsertItems(m_pcArrowListBoxValues, 0);
		if (m_pcArrowListBoxValues.GetCount() > 0) {
			if (m_pcArrowListBoxValues.GetCount() > (unsigned long) l_nAType) {
				SP_LOGDEBUG(wxT("SP_WDG_DialogExtendedGraphic: l_nAType=") + (wxString() << l_nAType));
				m_pcArrowLeftListBox->SetSelection((int) l_nAType);
			} 
			else
			{
				m_pcArrowLeftListBox->SetSelection((int) m_pcArrowListBoxValues.GetCount() - 1);
			}
		}
		l_gSizer->Add(m_pcArrowLeftListBox, 1, wxALL, 5);
#endif
		//checkbox2 to Arrow
		l_pcGr = *m_lGraphics.begin();
		SP_EXTENDED_ARROW_TYPE_EDGE l_nATypeR = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetArrowTypeRight();
		wxString l_pcATypeR;
		l_pcATypeR << l_nATypeR;
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcArrowRightSpinCtrl = new wxSpinCtrl(l_pcPage, -1, l_pcATypeR);
		m_pcArrowRightSpinCtrl->SetRange(0, 3);
		l_pcSizer->Add(m_pcArrowRightSpinCtrl, 1, wxALL, 5);
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		m_pcArrowRightListBox = new wxListBox(l_pcPage, -1);
		m_pcArrowRightListBox->InsertItems(m_pcArrowListBoxValues, 0);
		if (m_pcArrowListBoxValues.GetCount() > 0) {
			if (m_pcArrowListBoxValues.GetCount() > (unsigned long) l_nATypeR) {
				SP_LOGDEBUG(wxT("SP_WDG_DialogExtendedGraphic: l_nAType=") + (wxString() << l_nATypeR));
				m_pcArrowRightListBox->SetSelection((int) l_nATypeR);
			} 
			else 
			{
				m_pcArrowRightListBox->SetSelection((int) m_pcArrowListBoxValues.GetCount() - 1);
			}
		}
		l_gSizer->Add(m_pcArrowRightListBox, 1, wxALL, 5);
#endif
		l_gSizer->Add(new wxStaticText(l_pcPage, -1, _T("Arrowsheads\n thickness")), 0, wxALL, 5);
		
		SP_Graphic* l_pcGr = *m_lGraphics.begin();
		size_t l_intALT = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetArrowLeftThickness();		
		m_scArrowLThickness = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_WIDTH + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%lu"), l_intALT),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intALT);
		l_gSizer->Add(m_scArrowLThickness, 0, wxALL, 5);
		
		l_pcGr = *m_lGraphics.begin();
		size_t l_intART = (dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr))->GetArrowRightThickness();
		m_scArrowRThickness = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_WIDTH + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%lu"), l_intART),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intART);
		l_gSizer->Add(m_scArrowRThickness, 0, wxALL, 5);
		l_pcPage->AddControl(l_gSizer, 0, wxEXPAND);
	}
	ConnectEvents();
    p_pcDlg->PushEventHandler(this);

    return TRUE;
}

bool
SP_WDG_DialogExtendedGraphic::OnDlgOk()
{
	SP_GUI_Canvas *l_cCanvas = (*m_lGraphics.begin())->GetCanvas();
	if (!l_cCanvas) return TRUE;

    SP_ListGraphic::iterator l_Iter;
    for (l_Iter = m_lGraphics.begin(); l_Iter != m_lGraphics.end(); ++l_Iter)
    {
    	SP_Graphic* l_pcGr = *l_Iter;
		if (m_bBrushSet)
		{
			for(SP_ListGraphic::iterator itG = l_pcGr->GetParent()->GetGraphics()->begin();
					itG != l_pcGr->GetParent()->GetGraphics()->end(); ++itG)
			{
				wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(m_pcButtonBrush->GetColour(), (*itG)->GetBrush()->GetStyle());
				(*itG)->SetBrush(l_pcBrush);
			}
        }
        if (m_bPenSet)
		{
			for(SP_ListGraphic::iterator itG = l_pcGr->GetParent()->GetGraphics()->begin();
					itG != l_pcGr->GetParent()->GetGraphics()->end(); ++itG)
			{
				(*itG)->SetPen(wxThePenList->FindOrCreatePen(m_pcButtonPen->GetColour(), (*itG)->GetPen()->GetWidth(), (*itG)->GetPen()->GetStyle()));
				(*itG)->SetPenColour(m_pcButtonPen->GetColour());
			}
		}

        /* change ck 19.06.09 */
		if (m_parent_tmp->GetElementType() == SP_ELEMENT_NODE) //node
		{
			SP_GR_ExtendedDrawnShape* l_pcExtGr = dynamic_cast<SP_GR_ExtendedDrawnShape*>(l_pcGr);
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
				l_pcExtGr->SetType((SP_EXTENDED_TYPE) m_pcSpinCtrl->GetValue());
#endif
#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
				l_pcExtGr->SetType((SP_EXTENDED_TYPE) m_pcListBox->GetSelection());
#endif

			l_pcExtGr->SetThickness(m_scThickness->GetValue());

		}

		if (m_parent_tmp->GetElementType() == SP_ELEMENT_EDGE) //edge
		{
			SP_GR_ExtendedEdge* l_pcExtGr = dynamic_cast<SP_GR_ExtendedEdge*>(l_pcGr);
#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
	        l_pcExtGr->SetType((SP_EXTENDED_TYPE_EDGE) m_pcSpinCtrl->GetValue());
#endif

#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		    l_pcExtGr->SetType((SP_EXTENDED_TYPE_EDGE) m_pcListBox_Edge->GetSelection());
#endif

#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		     l_pcExtGr->SetArrowTypeLeft((SP_EXTENDED_ARROW_TYPE_EDGE) m_pcArrowLeftSpinCtrl->GetValue());
#endif

#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		     l_pcExtGr->SetArrowTypeLeft((SP_EXTENDED_ARROW_TYPE_EDGE) m_pcArrowLeftListBox->GetSelection());
#endif

#ifndef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		     l_pcExtGr->SetArrowTypeRight((SP_EXTENDED_ARROW_TYPE_EDGE) m_pcArrowRightSpinCtrl->GetValue());
#endif

#ifdef __SP_WDG_DIALOGEXTENDEDGRAPHIC_USE_LISTBOX__
		     l_pcExtGr->SetArrowTypeRight((SP_EXTENDED_ARROW_TYPE_EDGE) m_pcArrowRightListBox->GetSelection());
#endif
			l_pcExtGr->SetThickness(m_scThickness->GetValue());
			l_pcExtGr->SetArrowLeftThickness(m_scArrowLThickness->GetValue());
			l_pcExtGr->SetArrowRightThickness(m_scArrowRThickness->GetValue());

		}

		if (m_parent_tmp->GetElementType() == SP_ELEMENT_NODE) //node high
		{
			(*l_Iter)->SetWidth(m_scWidth->GetValue());
			(*l_Iter)->SetHeight(m_scHeight->GetValue());
		}
		if ((*l_Iter)->GetPrimitive() && (*l_Iter)->GetGraphicType() == SP_GRAPHIC_NODE)
		{
			l_cCanvas->MoveShape((*l_Iter)->GetPrimitive(), 0, 0);
		}

		(*l_Iter)->Update(true);
	}

	l_cCanvas->Refresh();


    return TRUE;
}

void
SP_WDG_DialogExtendedGraphic::OnColourPen(wxColourPickerEvent& event)
{
		m_bPenSet = TRUE;
}

void
SP_WDG_DialogExtendedGraphic::OnColourBrush(wxColourPickerEvent& p_cEvent)
{
        m_bBrushSet = TRUE;
}


void
SP_WDG_DialogExtendedGraphic::ConnectEvents()
{
    Connect(m_nDialogID+ SP_COLOURPEN + wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogExtendedGraphic::OnColourPen );

    Connect(SP_COLOURBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogExtendedGraphic::OnColourBrush );

    Connect(SP_DEFAULTPEN + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogExtendedGraphic::OnDefaultPen );

    Connect(SP_DEFAULTBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogExtendedGraphic::OnDefaultBrush );

}

void
SP_WDG_DialogExtendedGraphic::DisconnectEvents()
{
    Disconnect(m_nDialogID+ SP_COLOURPEN + wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogExtendedGraphic::OnColourPen );

    Disconnect(SP_COLOURBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogExtendedGraphic::OnColourBrush );

    Disconnect(SP_DEFAULTPEN + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogExtendedGraphic::OnDefaultPen );

    Disconnect(SP_DEFAULTBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogExtendedGraphic::OnDefaultBrush );
}

void
SP_WDG_DialogExtendedGraphic::OnDefaultPen(wxCommandEvent& p_cEvent)
{
    SP_Graphic* l_pcGr = m_lGraphics.front();
	m_pcButtonPen->SetColour(l_pcGr->GetDefaultPen()->GetColour());
    m_bPenSet = true;
}

void
SP_WDG_DialogExtendedGraphic::OnDefaultBrush(wxCommandEvent& p_cEvent)
{
    SP_Graphic* l_pcGr = m_lGraphics.front();
	m_pcButtonBrush->SetColour(l_pcGr->GetDefaultBrush()->GetColour());
    m_bBrushSet = true;
}



