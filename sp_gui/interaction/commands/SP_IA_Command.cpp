//
//////////////////////////////////////////////////////////////////////

#include "SP_IA_Command.h"

//note: order is synchronized with SP_IA_CMD_T
static const wxChar* m_lsTypeStrings[] =
{ wxT("General"), wxT("Colorize"), wxT("Insert"), wxT("Delete"), wxT("Set Attribute"), wxT("Hide"), wxT("UnHide") };

wxString SP_IA_Command::GetCommandTypeString() const
{
	return m_lsTypeStrings[m_eType];
}

//BEGIN_EVENT_TABLE(SP_IA_Command, wxEvtHandler)
//END_EVENT_TABLE()


SP_IA_Command::SP_IA_Command() :
	wxCommand(false, wxT("<Default Name>")), m_eType(SP_IA_CMD_GENERAL),
			m_sDescription(wxT("<No Description>")), m_sSourceNetclass(wxT("")),
			m_sTargetNetclass(wxT("")), m_pcTarget(NULL), m_pcTriggerEvent(NULL),
			m_pcDataProcessors(NULL), m_bHasConfig(false), m_bIsCopy(false)
{
}

SP_IA_Command::SP_IA_Command(SP_IA_CMD_T p_eType, const wxString& p_sName,
		bool p_bHasConfig, bool p_bCanUndo) :
	wxCommand(p_bCanUndo, p_sName), m_eType(p_eType),
			m_sDescription(wxT("<No Description>")), m_sSourceNetclass(wxT("")),
			m_sTargetNetclass(wxT("")), m_pcTarget(NULL), m_pcTriggerEvent(NULL),
			m_pcDataProcessors(NULL), m_bHasConfig(p_bHasConfig),
			m_bIsCopy(false)
{
}

SP_IA_Command::~SP_IA_Command()
{

	//stamped copies neither have their own data processors nor their own events
	if (!IsCopy())
	{
		list<SP_IA_DataProcessor*>::iterator it;
		if (m_pcDataProcessors)
		{
			for (it = m_pcDataProcessors->begin(); it
					!= m_pcDataProcessors->end(); ++it)
			{
				SP_IA_DataProcessor *l_pcDP = *it;
				wxDELETE(l_pcDP);
			}
			wxDELETE(m_pcDataProcessors);
			m_pcDataProcessors = 0;
		}
		if (m_pcTriggerEvent)
			wxDELETE(m_pcTriggerEvent);
	}
}

SP_IA_Command* SP_IA_Command::InitializedCopy(SP_DS_Graph* p_pcSource,
		SP_DS_Graph* p_pcTarget)
{

	SP_IA_Command* l_pcResult = Copy();
	if (!l_pcResult)
		return 0;

	//this copy gets its own data processors and its own trigger-event.
	//but the copy stamp only marks copies w/out dp's and events of their own
	//so we reset it to false
	l_pcResult->SetCopyStamp(false);
	l_pcResult->SetTargetGraph(p_pcTarget);

	if (m_pcTriggerEvent)
	{
		l_pcResult->SetTriggerEvent(m_pcTriggerEvent->InitializedCopy(p_pcSource));
	}
	else
	{
		SP_LOGDEBUG(wxT("ERROR in InitializedCopy(...): No trigger event available."));
		return 0;
	}

	list<SP_IA_DataProcessor*>::iterator it;
	if (m_pcDataProcessors)
	{
		for (it = m_pcDataProcessors->begin(); it != m_pcDataProcessors->end(); ++it)
		{
			l_pcResult->AddDataProcessor((*it)->InitializedCopy(l_pcResult), false);
		}
	}

	return l_pcResult;
}

void SP_IA_Command::AddDataProcessor(SP_IA_DataProcessor *p_pcDP,
		bool p_bAdoptSelect)
{

	if (!p_pcDP)
		return;

	if (!m_pcDataProcessors)
		m_pcDataProcessors = new list<SP_IA_DataProcessor*>();

	if (p_bAdoptSelect && !m_pcDataProcessors->empty())
	{
		//this means we set ProcessSelect according to the last appended data processor
		//that way we don't need to set it again and again while defining the command
		p_pcDP->SetProcessSelected(m_pcDataProcessors->back()->ProcessSelected());
	}
	m_pcDataProcessors->push_back(p_pcDP);

}

void SP_IA_Command::DoDataProcessing(SP_ListData *p_pcData)
{

	if (!p_pcData || !m_pcDataProcessors)
		return;

	list<SP_IA_DataProcessor*>::iterator it;
	for (it = m_pcDataProcessors->begin(); it != m_pcDataProcessors->end(); ++it)
	{
		p_pcData = (*it)->DoProcessing(p_pcData);
	}

}

SP_DS_Graph* SP_IA_Command::GetSourceGraph() const
{

	if (m_pcTriggerEvent)
		return m_pcTriggerEvent->GetSourceGraph();

	return 0;
}

bool SP_IA_Command::Equals(SP_IA_Command *p_pcCmd) const
{
	//null pointer never equals
	if (!p_pcCmd)
		return false;
	//same reference always equals
	if (this == p_pcCmd)
		return true;

	if (m_eType != p_pcCmd->GetCommandType())
		return false;

	if (GetName().Cmp(p_pcCmd->GetName()) != 0)
		return false;

	if (m_pcTarget != p_pcCmd->GetTargetGraph())
		return false;

	return m_pcTriggerEvent->Equals(p_pcCmd->GetTriggerEvent());

}

void SP_IA_Command::SetNetclassNames(const wxString& p_sSource, const wxString& p_sTarget)
{

	if (!p_sSource.IsNull())
		m_sSourceNetclass = p_sSource;
	if (!p_sTarget.IsNull())
		m_sTargetNetclass = p_sTarget;
}

void SP_IA_Command::SetTargetGraph(SP_DS_Graph *p_pcGraph)
{

	if (p_pcGraph && p_pcGraph->GetNetclass()
			&& m_sTargetNetclass.Cmp(p_pcGraph->GetNetclass()->GetName()) == 0)
	{
		m_pcTarget = p_pcGraph;
	}
	else
	{
		SP_LOGDEBUG(wxT("ERROR in SetTargetGraph(...): class of target graph doesn't match predefined class."));
	}
}

