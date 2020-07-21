/*
 * SP_ImportCSV2SPN.cpp
 *
 *  Created on: May 26, 2010
 *      Author: crohr
 */
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#include "SP_ImportCSV2SPN.h"
#include "snoopy.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

SP_ImportCSV2SPN::SP_ImportCSV2SPN()
{
}

SP_ImportCSV2SPN::~SP_ImportCSV2SPN()
{
}

bool
SP_ImportCSV2SPN::ReadFile(const wxString& fileName,SP_ImportRoutine* p_sIR)
{
	m_mImportValues.clear();
	ResetErrors();

	SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(wxGetApp().GetDocmanager()->GetCurrentDocument());

	if(l_pcDoc)
	{
		m_pcGraph = l_pcDoc->GetGraph();
		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		{
			wxTextFile l_File;
			if(l_File.Open(fileName))
			{
				LoadPlaces();
				LoadTransitions();
				LoadParameters();

				wxString str;
				for ( str = l_File.GetFirstLine(); !l_File.Eof(); str = l_File.GetNextLine() )
				{
				    ParseLine(str);
				}

				CheckImportValues();
			    SetImportValues();
			    dynamic_cast<SP_MDI_View*>(l_pcDoc->GetFirstView())->DoRefresh();
			}
			else
			{
				wxString l_sMsg = wxT("Error: file not found  ");
				l_sMsg << fileName;
				SP_MESSAGEBOX(l_sMsg, wxT("Error"));
				SetLastError(l_sMsg);
			}
		}
		else
		{
			wxString l_sMsg = wxT("Error: Only ");
			l_sMsg << SP_DS_SPN_CLASS << wxT(" netclass allowed!");
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			SetLastError(l_sMsg);
		}
	}
	else
	{
		wxString l_sMsg = wxT("Error: No document open.");
		SP_MESSAGEBOX(l_sMsg, wxT("Error"));
		SetLastError(l_sMsg);
	}

	return !GetErrorSet();
}

void
SP_ImportCSV2SPN::ParseLine(const wxString& p_sValue)
{
	wxString l_sValue = p_sValue;

	l_sValue.Trim();

	if(l_sValue.IsEmpty()){ return; }

	wxStringTokenizer l_cTokenizer(l_sValue, wxT(";"), wxTOKEN_RET_EMPTY_ALL);

	SP_VectorString l_vLine;

	while(l_cTokenizer.HasMoreTokens())
	{
		wxString l_sToken = l_cTokenizer.GetNextToken();

		if(l_sToken == SP_DS_DISCRETE_PLACE
			|| l_sToken == SP_DS_DISCRETE_TRANS
			|| l_sToken == SP_DS_PARAM
			|| SP_DS_META_CONSTANT.StartsWith(l_sToken))
		{
			m_sClassname = l_sToken;
		}
		l_vLine.push_back(l_sToken);
	}

	if(!m_sClassname.IsEmpty())
		m_mImportValues[m_sClassname].push_back(l_vLine);
}

void
SP_ImportCSV2SPN::CheckImportValues()
{
	if(GetErrorSet()) return;

	if(m_mImportValues.empty())
	{
		wxString l_sMsg = wxT("Error: No values to import!");
		SP_MESSAGEBOX(l_sMsg, wxT("Error"));
		SetLastError(l_sMsg);
		return;
	}

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	SP_MapImportCSV::const_iterator l_itImport;
	for(l_itImport = m_mImportValues.begin();
		l_itImport != m_mImportValues.end();
		++l_itImport)
	{
		wxString l_sClassname = l_itImport->first;
		if(l_sClassname != SP_DS_DISCRETE_PLACE
			&& l_sClassname != SP_DS_DISCRETE_TRANS
			&& l_sClassname != SP_DS_PARAM
			&& !SP_DS_META_CONSTANT.StartsWith(l_sClassname))
		{
			wxString l_sMsg = wxT("Error: unknown classname \"");
			l_sMsg << l_sClassname << wxT("\"!");
			SP_MESSAGEBOX(l_sMsg, wxT("Error"));
			SetLastError(l_sMsg);
			return;
		}

		const SP_Vector2DString& l_vValues = l_itImport->second;
		size_t l_nColCount = l_vValues[0].size();

		for(size_t i = 1; i < l_vValues.size(); ++i)
		{
			const SP_VectorString& l_vRow = l_vValues[i];
			if(l_nColCount != l_vRow.size())
			{
				wxString l_sMsg = wxT("Error: In nodeclass \"");
				l_sMsg << l_sClassname << wxT("\" line ") << i << wxT(", number of cols differs from ") << l_nColCount << wxT(" to ") << l_vRow.size();
				SP_MESSAGEBOX(l_sMsg, wxT("Error"));
				SetLastError(l_sMsg);
				return;
			}
			bool l_bError = false;
			if(l_sClassname == SP_DS_DISCRETE_PLACE)
			{
				if(m_mPlaces.find(l_vRow[0]) == m_mPlaces.end())
				{
					l_bError = true;
				}
			}
			else if(l_sClassname == SP_DS_DISCRETE_TRANS)
			{
				if(m_mTransitions.find(l_vRow[0]) == m_mTransitions.end())
				{
					l_bError = true;
				}
			}
			else if(l_sClassname == SP_DS_PARAM
					|| SP_DS_META_CONSTANT.StartsWith(l_sClassname))
			{
				if(m_mParameters.find(l_vRow[0]) == m_mParameters.end())
				{
					if (mc)
					{
						SP_DS_Metadata* l_constant = mc->NewElement(1);
						l_constant->GetAttribute(wxT("Name"))->SetValueString(l_vRow[0]);
					}
				}
			}
			if(l_bError)
			{
				wxString l_sMsg = wxT("Error: ");
				l_sMsg << l_sClassname << wxT(" \"") << l_vRow[0] << wxT("\" not found!");
				SP_MESSAGEBOX(l_sMsg, wxT("Error"));
				SetLastError(l_sMsg);
				return;
			}
		}

	}
}

void
SP_ImportCSV2SPN::SetImportValues()
{
	if(GetErrorSet()) return;

	bool l_bOverride = true;

	SP_MapImportCSV::const_iterator l_itImport;
	for(l_itImport = m_mImportValues.begin();
		l_itImport != m_mImportValues.end();
		++l_itImport)
	{
		wxString l_sClassname = l_itImport->first;

		const SP_Vector2DString& l_vValues = l_itImport->second;

		const SP_VectorString& l_vColHead = l_vValues[0];

		for(size_t i = 1; i < l_vValues.size(); ++i)
		{
			const SP_VectorString& l_vRow = l_vValues[i];

			if(l_sClassname == SP_DS_DISCRETE_PLACE)
			{
				SP_DS_Attribute* l_pcAttr = m_mPlaces[l_vRow[0]];
				wxString l_sValue = l_vRow[1];
				l_pcAttr->SetValueString(l_sValue);
			}
			else
			{
				SP_DS_ColListAttribute* l_pcAttr = NULL;
				if(l_sClassname == SP_DS_DISCRETE_TRANS)
				{
					l_pcAttr = m_mTransitions[l_vRow[0]];
				}
				else if(l_sClassname == SP_DS_PARAM)
				{
					l_pcAttr = m_mParameters[l_vRow[0]];
				}
				else if(l_sClassname == SP_DS_META_CONSTANT)
				{
					l_pcAttr = m_mParameters[l_vRow[0]];
				}
				if(l_pcAttr)
				{
					if(l_bOverride)
					{
						l_pcAttr->Clear();
						for(size_t j = 1; j < l_vRow.size(); ++j)
						{
							unsigned int l_nRow = l_pcAttr->AppendEmptyRow();
							l_pcAttr->SetCell(l_nRow, 0, l_vColHead[j]);
							wxString l_sValue = l_vRow[j];
							//special handling for transitions
							if(l_sValue.StartsWith(wxT("ImmediateFiring(")))
							{
								wxString l_sNewValue = l_sValue.AfterFirst('(');
								l_sNewValue = l_sNewValue.BeforeLast(')');
								l_pcAttr->SetCell(l_nRow, 1, l_sNewValue);
							}
							else if(l_sValue.StartsWith(wxT("TimedFiring(")))
							{
								wxString l_sNewValue = l_sValue.AfterFirst('(');
								l_sNewValue = l_sNewValue.BeforeLast(')');
								l_pcAttr->SetCell(l_nRow, 1, l_sNewValue);
							}
							else if(l_sValue.StartsWith(wxT("FixedTimedFiring_Periodic(")))
							{
								wxString l_sNewValue = l_sValue.AfterFirst('(');
								l_pcAttr->SetCell(l_nRow, 1, l_sNewValue.BeforeFirst(','));
								l_sNewValue = l_sNewValue.AfterFirst(',');
								l_pcAttr->SetCell(l_nRow, 2, l_sNewValue.BeforeFirst(','));
								l_sNewValue = l_sNewValue.AfterFirst(',');
								l_pcAttr->SetCell(l_nRow, 3, l_sNewValue.BeforeFirst(')'));
							}
							else
							{
								l_pcAttr->SetCell(l_nRow, 1, l_sValue);
							}
						}
					}
					else
					{

					}
				}
			}
		}
	}
	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	l_pcFR->LoadFromNet(m_pcGraph);

	m_pcGraph->GetParentDoc()->Modify(true);
}

void
SP_ImportCSV2SPN::LoadPlaces()
{
	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Place") );

	m_mPlaces.clear();

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

//		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("Marking")));
//		wxASSERT_MSG(l_pcAttr, wxT("MarkingList not found!"));
		SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Marking"));
		m_mPlaces[l_sPlaceName] = l_pcAttr;
	}
}

void
SP_ImportCSV2SPN::LoadTransitions()
{
	m_mTransitions.clear();

	SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Transition") );

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
		wxASSERT_MSG(l_pcAttr, wxT("FunctionList not found!"));
		m_mTransitions[l_sName] = l_pcAttr;
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Immediate Transition") );

	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("FunctionList")));
		wxASSERT_MSG(l_pcAttr, wxT("FunctionList not found!"));
		m_mTransitions[l_sName] = l_pcAttr;
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Deterministic Transition") );

	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("DelayList")));
		wxASSERT_MSG(l_pcAttr, wxT("DelayList not found!"));
		m_mTransitions[l_sName] = l_pcAttr;
	}

	l_pcNodeclass = m_pcGraph->GetNodeclass( wxT("Scheduled Transition") );

	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem)
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcNode->GetAttribute(wxT("PeriodicList")));
		wxASSERT_MSG(l_pcAttr, wxT("PeriodicList not found!"));
		m_mTransitions[l_sName] = l_pcAttr;
	}
}

void
SP_ImportCSV2SPN::LoadParameters()
{
	SP_DS_Metadataclass* l_pcMC = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	m_mParameters.clear();

	SP_ListMetadata::const_iterator l_itElem;
	for(l_itElem = l_pcMC->GetElements()->begin();
		l_itElem != l_pcMC->GetElements()->end();
		++l_itElem )
	{
		SP_DS_Metadata* l_pcNode = (*l_itElem);
		wxString l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( l_pcNode->GetAttribute( wxT("ValueList") ) );
		wxASSERT_MSG(l_pcColList, wxT("ValueList not found!"));
		m_mParameters[l_sParameterName] = l_pcColList;
	}

}
