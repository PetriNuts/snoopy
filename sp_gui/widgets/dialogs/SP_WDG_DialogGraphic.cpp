//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
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

BEGIN_EVENT_TABLE(SP_WDG_DialogGraphic, SP_WDG_DialogBase)
// intentionally left blank
END_EVENT_TABLE()


SP_WDG_DialogGraphic::SP_WDG_DialogGraphic(const wxString& p_sPage, unsigned int p_nOrdering)
  : SP_WDG_DialogBase(p_sPage, p_nOrdering),
    m_pcButtonPen(NULL),
    m_pcButtonBrush(NULL),
    m_bBrushSet(FALSE),
    m_bPenSet(FALSE)
{

}

SP_WDG_DialogGraphic::~SP_WDG_DialogGraphic()
{
    DisconnectEvents();
    CleanUpAttributes();
}

SP_WDG_DialogBase*
SP_WDG_DialogGraphic::Clone()
{
    return new SP_WDG_DialogGraphic(GetName(), GetDialogOrdering());
}

bool
SP_WDG_DialogGraphic::AddToDialog(SP_ListGraphic* p_plGraphics,
	SP_DLG_ShapeProperties* p_pcDlg)
{
	CHECK_POINTER(p_plGraphics, return FALSE);
	CHECK_BOOL((!p_plGraphics->empty()), return TRUE);
	CHECK_POINTER(p_pcDlg, return FALSE);

	/* change ckruge2 19.06.2009 */
	//parent element
	SP_Data* l_pctmp = (*(p_plGraphics->begin()))->GetParent();
	//remeber parent for OkDlg
	m_parent_tmp = l_pctmp;
	/* change end */
	// remember the graphics
	m_lGraphics = *p_plGraphics;
	m_pcDlg = p_pcDlg;
	m_bMultiple = (p_plGraphics->size() > 1);

	if ((*m_lGraphics.begin())->GetDelete() == true)
		return true;

	wxString l_sPage = GetName() + wxT(":") + (*m_lGraphics.begin())->GetParent()->GetClassName();
	SP_WDG_NotebookPage* l_pcPage = p_pcDlg->AddPage(l_sPage, GetDialogOrdering());
	CHECK_POINTER(l_pcPage, return FALSE);

	wxBoxSizer* l_pcSizer;

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Pen Colour:")), 0, wxALL, 5);
	m_pcButtonPen = new wxColourPickerCtrl(l_pcPage, m_nDialogID + SP_COLOURPEN + wxID_HIGHEST, (*m_lGraphics.begin())->GetPen()->GetColour(),
		wxDefaultPosition, wxDefaultSize,
		wxCLRP_DEFAULT_STYLE);
	l_pcSizer->Add(m_pcButtonPen, 0, wxALL, 5);
	l_pcSizer->Add(new wxButton(l_pcPage, SP_DEFAULTPEN + m_nDialogID + wxID_HIGHEST, _T("Default")), 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Brush Colour:")), 0, wxALL, 5);

	m_pcButtonBrush = new wxColourPickerCtrl(l_pcPage, SP_COLOURBRUSH + m_nDialogID + wxID_HIGHEST, (*m_lGraphics.begin())->GetBrush()->GetColour(),
		wxDefaultPosition, wxDefaultSize);

	l_pcSizer->Add(m_pcButtonBrush, 0, wxALL, 5);
	l_pcSizer->Add(new wxButton(l_pcPage, SP_DEFAULTBRUSH + m_nDialogID + wxID_HIGHEST, _T("Default")), 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

	SP_Graphic* l_pcGr;
	if (l_pctmp->GetElementType() != SP_ELEMENT_EDGE && l_pctmp->GetElementType() != SP_ELEMENT_METADATA )
	{
		if (l_pctmp->GetElementType() == SP_ELEMENT_NODE )//by george
		{
			l_pcGr = *m_lGraphics.begin();
		}
		int l_nThick;
		// -1 means unmodified
		if (m_bMultiple) {
			l_nThick = -1;
		}
		else {
			if(l_pcGr &&  l_pcGr->GetParent()!=NULL)//comments do not have thickness we must check parent of the graphics
			{
			   l_nThick = l_pcGr->GetThickness();
			}
			else
			{
				l_nThick=1;
			}
		}
	


	//by george
	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcSizer->Add(new wxStaticText(l_pcPage, -1, _T("Thickness:")), 0, wxALL, 5);
	m_scThickness = new wxSpinCtrl(l_pcPage, SP_ID_SPINCTRL_THICKNESS + 10009 + SP_ID_LAST_ID, wxString::Format(wxT("%d"), l_nThick));
	l_pcSizer->Add(m_scThickness, 0, wxALL, 5);
	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	}
	/* change ckruge2 19.06.2009 */
	//only Nodes have hight and width
	if (l_pctmp->GetElementType() == SP_ELEMENT_NODE)
	{
		int w,h;
		if (m_bMultiple)
		{
			w=-1;
			h=-1;
		}
		else
		{
			w = (*m_lGraphics.begin())->GetWidth();
			h = (*m_lGraphics.begin())->GetHeight();
		}
		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Width:")), 0, wxALL , 5);
		m_scWidth = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_WIDTH + 1000+ SP_ID_LAST_ID);
		// -1 means unmodified
		if (m_bMultiple) {
			m_scWidth->SetRange(-1, 2000);	// TODO: set size to constants not numbers, now 2000 is canvas max
		} else {
			m_scWidth->SetRange(1, 2000);		// TODO: set size to constants not numbers, now 2000 is canvas max
		}
		m_scWidth->SetValue(wxString::Format(wxT("%d"), w));
		l_pcSizer->Add(m_scWidth, 0, wxALL, 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

		l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
		l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Height:")), 0, wxALL , 5);
		m_scHeight = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_HEIGHT + 1000+ SP_ID_LAST_ID);
		// -1 means unmodified
	    if (m_bMultiple)
		{
			m_scHeight->SetRange(-1,2000);  // TODO: set size to constants not numbers, now 2000 is canvas max
		} else {
			m_scHeight->SetRange(1,2000);	// TODO: set size to constants not numbers, now 2000 is canvas max
		}
		m_scHeight->SetValue(wxString::Format(wxT("%d"), h));
		l_pcSizer->Add(m_scHeight, 0, wxALL, 5);
		l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
	}

	SP_Graphic* l_pcGraphic;
	if (m_parent_tmp->GetElementType() == SP_ELEMENT_EDGE)
	{
		l_pcGraphic = *m_lGraphics.begin();

        int l_intT;
        // -1 means unmodified
        if (m_bMultiple) {
            l_intT = -1;
        } else {
        	if(l_pcGr)
        	{
            l_intT = l_pcGraphic->GetThickness();
        }
        }
        l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
        l_pcSizer->Add(new wxStaticText(l_pcPage, -1, wxT("Line Thickness:")), 0, wxALL , 5);
        m_scThickness = new wxSpinCtrl(l_pcPage, SP_SPINCTRL_HEIGHT + 1000+ SP_ID_LAST_ID, wxString::Format(wxT("%d"), l_intT),
                wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, l_intT);
        l_pcSizer->Add(m_scThickness, 0, wxALL, 5);
        l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);

    	l_pcSizer = new wxBoxSizer(wxHORIZONTAL);
    	l_pcSizer->Add(new wxStaticLine( l_pcPage, -1, wxDefaultPosition, wxSize(300,3), wxHORIZONTAL),0, wxALL , 5);
    	l_pcPage->AddControl(l_pcSizer, 0, wxEXPAND);
    }

    ConnectEvents();
    p_pcDlg->PushEventHandler(this);
    return TRUE;
}

bool
SP_WDG_DialogGraphic::OnDlgOk()
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

		//by george
		for (SP_ListGraphic::iterator itG = l_pcGr->GetParent()->GetGraphics()->begin();
			itG != l_pcGr->GetParent()->GetGraphics()->end(); ++itG)
		{
			//(*itG)->SetPen(wxThePenList->FindOrCreatePen(m_pcButtonPen->GetColour(), (*itG)->GetPen()->GetWidth(), (*itG)->GetPen()->GetStyle()));
			//(*itG)->SetPenColour(m_pcButtonPen->GetColour());
			if((*itG)->GetGraphicType()==SP_GRAPHIC_METADATA) continue;
			if (m_scThickness->GetValue()>1)
			(*itG)->SetThickness(m_scThickness->GetValue());
		}
		
		/* change ck 28.07.09 
		 if graphik use with extend shapes*/

		if (m_parent_tmp->GetElementType() == SP_ELEMENT_NODE)
		{
			if (m_scWidth->GetValue() >= 0) { // -1 means unmodified
				l_pcGr->SetWidth(m_scWidth->GetValue());
			}
			if (m_scHeight->GetValue() >= 0) { // -1 means unmodified
				l_pcGr->SetHeight(m_scHeight->GetValue());
			}
 			if (l_pcGr->GetPrimitive() && l_pcGr->GetGraphicType() == SP_GRAPHIC_NODE) {
				l_cCanvas->MoveShape(l_pcGr->GetPrimitive(), 0, 0);
			}
		}

		if (m_parent_tmp->GetElementType() == SP_ELEMENT_EDGE) //edge
		{
            if(m_scThickness->GetValue() >= 1) {
				l_pcGr->SetThickness(m_scThickness->GetValue());
            }
		}

       	l_pcGr->Update(true);
    }

    l_cCanvas->Refresh();
    return TRUE;
}

void
SP_WDG_DialogGraphic::OnColourPen(wxColourPickerEvent& event)
{
		m_bPenSet = TRUE;
}

void
SP_WDG_DialogGraphic::OnColourBrush(wxColourPickerEvent& p_cEvent)
{
        m_bBrushSet = TRUE;
}





void
SP_WDG_DialogGraphic::ConnectEvents()
{
	Connect(m_nDialogID+ SP_COLOURPEN + wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogGraphic::OnColourPen );

    Connect(SP_COLOURBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogGraphic::OnColourBrush );

    Connect(SP_DEFAULTPEN + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogGraphic::OnDefaultPen );

    Connect(SP_DEFAULTBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogGraphic::OnDefaultBrush );
}

void
SP_WDG_DialogGraphic::DisconnectEvents()
{
	Disconnect(m_nDialogID+ SP_COLOURPEN + wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogGraphic::OnColourPen );

    Disconnect(SP_COLOURBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_COLOURPICKER_CHANGED,
        (wxObjectEventFunction) (wxEventFunction) (wxColourPickerEventFunction) &SP_WDG_DialogGraphic::OnColourBrush );

    Disconnect(SP_DEFAULTPEN + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogGraphic::OnDefaultPen );

    Disconnect(SP_DEFAULTBRUSH + m_nDialogID+ wxID_HIGHEST,
        wxEVT_COMMAND_BUTTON_CLICKED,
        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &SP_WDG_DialogGraphic::OnDefaultBrush );
}

void
SP_WDG_DialogGraphic::OnDefaultPen(wxCommandEvent& p_cEvent)
{
    SP_Graphic* l_pcGr = m_lGraphics.front();
	m_pcButtonPen->SetColour(l_pcGr->GetDefaultPen()->GetColour());
    m_bPenSet = true;
}

void
SP_WDG_DialogGraphic::OnDefaultBrush(wxCommandEvent& p_cEvent)
{
    SP_Graphic* l_pcGr = m_lGraphics.front();
	m_pcButtonBrush->SetColour(l_pcGr->GetDefaultBrush()->GetColour());
    m_bBrushSet = true;
}



