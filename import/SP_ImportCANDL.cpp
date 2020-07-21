//////////////////////////////////////////////////////////////////////
// $Author:cr$
// $Version: 0.1 $
// $Date: 2011/12/14 $
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportCANDL.h"
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

#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_LayoutProperties.h"

#include "snoopy.h"

#include "sp_utilities.h"


#include "dssd/misc/net_reader.h"

#include <wx/spinctrl.h>

#include <algorithm>

#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"

#include "sp_gui/dialogs/SP_DLG_ImportProperties.h"
#include <iostream>
#include <queue>

using namespace std;
enum {
	SP_ID_DECLARATIONS_UPDATE,
	SP_ID_FUNS_DEF,
	SP_ID_DECLARATIONS_COLORSETS,
	SP_ID_CS_TESXT,
	SP_ID_DECLARATIONS_VARS,
	SP_ID_VARS_TESXT,
	SP_ID_DECLARATIONS_CONSTANTS,
	SP_ID_CONSTANTS_TESXT,
	SP_ID_CHECK_SELECTIVE,
	SP_ID_IS_NEW_DOC,
	SP_ID_CHECK_ALL


};
SP_ImportCANDL::SP_ImportCANDL()
{
	m_bIsNewDoc = true;
}

SP_ImportCANDL::~SP_ImportCANDL()
{
	for (auto p : m_vConstantsDepTrees)
	{
		delete p;
	}
	for (auto p : m_vColorsetDepTrees)
	{
		delete p;
	}
	for (auto p : m_vVarsDepTrees)
	{
		delete p;
	}

	for (auto p : m_vFunctionsDepTrees)
	{
		delete p;
	}
}

bool
SP_ImportCANDL::ReadFile(const wxString& p_sFile, SP_ImportRoutine* p_importRT)
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
			wxString l_sMsg = wxT("parse successful\n");
			SP_LOGMESSAGE(l_sMsg);

			dssd::andl::Net_ptr l_Colnet = p.get();
		   SP_DLG_ImportProperties*  l_pcDialog = new SP_DLG_ImportProperties(p_importRT, NULL, l_Colnet, p_sFile);
		   if (l_pcDialog->ShowModal() == wxID_OK)
		 	{
			 // bool l_bIsChecked= l_pcDialog->GetIncludeToCurrentdocState();

				   for (int i = 0; i < m_Options_funs.size(); i++)
				   {
					   if (m_pcRearrangelist_function->IsChecked(i))
					   {
						   l_finalFunSel.Add(m_Options_funs[i]);
					   }
				   }
				   for (int i = 0; i < m_Options_cs.size(); i++)
				   {
					   if (m_pcRearrangelist_colorsets->IsChecked(i))
					   {
						   l_finalcolorsets.Add(m_Options_cs[i]);
					   }
				   }
				   for (int i = 0; i < m_Options_vars.size(); i++)
				   {
					   if (m_pcRearrangelist_variables->IsChecked(i))
					   {
						   l_finalvariables.Add(m_Options_vars[i]);
					   }
				   }

				   for (int i = 0; i < m_Options_constants.size(); i++)
				   {
					   if (m_pcRearrangelist_constants->IsChecked(i))
					   {
						   l_finalconstants.Add(m_Options_constants[i]);
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
						   SP_MESSAGEBOX(wxT("There is no an opend Colored PN document!!"),wxT("Error"));
						   return false;
					   }
				   }

		    }
		 	else
		 	{
				ClearAll();
				return false;
		    }
		//	l_pcDialog->Destroy();
		// SP_Core::Instance()->GetRootDocument()->GetGraph()
			l_Return = CreateGraph(p_sFile, *l_Net);
			if (l_Return)
                doLayout();


		}
	}
	catch(const std::exception& e)
	{
		wxString l_sMsg = wxT("Error while parsing ") + p_sFile;
		SP_LOGERROR(l_sMsg);
		SP_LOGERROR(wxString(e.what()));

	}

	ClearAll();

	return l_Return;
}


void SP_ImportCANDL::doLayout()
{
    SP_DLG_LayoutProperties *l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
    l_pcDlg->ShowModal();
    l_pcDlg->Destroy();
}

void
SP_ImportCANDL::ClearAll()
{
	lookupParameters.clear();
	lookupConstants.clear();
	lookupColorsets.clear();
	lookupVariables.clear();
	lookupFunctions.clear();
	lookupPlaces.clear();
	lookupTransitions.clear();
	arcs.clear();



	m_vConstantsDepTrees.clear();
	m_vColorsetDepTrees.clear();
	m_varBackwordDep.clear();
	m_funBackwordDep.clear();
	m_mColorset2Dependncies.clear();
	m_mMap2ConstDep.clear();
	l_finalconstants.clear();
	l_finalcolorsets.clear();
	l_finalvariables.clear();
	l_finalFunSel.clear();
	m_mGroup2Const.clear();
	//if (p_pcDlg)
	//{
		//m_pcTextconstants1->SetValue("");
		//wxDELETE(p_pcDlg);
	//}
	m_bIsNewDoc = true;
	m_mDeclaration2Overwritten.clear();
}

/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////
bool SP_ImportCANDL::AddToGraph(const dssd::andl::Net& p_Net)
{

	if (!SP_Core::Instance()->GetRootDocument())
	{
		return false;
	}

	m_pcGraph =SP_Core::Instance()->GetRootDocument()->GetGraph();



	if (!m_pcGraph)
		return false;

	if (!m_pcGraph->GetNetclass()->GetName().Contains("Colored"))
	{
		return false;
	}
	if (p_Net.constants_ && p_Net.valuesets_)
		AppendConstants(*p_Net.constants_, *p_Net.valuesets_);

	if (p_Net.colorsets_)
			AppendColorsets(*p_Net.colorsets_);

	if (p_Net.variables_)
		AppendVariables(*p_Net.variables_);

	if (p_Net.color_functions_)
		AppendFunctions(*p_Net.color_functions_);


	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();
	wxString l_sLogMsg;
	if (m_mDeclaration2Overwritten.size()>0)
	{
		l_sLogMsg<<wxT("the defintions of the following declarations were overwritten:\n");
	}
	for (auto itMap = m_mDeclaration2Overwritten.begin(); itMap != m_mDeclaration2Overwritten.end(); ++itMap)
	{
		l_sLogMsg<<itMap->first << wxT(":\n");
		wxString l_sMssg;
		for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
		{
			l_sMssg << *itSet << wxT("\n");
		}
		l_sLogMsg << l_sMssg;
	}

	if(m_mDeclaration2Overwritten.size()>0)
		SP_LOGMESSAGE(l_sLogMsg);

	return true;

}

bool
SP_ImportCANDL::CreateGraph(const wxString& p_sFile, const dssd::andl::Net& p_Net)
{
	x = 350.0;
	y = 250.0;
	SP_LOGDEBUG(wxT("Creating Graph"));

	m_eNetType = p_Net.type_;
	m_pcGraph = CreateDocument(p_sFile, p_Net.type_);

	if(!m_pcGraph)
		return false;

	if(p_Net.constants_ && p_Net.valuesets_)
		CreateConst(*p_Net.constants_, *p_Net.valuesets_);

	if(p_Net.colorsets_)
		CreateColorsets(*p_Net.colorsets_);

	if(p_Net.variables_)
		CreateVariables(*p_Net.variables_);

	if(p_Net.color_functions_)
		CreateFunctions(*p_Net.color_functions_);

	if(p_Net.places_)
		CreatePlaces(*p_Net.places_);

	if(p_Net.observers_)
		CreateObservers(*p_Net.observers_);

	if(p_Net.transitions_)
		CreateTransitions(*p_Net.transitions_);

	CreateArcs();

	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

	return true;
}


bool
SP_ImportCANDL::CreatePlaces(const dssd::andl::Places& p_Places)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if(m_eNetType == dssd::andl::NetType::COL_CPN_T)
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	}
	else
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	}
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Attribute* l_pcAttr;
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
		AdaptColorExpression(marking);
		wxString colorset = p->colorset_;
        if(colorset == wxT("dot")) {
            colorset = wxT("Dot");
        }
		l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
		l_node->GetAttribute(wxT("Name"))->SetValueString(name);
		l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
		l_pcAttr = l_node->GetAttribute(SP_DS_CPN_COLORSETNAME);
		l_pcAttr->SetValueString(colorset);
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("MarkingList")));
		l_pcColAttr->Clear();

		wxArrayString l_Markings;
		size_t prev = 0;
		for(size_t i = marking.find(wxT("++")); i != wxNOT_FOUND; )
		{
			l_Markings.Add(marking.Mid(prev,i-prev));
		    marking.Replace(marking.Mid(prev, i - prev), "");
			prev = 0;
			marking = marking.AfterFirst('+');
			marking = marking.AfterFirst('+');
			if (!marking.Contains("++"))
			{
				l_Markings.Add(marking.Mid(0, marking.length()));
				marking.Replace("++", "");
			}
			i = marking.find(wxT("++"));
		}
		if(l_Markings.IsEmpty())
		{
			l_Markings.Add(marking);
		}
		for(size_t i = 0; i < l_Markings.Count(); ++i)
		{
			wxString color,token,tupel;
			tupel = wxT("()");
			if(l_Markings[i].Contains(wxT("`"))&& !l_Markings[i].Contains(wxT("[")))
			{
				token = l_Markings[i].BeforeFirst('`');
				token.Replace(wxT("("), wxT(""));//by george
				token.Replace(wxT(")"), wxT(""));//by george
				token.Replace(wxT("++"), wxT(""));//by george
				color = l_Markings[i].AfterFirst('`');
			  
				if (!color.Contains("all")&& !m_fileName.Contains(wxT(".candl")))
				{
					// by george
					color.Replace(wxT("("), wxT(""));
					color.Replace(wxT(")"), wxT(""));
					color.Replace(wxT("++"), wxT(""));
					wxString l_sColor;
					if (!color.Contains(wxT("dot")))
					{
						l_sColor << wxT("(") << color << wxT(")");
						color = l_sColor;
					}
				}
				

			}
			else if (l_Markings[i].Contains(wxT("`")) && l_Markings[i].Contains(wxT("[")))
			{
				token = l_Markings[i].BeforeFirst('`');
				token = token.AfterFirst(']');

				tupel= l_Markings[i].AfterFirst('`');

				color = l_Markings[i].BeforeFirst(']');
				color = color.AfterFirst('[');
			}
			else
			{
				token = wxT("1");
				color = l_Markings[i];
			}
			unsigned int l_nNewRow = l_pcColAttr->AppendEmptyRow();
			l_pcColAttr->SetCell(l_nNewRow,0, color);
			l_pcColAttr->SetCell(l_nNewRow,1, token);
			l_pcColAttr->SetCell(l_nNewRow, 2, tupel);
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

bool SP_ImportCANDL::AppendConstants(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets)
{

	struct temp_constant {
		wxString constname;
		wxString consttype;
		wxString constGroup;
		wxString constcomment;
		vector<wxString> vsets;
	};
	vector<temp_constant> l_vConstantsInGraph;
	SP_DS_Metadataclass*	l_pcConstantsOrgin = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	vector<wxString> l_vSetNAmes;
	int l_nOrgSize = l_pcConstantsOrgin->GetElements()->size();
	int k = 0;
	if (l_pcConstantsOrgin->GetElements()->size() > 0)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(l_pcConstantsOrgin->GetElements()))
		{
			k++;
			wxString l_sName=l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sGroup=l_pcMetadata->GetAttribute(wxT("Group"))->GetValueString();
			wxString l_sComment=l_pcMetadata->GetAttribute(wxT("Comment"))->GetValueString();
			wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
		//	wxString l_sType= l_pcMetadata->GetAttribute(wxT("type"))->GetValueString();

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
				wxString l_sMessage;
				l_sMessage << wxT("the constant ") << constant->name_ << wxT(" is exist in the model!") << wxT("\n");
				l_sMessage << wxT("Would you like to overwrite its value?");
				if (l_finalconstants.Index(constant->name_) != wxNOT_FOUND)
				{

					int l_nChoose = SP_MESSAGEBOX(l_sMessage, wxT("overwrite declarations"), wxYES | wxNO | wxICON_INFORMATION);

					if (l_nChoose == wxYES)
					{
						l_vConstantsInGraph[i].constGroup = constant->group_;
						if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
							l_vConstantsInGraph[i].consttype = wxT("double");
						else
							l_vConstantsInGraph[i].consttype = wxT("int");

						vector<wxString> vsets;
						for (auto& ss : constant->values_)
						{
							vsets.push_back(ss);
						}
						l_vConstantsInGraph[i].vsets = vsets;

						m_mDeclaration2Overwritten[wxT("Constant")].insert(l_vConstantsInGraph[i].constname);
					}

				}
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

	SP_DS_Metadataclass*	l_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);


	if (l_pcConstants->GetElements()->size() > 0)
	{
		l_pcConstants->RemoveAll();

	}

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
			if(l_vSetNAmes.size()==1)
				l_pcColList->SetCell(l_nRowCol, 0, l_sVsetMain);
			else
			{
				if(l_vConstantsInGraph[i].vsets.size()== l_vSetNAmes.size())
				l_pcColList->SetCell(l_nRowCol, 0, l_vSetNAmes[j]);
			}

		}
		wxString l_sMainVal = wxT("0");
		if(l_vConstantsInGraph[i].vsets.size()>0)
			l_sMainVal=l_vConstantsInGraph[i].vsets[0];
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

bool
SP_ImportCANDL::CreateConst(const dssd::andl::Constants& p_Constants, const dssd::andl::Valuesets& p_Valuesets)
{

	unsigned int l_nSize = l_finalconstants.size();
	int l_nCount = 0;
SP_DS_Metadataclass*	l_pcConstants = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
for (auto& constant : p_Constants)
{
	if(l_nCount<l_nSize)
	l_pcConstants->NewElement(1);

	l_nCount++;
}
SP_ListMetadata::const_iterator l_itElem;
l_itElem = l_pcConstants->GetElements()->begin();
for (auto& constant : p_Constants)
	{
		if (!constant) continue;

		if (lookupParameters.find(constant->name_) != lookupParameters.end())
		{
			continue;
	 
		}
		if (lookupConstants.find(constant->name_) != lookupConstants.end())
		{
			continue;
	 
		}

		if (l_finalconstants.Index(constant->name_, true) == wxNOT_FOUND) { continue; }//by george

		wxString name = constant->name_;
		auto type = constant->type_;
		wxString group = constant->group_;
	 

		if (constant->type_ == dssd::andl::ConstType::DOUBLE_T || constant->type_ == dssd::andl::ConstType::INT_T
			&& (m_eNetType == dssd::andl::NetType::COL_SPN_T
				|| m_eNetType == dssd::andl::NetType::COL_CPN_T
				|| m_eNetType == dssd::andl::NetType::COL_HPN_T
				 || m_eNetType == dssd::andl::NetType::COL_XPN_T
				|| m_eNetType == dssd::andl::NetType::COL_PN_T))
		{

			
			SP_DS_Metadata* l_pcConstant;
			l_pcConstant = *l_itElem;
			l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(name);
			l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(group);
			l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(wxT(""));

			if (constant->type_ == dssd::andl::ConstType::DOUBLE_T)
			{
				bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
			}
			else {
				bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(wxT("int"));
			}
			
			
			wxString value = wxT("0");
			wxString l_sMain;
			if (!constant->values_.empty())
			{
				value = constant->values_.front();
				l_sMain << value;
			}
			SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			l_pcColList1->Clear();

			for (unsigned int i = 0; i < constant->values_.size(); i++)
			{
				value = constant->values_[i];
				int l_nRowCol = l_pcColList1->AppendEmptyRow();
				//wxString l_sVset;
				///l_sVset << "Value-Set" << wxT("-")<<i-1;
				if(p_Valuesets.size()>0)
				l_pcColList1->SetCell(l_nRowCol, 0, p_Valuesets[i]);

				l_pcColList1->SetCell(l_nRowCol, 1, value);
			}

			m_pcGraph->GetFunctionRegistry()->registerFunction(name, l_sMain);

			l_pcColList1->UpdateActiveListColumnPtr();

		 
			l_pcConstant->Update();
			lookupConstants[constant->name_] = *l_itElem;
			 ++l_itElem;
			//l_nCount++;
		}
		 


	}

	return true;
}

bool SP_ImportCANDL::AppendColorsets(const dssd::andl::Colorsets& p_Colorsets)
{
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewSimple = *(l_pcMetadataclass->GetElements()->begin());
	if (!l_pcNewSimple)
		return false;
	SP_DS_ColListAttribute* l_pcColListSimple = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewSimple->GetAttribute(wxT("ColorsetList")));
	if (!l_pcColListSimple)
		return false;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewCompound = *(l_pcMetadataclass->GetElements()->begin());
	if (!l_pcNewCompound)
		return false;
	SP_DS_ColListAttribute* l_pcColListCompound = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewCompound->GetAttribute(wxT("StructuredColorsetList")));
	if (!l_pcColListCompound)
		return false;

	// CS "Dot" is already defined
	lookupColorsets["Dot"] = *(l_pcMetadataclass->GetElements()->begin());

	for (auto& cs : p_Colorsets)
	{
		if (!cs) continue;

		if (l_finalcolorsets.Index(cs->name_, true) == wxNOT_FOUND) { continue; }//by george

		if (lookupColorsets.find(cs->name_) != lookupColorsets.end() || cs->name_ == "dot")
		{
			continue;
			//return false;
		}

		wxString name = cs->name_;
		//TODO: add more
		wxString type = wxT("int");
		if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
		{
			type = wxT("enum");
		}
		else if (cs->type_ == dssd::andl::ColorsetType::STRING_T)
		{
			type = wxT("string");
		}
		wxString colors = cs->definition_;
		bool l_bSimple = true;
		if (colors.StartsWith(wxT("PROD")))
		{
			l_bSimple = false;
			type = wxT("product");
			colors = colors.AfterFirst('(').BeforeLast(')');
		}
		else
		{
			colors.Replace(wxT(".."), wxT("-"));
			colors.Replace(wxT("{"), wxT(""));
			colors.Replace(wxT("}"), wxT(""));
			//bugfix by george, importing subsets
			if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
			{
				wxString l_sExpression = colors;
				colors = colors.AfterFirst(wxChar('['));
				colors = colors.BeforeLast(wxChar(']'));
				type = l_sExpression.BeforeFirst(wxChar('['));
				l_bSimple = false;

			}
		}
		//TODO: add acyclic attribute to colorset definition in snoopy
		if (!cs->cyclic_)
		{
			SP_LOGWARNING(wxT("colorset ") + cs->name_ + wxT(" is acyclic!\n") + wxT("This is not supported in Snoopy!\n"));
		}
		AdaptColorExpression(colors);
		if (l_bSimple)
		{//simple cs
			bool l_bIsUsed = false;
			bool l_bOverwrite = false;
			for (int i = 0; i < l_pcColListSimple->GetRowCount(); i++)
			{
				if (name == l_pcColListSimple->GetCell(i, 0))
				{

					l_bIsUsed = true;
					break;
				}
			}

			/////////////////////
			if (l_bIsUsed)
			{//ask user about his decision

				wxString l_sMessage;
				l_sMessage << wxT("the color set ") << name << wxT(" is exist in the model!") << wxT("\n");
				l_sMessage << wxT("Would you like to overwrite its definition?");

				int l_nChoose = SP_MESSAGEBOX(l_sMessage, wxT("overwrite declarations"), wxYES | wxNO  | wxICON_INFORMATION);

				 if(l_nChoose == wxYES)
				{
					 l_bOverwrite = true;

				}
				 else if (l_nChoose == wxNO)
				 {
					 l_bOverwrite = false;
				 }

			}
			///////////////////

			if (l_bIsUsed && !l_bOverwrite) continue;//color set is already defined in the model

			if (l_bIsUsed &&  l_bOverwrite)
			{//to overwrite an existing simple cs
				for (int i = 0; i < l_pcColListSimple->GetRowCount(); i++)
				{
					if (name == l_pcColListSimple->GetCell(i, 0))
					{

						l_pcColListSimple->SetCell(i, 2, colors);
						l_bOverwrite = false;
						m_mDeclaration2Overwritten[wxT("ColorSet")].insert(name);
						break;
					}
				}
				if (!l_bOverwrite) continue;
			}
			unsigned int l_nNewRow = l_pcColListSimple->AppendEmptyRow();

			l_pcColListSimple->SetCell(l_nNewRow, 0, name);
			l_pcColListSimple->SetCell(l_nNewRow, 1, type);
			l_pcColListSimple->SetCell(l_nNewRow, 2, colors);
			lookupColorsets[cs->name_] = l_pcNewSimple;
		}
		else
		{//compound cs
			bool l_bIsUsed = false;
			bool l_bOverwrite = false;
			for (int i = 0; i < l_pcColListCompound->GetRowCount(); i++)
			{
				if (name == l_pcColListCompound->GetCell(i, 0))
				{

					l_bIsUsed = true;
					break;
				}
			}
			/////////////////////
			if (l_bIsUsed)
			{//ask user about his decision

				wxString l_sMessage;
				l_sMessage << wxT("the color set ") << name << wxT(" is exist in the model!") << wxT("\n");
				l_sMessage << wxT("Would you like to overwrite its definition?");

				int l_nChoose = SP_MESSAGEBOX(l_sMessage, wxT("overwrite declarations"), wxYES | wxNO | wxICON_INFORMATION);

				if (l_nChoose == wxYES)
				{
					l_bOverwrite = true;

				}
				else if (l_nChoose == wxNO)
				{
					l_bOverwrite = false;
				}

			}
			///////////////////
			if (l_bIsUsed && !l_bOverwrite) continue;//color set is already defined in the model

			if (l_bIsUsed &&  l_bOverwrite)
			{
				for (int i = 0; i < l_pcColListCompound->GetRowCount(); i++)
				{
					if (name == l_pcColListCompound->GetCell(i, 0))
					{

						l_pcColListCompound->SetCell(i, 2, colors);
						l_bOverwrite = false;
						m_mDeclaration2Overwritten[wxT("ColorSet")].insert(name);
						break;
					}
				}
				if (!l_bOverwrite) continue;
			}
			unsigned int l_nNewRow = l_pcColListCompound->AppendEmptyRow();

			l_pcColListCompound->SetCell(l_nNewRow, 0, name);
			l_pcColListCompound->SetCell(l_nNewRow, 1, type);
			l_pcColListCompound->SetCell(l_nNewRow, 2, colors);
			lookupColorsets[cs->name_] = l_pcNewCompound;
		}
	}
	return true;
}


bool SP_ImportCANDL::AppendVariables(const dssd::andl::Variables& p_Variables)
{
	for (auto& v : p_Variables)
	{
		if (!v) continue;

		if (lookupVariables.find(v->name_) != lookupVariables.end())
		{
			continue;
			//return false;
		}
		if (l_finalvariables.Index(v->name_, true) == wxNOT_FOUND) { continue; }//by george

		wxString name = v->name_;
		wxString type = v->colorset_;
		if (type == wxT("dot")) {
			type = wxT("Dot");
		}

		SP_DS_Metadataclass* l_pcMetadataclass;
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
		if (!l_pcMetadataclass)
			return false;
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if (!l_pcNewMetadata)
			return false;
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
		if (!l_pcColList)
			return false;

		bool l_bIsExist = false;
		bool l_bOverwrite = false;
		for (int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if (l_pcColList->GetCell(i,0) == name)
			{
				l_bIsExist = true;
				break;
			}
		}
		////ask user about his decision
		if (l_bIsExist)
		{//ask user about his decision

			wxString l_sMessage;
			l_sMessage << wxT("the color set ") << name << wxT(" is exist in the model!") << wxT("\n");
			l_sMessage << wxT("Would you like to overwrite its definition?");

			int l_nChoose = SP_MESSAGEBOX(l_sMessage, wxT("overwrite declarations"), wxYES | wxNO | wxICON_INFORMATION);

			if (l_nChoose == wxYES)
			{
				l_bOverwrite = true;

			}
			else if (l_nChoose == wxNO)
			{
				l_bOverwrite = false;
			}

		}
		///////////////////
		if (l_bIsExist && !l_bOverwrite) continue;//the variable is already defined in the model

		if (l_bIsExist &&   l_bOverwrite)
		{
			for (int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				if (l_pcColList->GetCell(i, 0) == name)
				{
					l_bOverwrite = false;
					l_pcColList->SetCell(i, 1, type);
					m_mDeclaration2Overwritten[wxT("Variable")].insert(name);

					break;
				}
			}
			if (!l_bOverwrite)
				continue;
		}

		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow, 0, name);
		l_pcColList->SetCell(l_nNewRow, 1, type);
		lookupVariables[v->name_] = l_pcNewMetadata;

	}
	return true;
}

bool SP_ImportCANDL::AppendFunctions(const dssd::andl::ColorFunctions& p_Functions)
{
	for (auto& f : p_Functions)
	{
		if (!f) continue;

		if (lookupFunctions.find(f->name_) != lookupFunctions.end())
		{
			continue;
			//return false;
		}

		if (l_finalFunSel.Index(f->name_, true) == wxNOT_FOUND) { continue; }//by george

		wxString name = f->name_;
		wxString type = f->ret_;
		dssd::andl::ColorFunction::Params params = f->params_;
		wxString param;
		for (auto& p : params)
		{
			if (!param.IsEmpty())
				param << wxT(",");
			param << p.first << wxT(" ") << p.second;
		}
		wxString body = f->body_;
		AdaptColorExpression(body);
		SP_DS_Metadataclass* l_pcMetadataclass;
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		if (!l_pcMetadataclass)
			return false;
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if (!l_pcNewMetadata)
			return false;
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		if (!l_pcColList)
			return false;

		bool l_bIsExist = false;
		bool l_bOverwrite = false;
		for (int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			if (l_pcColList->GetCell(i, 1) == name)
			{
				l_bIsExist = true;
				break;
			}
		}
		//////////////////////
		if (l_bIsExist)
		{//ask user about his decision

			wxString l_sMessage;
			l_sMessage << wxT("the color set ") << name << wxT(" is exist in the model!") << wxT("\n");
			l_sMessage << wxT("Would you like to overwrite its definition?");

			int l_nChoose = SP_MESSAGEBOX(l_sMessage, wxT("overwrite declarations"), wxYES | wxNO | wxICON_INFORMATION);

			if (l_nChoose == wxYES)
			{
				l_bOverwrite = true;

			}
			else if (l_nChoose == wxNO)
			{
				l_bOverwrite = false;
			}

		}
		///////////////////
		if (l_bIsExist && !l_bOverwrite) continue;//the function is already defined in the model

		if (l_bIsExist && l_bOverwrite)
		{
			for (int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				if (l_pcColList->GetCell(i, 1) == name)
				{
					l_pcColList->SetCell(i, 0, type);
					//l_pcColList->SetCell(l_nNewRow, 1, name);
					l_pcColList->SetCell(i, 2, param);
					l_pcColList->SetCell(i, 3, body);
					l_bOverwrite = false;

					m_mDeclaration2Overwritten[wxT("Function")].insert(name);
					break;
				}
			}
			if (!l_bOverwrite) continue;
		}

		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow, 0, type);
		l_pcColList->SetCell(l_nNewRow, 1, name);
		l_pcColList->SetCell(l_nNewRow, 2, param);
		l_pcColList->SetCell(l_nNewRow, 3, body);
		lookupFunctions[f->name_] = l_pcNewMetadata;

	}
	return true;
}


bool SP_ImportCANDL::CreateColorsets(const dssd::andl::Colorsets& p_Colorsets)
{
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewSimple = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewSimple)
		return false;
	SP_DS_ColListAttribute* l_pcColListSimple = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewSimple->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColListSimple)
		return false;
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewCompound = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewCompound)
		return false;
	SP_DS_ColListAttribute* l_pcColListCompound = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewCompound->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColListCompound)
		return false;

	// CS "Dot" is already defined
	lookupColorsets["Dot"] = *(l_pcMetadataclass->GetElements()->begin());

	for(auto& cs : p_Colorsets)
	{
		if(!cs) continue;

		if (l_finalcolorsets.Index(cs->name_, true) == wxNOT_FOUND) { continue; }//by george

		if(lookupColorsets.find(cs->name_) != lookupColorsets.end() || cs->name_ == "dot")
		{
			continue;
			//return false;
		}

		wxString name = cs->name_;
		//TODO: add more
		wxString type = wxT("int");
		if(cs->type_ == dssd::andl::ColorsetType::ENUM_T)
		{
			type = wxT("enum");
		}
		else if(cs->type_ == dssd::andl::ColorsetType::STRING_T)
		{
			type = wxT("string");
		}
		wxString colors = cs->definition_;
		bool l_bSimple = true;
		if(colors.StartsWith(wxT("PROD")))
		{
			l_bSimple = false;
			type = wxT("product");
			colors = colors.AfterFirst('(').BeforeLast(')');
		}
		else
		{
			colors.Replace(wxT(".."), wxT("-"));
			colors.Replace(wxT("{"), wxT(""));
			colors.Replace(wxT("}"), wxT(""));
			//bugfix by george, importing subsets
			if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
			{
				wxString l_sExpression = colors;
				colors = colors.AfterFirst(wxChar('['));
				colors = colors.BeforeLast(wxChar(']'));
				type = l_sExpression.BeforeFirst(wxChar('['));
				l_bSimple = false;

			}
		}
		//TODO: add acyclic attribute to colorset definition in snoopy
		if(!cs->cyclic_)
		{
			SP_LOGWARNING(wxT("colorset ") + cs->name_ + wxT(" is acyclic!\n") + wxT("This is not supported in Snoopy!\n"));
		}
		AdaptColorExpression(colors);
		if(l_bSimple)
		{
			unsigned int l_nNewRow = l_pcColListSimple->AppendEmptyRow();

			l_pcColListSimple->SetCell(l_nNewRow,0,name);
			l_pcColListSimple->SetCell(l_nNewRow,1,type);
			l_pcColListSimple->SetCell(l_nNewRow,2,colors);
			lookupColorsets[cs->name_] = l_pcNewSimple;
		}
		else
		{
			unsigned int l_nNewRow = l_pcColListCompound->AppendEmptyRow();

			l_pcColListCompound->SetCell(l_nNewRow,0,name);
			l_pcColListCompound->SetCell(l_nNewRow,1,type);
			l_pcColListCompound->SetCell(l_nNewRow,2,colors);
			lookupColorsets[cs->name_] = l_pcNewCompound;
		}
	}
	return true;
}

bool SP_ImportCANDL::CreateVariables(const dssd::andl::Variables& p_Variables)
{
	for(auto& v : p_Variables)
	{
		if(!v) continue;

		if(lookupVariables.find(v->name_) != lookupVariables.end())
		{
			continue;
			//return false;
		}
		if (l_finalvariables.Index(v->name_, true) == wxNOT_FOUND) { continue; }//by george

		wxString name = v->name_;
		wxString type = v->colorset_;
        if(type == wxT("dot")) {
            type = wxT("Dot");
        }

		SP_DS_Metadataclass* l_pcMetadataclass;
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
		if(!l_pcMetadataclass)
			return false;
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if(!l_pcNewMetadata)
			return false;
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
		if(!l_pcColList)
			return false;
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,name);
		l_pcColList->SetCell(l_nNewRow,1,type);
		lookupVariables[v->name_] = l_pcNewMetadata;

	}
	return true;
}

bool SP_ImportCANDL::CreateFunctions(const dssd::andl::ColorFunctions& p_Functions)
{
	for(auto& f : p_Functions)
	{
		if(!f) continue;

		if(lookupFunctions.find(f->name_) != lookupFunctions.end())
		{
			continue;
			//return false;
		}

		if (l_finalFunSel.Index(f->name_, true) == wxNOT_FOUND) { continue; }//by george

		wxString name = f->name_;
		wxString type = f->ret_;
		dssd::andl::ColorFunction::Params params = f->params_;
		wxString param;
		for(auto& p : params)
		{
			if(!param.IsEmpty())
				param << wxT(",");
			param << p.first << wxT(" ") << p.second;
		}
		wxString body = f->body_;
		AdaptColorExpression(body);
		SP_DS_Metadataclass* l_pcMetadataclass;
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		if(!l_pcMetadataclass)
			return false;
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if(!l_pcNewMetadata)
			return false;
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		if(!l_pcColList)
			return false;
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

		l_pcColList->SetCell(l_nNewRow,0,type);
		l_pcColList->SetCell(l_nNewRow,1,name);
		l_pcColList->SetCell(l_nNewRow,2,param);
		l_pcColList->SetCell(l_nNewRow,3,body);
		lookupFunctions[f->name_] = l_pcNewMetadata;

	}
	return true;
}

bool SP_ImportCANDL::CreateObservers(const dssd::andl::Observers& p_Observers)
{
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (mc)
	{
		for(auto& obs : p_Observers)
		{
			if(!obs) continue;

			wxString name = obs->name_;
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

bool
SP_ImportCANDL::CreateTransitions(const dssd::andl::Transitions& p_Transitions)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dssd::andl::NetType::COL_CPN_T)
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

		wxString guard = t->guard_;
		AdaptColorExpression(guard);
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(SP_DS_CPN_GUARDLIST));
		guard = guard.AfterFirst('[');//by george
		guard = guard.BeforeLast(']');//by george
		l_pcAttr->SetCell(0,1,guard);

		if (m_eNetType == dssd::andl::NetType::COL_SPN_T
			|| m_eNetType == dssd::andl::NetType::COL_GSPN_T
			|| m_eNetType == dssd::andl::NetType::COL_XSPN_T
			|| m_eNetType == dssd::andl::NetType::COL_CPN_T
			|| m_eNetType == dssd::andl::NetType::COL_HPN_T)
		{
			wxString function = wxT("1");
			if (!t->function_.empty())
			{
				function = t->function_;
				AdaptColorExpression(function);
			}

			SP_DS_ColListAttribute* l_pcAttr;
			if(nodeClass->GetName() == SP_DS_DETERMINISTIC_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("DelayList")));
			}
			else if(nodeClass->GetName() == SP_DS_SCHEDULED_TRANS)
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("PeriodicList")));
			}
			else
			{
				l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("FunctionList")));
			}
			l_pcAttr->Clear();
			wxArrayString l_Functions;
			size_t prev = 0;
			for(size_t i = function.find(wxT("++")); i != wxNOT_FOUND; i = function.find(wxT("++"), i+2))
			{
				l_Functions.Add(function.Mid(prev,i-prev));
				prev = i;
			}
			if(l_Functions.IsEmpty())
			{
				l_Functions.Add(function);
			}
			for(size_t i = 0; i < l_Functions.Count(); ++i)
			{
				wxString pred;
				wxString func;
				if(l_Functions[i].Contains(wxT("]")))
				{
					pred = l_Functions[i].AfterFirst('[').BeforeFirst(']');
					func =  l_Functions[i].AfterFirst(']');
				}
				else
				{
					pred = wxT("true");
					func = l_Functions[i];
				}
				unsigned int l_nNewRow = l_pcAttr->AppendEmptyRow();
				l_pcAttr->SetCell(l_nNewRow,0, pred);
				if(nodeClass->GetName() == SP_DS_SCHEDULED_TRANS)
				{
					wxArrayString vals = wxStringTokenize(func,  wxT(","));
					l_pcAttr->SetCell(l_nNewRow,1, vals[0]);
					l_pcAttr->SetCell(l_nNewRow,2, vals[1]);
					l_pcAttr->SetCell(l_nNewRow,3, vals[2]);
				}
				else
				{
					l_pcAttr->SetCell(l_nNewRow,1, func);
				}
			}
		}
		l_node->ShowOnCanvas(l_pcCanvas, FALSE, x ,y ,0 );
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
					w.modifier_ = c->value_;
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
					w.reset_ = u->value_;
					break;
				default:
					SP_LOGWARNING(wxT("unknown update of '") + u->place_ + wxT("' and '") + t->name_ + wxT("'"));
					break;
			}
		}
		lookupTransitions[t->name_] = l_node;
	}
	return true;
}

bool
SP_ImportCANDL::CreateArcs()
{
	map<pair<std::string, std::string>, Weights >::const_iterator aIt;

	for(aIt = arcs.begin();aIt != arcs.end(); ++aIt)
	{
		const Weights& w = (*aIt).second;
		if(!w.vpt_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.vpt_, SP_DS_EDGE);
		}
		if(!w.ve_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.ve_, SP_DS_EQUAL_EDGE);
		}
		if(!w.vr_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.vr_, SP_DS_READ_EDGE);
		}
		if(!w.vi_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.vi_, SP_DS_INHIBITOR_EDGE);
		}
		if(!w.modifier_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.modifier_, SP_DS_MODIFIER_EDGE);
		}
		if(!w.reset_.IsEmpty())
		{
			CreateEdge(w.place_, w.trans_ , w.reset_, SP_DS_RESET_EDGE);
		}
		if(!w.vtp_.IsEmpty())
		{
			CreateEdge(w.trans_ , w.place_,  w.vtp_, SP_DS_EDGE);
		}
	}
    return true;
}

SP_DS_Graph*
SP_ImportCANDL::CreateDocument(const wxString& p_sFile, dssd::andl::NetType p_eType)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
    wxString netName;
	if(m_eNetType == dssd::andl::NetType::COL_PN_T)
	{
		netName = SP_DS_COLPN_CLASS;
	}
	else if(m_eNetType == dssd::andl::NetType::COL_XPN_T)
	{
		netName = SP_DS_COLEPN_CLASS;
	}
	else if(m_eNetType == dssd::andl::NetType::COL_SPN_T
			|| m_eNetType == dssd::andl::NetType::COL_GSPN_T
			|| m_eNetType == dssd::andl::NetType::COL_XSPN_T)
	{
		netName = SP_DS_COLSPN_CLASS;
	}
	else if(m_eNetType == dssd::andl::NetType::COL_CPN_T)
	{
		netName = SP_DS_COLCPN_CLASS;
	}
	else if(m_eNetType == dssd::andl::NetType::COL_HPN_T)
	{
		netName = SP_DS_COLHPN_CLASS;
	}
	else
	{
		SP_LOGERROR(wxT("net class not supported!"));
		return nullptr;
	}

	SP_GM_DocTemplate* l_pcTemplate = l_pcDM->GetTemplate(netName);
	SP_DS_Netclass* newClass = l_pcTemplate->GetNetclass();

	m_pcMyDoc = dynamic_cast<SP_MDI_Doc*>(((wxDocTemplate*)l_pcTemplate)->CreateDocument(wxT(""), 1));
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

bool
SP_ImportCANDL::CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight, const wxString& type)
{
	CHECK_POINTER(source, SP_LOGDEBUG(wxString(wxT("source is NULL"))));
	CHECK_POINTER(target, SP_LOGDEBUG(wxString(wxT("target is NULL"))));

	wxString w = weight;
	AdaptColorExpression(w);
	SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(type);
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Edge *l_edge =  l_pcEC->GetPrototype()->Clone();
	l_edge->SetNetnumber(l_pcCanvas->GetNetnumber());
	l_edge->SetNodes(source,target);
	l_edge->SetNodes(*(source->GetGraphics()->begin()),*(target->GetGraphics()->begin()));
	SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_edge->GetAttribute(SP_DS_CPN_INSCRIPTION));
	if(l_pcAttr)
		l_pcAttr->SetCell(0,1,w);
	l_pcEC->AddElement(l_edge);
	l_edge->ShowOnCanvas(l_pcCanvas, FALSE);
	CHECK_POINTER(l_edge->GetGraphics(),SP_LOGDEBUG(wxT("no graphics")));

    return true;
}

void
SP_ImportCANDL::AdaptColorExpression(wxString& p_ColorExpression)
{
	p_ColorExpression.Replace("all", "all()");
	p_ColorExpression.Replace("auto", "auto()");
	p_ColorExpression.Replace("!=", "<>");
	if (p_ColorExpression.Contains(wxT("all")) && p_ColorExpression[0] == '(')
	{//by george
		wxString l_sToken,l_sColor;
		l_sToken = p_ColorExpression.BeforeFirst('`');

		l_sColor = p_ColorExpression.AfterFirst('`');

		wxString l_sCNew = l_sColor.AfterFirst('(');
		l_sCNew = l_sCNew.BeforeLast(')');
		p_ColorExpression = wxT("");
		p_ColorExpression << l_sToken << wxT('`') << l_sCNew;

	}
}


bool
SP_ImportCANDL::AddToDialog(SP_DLG_ImportProperties* p_pcDlg,  dssd::andl::Net_ptr net)
{
	SP_LOGMESSAGE("inside AddToDialog for SP_ExportReport");

	m_pcCheckSelectiveChoices = new wxCheckBox(p_pcDlg, SP_ID_CHECK_SELECTIVE, "Selective import");
	 m_pcCheckIsNewDoc= new wxCheckBox(p_pcDlg, SP_ID_IS_NEW_DOC, "New model");
	 m_pcCheckIsNewDoc->SetValue(true);

	m_pcCheckSelectiveChoices->Bind(wxEVT_CHECKBOX, &SP_ImportCANDL::OnSelectiveChoice, this, SP_ID_CHECK_SELECTIVE);
	m_pcCheckIsNewDoc->Bind(wxEVT_CHECKBOX, &SP_ImportCANDL::OnIsNewDoc, this, SP_ID_IS_NEW_DOC);

	p_pcDlg->SetNotebookSize(400, 200);
	p_pcDlg->AddCheckBox(m_pcCheckSelectiveChoices);
	p_pcDlg->AddCheckBox(m_pcCheckIsNewDoc);


	this->p_pcDoc = net;
	this->p_pcDlg = p_pcDlg;
	m_Options_funs.Clear();
	m_Options_funs_order.Clear();

	m_Options_cs.Clear();
	m_Options_cs_order.Clear();
	m_Options_constants.clear();
	m_Options_constants_order.Clear();
	m_Options_vars.clear();
	m_Options_vars_order.Clear();
	l_finalFunSel.clear();
	l_finalcolorsets.clear();
	l_finalvariables.clear();
	l_finalconstants.clear();



	AddConstants();
	AddColorSets();
	AddFunctions();
	AddVariables();

	CreateConstantsDepTrees();
	CreateColorsetDepTrees();
	BuildVariableDependencies();
	BuildFunctionsDependencyTrees();
	ComputeBackwardDependencies();
	ComputeColorSetsBackwardDependencies();
	ComputeConstantBackwardDependencies();

	return true;

}

void SP_ImportCANDL::ONselectAll(wxCommandEvent& p_cEvent)
{
	if (m_pcCheckAll->IsChecked())
	{

		for (int i = 0; i < m_Options_constants_order.size(); i++)
		{
			m_pcRearrangelist_constants->Check(m_Options_constants_order[i], true);
		}
		for (int i = 0; i < m_Options_cs_order.size(); i++)
		{
			m_pcRearrangelist_colorsets->Check(m_Options_cs_order[i], true);
		}
		for (int i = 0; i < m_Options_funs_order.size(); i++)
		{
			m_pcRearrangelist_function->Check(m_Options_funs_order[i], true);
		}
		for (int i = 0; i < m_Options_vars_order.size(); i++)
		{
			m_pcRearrangelist_variables->Check(m_Options_vars_order[i], true);
		}
		PrintChosenConstants();
		PrintChosenColorSets();
		PrintColorFunctions();
		PrintVariables();

	}
	else
	{

		for (int i = 0; i < m_Options_constants_order.size(); i++)
		{
			m_pcRearrangelist_constants->Check(m_Options_constants_order[i], false);
		}
		for (int i = 0; i < m_Options_cs_order.size(); i++)
		{
			m_pcRearrangelist_colorsets->Check(m_Options_cs_order[i], false);
		}
		for (int i = 0; i < m_Options_funs_order.size(); i++)
		{
			m_pcRearrangelist_function->Check(m_Options_funs_order[i], false);
		}
		for (int i = 0; i < m_Options_vars_order.size(); i++)
		{
			m_pcRearrangelist_variables->Check(m_Options_vars_order[i], false);
		}

		PrintChosenConstants();
		PrintChosenColorSets();
		PrintColorFunctions();
		PrintVariables();
		m_pcTextconstants1->SetValue(wxT(""));
	}
}

void SP_ImportCANDL:: OnIsNewDoc(wxCommandEvent& p_cEvent)
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

void SP_ImportCANDL::OnSelectiveChoice(wxCommandEvent& p_cEvent)
{
	if (m_pcCheckSelectiveChoices->IsChecked())
	{
		m_pcCheckAll->Enable(true);
		m_pcRearrangelist_function->Enable(true);
		m_pcRearrangelist_constants->Enable(true);
		m_pcRearrangelist_variables->Enable(true);
		m_pcRearrangelist_colorsets->Enable(true);
    }
	else
	{
		m_pcCheckAll->Enable(false);
		m_pcRearrangelist_function->Enable(false);
		m_pcRearrangelist_constants->Enable(false);
		m_pcRearrangelist_variables->Enable(false);
		m_pcRearrangelist_colorsets->Enable(false);
	}
}

void
SP_ImportCANDL::AddFunctions()
{
	m_pcNotebookPageFunctions = p_pcDlg->AddPage(wxT("color functions"));

	m_pcMainSizer_funs = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_funs = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageFunctions, -1, wxT("colour functions")), wxVERTICAL);
	m_pcRightSizer_funs = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_funs = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageFunctions, -1, wxT("function definitions")), wxVERTICAL);
	m_pcTextfunctions1= new wxTextCtrl(m_pcNotebookPageFunctions, SP_ID_FUNS_DEF, "", wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_funs->Add(m_pcLeftSizer_funs, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_funs->Add(m_pcRightSizer_funs, 1, wxALL | wxEXPAND, 5);

	AddFunctions_Att();

	//Rearrangelist for reordering the sequence of Declarations
	m_pcRearrangelist_function = new wxRearrangeList(m_pcNotebookPageFunctions, SP_ID_DECLARATIONS_UPDATE, wxDefaultPosition,
		wxSize(50, 200), m_Options_funs_order, m_Options_funs);
	m_pcLeftSizer_funs->Add(m_pcRearrangelist_function, 1, wxALL | wxEXPAND, 5);


	m_pcRearrangelist_function->Enable(false);
	m_pcRightSizer_funs->Add(m_pcTextfunctions1);


	m_pcRearrangelist_function->Bind(wxEVT_CHECKLISTBOX, &SP_ImportCANDL::OnSelChange_funs, this, SP_ID_DECLARATIONS_UPDATE);




	m_pcNotebookPageFunctions->AddControl(m_pcMainSizer_funs, 1, wxALL | wxEXPAND, 5);
}

void SP_ImportCANDL::AddColorSets()
{
	m_pcNotebookPagecolorsets = p_pcDlg->AddPage(wxT("Color Sets"));

	m_pcMainSizer_colorsets = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_colorsets = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPagecolorsets, -1, wxT("Color set names")), wxVERTICAL);
	m_pcRightSizer_colorsets = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_colorsets = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPagecolorsets, -1, wxT("color sets")), wxVERTICAL);
	m_pcTextcolorsets1 = new wxTextCtrl(m_pcNotebookPagecolorsets, SP_ID_CS_TESXT,"", wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);


	m_pcMainSizer_colorsets->Add(m_pcLeftSizer_colorsets, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_colorsets->Add(m_pcRightSizer_colorsets, 1, wxALL | wxEXPAND, 5);

	AddColorSets_Att();

	m_pcTextcolorsets1->Fit();


	//Rearrangelist for reordering the sequence of Declarations
	m_pcRearrangelist_colorsets = new wxRearrangeList(m_pcNotebookPagecolorsets, SP_ID_DECLARATIONS_COLORSETS, wxDefaultPosition,
		wxSize(50, 200), m_Options_cs_order, m_Options_cs);
	m_pcLeftSizer_colorsets->Add(m_pcRearrangelist_colorsets, 1, wxALL | wxEXPAND, 5);

	m_pcRearrangelist_colorsets->Enable(false);

	m_pcRightSizer_colorsets->Add(m_pcTextcolorsets1);


	m_pcRearrangelist_colorsets->Bind(wxEVT_CHECKLISTBOX, &SP_ImportCANDL::OnSelChange_Colorsets, this, SP_ID_DECLARATIONS_COLORSETS);




	m_pcNotebookPagecolorsets->AddControl(m_pcMainSizer_colorsets, 1, wxALL | wxEXPAND, 5);

}

void SP_ImportCANDL::AddConstants()
{
	m_pcNotebookPageConstants = p_pcDlg->AddPage(wxT("constants"));

	m_pcMainSizer_constants = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_constants = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageConstants, -1, wxT("Constants")), wxVERTICAL);
	m_pcRightSizer_constants = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_constants = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageConstants, -1, wxT("constants definitions")), wxVERTICAL);

	m_pcTextconstants1= new wxTextCtrl(m_pcNotebookPageConstants, SP_ID_CONSTANTS_TESXT, "", wxDefaultPosition, wxSize(300, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_constants->Add(m_pcLeftSizer_constants, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_constants->Add(m_pcRightSizer_constants, 1, wxALL | wxEXPAND, 5);

	m_pcCheckAll = new wxCheckBox(p_pcDlg, SP_ID_CHECK_ALL, wxT("Select all"), wxDefaultPosition);
	m_pcCheckAll->SetValue(true);
	p_pcDlg->AddCheckBoxSelectAll(m_pcCheckAll);
	m_pcCheckAll->Bind(wxEVT_CHECKBOX, &SP_ImportCANDL::ONselectAll, this, SP_ID_CHECK_ALL);
	m_pcCheckAll->Enable(false);

	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	AddConstants_Att();

	m_pcRearrangelist_constants = new wxRearrangeList(m_pcNotebookPageConstants, SP_ID_DECLARATIONS_CONSTANTS, wxDefaultPosition,
		wxSize(40, 175), m_Options_constants_order, m_Options_constants);
	m_pcLeftSizer_constants->Add(m_pcRearrangelist_constants, 1, wxALL | wxEXPAND, 5);

	m_pcRearrangelist_constants->Enable(false);


	m_pcRightSizer_constants->Add(m_pcTextconstants1);


	m_pcRearrangelist_constants->Bind(wxEVT_CHECKLISTBOX, &SP_ImportCANDL::OnSelChange_Constants, this, SP_ID_DECLARATIONS_CONSTANTS);


	m_pcNotebookPageConstants->AddControl(m_pcMainSizer_constants, 1, wxALL | wxEXPAND, 5);
}

void SP_ImportCANDL::AddVariables()
{
	m_pcNotebookPagevariables = p_pcDlg->AddPage(wxT("Variables"));

	m_pcMainSizer_variables = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_variables = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPagevariables, -1, wxT("Variable names")), wxVERTICAL);
	m_pcRightSizer_variables = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_variables = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPagevariables, -1, wxT("Variable defintions")), wxVERTICAL);
	//m_pcTextvariables = new wxStaticText(m_pcNotebookPagevariables, SP_ID_VARS_TESXT, "variables", wxDefaultPosition, wxSize(300, 200));
	m_pcTextvariable1= new wxTextCtrl(m_pcNotebookPagevariables, SP_ID_VARS_TESXT, "", wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_variables->Add(m_pcLeftSizer_variables, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_variables->Add(m_pcRightSizer_variables, 1, wxALL | wxEXPAND, 5);

	AddVariable_Att();

	m_pcRearrangelist_variables = new wxRearrangeList(m_pcNotebookPagevariables, SP_ID_DECLARATIONS_VARS, wxDefaultPosition,
		wxSize(50, 200), m_Options_vars_order, m_Options_vars);
	m_pcRearrangelist_variables->Enable(false);
	m_pcLeftSizer_variables->Add(m_pcRearrangelist_variables, 1, wxALL | wxEXPAND, 5);


	m_pcRightSizer_variables->Add(m_pcTextvariable1);


	m_pcRearrangelist_variables->Bind(wxEVT_CHECKLISTBOX, &SP_ImportCANDL::OnSelChange_Variables, this, SP_ID_DECLARATIONS_VARS);




	m_pcNotebookPagevariables->AddControl(m_pcMainSizer_variables, 1, wxALL | wxEXPAND, 5);
}

void SP_ImportCANDL::OnSelChange_Colorsets(wxCommandEvent& p_cEvent)
{

	bool b = m_pcRearrangelist_colorsets->IsChecked(p_cEvent.GetInt());
	m_pcCheckAll->SetValue(false);

	if (!b)
	{
		wxString l_scolorsets;
		wxString csName = m_Options_cs[p_cEvent.GetInt()];
		for (auto& cs : *p_pcDoc->colorsets_)
		{
			if (m_Options_cs.Index(cs->name_) == wxNOT_FOUND)
			{
				continue;
			}
			int ind = m_Options_cs.Index(cs->name_);
			if (!m_pcRearrangelist_colorsets->IsChecked(ind)) { continue; }
			if (cs->name_ == csName) { continue; }

			wxString type = wxT("int");
			if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
			{
				type = wxT("enum");
			}
			else if (cs->type_ == dssd::andl::ColorsetType::DOT_T)
			{
				type = wxT("Dot");
			}
			else if (cs->type_ == dssd::andl::ColorsetType::STRING_T)
			{
				type = wxT("string");
			}
			else
			{

			}
			wxString colors = cs->definition_;
			bool l_bSimple = true;
			if (colors.StartsWith(wxT("PROD")))
			{
				l_bSimple = false;
				type = wxT("product");

			}
			else if (colors.Contains("dot"))
			{
				type = wxT("Dot");
			}
			else
			{
				if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
				{
					type = colors.BeforeFirst(wxChar('['));
				}

			}


			l_scolorsets << csName << wxT("=") << colors << wxT("\n");

		}

		m_pcTextcolorsets1->SetValue(l_scolorsets);
	}
	else
	{
		wxString l_sCS;
		for (int i = 0; i < m_Options_cs.size(); i++)
		{
			wxString cs1 = m_Options_cs[i];
			if (m_pcRearrangelist_colorsets->IsChecked(i))
			{
				for (auto& cs : *p_pcDoc->colorsets_)
				{
					if (cs->name_ != cs1) { continue; }

					wxString type = wxT("int");
					if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
					{
						type = wxT("enum");
					}
					else if (cs->type_ == dssd::andl::ColorsetType::STRING_T)
					{
						type = wxT("string");
					}
					else if (cs->type_ == dssd::andl::ColorsetType::DOT_T)
					{
						type = wxT("Dot");
					}
					wxString colors = cs->definition_;
					bool l_bSimple = true;
					if (colors.StartsWith(wxT("PROD")))
					{
						l_bSimple = false;
						type = wxT("product");
					}
					else if (colors.Contains("dot"))
					{
						type = wxT("Dot");
					}
					else
					{
						if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
						{
							type = colors.BeforeFirst(wxChar('['));
						}

					}

					l_sCS << cs1 << wxT("=") << colors << wxT("\n");

				}
			}

		}

		m_pcTextcolorsets1->SetValue(l_sCS);
	}

	if (!b)
	{
		wxString l_selectedCs = m_Options_cs[p_cEvent.GetInt()];

		for (int j = 0; j < m_vColorsetDepTrees.size(); j++)
		{
			if (m_vColorsetDepTrees[j]->key == l_selectedCs)
			{
				std::map<DEC_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vColorsetDepTrees[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{
						if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_cs.Index(*itSet);
								m_pcRearrangelist_colorsets->Check(index, false);
							}

						}
						else if (itMap->first == DEC_TYPE::VAR_DEC)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_vars.Index(*itSet);
								m_pcRearrangelist_variables->Check(index, false);
							}
						}
						else if (itMap->first == DEC_TYPE::FUN)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, false);
							}
						}
					}
				}
			}
		}

	}

	if (b)//user wnats to choose
	{
		wxString l_selectedCs = m_Options_cs[p_cEvent.GetInt()];
		for (auto itMapCS = m_mColorset2Dependncies.begin(); itMapCS != m_mColorset2Dependncies.end(); ++itMapCS)
		{
			if (itMapCS->first == l_selectedCs)
			{
				for (auto itMapDec = itMapCS->second.begin(); itMapDec != itMapCS->second.end(); ++itMapDec)
				{
					if (itMapDec->first == DEC_TYPE::COLOR_SET_DEC)
					{
						for (auto itSet = itMapDec->second.begin(); itSet != itMapDec->second.end(); ++itSet)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}
					}
					if (itMapDec->first == DEC_TYPE::CONSTANT_DEC)
					{
						for (auto itSet = itMapDec->second.begin(); itSet != itMapDec->second.end(); ++itSet)
						{
							int index = m_Options_constants.Index(*itSet);
							m_pcRearrangelist_constants->Check(index, true);
						}
					}
					//added
					if (itMapDec->first == DEC_TYPE::VAR_DEC)
					{
						for (auto itSet = itMapDec->second.begin(); itSet != itMapDec->second.end(); ++itSet)
						{
							int index = m_Options_vars.Index(*itSet);
							m_pcRearrangelist_variables->Check(index, true);
						}
					}

				}

			}
		}

	}

	PrintChosenColorSets();
	PrintChosenConstants();
	PrintVariables();
	PrintColorFunctions();
}
void SP_ImportCANDL::OnSelChange_Constants(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_constants->IsChecked(p_cEvent.GetInt());

	m_pcCheckAll->SetValue(false);
	if (!b)
	{
		bool l_bOverwrite = 0;
		wxString l_sCheckedconstName = m_Options_constants[p_cEvent.GetInt()];
		for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
		{
			if (!l_bOverwrite)
				m_pcTextconstants1->SetValue(itMap->first + _T(":\n"));
			else
				m_pcTextconstants1->AppendText(itMap->first + _T(":\n"));

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
						m_pcTextconstants1->AppendText(l_sConstDef);
					}
				}
			}

			m_pcTextconstants1->AppendText(wxT("\n"));

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
								m_pcTextconstants1->AppendText(l_sConstDef);
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
		for (int j = 0; j < m_vConstantsDepTrees.size(); j++)
		{
			if (m_vConstantsDepTrees[j]->key == l_selectedConstant)
			{
				std::map<DEC_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vConstantsDepTrees[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{
						if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_cs.Index(*itSet);
								m_pcRearrangelist_colorsets->Check(index, false);

							}

						}
						else if (itMap->first == DEC_TYPE::VAR_DEC)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_vars.Index(*itSet);
								m_pcRearrangelist_variables->Check(index, false);
							}
						}
						else if (itMap->first == DEC_TYPE::FUN)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, false);
							}
						}
						else if (itMap->first == DEC_TYPE::CONSTANT_DEC)
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

						}
					}
				}
			}
		}

	}
	if (b)
	{
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

	///do printing on the right hand panel
	PrintChosenConstants();
	PrintChosenColorSets();
	PrintVariables();
	PrintColorFunctions();

}

void SP_ImportCANDL::PrintChosenConstants()
{
	m_pcTextconstants1->Clear();
	for (auto itMap = m_mGroup2Const.begin(); itMap != m_mGroup2Const.end(); ++itMap)
	{
		if (m_mGroup2Const.size() == 1)//for preventing overwriting the old value
			m_pcTextconstants1->SetValue(itMap->first + _T(":\n"));
		else
			m_pcTextconstants1->AppendText(itMap->first + _T(":\n"));

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
					m_pcTextconstants1->AppendText(l_sConstDef);
				}
			}
		}
	}

}

void SP_ImportCANDL::PrintChosenColorSets()
{
	wxString l_sCS;


	for (auto& cs : *p_pcDoc->colorsets_)
	{
		wxString type = wxT("int");
		if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
		{
			type = wxT("enum");
		}
		else if (cs->type_ == dssd::andl::ColorsetType::STRING_T)
		{
			type = wxT("string");
		}
		else if (cs->type_ == dssd::andl::ColorsetType::DOT_T)
		{
			type = wxT("Dot");
		}
		wxString colors = cs->definition_;
		bool l_bSimple = true;
		if (colors.StartsWith(wxT("PROD")))
		{
			l_bSimple = false;
			type = wxT("product");
		}
		else if (colors.Contains("dot"))
		{
			type = wxT("Dot");
		}
		else
		{
			if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
			{
				type = colors.BeforeFirst(wxChar('['));
			}


		}
		wxString l_sCSNAME;
		l_sCSNAME << cs->name_;

		int ind = m_Options_cs.Index(l_sCSNAME);
		if (!m_pcRearrangelist_colorsets->IsChecked(ind)) { continue; }

		l_sCS << cs->name_ << wxT("=") << colors << wxT("\n");

	}

	m_pcTextcolorsets1->SetValue(l_sCS);
}


void SP_ImportCANDL::OnSelChange_Variables(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_variables->IsChecked(p_cEvent.GetInt());
	m_pcCheckAll->SetValue(false);

	if (!b)
	{
		wxString l_sVars;
		wxString varName = m_Options_vars[p_cEvent.GetInt()];
		for (auto& var : *p_pcDoc->variables_)
		{
			int ind = m_Options_vars.Index(var->name_);
			if (!m_pcRearrangelist_variables->IsChecked(ind)) { continue; }
			if (var->name_ == varName) { continue; }

			l_sVars << var->colorset_ << wxT(" : ") << var->name_ << wxT(";") << wxT("\n");


		}
		m_pcTextvariable1->SetValue(l_sVars);
	}
	else
	{
		wxString l_svaraibles;
		for (int i = 0; i < m_Options_vars.size(); i++)
		{
			wxString varName1 = m_Options_vars[i];
			if (m_pcRearrangelist_variables->IsChecked(i))
			{
				for (auto& var : *p_pcDoc->variables_)
				{
					if (var->name_ != varName1) { continue; }
					l_svaraibles << var->colorset_ << wxT(" : ") << var->name_ << wxT(";") << wxT("\n");
				}
			}

		}

		m_pcTextvariable1->SetValue(l_svaraibles);
	}

	if (!b)
	{
		wxString l_selectedVar = m_Options_vars[p_cEvent.GetInt()];

			for (int j = 0; j < m_vVarsDepTrees.size(); j++)
			{
				if (m_vVarsDepTrees[j]->key == l_selectedVar)
				{
					std::map<DEC_TYPE, set<wxString>> l_DepMap;

					LevelOrderTraversal(m_vVarsDepTrees[j], l_DepMap);

					if (l_DepMap.size() > 0)
					{
						for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
						{
							if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
							{
								for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
								{
									int index = m_Options_cs.Index(*itSet);
									m_pcRearrangelist_colorsets->Check(index, false);

								}

							}


							else if (itMap->first == DEC_TYPE::CONSTANT_DEC)
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

							}
						}
					}
				}
			}


	}

	if (b)
	{
		wxString varName = m_Options_vars[p_cEvent.GetInt()];
		for (unsigned i = 0; i < m_Options_vars.size(); i++)
		{

			if (varName != m_Options_vars[i]) continue;
			auto itfound = m_varBackwordDep.find(m_Options_vars[i]);

			if (itfound != m_varBackwordDep.end())
			{

				for (auto itMap = itfound->second.begin(); itMap != itfound->second.end(); ++itMap)
				{
					if (itMap->first == DEC_TYPE::CONSTANT_DEC)
					{
						for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
						{
							int index = m_Options_constants.Index(*itSet);
							m_pcRearrangelist_constants->Check(index, true);
						}
					}
					if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
					{
						for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}
					}
				}
			}
		}
	}
	PrintVariables();
	PrintChosenConstants();
	PrintChosenColorSets();
	PrintColorFunctions();

}
void SP_ImportCANDL::OnSelChange_funs(wxCommandEvent& p_cEvent)
{
	bool b=m_pcRearrangelist_function->IsChecked(p_cEvent.GetInt());
	m_pcCheckAll->SetValue(false);
	if (!b)
	{
		wxString l_sFunctions;
		wxString funName=m_Options_funs[p_cEvent.GetInt()];
		for (auto& fun : *p_pcDoc->color_functions_)
		{
			int ind=m_Options_funs.Index(fun->name_);
			if (!m_pcRearrangelist_function->IsChecked(ind)) { continue; }
			if (fun->name_ == funName) { continue; }
			wxString par;
			for (auto it : fun->params_)
			{
				par << it.first << "," << it.second;
			}
			l_sFunctions << fun->ret_ << wxT(" ") << fun->name_ << wxT("(") << par << wxT(") ") << wxT("{") << fun->body_ << wxT("};\n");

		}
		m_pcTextfunctions1->SetValue(l_sFunctions);
	}
	else
	{
		wxString l_sfundef;
		for (int i = 0; i < m_Options_funs.size(); i++)
		{
			wxString funName1 = m_Options_funs[i];
			if (m_pcRearrangelist_function->IsChecked(i))
			{
				for (auto& fun : *p_pcDoc->color_functions_)
				{
					if (fun->name_ != funName1) { continue; }
					wxString par1;
					for (auto it : fun->params_)
					{
						par1 << it.first << "," << it.second;
					}
					l_sfundef << fun->ret_ << wxT(" ") << fun->name_ << wxT("(") << par1 << wxT(")\n") << wxT("{") << fun->body_ << wxT("};\n");
				}
			}

		}
		m_pcTextfunctions1->SetValue(l_sfundef);
	}

	if (b)
	{
		for (unsigned i = 0; i < m_Options_funs.size(); i++)
		{
			auto itfound = m_funBackwordDep.find(m_Options_funs[i]);

			if (itfound != m_funBackwordDep.end())
			{

				for (auto itMap = itfound->second.begin(); itMap != itfound->second.end(); ++itMap)
				{
					if (itMap->first == DEC_TYPE::CONSTANT_DEC)
					{
						for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
						{
							int index = m_Options_constants.Index(*itSet);
							m_pcRearrangelist_constants->Check(index, true);
						}
					}
					if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
					{
						for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}
					}

					if (itMap->first == DEC_TYPE::FUN)
					{
						for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
						{
							int index = m_Options_funs.Index(*itSet);
							m_pcRearrangelist_function->Check(index, true);
						}
					}
				}
			}
		}
	}

	if (!b)
	{
		wxString l_selectedfun = m_Options_funs[p_cEvent.GetInt()];

		for (int j = 0; j < m_vFunctionsDepTrees.size(); j++)
		{
			if (m_vFunctionsDepTrees[j]->key == l_selectedfun)
			{
				std::map<DEC_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vFunctionsDepTrees[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{
						if (itMap->first == DEC_TYPE::FUN)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_funs.Index(*itSet);
								m_pcRearrangelist_function->Check(index, false);

							}

						}
					}
				}
			}
		}



	}
	PrintChosenColorSets();
	PrintColorFunctions();
	PrintChosenConstants();



}


void SP_ImportCANDL::AddColorSets_Att()
{
	wxString l_sCS;

	int l_nOrder = 0;
	for (auto& cs: *p_pcDoc->colorsets_)
	{
		wxString type = wxT("int");
		if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
		{
			type = wxT("enum");
		}
		else if (cs->type_ == dssd::andl::ColorsetType::STRING_T)
		{
			type = wxT("string");
		}
		else if (cs->type_ == dssd::andl::ColorsetType::DOT_T)
		{
			type = wxT("Dot");
		}
		wxString colors = cs->definition_;
		bool l_bSimple = true;
		if (colors.StartsWith(wxT("PROD")))
		{
			l_bSimple = false;
			type = wxT("product");
		}
		else if (colors.Contains("dot"))
		{
			type = wxT("Dot");
		}
		else
		{
			if (colors.Contains(wxT("[") )&& colors.Contains(wxT("]")))
			{
				type=colors.BeforeFirst(wxChar('['));
			}


		}

		l_sCS<< cs->name_ << wxT("=") << colors << wxT("\n");

		m_Options_cs.Add(cs->name_);

		m_Options_cs_order.Add(l_nOrder);
		l_nOrder++;

	}

	m_pcTextcolorsets1->SetValue(l_sCS);

}

void SP_ImportCANDL::AddConstants_Att()
{
	m_pcTextconstants1->SetValue(wxT(""));
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
		if (m_mGroup2Const.size() == 1)//for preventing overwriting the old value
			m_pcTextconstants1->SetValue(itMap->first + _T(":\n"));
		else
			m_pcTextconstants1->AppendText(itMap->first + _T(":\n"));

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
					m_pcTextconstants1->AppendText(l_sConstDef);
				}
			}
		}
	}
}

void SP_ImportCANDL::AddVariable_Att()
{
	wxString l_sVar;

	int l_nOrder = 0;
	for (auto& var : *p_pcDoc->variables_)
	{

		l_sVar << var->colorset_ << wxT(" : ") << var->name_ << wxT(";")  << wxT("\n");

		m_Options_vars.Add(var->name_);

		m_Options_vars_order.Add(l_nOrder);
		l_nOrder++;

	}

	m_pcTextvariable1->SetValue(l_sVar);
}

void SP_ImportCANDL::AddFunctions_Att()
{
	wxString l_sFunctions;

	int l_nOrder=0;
	for (auto& fun : *p_pcDoc->color_functions_)
	{
		wxString par;
		for (auto it : fun->params_)
		{
			par << it.first << "," << it.second;
		}
		l_sFunctions << fun->ret_ << wxT(" ") << fun->name_<<wxT("(")<< par <<wxT(") ") << wxT("{") << fun->body_ << wxT("};\n");
		m_Options_funs.Add(fun->name_);

		m_Options_funs_order.Add(l_nOrder);
		l_nOrder++;

	}
	m_pcTextfunctions1->SetValue(l_sFunctions);

}

void SP_ImportCANDL::PrintVariables()
{
	wxString l_sVar;


	for (auto& var : *p_pcDoc->variables_)
	{
		wxString var_name_(var->name_.c_str(), wxConvUTF8);

		int ind = m_Options_vars.Index(var_name_);
		if (!m_pcRearrangelist_variables->IsChecked(ind)) { continue; }

		l_sVar << var->colorset_ << wxT(" : ") << var->name_ << wxT(";") << wxT("\n");


	}

	m_pcTextvariable1->SetValue(l_sVar);
}


void SP_ImportCANDL::PrintColorFunctions()
{
	wxString l_sFunctions;

	//int l_nOrder = 0;
	for (auto& fun : *p_pcDoc->color_functions_)
	{
		wxString val_(fun->name_.c_str(), wxConvUTF8);
		int ind = m_Options_funs.Index(val_);
		if (!m_pcRearrangelist_function->IsChecked(ind)) { continue; }

		wxString par;
		for (auto it : fun->params_)
		{
			par << it.first << "," << it.second;
		}
		l_sFunctions << fun->ret_ << wxT(" ") << fun->name_ << wxT("(") << par << wxT(") ") << wxT("{") << fun->body_ << wxT("};\n");


	}
	m_pcTextfunctions1->SetValue(l_sFunctions);

}

void  SP_ImportCANDL::BuildColorSetDependencyTree(const wxString& p_sCS, dec_node* tree)
{
	for (int i = 0; i < m_Options_cs.size(); i++)
	{
		if (m_Options_cs[i] != p_sCS) continue;

		for (auto& var : *p_pcDoc->variables_)
		{

			if (var->colorset_ == m_Options_cs[i])
			{
				wxString  l_sVarlevel1(var->name_.c_str(), wxConvUTF8);
				(tree->child).push_back(newNode(l_sVarlevel1, DEC_TYPE::VAR_DEC));
			}

		}
		for (auto& fun : *p_pcDoc->color_functions_)
		{
			wxString listParam;
			for (auto it1 = fun->params_.begin(); it1 != fun->params_.end(); ++it1)
			{
				listParam << it1->first << wxT(",") << it1->second << wxT(",");
			}

			wxString fun1(fun->name_.c_str(), wxConvUTF8);
			if (IsToken(listParam, m_Options_cs[i]))
				(tree->child).push_back(newNode(fun1, DEC_TYPE::FUN));
		}


		for (auto& cs : *p_pcDoc->colorsets_)
		{
			wxString CSdep(cs->definition_.c_str(), wxConvUTF8);
			if (IsToken(CSdep, m_Options_cs[i]))
			{
				(tree->child).push_back(newNode(cs->name_, DEC_TYPE::COLOR_SET_DEC));
				wxString CSnewLevel(cs->name_.c_str(), wxConvUTF8);
				BuildColorSetDependencyTree(CSnewLevel, tree->child[tree->child.size() - 1]);
			}


		}

	}
}

void SP_ImportCANDL::CreateColorsetDepTrees()
{
	for (int i = 0; i < m_Options_cs.size(); i++)
	{
		dec_node* l_pcCsDepTree = newNode(m_Options_cs[i], DEC_TYPE::COLOR_SET_DEC);

		for (auto& var : *p_pcDoc->variables_)
		{

			if (var->colorset_ == m_Options_cs[i])
			{
				wxString  l_sVarlevel1(var->name_.c_str(), wxConvUTF8);
				(l_pcCsDepTree->child).push_back(newNode(l_sVarlevel1, DEC_TYPE::VAR_DEC));
			}

		}
		for (auto& fun : *p_pcDoc->color_functions_)
		{
			wxString listParam;
			for (auto it1 = fun->params_.begin(); it1 != fun->params_.end(); ++it1)
			{
				listParam << it1->first << wxT(",") << it1->second<<wxT(",");
			}

			wxString fun1(fun->name_.c_str(), wxConvUTF8);
			if (IsToken(listParam, m_Options_cs[i]))
				(l_pcCsDepTree->child).push_back(newNode(fun1, DEC_TYPE::FUN));
		}

		for (auto& cs : *p_pcDoc->colorsets_)
		{
			wxString CSdep(cs->definition_.c_str(), wxConvUTF8);
			if (IsToken(CSdep, m_Options_cs[i]))
			{
				(l_pcCsDepTree->child).push_back(newNode(cs->name_, DEC_TYPE::COLOR_SET_DEC));
				wxString CSnewLevel(cs->name_.c_str(), wxConvUTF8);
				BuildColorSetDependencyTree(CSnewLevel, l_pcCsDepTree->child[l_pcCsDepTree->child.size()-1]);
			}


		}

		m_vColorsetDepTrees.push_back(l_pcCsDepTree);
		}


}

void SP_ImportCANDL::BuildConstantDependencyTree(const wxString& p_sConstant, dec_node* tree)
{
	for (int i = 0; i < m_Options_constants.size(); i++)
	{
		if (p_sConstant != m_Options_constants[i]) continue;
		//level 0

		for (auto& cs : *p_pcDoc->colorsets_)
		{

			wxString colors = cs->definition_;

			//if (colors.Contains(m_Options_constants[i]))
			if (IsToken(colors, m_Options_constants[i]))
			{
				(tree->child).push_back(newNode(cs->name_, DEC_TYPE::COLOR_SET_DEC));
				wxString CSnewLevel(cs->name_.c_str(), wxConvUTF8);
				BuildColorSetDependencyTree(CSnewLevel, tree->child[tree->child.size() - 1]);

		    }

		}

			for (auto& fun : *p_pcDoc->color_functions_)
			{
				wxString fun1(fun->name_.c_str(), wxConvUTF8);

				bool l_bIsConstUsed = false;
				wxString listParam;
				for (auto it1 = fun->params_.begin(); it1 != fun->params_.end(); ++it1)
				{
					listParam << it1->first;//<< wxT(",") << it1->second << wxT(",");

					for (auto& cs : *p_pcDoc->colorsets_)
					{

						wxString colors = cs->definition_;

						if (IsToken(colors, m_Options_constants[i]))
						{
							l_bIsConstUsed = true; break;
						}
					}
				}

				wxString funbody(fun->body_.c_str(), wxConvUTF8);
				if (IsToken(funbody,m_Options_constants[i])|| l_bIsConstUsed)//or param use this const
					(tree->child).push_back(newNode(fun1, DEC_TYPE::FUN));
			}


			for (auto& con : *p_pcDoc->constants_)
			{
				if (con->name_ != m_Options_constants[i].ToStdString())
				{

					std::string val = con->values_[0];
					wxString conDef(val.c_str(), wxConvUTF8);
					if (IsToken(conDef, m_Options_constants[i]))
					{
						wxString constant(con->name_.c_str(), wxConvUTF8);
						(tree->child).push_back(newNode(constant, DEC_TYPE::CONSTANT_DEC));
						BuildConstantDependencyTree(constant, tree->child[tree->child.size() - 1]);
					}
				}
			}

		}


}


void SP_ImportCANDL::CreateConstantsDepTrees()
{
	for (int i = 0; i < m_Options_constants.size(); i++)
	{
		//level 0
		dec_node* l_pcConstTreeRoot=newNode(m_Options_constants[i], DEC_TYPE::CONSTANT_DEC);
		for (auto& cs : *p_pcDoc->colorsets_)
		{

			wxString colors = cs->definition_;


			if (IsToken(colors, m_Options_constants[i]))
			{
				wxString colorset(cs->name_.c_str(), wxConvUTF8);
				(l_pcConstTreeRoot->child).push_back(newNode(colorset, DEC_TYPE::COLOR_SET_DEC));
				BuildColorSetDependencyTree(colorset, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size()-1]);

			}
			for (auto& fun : *p_pcDoc->color_functions_)
			{
				bool l_bIsConstUsed = false;

				for (auto it1 = fun->params_.begin(); it1 != fun->params_.end(); ++it1)
				{
					//listParam << it1->first;//<< wxT(",") << it1->second << wxT(",");

					for (auto& cs : *p_pcDoc->colorsets_)
					{
						if (it1->first != cs->name_) continue;

						wxString colors = cs->definition_;

						if (IsToken(colors, m_Options_constants[i]))
						{
							l_bIsConstUsed = true; break;
						}
					}
				}

				wxString fun1(fun->name_.c_str(), wxConvUTF8);
				wxString funbody(fun->body_.c_str(), wxConvUTF8);

				if (IsToken(funbody, m_Options_constants[i])|| l_bIsConstUsed)
					(l_pcConstTreeRoot->child).push_back(newNode(fun1, DEC_TYPE::FUN));
			}
		}

		for (auto& con : *p_pcDoc->constants_)
		{
			if (con->name_ != m_Options_constants[i].ToStdString())
			{

				std::string val = con->values_[0];
				wxString conDef(val.c_str(), wxConvUTF8);
				if (IsToken(conDef, m_Options_constants[i]))
				{
					wxString constant(con->name_.c_str(), wxConvUTF8);
					(l_pcConstTreeRoot->child).push_back(newNode(constant, DEC_TYPE::CONSTANT_DEC));
					BuildConstantDependencyTree(constant, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size() - 1]);
				}
			}
		}

		m_vConstantsDepTrees.push_back(l_pcConstTreeRoot);
	}
}

bool SP_ImportCANDL::IsToken(const wxString& p_sExp, const wxString& p_sToken)
{

	wxStringTokenizer tokenizer(p_sExp, ":+-/*%(),={.}><[]&| ");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		if (token == p_sToken) {
		  return true;
		}

	}
	return false;
}

void  SP_ImportCANDL::LevelOrderTraversal(dec_node * root, std::map<DEC_TYPE,std::set<wxString>>& l_mapResultKey)
{
	if (root == NULL)
		return;

	// Standard level order traversal code
	// using queue
	queue<dec_node *> q;  // Create a queue
	q.push(root); // Enqueue root
	while (!q.empty())
	{
		int n = q.size();

		// If this node has children
		while (n > 0)
		{
			// Dequeue an item from queue and print it
			dec_node * p = q.front();
			q.pop();
			l_mapResultKey[p->type].insert(p->key);
			// Enqueue all children of the dequeued item
			for (int i = 0; i<p->child.size(); i++)
				q.push(p->child[i]);
			n--;
		}
	}
}

void SP_ImportCANDL::ComputeBackwordDependency(const wxString& p_sVarorFun, DEC_TYPE type, std::map<DEC_TYPE, set<wxString>>&p_mTypeToDec)
{
	for (unsigned i = 0; i < m_vColorsetDepTrees.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
		LevelOrderTraversal(m_vColorsetDepTrees[i], l_mtype2Declarations);
		//m_varBackwordDep
		for (auto itMap= l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
		{
			if (itMap->first == type)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sVarorFun)
					{
						p_mTypeToDec[DEC_TYPE::COLOR_SET_DEC].insert(m_vColorsetDepTrees[i]->key);
						break;
					}
				}
			}
		}
	}

	for (unsigned i = 0; i < m_vConstantsDepTrees.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
		LevelOrderTraversal(m_vConstantsDepTrees[i], l_mtype2Declarations);
		//m_varBackwordDep
		for (auto itMap = l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
		{
			if (itMap->first == type)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sVarorFun)
					{
						p_mTypeToDec[DEC_TYPE::CONSTANT_DEC].insert(m_vConstantsDepTrees[i]->key);
						break;
					}
				}
			}
		}
	}
 //added
	for (unsigned int i = 0; i < m_vFunctionsDepTrees.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
		LevelOrderTraversal(m_vFunctionsDepTrees[i], l_mtype2Declarations);

		for (auto itMap = l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
		{
			if (itMap->first ==  type)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sVarorFun && m_vFunctionsDepTrees[i]->key!= p_sVarorFun)
					{
					 	p_mTypeToDec[DEC_TYPE::FUN].insert(m_vFunctionsDepTrees[i]->key);
						break;
					}
				}
			}
		}

	}

}

void SP_ImportCANDL::ComputeBackwardDependencies()
{
//	m_varBackwordDep
	for (unsigned i = 0; i < m_Options_vars.size(); i++)
	{
		map<DEC_TYPE, set<wxString>> l_mapvarDep;
		ComputeBackwordDependency(m_Options_vars[i], DEC_TYPE::VAR_DEC, l_mapvarDep);
		m_varBackwordDep[m_Options_vars[i]] = l_mapvarDep;
	}

	for (unsigned i = 0; i < m_Options_funs.size(); i++)
	{
		map<DEC_TYPE, set<wxString>> l_mapFunDep;
		ComputeBackwordDependency(m_Options_funs[i], DEC_TYPE::FUN, l_mapFunDep);
		m_funBackwordDep[m_Options_funs[i]] = l_mapFunDep;
	}

}

void SP_ImportCANDL::ComputeColorSetBackwardDependencies(const wxString& p_sCsName, std::map<DEC_TYPE,std::set<wxString>>& p_node2dep)
{
	for (unsigned i = 0; i < m_vColorsetDepTrees.size(); i++)
	{
		if (m_vColorsetDepTrees[i]->key == p_sCsName)
		{
			for (unsigned j = 0; j < m_vColorsetDepTrees.size(); j++)
			{

					if (m_vColorsetDepTrees[j]->key != p_sCsName)
					{
						map<DEC_TYPE, set<wxString>> l_mNode2Declarations;
						LevelOrderTraversal(m_vColorsetDepTrees[j], l_mNode2Declarations);
						for (auto itMap = l_mNode2Declarations.begin(); itMap != l_mNode2Declarations.end(); ++itMap)
						{
							for (auto itMap = l_mNode2Declarations.begin(); itMap != l_mNode2Declarations.end(); ++itMap)
							{
								if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
								{
									for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
									{
										if (*itSet == p_sCsName)
										{
											p_node2dep[DEC_TYPE::COLOR_SET_DEC].insert(m_vColorsetDepTrees[j]->key);
											break;
										}
								    }
								}
							}
						}
					}

			}
		}
	}
///////////////////////////
	for (unsigned i = 0; i < m_vConstantsDepTrees.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
		LevelOrderTraversal(m_vConstantsDepTrees[i], l_mtype2Declarations);

		for (auto itMap = l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
		{
			if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sCsName)
					{
						p_node2dep[DEC_TYPE::CONSTANT_DEC].insert(m_vConstantsDepTrees[i]->key);

						break;
					}
				}
			}
		}
	}
	///////////////////////
	//added
	for (unsigned i = 0; i < m_vVarsDepTrees.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
		LevelOrderTraversal(m_vVarsDepTrees[i], l_mtype2Declarations);

		for (auto itMap = l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
		{
			if (itMap->first == DEC_TYPE::COLOR_SET_DEC)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sCsName)
					{
						p_node2dep[DEC_TYPE::VAR_DEC].insert(m_vVarsDepTrees[i]->key);

						break;
					}
				}
			}
		}
	}

}

void SP_ImportCANDL::ComputeColorSetsBackwardDependencies()
{
	for (unsigned i = 0; i < m_Options_cs.size(); i++)
	{
		std::map<DEC_TYPE, set<wxString>> l_mNode2Dec;
		ComputeColorSetBackwardDependencies(m_Options_cs[i], l_mNode2Dec);
		map<DEC_TYPE, set<wxString>> l_mType2dec;
		for (auto itMap = l_mNode2Dec.begin(); itMap != l_mNode2Dec.end(); ++itMap)
		{
			if (itMap->first == DEC_TYPE::COLOR_SET_DEC || itMap->first == DEC_TYPE::CONSTANT_DEC || itMap->first == DEC_TYPE::VAR_DEC)
			{
				l_mType2dec[itMap->first].insert(itMap->second.begin(), itMap->second.end());

			}

		}
		m_mColorset2Dependncies[m_Options_cs[i]] = l_mType2dec;
	}

}

void SP_ImportCANDL::ComputeConstantBackwardDependencies()
{
	for (unsigned i = 0; i < m_Options_constants.size(); i++)
	{
		std::set<wxString> l_setConstants;
		ComputeBackwardDependencyForConstant(m_Options_constants[i], l_setConstants);
		m_mMap2ConstDep[m_Options_constants[i]] = l_setConstants;
	}
}

void SP_ImportCANDL::ComputeBackwardDependencyForConstant(const wxString& p_sConstant, set<wxString>& p_setRes)
{
	for (unsigned i = 0; i < m_vConstantsDepTrees.size(); i++)
	{
		if (m_vConstantsDepTrees[i]->key == p_sConstant)
		{
			for (unsigned j = 0; j < m_vConstantsDepTrees.size(); j++)
			{
				if (m_vConstantsDepTrees[j]->key != p_sConstant)
				{
					std::map<DEC_TYPE, set<wxString>> l_mtype2Declarations;
					LevelOrderTraversal(m_vConstantsDepTrees[j], l_mtype2Declarations);

					for (auto itMap = l_mtype2Declarations.begin(); itMap != l_mtype2Declarations.end(); ++itMap)
					{
						if (itMap->first == DEC_TYPE::CONSTANT_DEC)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								if (*itSet == p_sConstant)
								{
									p_setRes.insert(m_vConstantsDepTrees[j]->key);
								}
							}
						}
					}

				}

			}
		}
	}
}


void SP_ImportCANDL::BuildVariableDependencyTree(const wxString& p_Var, dec_node* tree)
{
	for (int i = 0; i < m_Options_vars.size(); i++)
	{
		if (m_Options_vars[i] == p_Var)
		{

				for (auto& cs : *p_pcDoc->colorsets_)
				{
					wxString colsetDef(cs->definition_.c_str(), wxConvUTF8);

					if (IsToken(colsetDef, m_Options_vars[i]))
					{
						wxString colset(cs->name_.c_str(), wxConvUTF8);
						tree->child.push_back(newNode(colset, DEC_TYPE::COLOR_SET_DEC));
						BuildColorSetDependencyTree(colset, tree->child[tree->child.size() - 1]);
					}
				}
		}
	}
}


void SP_ImportCANDL::BuildVariableDependencies()
{
	for (int i = 0; i < m_Options_vars.size(); i++)
	{
		dec_node* l_pcConstTreeRoot = newNode(m_Options_vars[i], DEC_TYPE::VAR_DEC);
		BuildVariableDependencyTree(m_Options_vars[i], l_pcConstTreeRoot);
		m_vVarsDepTrees.push_back(l_pcConstTreeRoot);
	}
}


void SP_ImportCANDL::BuildFunctionsDependencyTrees()
{
	for (int i =0; i< m_Options_funs.size(); i++)
	{
		dec_node* l_pcfunTreeRoot = newNode(m_Options_funs[i], DEC_TYPE::FUN);
		//BuildVariableDependencyTree(itFunMap->first, l_pcfunTreeRoot);
		BuildFunctionDependencyTree(m_Options_funs[i], l_pcfunTreeRoot);
		m_vFunctionsDepTrees.push_back(l_pcfunTreeRoot);
	}
}

void SP_ImportCANDL::BuildFunctionDependencyTree(const wxString& p_sFunName, dec_node* tree)
{
	for (int i = 0; i< m_Options_funs.size(); i++)
	{
		if (m_Options_funs[i] != p_sFunName)
		{
			for (auto& fun : *p_pcDoc->color_functions_)
			{
				wxString funName(fun->name_.c_str(), wxConvUTF8);

				if (funName != p_sFunName)
				{
					wxString funbody(fun->body_.c_str(), wxConvUTF8);

					//if (IsToken(funbody, m_Options_funs[i]))
					if (IsToken(funbody, p_sFunName))
					{
						(tree->child).push_back(newNode(funName, DEC_TYPE::FUN));
						BuildFunctionDependencyTree(funName, tree->child[tree->child.size() - 1]);
					}
				}
			}

		}
	}
}
