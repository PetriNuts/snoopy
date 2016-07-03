// SP_IA_Command_Colorize.cpp: Implementierung der Klasse SP_IA_Command_Colorize.
//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Command_Colorize.h"
#include <wx/colordlg.h>

BEGIN_EVENT_TABLE(SP_IA_Command_Colorize, wxEvtHandler)

EVT_COLOURPICKER_CHANGED(SP_ID_BUTTON_COLOR, SP_IA_Command_Colorize::OnChooseColor)

END_EVENT_TABLE()

SP_IA_Command_Colorize::SP_IA_Command_Colorize() :
	SP_IA_Command(SP_IA_CMD_COLORIZE, wxT("Command: Colorize <Default Name>"), true,
			true), m_pcCommandHistory(0)
{
	m_bUseSourceColor = true;
}

SP_IA_Command_Colorize::SP_IA_Command_Colorize(const wxString& p_sName,
		wxColour p_cColor, bool p_bHasConfig, bool p_bCanUndo) :
	SP_IA_Command(SP_IA_CMD_COLORIZE, p_sName, p_bHasConfig, p_bCanUndo),
			m_pcCommandHistory(0)
{
	SetColor(p_cColor);
	m_bUseSourceColor = (p_cColor == wxNullColour);
}

SP_IA_Command_Colorize::~SP_IA_Command_Colorize()
{
	if (m_pcCommandHistory)
	{
		if (!m_pcCommandHistory->empty())
		{
			SP_CMD_ColorizeGraphic *cmd;
			for (list<SP_CMD_ColorizeGraphic*>::iterator it =
					m_pcCommandHistory->begin(); it
					!= m_pcCommandHistory->end(); ++it)
			{
				cmd = *it;
				wxDELETE(cmd);
			}
		}
		wxDELETE(m_pcCommandHistory);
	}
}

SP_IA_Command* SP_IA_Command_Colorize::Copy(bool p_bTryInit)
{
	SP_IA_Command_Colorize* l_pcResult = new SP_IA_Command_Colorize(GetName(), m_cColorData.GetColour(), HasConfig(), CanUndo());
	l_pcResult->SetDescription(m_sDescription);
	l_pcResult->SetNetclassNames(m_sSourceNetclass, m_sTargetNetclass);
	l_pcResult->SetCopyStamp(true);
	l_pcResult->UseSourceColor(m_bUseSourceColor);
	if (p_bTryInit)
	{
		l_pcResult->SetTargetGraph(m_pcTarget);
		l_pcResult->SetTriggerEvent(m_pcTriggerEvent);
		l_pcResult->SetDataProcessors(m_pcDataProcessors);
	}
	return l_pcResult;
}

bool SP_IA_Command_Colorize::Do(SP_ListData* p_pcData)
{

	if (!m_pcCommandHistory)
		m_pcCommandHistory = new list<SP_CMD_ColorizeGraphic*>();

	SP_ListGraphic* l_gr;
	SP_ListGraphic::iterator itgr;
	if (m_bUseSourceColor)
	{
		//for several elements only the color of the first is used
		l_gr = (p_pcData->front())->GetGraphics();
		m_cColorData.SetColour(l_gr->front()->GetBrush()->GetColour());
	}

	//calculating the elements to colorize...
	DoDataProcessing(p_pcData);

	//we found nothing means that something went wrong...
	//...or the connected nets do not really correspond
	if (!p_pcData)
		return false;
	if (p_pcData->size() == 0)
		return false;

	SP_ListData::iterator it;

	//collecting graphic objects of the elements to color
	for (it = p_pcData->begin(); it != p_pcData->end(); ++it)
	{
		l_gr = (*it)->GetGraphics();
		if (!l_gr)
			continue;
		//creating a color command for every one found
		for (itgr = l_gr->begin(); itgr != l_gr->end(); ++itgr)
		{
			if ((*itgr)->GetGraphicType() == SP_GRAPHIC_NODE || (*itgr)->GetGraphicType() == SP_GRAPHIC_EDGE)
			{
				m_pcCommandHistory->push_back(new SP_CMD_ColorizeGraphic(*itgr, m_cColorData.GetColour(), m_cColorData.GetColour()));
			}
		}
	}

	list<SP_CMD_ColorizeGraphic*>::iterator it2;
	//executing all color commands
	for (it2 = m_pcCommandHistory->begin(); it2 != m_pcCommandHistory->end(); ++it2)
	{
		(*it2)->Do();
	}

	for (it = p_pcData->begin(); it != p_pcData->end(); ++it)
	{
		SP_IA_DataProcessor::ReleaseData(*it, false, true);
	}

	return true;
}

bool SP_IA_Command_Colorize::Undo()
{

	if (!m_pcCommandHistory || m_pcCommandHistory->empty())
		return false;

	list<SP_CMD_ColorizeGraphic*>::iterator it2;
	//undoing all previously saved color commands
	for (it2 = m_pcCommandHistory->begin(); it2 != m_pcCommandHistory->end(); ++it2)
	{
		SP_CMD_ColorizeGraphic* l_pcCMD = *it2;
		l_pcCMD->Undo();
		wxDELETE(l_pcCMD);
	}
	m_pcCommandHistory->clear();

	return true;
}

void SP_IA_Command_Colorize::AddToConfigDialog(wxWindow *p_pcParent,
		wxSizer *p_pcSizer)
{

	wxBoxSizer *l_pcColorSizer = new wxBoxSizer(wxHORIZONTAL);
	l_pcColorSizer->Add(new wxStaticText(p_pcParent, -1, wxT("Color:")), 0);

	m_buChooseColor = new wxColourPickerCtrl(p_pcParent, SP_ID_BUTTON_COLOUR, GetColor(),
	            wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	l_pcColorSizer->Add(m_buChooseColor, 0, wxLEFT, 10);
	p_pcSizer->Add(l_pcColorSizer, 0, wxALL, 10);

	m_cbUseSource = new wxCheckBox(p_pcParent, SP_CHECKBOX_USESOURCE, wxT("Use color of source elements"));
	m_cbUseSource->SetValue(m_bUseSourceColor);
	p_pcSizer->Add(m_cbUseSource, 0, wxLEFT | wxBOTTOM, 10);

	m_cbUndo = new wxCheckBox(p_pcParent, -1, wxT("Preserve previous coloring (don't undo)"));
	m_cbUndo->SetValue(!CanUndo());
	p_pcSizer->Add(m_cbUndo, 0, wxLEFT | wxRIGHT | wxTOP, 10);

	//TODO: couldn't get this to work properly
	//p_pcParent->PushEventHandler(this);

}

void SP_IA_Command_Colorize::OnConfigDialogOk()
{
	m_bUseSourceColor = m_cbUseSource->IsChecked();
	m_canUndo = !m_cbUndo->IsChecked();

}

void SP_IA_Command_Colorize::OnChooseColor(wxColourPickerEvent& p_cEvent)
{
	SetColor(m_buChooseColor->GetColour());
	RefreshColor();
	m_cbUseSource->SetValue(m_bUseSourceColor = false);
}

void SP_IA_Command_Colorize::RefreshColor()
{
	m_buChooseColor->Refresh();
}
