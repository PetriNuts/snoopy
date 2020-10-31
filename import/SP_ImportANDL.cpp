//////////////////////////////////////////////////////////////////////
// $Author: george assaf $
// $Version: 0.1 $
// $Date: 2020/10/27 $
////////////////////////////////////////////////////////////////////

#include "import/SP_ImportANDL.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_core/base/SP_Graphic.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "snoopy.h"
#include "sp_utilities.h"
#include "dssd/misc/net_reader.h"
#include <wx/spinctrl.h>
#include <algorithm>
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
#include "sp_gui/dialogs/SP_DLG_OverwriteDeclarations.h"

enum {
	SP_ID_CHECK_SELECTIVE,
	SP_ID_IS_NEW_DOC,
	SP_ID_CONSTANTS_TESXT,
	SP_ID_CHECK_ALL,
	SP_ID_DECLARATIONS_CONSTANTS,
	SP_ID_FUNS_DEF,
	SP_ID_DECLARATIONS_UPDATE,
	SP_ID_DECLARATIONS_OBSERVERS,
	SP_ID_CONSTANTS_OBSERVERS,
};

SP_ImportANDL::SP_ImportANDL()
{
	m_bIsNewDoc = true;
}

SP_ImportANDL::~SP_ImportANDL()
{

	for (auto p : m_vFunDependenciesVector)
	{
		delete p;
	}
	for (auto p : m_vObserversDependenciesVector)
	{
		delete p;
	}

	for (auto p : m_vConstDependenciesVector)
	{
		delete p;
	}
}

bool SP_ImportANDL::ReadFile(const wxString& p_sFile, SP_ImportRoutine* p_importRT)
{
	m_fileName = p_sFile;
	dssd::net::reader p;
	bool l_Return = false;
	try
	{
		l_Return = p(p_sFile.ToStdString());
		if (l_Return)
		{
			auto l_Net = p.get();

			/********************/
			dssd::andl::Net_ptr l_plainNet = p.get();
			SP_DS_Graph_Declarations l_CheckDec(l_plainNet, true);

			 l_CheckDec();//build dependency trees
			 m_CheckDec = l_CheckDec;
			 m_vConstDependenciesVector = l_CheckDec.GetConstantsDependencyVector();
			 m_vFunDependenciesVector = l_CheckDec.GetFunctionDependencyVector();

			SP_DLG_ImportProperties*  l_pcDialog = new SP_DLG_ImportProperties(p_importRT, NULL, l_plainNet, p_sFile);
			if (l_pcDialog->ShowModal() == wxID_OK)
			{
				for (int i = 0; i < m_Options_constants.size(); i++)
				{
					if (m_pcRearrangelist_constants->IsChecked(i))
					{
						l_finalconstants.Add(m_Options_constants[i]);
					}
				}

				for (int i = 0; i < m_Options_funs.size(); i++)
				{
					if (m_pcRearrangelist_function->IsChecked(i))
					{
						l_finalFunSel.Add(m_Options_funs[i]);
					}
				}
				for (int i = 0; i < m_Options_observers.size(); i++)
				{
					if (m_pcRearrangelist_observers->IsChecked(i))
					{
						l_finalobservers.Add(m_Options_observers[i]);
					}
				}
				if (!m_bIsNewDoc)
				{
					if (AddToGraph(*l_Net))
					{
						ClearAll();
						return true;
					}
					else
					{
						SP_MESSAGEBOX(wxT("There is no an opend PN document!!"), wxT("Error"));
						m_bIsNewDoc = true;
						return false;
					}
				}

			}
			else
			{
				ClearAll();
				return false;
			}

			wxString l_sMsg = wxT("parse successful\n");
			SP_LOGMESSAGE(l_sMsg);
			l_Return = CreateGraph(p_sFile, *l_Net);
            if(l_Return)
    			doLayout();
		}
	}
	catch(const std::exception& e)
	{
		wxString l_sMsg = wxT("While parsing ") + p_sFile;
		SP_LOGERROR(l_sMsg);
		SP_LOGERROR(wxString(e.what()));
	}

	ClearAll();
	return l_Return;
}

void SP_ImportANDL::doLayout()
{
	SP_DLG_LayoutProperties *l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();
}

void SP_ImportANDL::ClearAll()
{
	lookupParameters.clear();
	lookupPlaces.clear();
	lookupTransitions.clear();
	arcs.clear();

	l_finalFunSel.clear();
	l_finalconstants.clear();
	l_finalobservers.clear();
	m_mGroup2Const.clear();
	m_mType2Ob.clear();
	m_mType2Declartions.clear();
	if (p_pcDlg)
	{
		if (m_pcTextconstants)
		{
			m_pcTextconstants->SetValue("");
			wxDELETE(m_pcTextfunctions);
		}

		if (m_pcTextfunctions)
		{
			m_pcTextfunctions->SetValue("");
			wxDELETE(m_pcTextfunctions);
		}

		wxDELETE(p_pcDlg);
	}
	m_bIsNewDoc = true;
}

void SP_ImportANDL::AddConstants()
{
	m_pcNotebookPageConstants = p_pcDlg->AddPage(wxT("constants"));
	m_pcMainSizer_constants = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_constants = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageConstants, -1, wxT("Constants")), wxVERTICAL);
	m_pcRightSizer_constants = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_constants = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageConstants, -1, wxT("constants definitions")), wxVERTICAL);
	m_pcTextconstants = new wxTextCtrl(m_pcNotebookPageConstants, SP_ID_CONSTANTS_TESXT, "", wxDefaultPosition, wxSize(300, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_constants->Add(m_pcLeftSizer_constants, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_constants->Add(m_pcRightSizer_constants, 1, wxALL | wxEXPAND, 5);

	m_pcCheckAll = new wxCheckBox(p_pcDlg, SP_ID_CHECK_ALL, wxT("Select all"), wxDefaultPosition);
	m_pcCheckAll->SetValue(true);
	p_pcDlg->AddCheckBoxSelectAll(m_pcCheckAll);
	m_pcCheckAll->Bind(wxEVT_CHECKBOX, &SP_ImportANDL::ONselectAll, this, SP_ID_CHECK_ALL);
	m_pcCheckAll->Enable(false);

	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);
	AddConstants_Att();

	m_pcRearrangelist_constants = new wxRearrangeList(m_pcNotebookPageConstants, SP_ID_DECLARATIONS_CONSTANTS, wxDefaultPosition,
		wxSize(40, 175), m_Options_constants_order, m_Options_constants);
	m_pcLeftSizer_constants->Add(m_pcRearrangelist_constants, 1, wxALL | wxEXPAND, 5);
	m_pcRearrangelist_constants->Enable(false);
	m_pcRightSizer_constants->Add(m_pcTextconstants);
	m_pcRearrangelist_constants->Bind(wxEVT_CHECKLISTBOX, &SP_ImportANDL::OnSelChange_Constants, this, SP_ID_DECLARATIONS_CONSTANTS);
	m_pcNotebookPageConstants->AddControl(m_pcMainSizer_constants, 1, wxALL | wxEXPAND, 5);
}

void SP_ImportANDL::AddFunctions()
{
	m_pcNotebookPageFunctions = p_pcDlg->AddPage(wxT("functions"));

	m_pcMainSizer_funs = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_funs = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageFunctions, -1, wxT("functions")), wxVERTICAL);
	m_pcRightSizer_funs = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_funs = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageFunctions, -1, wxT("function definitions")), wxVERTICAL);

	m_pcTextfunctions = new wxTextCtrl(m_pcNotebookPageFunctions, SP_ID_FUNS_DEF, "", wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_funs->Add(m_pcLeftSizer_funs, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_funs->Add(m_pcRightSizer_funs, 1, wxALL | wxEXPAND, 5);

	AddFunctions_Att();
	//Rearrangelist for reordering the sequence of Declarations
	m_pcRearrangelist_function = new wxRearrangeList(m_pcNotebookPageFunctions, SP_ID_DECLARATIONS_UPDATE, wxDefaultPosition,
		wxSize(50, 200), m_Options_funs_order, m_Options_funs);
	m_pcLeftSizer_funs->Add(m_pcRearrangelist_function, 1, wxALL | wxEXPAND, 5);

	m_pcRearrangelist_function->Enable(false);
	m_pcRightSizer_funs->Add(m_pcTextfunctions);
	m_pcRearrangelist_function->Bind(wxEVT_CHECKLISTBOX, &SP_ImportANDL::OnSelChange_funs, this, SP_ID_DECLARATIONS_UPDATE);
	m_pcNotebookPageFunctions->AddControl(m_pcMainSizer_funs, 1, wxALL | wxEXPAND, 5);
}


void SP_ImportANDL::AddObservers()
{
	m_pcNotebookPageObservers= p_pcDlg->AddPage(wxT("Observers"));

	m_pcMainSizer_observers = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_observers = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageObservers, -1, wxT("Observers")), wxVERTICAL);
	m_pcRightSizer_observers= new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_observers = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageObservers, -1, wxT("observers definitions")), wxVERTICAL);

	m_pcTextobservers = new wxTextCtrl(m_pcNotebookPageObservers, SP_ID_CONSTANTS_OBSERVERS, "", wxDefaultPosition, wxSize(300, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_observers->Add(m_pcLeftSizer_observers, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_observers->Add(m_pcRightSizer_observers, 1, wxALL | wxEXPAND, 5);

	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);
	AddObservers_Att();

	m_pcRearrangelist_observers = new wxRearrangeList(m_pcNotebookPageObservers, SP_ID_DECLARATIONS_OBSERVERS, wxDefaultPosition,
		wxSize(40, 175), m_Options_observers_order, m_Options_observers);
	m_pcLeftSizer_observers->Add(m_pcRearrangelist_observers, 1, wxALL | wxEXPAND, 5);

	m_pcRearrangelist_observers->Enable(false);


	m_pcRightSizer_observers->Add(m_pcTextobservers);


	m_pcRearrangelist_observers->Bind(wxEVT_CHECKLISTBOX, &SP_ImportANDL::OnSelChange_Observers, this, SP_ID_DECLARATIONS_OBSERVERS);


	m_pcNotebookPageObservers->AddControl(m_pcMainSizer_observers, 1, wxALL | wxEXPAND, 5);
}
/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////
bool SP_ImportANDL::AddToGraph(const dssd::andl::Net& p_Net)
{

	if (!SP_Core::Instance()->GetRootDocument())
	{
		return false;
	}

	m_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();



	if (!m_pcGraph)
		return false;

	if (m_pcGraph->GetNetclass()->GetName().Contains("Colored"))
	{
		return false;
	}
	if (p_Net.constants_ && p_Net.valuesets_)
		AppendConstants(*p_Net.constants_, *p_Net.valuesets_);


	if (p_Net.functions_)
		AppendFunctions(*p_Net.functions_);

	if (p_Net.observers_)
		AppendObservers(*p_Net.observers_);

	if (m_mType2Declartions.size() > 0)
	{

		SP_DLG_OverwriteDeclarations* l_pcOverwriteDlg = new SP_DLG_OverwriteDeclarations(NULL, wxT("Overwrite Declarations"), m_mType2Declartions);

		if (l_pcOverwriteDlg->ShowModal() == wxID_OK)
		{
			m_mType2Declartions.clear();
			m_mType2Declartions = l_pcOverwriteDlg->GetSelectedItems();

			if (m_mType2Declartions.size()>0)
			{
				Overwritedeclarations(m_mType2Declartions, p_Net);
			}
		}
	}

	m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();
	return true;

}

bool SP_ImportANDL::AppendConstants(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets)
{
	struct temp_constant {
		wxString constname;
		wxString consttype;
		wxString constGroup;
		wxString constcomment;
		vector<wxString> vsets;
	};

	vector<temp_constant> l_vConstantsInGraph;
	SP_DS_Metadataclass*	l_pcConstantsOrgin = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	vector<wxString> l_vSetNAmes;
	int l_nOrgSize = l_pcConstantsOrgin->GetElements()->size();
	int k = 0;

	if (l_pcConstantsOrgin->GetElements()->size() > 0)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(l_pcConstantsOrgin->GetElements()))
		{
			k++;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sGroup = l_pcMetadata->GetAttribute(wxT("Group"))->GetValueString();
			wxString l_sComment = l_pcMetadata->GetAttribute(wxT("Comment"))->GetValueString();
			wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();


			SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));
			vector<wxString> l_vSets;
			for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
			{
				l_vSets.push_back(l_pcColList1->GetCell(i, 1));
				if (k == 1)
				{
					l_vSetNAmes.push_back(l_pcColList1->GetCell(i, 0));
				}
			}

			temp_constant l_const;
			l_const.constname = l_sName;
			l_const.constGroup = l_sGroup;
			l_const.consttype = l_sType;
			l_const.constcomment = l_sComment;
			l_const.vsets = l_vSets;
			l_vConstantsInGraph.push_back(l_const);
		}
	}

	for (auto& constant : p_Constants)
	{
		bool l_bFound = false;
		for (int i = 0; i < l_vConstantsInGraph.size(); i++)
		{

			if (constant->name_ == l_vConstantsInGraph[i].constname)
			{
				l_bFound = true;
				m_mType2Declartions[wxT("constant")].insert(constant->name_);
				continue;
			}
		}

		if (!l_bFound && l_finalconstants.Index(constant->name_) != wxNOT_FOUND)
		{
			temp_constant l_const;
			l_const.constname = constant->name_;
			l_const.constGroup = constant->group_;
			if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
				l_const.consttype = wxT("double");
			else
				l_const.consttype = wxT("int");

			vector<wxString> vsets;
			for (auto& ss : constant->values_)
			{
				vsets.push_back(ss);
			}
			l_const.vsets = vsets;
			l_vConstantsInGraph.push_back(l_const);
		}
	}

	SP_DS_Metadataclass*	l_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);


	//remove old constants
	if (l_pcConstants->GetElements()->size() > 0)
	{
		l_pcConstants->RemoveAll();

	}

	//add new constants with number of temporarliy stored ones
	for (int i = 0; i < l_vConstantsInGraph.size(); i++)
	{

		l_pcConstants->NewElement(1);

	}
	SP_ListMetadata::const_iterator l_itElem;
	l_itElem = l_pcConstants->GetElements()->begin();
	SP_DS_Metadata* l_pcConstant;
	for (int i = 0; i < l_vConstantsInGraph.size(); i++)
	{
		l_pcConstant = *l_itElem;

		wxString l_sName = l_vConstantsInGraph[i].constname;
		wxString l_sGroup = l_vConstantsInGraph[i].constGroup;
		wxString l_sType = l_vConstantsInGraph[i].consttype;
		wxString l_sComment = l_vConstantsInGraph[i].constcomment;

		l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_sName);
		l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_sGroup);
		l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

		bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_sType);


		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));

		l_pcColList->Clear();
		for (int j = 0; j < l_vConstantsInGraph[i].vsets.size(); j++)
		{
			int l_nRowCol = l_pcColList->AppendEmptyRow();
			wxString l_sVsetMain = wxT("Main");
			l_pcColList->SetCell(l_nRowCol, 1, l_vConstantsInGraph[i].vsets[j]);
			if (l_vSetNAmes.size() == 1)
				l_pcColList->SetCell(l_nRowCol, 0, l_sVsetMain);
			else
			{
				if (l_vConstantsInGraph[i].vsets.size() == l_vSetNAmes.size())
					l_pcColList->SetCell(l_nRowCol, 0, l_vSetNAmes[j]);
			}

		}

		wxString l_sMainVal = wxT("0");
		if (l_vConstantsInGraph[i].vsets.size()>0)
			l_sMainVal = l_vConstantsInGraph[i].vsets[0];

		SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
		SP_FunctionPtr l_pcFunction(l_pcFR->parseFunctionString(l_sMainVal));
		if (l_sType == wxT("double"))
		{
			double l_Val = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_pcFunction, std::numeric_limits<double>::min() }();
			wxString l_sDouble;
			l_sDouble << l_Val;
			m_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sDouble);
			l_pcColList->UpdateActiveListColumnPtr();

			l_pcConstant->SetShow(true);
			l_pcConstant->Update();

			++l_itElem;
			continue;
		}

		m_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, l_sMainVal);

		l_pcColList->UpdateActiveListColumnPtr();

		l_pcConstant->SetShow(true);
		l_pcConstant->Update();

		++l_itElem;

	}
	return true;
}

bool SP_ImportANDL::AppendFunctions(const dssd::andl::Functions& p_Functions)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if (mc)
	{
		for (auto& func : p_Functions)
		{
			if (!func) continue;

			wxString name = func->name_;
			wxString param = func->param_;
			wxString body = func->body_;

			if (l_finalFunSel.Index(name, true) == wxNOT_FOUND) { continue; }

			bool l_bIsExist = false;
			bool l_bOverwrite = false;

			SP_DS_Metadataclass* l_pcMetadataclass;
			l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
			if (!l_pcMetadataclass)
				return false;
			SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
			if (l_pcMetadataclass)
			{
				if (l_pcMetadataclass)
				{
					SP_ListMetadata::const_iterator l_itElem;
					for (l_itElem = l_pcMetadataclass->GetElements()->begin();
						l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
					{
						SP_DS_Metadata* l_pcMetadata = *l_itElem;
						wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
						wxString l_sParam = l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString();
						wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

						if (l_sName == name)
						{
							l_bIsExist = true;
							break;
						}
					}
				}
			}


			if (l_bIsExist)
			{
				m_mType2Declartions[wxT("function")].insert(name);
				continue;

			}
				SP_DS_Metadata* l_Func = mc->NewElement(1);
				l_Func->GetAttribute(wxT("Name"))->SetValueString(name);
				l_Func->GetAttribute(wxT("Parameter"))->SetValueString(param);
				l_Func->GetAttribute(wxT("Body"))->SetValueString(body);

		}
	}
	return true;
}

bool SP_ImportANDL::AppendObservers(const dssd::andl::Observers& p_Observers)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (mc)
	{
		for (auto& obs : p_Observers)
		{
			if (!obs) continue;

			wxString name = obs->name_;

			//if the constant is not chosen, then ignore it
			if (l_finalobservers.Index(name, true) == wxNOT_FOUND) { continue; }//by george

			bool l_bIsExist = false;
			for (auto mc1 : *mc->GetElements())
			{
				wxString l_sObInNet=mc1->GetAttribute(wxT("Name"))->GetValueString();
				if (l_sObInNet == name)
				{
					l_bIsExist = true;
					m_mType2Declartions[wxT("observer")].insert(name);
					break;
				}
			}

			if (l_bIsExist) continue;

			wxString type = wxT("Place");
			if (obs->type_ == dssd::andl::ObserverType::TRANS_T) {
				type = wxT("Transition");
			}
			wxString body = obs->function_;
			SP_DS_Metadata* l_Obs = mc->NewElement(1);
			l_Obs->GetAttribute(wxT("Name"))->SetValueString(name);
			l_Obs->GetAttribute(wxT("Type"))->SetValueString(type);
			l_Obs->GetAttribute(wxT("Body"))->SetValueString(body);
		}
	}

	return true;
}

bool SP_ImportANDL::Overwritedeclarations(const std::map<wxString, std::set<wxString>>& p_mType2Dec, const dssd::andl::Net& p_Net)
{
	for (auto itMap = p_mType2Dec.begin(); itMap != p_mType2Dec.end(); ++itMap)
	{
		if (itMap->first == wxT("constant"))
		{
			for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
			{
				OverwriteExistConstant(*itSet, p_Net);

			}
		}

		if (itMap->first == wxT("function"))
		{
			for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
			{
				OverwriteExistFunction(*itSet, p_Net);

			}
		}
		if (itMap->first == wxT("observer"))
		{
			for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
			{
				OverwriteExistObserver(*itSet, p_Net);

			}
		}
	}
	return true;
}

bool SP_ImportANDL::OverwriteExistConstant(const wxString& p_sConstant, const dssd::andl::Net& p_Net)
{

	SP_DS_Metadataclass*	l_pcConstantsOrgin = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);

	if (l_pcConstantsOrgin->GetElements()->size() > 0)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(l_pcConstantsOrgin->GetElements()))
		{

			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();

			if (l_sName.IsSameAs(p_sConstant))
			{
				for (auto& con : *p_Net.constants_)
				{
					if (con->name_ == l_sName)
					{
						l_pcMetadata->GetAttribute(wxT("Group"))->SetValueString(con->group_.c_str());
						l_pcMetadata->GetAttribute(wxT("Comment"))->SetValueString(wxT(""));
						if (con->type_ == dssd::andl::ConstType::INT_T)
							dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->SetValueString(wxT("int"));
						else
							dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->SetValueString(wxT("double"));

						SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));

						if (l_pcColList)
						{
							if (con->values_.size() == l_pcColList->GetRowCount())
							{
								for (int j = 0; j < con->values_.size(); j++)
								{
									wxString l_sVal = con->values_[j].c_str();
									l_pcColList->SetCell(j, 1, l_sVal);
								}
								m_pcGraph->GetFunctionRegistry()->registerFunction(l_sName, con->values_[0].c_str());
								l_pcColList->UpdateActiveListColumnPtr();
								l_pcMetadata->Update();
								return true;
							}
							else {
								wxString l_sMessage;
								l_sMessage << wxT("contant ") << p_sConstant << wxT("cannot be overwritten. ");
								l_sMessage << wxT("The number of value sets is not equal");
								SP_LOGMESSAGE_(l_sMessage);
							}
						}
					}
				}

			}
		}
	}
	return false;
}

bool SP_ImportANDL::OverwriteExistFunction(const wxString& p_sFunction, const dssd::andl::Net& p_Net)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if (mc)
	{
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = mc->GetElements()->begin();
				l_itElem != mc->GetElements()->end(); ++l_itElem)
				{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
				//wxString l_sParam = l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString();
				//wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

						if (l_sName == p_sFunction)
						{
							for (auto& fun : *p_Net.functions_)
							{
								if (l_sName != fun->name_.c_str()) continue;

								l_pcMetadata->GetAttribute(wxT("Parameter"))->SetValueString(fun->param_.c_str());
								l_pcMetadata->GetAttribute(wxT("Body"))->SetValueString(fun->body_.c_str());
								return true;
							}
						}
					}
		}

	return false;
}


bool SP_ImportANDL::OverwriteExistObserver(const wxString& p_sObserver, const dssd::andl::Net& p_Net)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (mc)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
		{
			wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			if (l_sMetadataName == p_sObserver)
			{
				for (auto& observer : *p_Net.observers_)
				{
					if (observer->name_.c_str() == p_sObserver)
					{
						wxString type;
						if (observer->type_ == dssd::andl::ObserverType::PLACE_T)
						{
							type = wxT("Place");
						}
						else
						{
							type = wxT("Transition");
						}
						dynamic_cast<SP_DS_EquationAttribute*>(l_pcMetadata->GetAttribute(wxT("Body")))->SetValueString(observer->function_.c_str());
						dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->SetValueString(type);
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool SP_ImportANDL::CreateGraph(const wxString& p_sFile, const dssd::andl::Net& p_Net)
{
	x = 350.0;
	y = 250.0;
	SP_LOGDEBUG(wxT("Creating Graph"));

	m_eNetType = p_Net.type_;
	m_pcGraph = CreateDocument(p_sFile, p_Net.type_);

	if(!m_pcGraph)
		return false;

	if(p_Net.functions_)
		CreateFunc(*p_Net.functions_);
	if(p_Net.constants_ && p_Net.valuesets_)
		CreateConst(*p_Net.constants_, *p_Net.valuesets_);

	SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();
	l_pcFR->LoadFromNet(m_pcGraph);

    if(p_Net.places_)
		CreatePlaces(*p_Net.places_);
	if(p_Net.observers_)
		CreateObservers(*p_Net.observers_);
	if(p_Net.transitions_)
		CreateTransitions(*p_Net.transitions_);
	CreateArcs();

	m_pcGraph->CreateDeclarationTree()->UpdateOtherTree();

    return true;
}

bool SP_ImportANDL::CreateFunc(const dssd::andl::Functions& p_Functions)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if (mc)
	{
		for(auto& func : p_Functions)
		{
			if(!func) continue;

			wxString name = func->name_;
			wxString param = func->param_;
			wxString body = func->body_;

			if (l_finalFunSel.Index(name, true) == wxNOT_FOUND) { continue; }//by george

			SP_DS_Metadata* l_Func = mc->NewElement(1);
			l_Func->GetAttribute(wxT("Name"))->SetValueString(name);
			l_Func->GetAttribute(wxT("Parameter"))->SetValueString(param);
			l_Func->GetAttribute(wxT("Body"))->SetValueString(body);
		}
	}

	return true;
}

bool SP_ImportANDL::CreatePlaces(const dssd::andl::Places& p_Places)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dssd::andl::NetType::CPN_T)
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	}
	else
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	}
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_node;
	for (auto& p : p_Places)
	{
		if(!p) continue;

		if (lookupPlaces.find(p->name_) != lookupPlaces.end())
		{
			continue;
			//return false;
		}
		if (p->type_ == dssd::andl::PlType::DISCRETE_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
		}
		else if (p->type_ == dssd::andl::PlType::CONTINUOUS_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
		}

		wxString name = p->name_;
		wxString marking = p->marking_;
		l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
		l_node->GetAttribute(wxT("Name"))->SetValueString(name);
		l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
		SP_DS_Attribute* l_pcAttr = l_node->GetAttribute(wxT("Marking"));
		if (l_pcAttr)
		{
			l_pcAttr->SetValueString(marking);
		}
		if (p->fixed_)
		{
			l_pcAttr = l_node->GetAttribute(wxT("Fixed"));
			if (l_pcAttr)
			{
				l_pcAttr->SetValueString(wxT("1"));
			}
		}
		l_node->ShowOnCanvas(l_pcCanvas, FALSE, x, y, 0);
		lookupPlaces[p->name_] = l_node;
	}
	return true;
}

bool SP_ImportANDL::CreateConst(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if (mc)
	{
		SP_DS_Metadata* l_constant;
		for (auto constant : p_Constants)
		{
			if(!constant) continue;

			wxString name = constant->name_;
			wxString type = wxT("int");

			//if the constant is not chosen, then ignore it
			if (l_finalconstants.Index(name, true) == wxNOT_FOUND) { continue; }//by george

			if(constant->type_ == dssd::andl::ConstType::DOUBLE_T)
			{
				type = wxT("double");
			}
			l_constant = mc->NewElement(1);
			l_constant->GetAttribute(wxT("Name"))->SetValueString(name);
			wxString group = wxT("all");
			if(!constant->group_.empty())
				group = constant->group_;

			l_constant->GetAttribute(wxT("Group"))->SetValueString(group);
			l_constant->GetAttribute(wxT("Type"))->SetValueString(type);

			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
			if(p_Valuesets.size() <= 1)
			{
				if(!constant->values_.empty())
					l_pcAttr->SetCell(0, 1, constant->values_[0]);
			}
			else
			{
				l_pcAttr->Clear();
				for(size_t i = 0; i < p_Valuesets.size(); ++i)
				{
					l_pcAttr->AppendEmptyRow();
					wxString vset = p_Valuesets[i];
					l_pcAttr->SetCell(i, 0, vset);
					if(i < constant->values_.size())
					{
						wxString value = constant->values_[i];
						l_pcAttr->SetCell(i, 1, value);
					}
				}
			}
            l_pcAttr->UpdateActiveListColumnPtr();
		}
	}

	return true;
}

bool SP_ImportANDL::CreateObservers(const dssd::andl::Observers & p_Observers)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (mc)
	{
		for(auto& obs : p_Observers)
		{
			if(!obs) continue;

			wxString name = obs->name_;

			//if the constant is not chosen, then ignore it
			if (l_finalobservers.Index(name, true) == wxNOT_FOUND) { continue; }//by george

			wxString type = wxT("Place");
			if(obs->type_ == dssd::andl::ObserverType::TRANS_T) {
				type = wxT("Transition");
			}
			wxString body = obs->function_;
			SP_DS_Metadata* l_Obs = mc->NewElement(1);
			l_Obs->GetAttribute(wxT("Name"))->SetValueString(name);
			l_Obs->GetAttribute(wxT("Type"))->SetValueString(type);
			l_Obs->GetAttribute(wxT("Body"))->SetValueString(body);
		}
	}

	return true;
}

bool SP_ImportANDL::CreateTransitions(const dssd::andl::Transitions& p_Transitions)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dssd::andl::NetType::CPN_T)
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
	}
	else
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
	}
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_node;

	for (auto& t : p_Transitions)
	{
		if(!t) continue;

		if (lookupTransitions.find(t->name_) != lookupTransitions.end())
		{
			continue;
			//return false;
		}

		if (t->type_ == dssd::andl::TrType::STOCHASTIC_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
		}
		else if (t->type_ == dssd::andl::TrType::IMMEDIATE_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
		}
		else if (t->type_ == dssd::andl::TrType::DETERMINISTIC_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
		}
		else if (t->type_ == dssd::andl::TrType::SCHEDULED_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
		}
		else if (t->type_ == dssd::andl::TrType::CONTINUOUS_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
		}

		wxString name = t->name_;
		l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
		l_node->GetAttribute(wxT("Name"))->SetValueString(name);
		l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);

		if(t->reversible_)
		{
			SP_DS_Attribute* l_pcAttr = l_node->GetAttribute(wxT("Reversible"));
			if(l_pcAttr)
			{
				l_pcAttr->SetValueString(wxT("1"));
			}
		}

		if (m_eNetType == dssd::andl::NetType::SPN_T
			|| m_eNetType == dssd::andl::NetType::GSPN_T
			|| m_eNetType == dssd::andl::NetType::XSPN_T
			|| m_eNetType == dssd::andl::NetType::CPN_T
			|| m_eNetType == dssd::andl::NetType::HPN_T)
		{
			wxString function = wxT("1");
			if (!t->function_.empty())
			{
				function = t->function_;
			}
			if (nodeClass->GetName() == SP_DS_DETERMINISTIC_TRANS)
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("DelayList")));
				l_pcAttr->SetCell(0, 1, function);
			}
			else if (nodeClass->GetName() == SP_DS_SCHEDULED_TRANS)
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("PeriodicList")));
				wxArrayString vals = wxStringTokenize(function, wxT(","));
				l_pcAttr->SetCell(0, 1, vals[0]);
				l_pcAttr->SetCell(0, 2, vals[1]);
				l_pcAttr->SetCell(0, 3, vals[2]);
			}
			else
			{
				SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("FunctionList")));
				l_pcAttr->SetCell(0, 1, function);
			}
		}
		l_node->ShowOnCanvas(l_pcCanvas, FALSE, x, y, 0);

		if(t->conditions_)
		for (auto& c : *(t->conditions_))
		{
			if(!c) continue;

			Weights& w = arcs[make_pair(c->place_, t->name_)];
			w.place_ = lookupPlaces[c->place_];
			if (!w.place_)
			{
				wxString msg = wxT("unknown place: ");
				msg << c->place_;
				SP_LOGERROR(msg);
				continue;
			}
			w.trans_ = l_node;

			switch (c->type_)
			{
				case dssd::andl::CondType::READ_T:
					w.vr_ = c->value_;
					break;
				case dssd::andl::CondType::INHIBITOR_T:
					w.vi_ = c->value_;
					break;
				case dssd::andl::CondType::EQUAL_T:
					w.vr_ = c->value_;
					w.vi_ = c->value_;
					break;
				case dssd::andl::CondType::MODIFIER_T:
					w.modifier_ = true;
					break;
				default:
					SP_LOGWARNING(wxT("unknown condition of '") + c->place_ + wxT("' and '") + t->name_ + wxT("'"));
					break;
			}
		}

		if(t->updates_)
		for (auto& u : *(t->updates_))
		{
			if(!u) continue;

			Weights& w = arcs[make_pair(u->place_, t->name_)];
			w.place_ = lookupPlaces[u->place_];
			if (!w.place_)
			{
				wxString msg = wxT("unknown place: ");
				msg << u->place_;
				SP_LOGERROR(msg);
				continue;
			}
			w.trans_ = l_node;
			switch (u->type_) {
				case dssd::andl::UpdateType::DECREASE_T:
					w.vpt_ = u->value_;
					break;
				case dssd::andl::UpdateType::INCREASE_T:
					w.vtp_ = u->value_;
					break;
				case dssd::andl::UpdateType::ASSIGN_T:
				{
					w.reset_ = true;
					wxString val = u->value_;
					if (val != wxT("0"))
						w.vtp_ = w.vtp_.IsEmpty() ? val : w.vtp_ + "+" + val;
					break;
				}
				default:
					SP_LOGWARNING(wxT("unknown update of '") + u->place_ + wxT("' and '") + t->name_ + wxT("'"));
					break;
			}
		}
		lookupTransitions[t->name_] = l_node;
	}
	return true;
}

bool SP_ImportANDL::CreateArcs()
{
	map<pair<std::string, std::string>, Weights>::const_iterator aIt;

	for (aIt = arcs.begin(); aIt != arcs.end(); ++aIt)
	{
		const Weights& w = (*aIt).second;
		if (!w.vpt_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_, w.vpt_, SP_DS_EDGE);
		}
		if (!w.ve_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_, w.ve_, SP_DS_EQUAL_EDGE);
		}
		if (!w.vr_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_, w.vr_, SP_DS_READ_EDGE);
		}
		if (!w.vi_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_, w.vi_, SP_DS_INHIBITOR_EDGE);
		}
		if (w.modifier_)
		{
			CreateEdge(w.place_, w.trans_, wxT(""), SP_DS_MODIFIER_EDGE);
		}
		if (w.reset_)
		{
			CreateEdge(w.place_, w.trans_, wxT(""), SP_DS_RESET_EDGE);
		}
		if (!w.vtp_.IsEmpty())
		{
			CreateEdge(w.trans_, w.place_, w.vtp_, SP_DS_EDGE);
		}
	}
    return true;
}

SP_DS_Graph*
SP_ImportANDL::CreateDocument(const wxString& p_sFile, dssd::andl::NetType p_eType)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxString netName;
	if (m_eNetType == dssd::andl::NetType::PN_T)
	{
		netName = SP_DS_PN_CLASS;
	}
	else if (m_eNetType == dssd::andl::NetType::XPN_T)
	{
		netName = SP_DS_EXTPN_CLASS;
	}
	else if (m_eNetType == dssd::andl::NetType::SPN_T
			|| m_eNetType == dssd::andl::NetType::GSPN_T
			|| m_eNetType == dssd::andl::NetType::XSPN_T)
	{
		netName = SP_DS_SPN_CLASS;
	}
	else if (m_eNetType == dssd::andl::NetType::CPN_T)
	{
		netName = SP_DS_CONTINUOUSPED_CLASS;
	}
	else if (m_eNetType == dssd::andl::NetType::HPN_T)
	{
		netName = SP_DS_HYBRIDPN_CLASS;
	}
	else if (m_eNetType == dssd::andl::NetType::TPN_T)
	{
		netName = SP_DS_TIMEPN_CLASS;
	}
	else
	{
		SP_LOGERROR(wxT("net class not supported!"));
		return nullptr;
	}

	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(netName);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	m_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(((wxDocTemplate*) l_pcTemplate)->CreateDocument(wxT(""), 1));
	if (m_pcMyDoc)
	{
		m_pcMyDoc->OnNewDocument();
		m_pcMyDoc->InitGraph(newClass);
		l_pcGraph = m_pcMyDoc->GetGraph();
		m_pcView = dynamic_cast<SP_MDI_View*>(m_pcMyDoc->GetFirstView());
		m_pcCanvas = m_pcView->GetCanvas();
		m_pcMyDoc->DoSetModify(true, m_pcMyDoc);
	}
	return l_pcGraph;
}

bool SP_ImportANDL::CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight, const wxString& type)
{
	CHECK_POINTER(source, SP_LOGDEBUG(wxString(wxT("source is NULL"))));
	CHECK_POINTER(target, SP_LOGDEBUG(wxString(wxT("target is NULL"))));

	SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(type);
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Edge *l_edge = l_pcEC->GetPrototype()->Clone();
	l_edge->SetNetnumber(l_pcCanvas->GetNetnumber());
	l_edge->SetNodes(source, target);
	l_edge->SetNodes(*(source->GetGraphics()->begin()), *(target->GetGraphics()->begin()));
	SP_DS_Attribute* l_pcAttr = l_edge->GetAttribute(wxT("Multiplicity"));
	if (l_pcAttr)
		l_pcAttr->SetValueString(weight);
	l_pcEC->AddElement(l_edge);
	l_edge->ShowOnCanvas(l_pcCanvas, FALSE);
	CHECK_POINTER(l_edge->GetGraphics(), SP_LOGDEBUG(wxT("no graphics")));

    return true;
}


bool
SP_ImportANDL::AddToDialog(SP_DLG_ImportProperties* p_pcDlg, dssd::andl::Net_ptr net)
{
	SP_LOGMESSAGE("inside AddToDialog for SP_ExportReport");

	m_pcCheckSelectiveChoices = new wxCheckBox(p_pcDlg, SP_ID_CHECK_SELECTIVE, "Selective import");
	m_pcCheckSelectiveChoices->SetValue(false);

	m_pcCheckIsNewDoc = new wxCheckBox(p_pcDlg, SP_ID_IS_NEW_DOC, "New model");
	m_pcCheckIsNewDoc->SetValue(true);

	m_pcCheckSelectiveChoices->Bind(wxEVT_CHECKBOX, &SP_ImportANDL::OnSelectiveChoice, this, SP_ID_CHECK_SELECTIVE);
	m_pcCheckIsNewDoc->Bind(wxEVT_CHECKBOX, &SP_ImportANDL::OnIsNewDoc, this, SP_ID_IS_NEW_DOC);

	p_pcDlg->SetNotebookSize(400, 200);
	p_pcDlg->AddCheckBox(m_pcCheckSelectiveChoices);
	p_pcDlg->AddCheckBox(m_pcCheckIsNewDoc);


	this->p_pcDoc = net;
	this->p_pcDlg = p_pcDlg;



	 m_Options_constants.clear();
	 m_Options_constants_order.Clear();
	 m_Options_funs.clear();
	 m_Options_funs_order.clear();
 	 l_finalconstants.clear();
	 l_finalFunSel.clear();
	 m_Options_observers.clear();
	 m_Options_observers_order.clear();
	 l_finalobservers.clear();

	 AddConstants();
	AddFunctions();
	AddObservers();

	//CreateConstantsDepTrees();
	//CreateColorsetDepTrees();
	//BuildVariableDependencies();
	//BuildFunctionsDependencyTrees();
	//ComputeBackwardDependencies();
	//ComputeColorSetsBackwardDependencies();
//	ComputeConstantBackwardDependencies();


	return true;

}

void SP_ImportANDL::OnSelectiveChoice(wxCommandEvent& p_cEvent)
{
	if (m_pcCheckSelectiveChoices->IsChecked())
	{
		m_pcCheckAll->Enable(true);
		m_pcRearrangelist_function->Enable(true);
		m_pcRearrangelist_constants->Enable(true);
		m_pcRearrangelist_observers->Enable(true);
	}
	else
	{
		m_pcCheckAll->Enable(false);
		m_pcRearrangelist_function->Enable(false);
		m_pcRearrangelist_constants->Enable(false);
		m_pcRearrangelist_observers->Enable(false);

	}
}

void SP_ImportANDL::OnIsNewDoc(wxCommandEvent& p_cEvent)
{
	if (m_pcCheckIsNewDoc->IsChecked())
	{
		m_bIsNewDoc = true;

	}
	else
	{
		m_bIsNewDoc = false;
	}
}

void SP_ImportANDL::OnSelChange_Observers(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_observers->IsChecked(p_cEvent.GetInt());
	wxString l_sCheckedObserverName = m_Options_observers[p_cEvent.GetInt()];
	m_pcCheckAll->SetValue(false);
	if (!b)
	{//user uncheck the option
		bool l_bOverwrite = 0;

		for (auto itMap = m_mType2Ob.begin(); itMap != m_mType2Ob.end(); ++itMap)
		{
			if (!l_bOverwrite)
				m_pcTextobservers->SetValue(itMap->first + _T(":\n"));
			else
				m_pcTextobservers->AppendText(itMap->first + _T(":\n"));

			for (auto itMapOb = itMap->second.begin(); itMapOb != itMap->second.end(); ++itMapOb)
			{

				for (auto& observ : *p_pcDoc->observers_)
				{
					wxString name_(observ->name_.c_str(), wxConvUTF8);

					if (name_ == itMapOb->first)
					{

						int ind = m_Options_observers.Index(name_);
						if (!m_pcRearrangelist_observers->IsChecked(ind)) { continue; }

						wxString l_sObs;
						wxString l_sBody = observ->function_;
						l_sObs << name_ << wxT(" = ") << l_sBody << wxT(";\n");
						m_pcTextobservers->AppendText(l_sObs);

					}
				}
			}

			m_pcTextobservers->AppendText(wxT("\n"));

		}

	}
	else
	{
		bool l_bOverwrite = 0;
		for (auto itMap = m_mType2Ob.begin(); itMap != m_mType2Ob.end(); ++itMap)
		{
			if (!l_bOverwrite)
				m_pcTextobservers->SetValue(itMap->first + _T(":\n"));
			else
				m_pcTextobservers->AppendText(itMap->first + _T(":\n"));



			for (auto itMapOb = itMap->second.begin(); itMapOb != itMap->second.end(); ++itMapOb)
			{
				for (auto& observ : *p_pcDoc->observers_)
				{
					wxString name_(observ->name_.c_str(), wxConvUTF8);
					l_bOverwrite = true;
					if (name_ == itMapOb->first)
					{
						int ind = m_Options_observers.Index(name_);
						if (m_pcRearrangelist_observers->IsChecked(ind))

						{
							wxString l_sObs;
							wxString l_sBody = observ->function_;
							l_sObs << name_ << wxT(" = ") << l_sBody << wxT(";\n");
							m_pcTextobservers->AppendText(l_sObs);
						}
					}
				}
			}

			m_pcTextobservers->AppendText(wxT("\n"));

		}


	}

	if (b)
	{
		set<wxString> l_setDec;
		m_CheckDec.ComputeBackwardDependency(l_sCheckedObserverName, NODE_TYPE::OBSERVER, l_setDec);
		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);

			}
			else if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_funs.Index(*itSet);
				m_pcRearrangelist_function->Check(index, true);
			}

		}

	}
}
void SP_ImportANDL::OnSelChange_Constants(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_constants->IsChecked(p_cEvent.GetInt());

	m_pcCheckAll->SetValue(false);
	if (!b)
	{
		bool l_bOverwrite = 0;
		wxString l_sCheckedconstName = m_Options_constants[p_cEvent.GetInt()];
		for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
		{
			if(!l_bOverwrite)
			m_pcTextconstants->SetValue(itMap->first + _T(":\n"));
			else
			m_pcTextconstants->AppendText(itMap->first + _T(":\n"));
			for (unsigned i = 0; i < itMap->second.size(); i++)
			{
				l_bOverwrite = true;
				for (auto& constant : *p_pcDoc->constants_)
				{
					wxString name_(constant->name_.c_str(), wxConvUTF8);

					if (name_ == itMap->second[i])
					{
						int ind = m_Options_constants.Index(name_);
						if (!m_pcRearrangelist_constants->IsChecked(ind)) { continue; }
						if (name_ == l_sCheckedconstName) continue;

						wxString l_sConstDef;
						wxString l_sType;
						wxString l_sValues;
						if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
						{
							l_sType = wxT("double");
						}
						if (constant->type_ == dssd::andl::ConstType::INT_T)
						{
							l_sType = wxT("int");
						}
						l_sValues << wxT("[");
						for (unsigned i = 0; i < constant->values_.size(); i++)
						{
							wxString val_(constant->values_[i].c_str(), wxConvUTF8);
							l_sValues << val_ << wxT(";");
						}
						l_sValues << wxT("]");

						if (constant->values_.size() == 1)
						{
							l_sValues.Replace(wxT("["), "");
							l_sValues.Replace(wxT("]"), "");
							l_sValues.Replace(wxT(";"), "");
						}
						l_sConstDef << l_sType << wxT(" ") << name_ << wxT(" =") << l_sValues << wxT(";\n");
						m_pcTextconstants->AppendText(l_sConstDef);
					}
				}
			}

			m_pcTextconstants->AppendText(wxT("\n"));

		}

	}
	else
	{
		wxString l_sCheckedconstName = m_Options_constants[p_cEvent.GetInt()];
		for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
		{
			for (unsigned i = 0; i < itMap->second.size(); i++)
			{

				for (auto& constant : *p_pcDoc->constants_)
				{
					wxString name_(constant->name_.c_str(), wxConvUTF8);

					if (name_ == itMap->second[i])
					{
						int ind = m_Options_constants.Index(name_);
						if (m_pcRearrangelist_constants->IsChecked(ind)) //{ continue; }

							if (name_ == l_sCheckedconstName)
							{

								wxString l_sConstDef;
								wxString l_sType;
								wxString l_sValues;
								if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
								{
									l_sType = wxT("double");
								}
								if (constant->type_ == dssd::andl::ConstType::INT_T)
								{
									l_sType = wxT("int");
								}
								l_sValues << wxT("[");
								for (unsigned i = 0; i < constant->values_.size(); i++)
								{
									wxString val_(constant->values_[i].c_str(), wxConvUTF8);
									l_sValues << val_ << wxT(";");
								}
								l_sValues << wxT("]");

								if (constant->values_.size() == 1)
								{
									l_sValues.Replace(wxT("["), "");
									l_sValues.Replace(wxT("]"), "");
									l_sValues.Replace(wxT(";"), "");
								}
								l_sConstDef << l_sType << wxT(" ") << name_ << wxT(" =") << l_sValues << wxT(";\n");
								m_pcTextconstants->AppendText(l_sConstDef);
							}
					}
				}
			}



		}



	}
	////////////////////dependency checking////////////////////
	wxString l_selectedConstant = m_Options_constants[p_cEvent.GetInt()];


	if (!b)
	{
		for (int j = 0; j < m_vConstDependenciesVector.size(); j++)
		{
			if (m_vConstDependenciesVector[j]->key == l_selectedConstant)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vConstDependenciesVector[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{


					 if (itMap->first == NODE_TYPE::FUN_COLOOR)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, false);
							}
						}
						else if (itMap->first == NODE_TYPE::CONSTANT)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, false);
								//CheckConstant(*itSet, false);
							}
						}
						else
						{
							if (itMap->first == NODE_TYPE::OBSERVER)
							{
								for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
								{
									int index = m_Options_observers.Index(*itSet);
									m_pcRearrangelist_observers->Check(index, false);
								}
							}
						}
					}
				}
			}
		}

	}
	if (b)
	{
		set<wxString> l_setDec;
		//vector<wxString> l_vDec;
		m_CheckDec.ComputeBackwardDependency(l_selectedConstant, NODE_TYPE::CONSTANT, l_setDec);

		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);
			///	m_mMap2ConstDep[l_selectedConstant].insert(*itSet);
			}
			else
			{
				if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
				{
					int index = m_Options_funs.Index(*itSet);
					m_pcRearrangelist_function->Check(index, true);
				}
			}

		}
	}
		/**
		for (auto itMap = m_mMap2ConstDep.begin(); itMap != m_mMap2ConstDep.end(); ++itMap)
		{
			if (itMap->first == l_selectedConstant)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					int index = m_Options_constants.Index(*itSet);
					m_pcRearrangelist_constants->Check(index, true);
					//	CheckConstant(*itSet, true);
				}
			}
		}
	}
	*/

	///do printing on the right hand panel
	PrintChosenConstants();
	PrintChosenFunctions();
	PrintChosenObservers();
}

void SP_ImportANDL::OnSelChange_funs(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_function->IsChecked(p_cEvent.GetInt());
	m_pcCheckAll->SetValue(false);
	if (!b)
	{
		wxString l_sFunctions;
		wxString funName = m_Options_funs[p_cEvent.GetInt()];
		for (auto& fun : *p_pcDoc->functions_)
		{
			int ind = m_Options_funs.Index(fun->name_);
			if (!m_pcRearrangelist_function->IsChecked(ind)) { continue; }
			if (fun->name_ == funName) { continue; }
			wxString par=fun->param_;

			l_sFunctions << fun->name_ << wxT("(") << par << wxT(") ") << wxT("= ") << fun->body_ << wxT(";\n");

		}

		m_pcTextfunctions->SetValue(l_sFunctions);
	}
	else
	{
		wxString l_sfundef;
		for (int i = 0; i < m_Options_funs.size(); i++)
		{
			wxString funName1 = m_Options_funs[i];
			if (m_pcRearrangelist_function->IsChecked(i))
			{
				for (auto& fun : *p_pcDoc->functions_)
				{
					if (fun->name_ != funName1) { continue; }
					wxString par1=fun->param_;

					l_sfundef<< fun->name_ << wxT("(") << par1 << wxT(") ") << wxT("= ") << fun->body_ << wxT(";\n");

				}
			}

		}
		m_pcTextfunctions->SetValue(l_sfundef);

	}
	wxString funName = m_Options_funs[p_cEvent.GetInt()];
	if (!b)
	{
		for (int j = 0; j < m_vFunDependenciesVector.size(); j++)
		{
			if (m_vFunDependenciesVector[j]->key == funName)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vFunDependenciesVector[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{


						if (itMap->first == NODE_TYPE::FUN_COLOOR)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, false);
							}
						}
						else if (itMap->first == NODE_TYPE::CONSTANT)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, false);
								//CheckConstant(*itSet, false);
							}
						}
						else
						{
							if (itMap->first == NODE_TYPE::OBSERVER)
							{
								for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
								{
									int index = m_Options_observers.Index(*itSet);
									m_pcRearrangelist_observers->Check(index, false);
									//CheckConstant(*itSet, false);
								}
							}
						}
					}
				}
			}
		}
	}

	if (b)
	{
		set<wxString> l_setDec;
		m_CheckDec.ComputeBackwardDependency(funName, NODE_TYPE::FUN_COLOOR, l_setDec);

		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);
				///	m_mMap2ConstDep[l_selectedConstant].insert(*itSet);
			}
			else if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_funs.Index(*itSet);
				m_pcRearrangelist_function->Check(index, true);

			}
			else
			{
				if (m_Options_observers.Index(*itSet) != wxNOT_FOUND)
				{
					int index = m_Options_observers.Index(*itSet);
					m_pcRearrangelist_observers->Check(index, true);

				}
			}

		}
	}

	PrintChosenConstants();
	PrintChosenFunctions();
	PrintChosenObservers();

}
void SP_ImportANDL::AddConstants_Att()
{
	wxString l_sConstant;

	int l_nOrder = 0;
	for (auto& constant : *p_pcDoc->constants_)
	{
		wxString group_(constant->group_.c_str(), wxConvUTF8);
		wxString name_(constant->name_.c_str(), wxConvUTF8);

		m_mGroup2Const[group_].push_back(name_);

		m_Options_constants.Add(name_);

		m_Options_constants_order.Add(l_nOrder);
		l_nOrder++;
	}

		for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
		{
			if(m_mGroup2Const.size()==1)//for preventing overwriting the old value
			m_pcTextconstants->SetValue(itMap->first + _T(":\n"));
			else
			m_pcTextconstants->AppendText(itMap->first + _T(":\n"));

			for (unsigned i = 0; i < itMap->second.size(); i++)
			{

				for (auto& constant : *p_pcDoc->constants_)
				{
					wxString name_(constant->name_.c_str(), wxConvUTF8);

					if (name_ == itMap->second[i])
					{
						wxString l_sConstDef;
						wxString l_sType;
						wxString l_sValues;
						if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
						{
							l_sType = wxT("double");
						}
						if (constant->type_ == dssd::andl::ConstType::INT_T)
						{
							l_sType = wxT("int");
						}
						l_sValues << wxT("[");
						for (unsigned i = 0; i < constant->values_.size(); i++)
						{
							wxString val_(constant->values_[i].c_str(), wxConvUTF8);
							l_sValues << val_ << wxT(";");
						}
						l_sValues << wxT("]");

						if (constant->values_.size() == 1)
						{
							l_sValues.Replace(wxT("["), "");
							l_sValues.Replace(wxT("]"), "");
							l_sValues.Replace(wxT(";"), "");
						}
						l_sConstDef << l_sType << wxT(" ") << name_ << wxT(" =") << l_sValues << wxT(";\n");
						m_pcTextconstants->AppendText(l_sConstDef);
					}
				}
			}
		}

}


void SP_ImportANDL::AddObservers_Att()
{
	int order = 0;
	for (auto& obs : *p_pcDoc->observers_)
	{
		if (!obs) continue;

		wxString name = obs->name_;
		wxString type = wxT("Place");
		if (obs->type_ == dssd::andl::ObserverType::TRANS_T) {
			type = wxT("Transition");
		}
		wxString body = obs->function_;
		std::pair<wxString,wxString> ob;
		ob.first = name;
		ob.second = body;
		m_mType2Ob[type].insert(ob);
		m_Options_observers.Add(name);
		m_Options_observers_order.Add(order);
		order++;
		//SP_DS_Metadata* l_Obs = mc->NewElement(1);
	//	l_Obs->GetAttribute(wxT("Name"))->SetValueString(name);
		//l_Obs->GetAttribute(wxT("Type"))->SetValueString(type);
		//l_Obs->GetAttribute(wxT("Body"))->SetValueString(body);
	}

	for (auto itMap = m_mType2Ob.begin(); itMap != m_mType2Ob.end(); ++itMap)
	{
		if (m_mType2Ob.size() == 1)//for preventing overwriting the old value
			m_pcTextobservers->SetValue(itMap->first + _T(":\n"));
		else
			m_pcTextobservers->AppendText(itMap->first + _T(":\n"));

		for (auto itMapOb= itMap->second.begin(); itMapOb!= itMap->second.end();++itMapOb)
		{

			for (auto& observ : *p_pcDoc->observers_)
			{
				wxString name_(observ->name_.c_str(), wxConvUTF8);

				if (name_ == itMapOb->first)
				{
					wxString l_sObs;
					wxString l_sBody = observ->function_;
					l_sObs << name_ << wxT(" = ") << l_sBody << wxT(";\n");
					m_pcTextobservers->AppendText(l_sObs);
				}
			}
		}
	}
}


void  SP_ImportANDL::AddFunctions_Att()
{
	wxString l_sFunctions;

	int l_nOrder = 0;
	for (auto& fun : *p_pcDoc->functions_)
	{
		wxString par= fun->param_;

		l_sFunctions  << fun->name_ << wxT("(") << par << wxT(") ") << wxT("= ") << fun->body_ << wxT(";\n");

		m_Options_funs.Add(fun->name_);

		m_Options_funs_order.Add(l_nOrder);
		l_nOrder++;

	}
	m_pcTextfunctions->SetValue(l_sFunctions);
}

void SP_ImportANDL::ONselectAll(wxCommandEvent& p_cEvent)
{
	if (m_pcCheckAll->IsChecked())
		{
			if (m_pcNotebookPageConstants)
			{
				if (m_pcNotebookPageConstants->IsFocusable())
				{
					for (auto& constant : *p_pcDoc->constants_)
					{
						wxString l_selectedConstant = constant->name_.c_str();

						if (m_Options_constants.Index(l_selectedConstant) != wxNOT_FOUND)
						{
							int index = m_Options_constants.Index(l_selectedConstant);
							m_pcRearrangelist_constants->Check(index, true);
						}


						set<wxString> l_setDec;
						m_CheckDec.ComputeBackwardDependency(l_selectedConstant, NODE_TYPE::CONSTANT, l_setDec);

						for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
						{
							if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, true);

							}
							else
							{
								if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
								{
									int index = m_Options_funs.Index(*itSet);
									m_pcRearrangelist_function->Check(index, true);
								}
							}

						}
					}
				}
			}

			if (m_pcNotebookPageFunctions)
			{
				if (m_pcNotebookPageFunctions->IsFocusable())
				{
					for (auto& fun : *p_pcDoc->functions_)
					{

						wxString funName = fun->name_.c_str();

						if (m_Options_funs.Index(funName) != wxNOT_FOUND)
						{
							int index = m_Options_funs.Index(funName);
							m_pcRearrangelist_function->Check(index, true);
						}

						set<wxString> l_setDec;
						m_CheckDec.ComputeBackwardDependency(funName, NODE_TYPE::FUN_COLOOR, l_setDec);

						for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
						{
							if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, true);

							}
							else if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, true);

							}
							else
							{
								if (m_Options_observers.Index(*itSet) != wxNOT_FOUND)
								{
									int index = m_Options_observers.Index(*itSet);
									m_pcRearrangelist_observers->Check(index, true);

								}
							}

						}
					}
				}
			}

			if (m_pcNotebookPageObservers)
			{
				if (m_pcNotebookPageObservers->IsFocusable())
				{
					for (auto& obs : *p_pcDoc->observers_)
					{
						wxString l_sCheckedObserverName = obs->name_.c_str();
						if (m_Options_observers.Index(l_sCheckedObserverName) != wxNOT_FOUND)
						{
							int index = m_Options_observers.Index(l_sCheckedObserverName);
							m_pcRearrangelist_observers->Check(index, true);
						}

						set<wxString> l_setDec;
						m_CheckDec.ComputeBackwardDependency(l_sCheckedObserverName, NODE_TYPE::OBSERVER, l_setDec);
						for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
						{
							if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, true);

							}
							else if (m_Options_funs.Index(*itSet) != wxNOT_FOUND)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, true);
							}

						}
					}
				}
			}

			PrintChosenConstants();

			PrintChosenFunctions();
			PrintChosenObservers();

		}
		else
		{
			if (m_pcNotebookPageConstants)
			{
				if (m_pcNotebookPageConstants->IsFocusable())
				{
					for (auto& con : *p_pcDoc->constants_)
					{
						wxString l_selectedConstant = con->name_.c_str();

						if (m_Options_constants.Index(l_selectedConstant) != wxNOT_FOUND)
						{
							int index = m_Options_constants.Index(l_selectedConstant);
							m_pcRearrangelist_constants->Check(index, false);
						}

						for (int j = 0; j < m_vConstDependenciesVector.size(); j++)
						{
							if (m_vConstDependenciesVector[j]->key == l_selectedConstant)
							{
								std::map<NODE_TYPE, set<wxString>> l_DepMap;

								LevelOrderTraversal(m_vConstDependenciesVector[j], l_DepMap);

								if (l_DepMap.size() > 0)
								{
									for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
									{


										if (itMap->first == NODE_TYPE::FUN_COLOOR)
										{
											for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
											{
												int index = m_Options_funs.Index(*itSet);
												m_pcRearrangelist_function->Check(index, false);
											}
										}
										else if (itMap->first == NODE_TYPE::CONSTANT)
										{
											for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
											{
												int index = m_Options_constants.Index(*itSet);
												m_pcRearrangelist_constants->Check(index, false);
												//CheckConstant(*itSet, false);
											}
										}
										else
										{
											if (itMap->first == NODE_TYPE::OBSERVER)
											{
												for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
												{
													int index = m_Options_observers.Index(*itSet);
													m_pcRearrangelist_observers->Check(index, false);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if (m_pcNotebookPageFunctions)
			{
				if (m_pcNotebookPageFunctions->IsFocusable())
				{
					for (auto& fun : *p_pcDoc->functions_)
					{
						wxString funName = fun->name_.c_str();

						if (m_Options_funs.Index(funName) != wxNOT_FOUND)
						{
							int index = m_Options_funs.Index(funName);
							m_pcRearrangelist_function->Check(index, false);
						}

						for (int j = 0; j < m_vFunDependenciesVector.size(); j++)
						{
							if (m_vFunDependenciesVector[j]->key == funName)
							{
								std::map<NODE_TYPE, set<wxString>> l_DepMap;

								LevelOrderTraversal(m_vFunDependenciesVector[j], l_DepMap);

								if (l_DepMap.size() > 0)
								{
									for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
									{


										if (itMap->first == NODE_TYPE::FUN_COLOOR)
										{
											for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
											{
												int index = m_Options_funs.Index(*itSet);
												m_pcRearrangelist_function->Check(index, false);
											}
										}
										else if (itMap->first == NODE_TYPE::CONSTANT)
										{
											for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
											{
												int index = m_Options_constants.Index(*itSet);
												m_pcRearrangelist_constants->Check(index, false);

											}
										}
										else
										{
											if (itMap->first == NODE_TYPE::OBSERVER)
											{
												for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
												{
													int index = m_Options_observers.Index(*itSet);
													m_pcRearrangelist_observers->Check(index, false);

												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if (m_pcNotebookPageObservers)
			{
				if (m_pcNotebookPageObservers->IsFocusable())
				{
					for (auto& obs : *p_pcDoc->observers_)
					{

					}
				}
			}

			for (int i = 0; i < m_Options_observers_order.size(); i++)
			{
				m_pcRearrangelist_observers->Check(m_Options_observers_order[i], false);
			}

			PrintChosenConstants();

			PrintChosenFunctions();

			PrintChosenObservers();

			m_pcTextconstants->SetValue(wxT(""));
		}
}


void SP_ImportANDL::PrintChosenConstants()
{
	m_pcTextconstants->Clear();
	for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
	{
		if (m_mGroup2Const.size() == 1)//for preventing overwriting the old value
			m_pcTextconstants->SetValue(itMap->first + _T(":\n"));
		else
			m_pcTextconstants->AppendText(itMap->first + _T(":\n"));

		for (unsigned i = 0; i < itMap->second.size(); i++)
		{

			for (auto& constant : *p_pcDoc->constants_)
			{
				wxString name_(constant->name_.c_str(), wxConvUTF8);

				int ind = m_Options_constants.Index(name_);
				if (!m_pcRearrangelist_constants->IsChecked(ind)) { continue; }

				if (name_ == itMap->second[i])
				{
					wxString l_sConstDef;
					wxString l_sType;
					wxString l_sValues;
					if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
					{
						l_sType = wxT("double");
					}
					if (constant->type_ == dssd::andl::ConstType::INT_T)
					{
						l_sType = wxT("int");
					}
					l_sValues << wxT("[");
					for (unsigned i = 0; i < constant->values_.size(); i++)
					{
						wxString val_(constant->values_[i].c_str(), wxConvUTF8);
						l_sValues << val_ << wxT(";");
					}
					l_sValues << wxT("]");

					if (constant->values_.size() == 1)
					{
						l_sValues.Replace(wxT("["), "");
						l_sValues.Replace(wxT("]"), "");
						l_sValues.Replace(wxT(";"), "");
					}
					l_sConstDef << l_sType << wxT(" ") << name_ << wxT(" =") << l_sValues << wxT(";\n");
					m_pcTextconstants->AppendText(l_sConstDef);
				}
			}
		}
	}

}
void SP_ImportANDL::LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_ResultKey)
{

	if (root == NULL)
		return;

	// Standard level order traversal code
	// using queue
	queue<sp_node *> q;  // Create a queue
	q.push(root); // Enqueue root
	while (!q.empty())
	{
		int n = q.size();

		// If this node has children
		while (n > 0)
		{
			// Dequeue an item from queue and print it
			sp_node * p = q.front();
			q.pop();
			l_ResultKey[p->type].insert(p->key);
			// Enqueue all children of the dequeued item
			if (p)//added later
				for (int i = 0; i<p->child.size(); i++)
					q.push(p->child[i]);
			n--;
		}
	}
}

void SP_ImportANDL::PrintChosenObservers()
{
	m_pcTextobservers->Clear();
	for (auto itMap = m_mType2Ob.begin(); itMap != m_mType2Ob.end(); ++itMap)
	{
		if (m_mType2Ob.size() == 1)//for preventing overwriting the old value
			m_pcTextobservers->SetValue(itMap->first + _T(":\n"));
		else
			m_pcTextobservers->AppendText(itMap->first + _T(":\n"));

		for (auto itMapOb = itMap->second.begin(); itMapOb != itMap->second.end(); ++itMapOb)
		{

			for (auto& observ : *p_pcDoc->observers_)
			{
				wxString name_(observ->name_.c_str(), wxConvUTF8);

				int ind = m_Options_observers.Index(name_);
				if (!m_pcRearrangelist_observers->IsChecked(ind)) { continue; }

				if (name_ == itMapOb->first)
				{
					wxString l_sObs;
					wxString l_sBody = observ->function_;
					l_sObs << name_ << wxT(" = ") << l_sBody << wxT(";\n");
					m_pcTextobservers->AppendText(l_sObs);
				}
			}
		}
	}
}


void SP_ImportANDL::PrintChosenFunctions()
{
	wxString l_sFunctions;

	//int l_nOrder = 0;
	for (auto& fun : *p_pcDoc->functions_)
	{
		wxString val_(fun->name_.c_str(), wxConvUTF8);
		int ind = m_Options_funs.Index(val_);
		if (!m_pcRearrangelist_function->IsChecked(ind)) { continue; }

		wxString par = fun->param_;

		l_sFunctions<< fun->name_ << wxT("(") << par << wxT(") ") << wxT("= ") << fun->body_ << wxT(";\n");


	}
	m_pcTextfunctions->SetValue(l_sFunctions);
}
