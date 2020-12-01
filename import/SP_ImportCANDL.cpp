//////////////////////////////////////////////////////////////////////
// $Author:george assaf$
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
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"
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
#include "sp_gui/dialogs/SP_DLG_OverwriteDeclarations.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
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
	SP_ID_CHECK_ALL,
	SP_ID_OBS_TESXT,
	SP_ID_DECLARATIONS_OBSERVERS


};
SP_ImportCANDL::SP_ImportCANDL()
{
	m_bIsNewDoc = true;
}

SP_ImportCANDL::~SP_ImportCANDL()
{
	for(auto p: m_vColFunDependenciesVector)
	{
		delete p;
	}

	for (auto p : m_vVariablesDependecncyTrees)
	{
		delete p;
	}

	for (auto p : m_vColorsetDependecncyTrees)
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
			SP_DS_Graph_Declarations l_CheckDec(l_Colnet, true);
			l_CheckDec();
			m_CheckDec = l_CheckDec;
			m_vConstDependenciesVector = l_CheckDec.GetConstantsDependencyVector();
			m_vColFunDependenciesVector = l_CheckDec.GetFunctionDependencyVector();
			m_vObserversDependenciesVector = l_CheckDec.GetObserverDependencyVector();
			m_vColorsetDependecncyTrees= l_CheckDec.GetColorSetDependenncyVector();
			m_vVariablesDependecncyTrees = l_CheckDec.GetVariableDependencyVector();
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

				   for (int i = 0; i < m_Options_observers.size(); i++)
				   {
					   if (m_pcRearrangelist_observers->IsChecked(i))
					   {
						   l_finalObservers.Add(m_Options_observers[i]);
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

			l_Return = CreateGraph(p_sFile, *l_Net);
			if (l_Return)
                doLayout();
			//compute initial marking
			ComputeInitMarking();
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
	lookupObservers.clear();
	lookupPlaces.clear();
	lookupTransitions.clear();
	arcs.clear();


	m_vColFunDependenciesVector.clear();
	m_vColorsetDependecncyTrees.clear();
	m_vConstDependenciesVector.clear();
	m_vObserversDependenciesVector.clear();
	m_vVariablesDependecncyTrees.clear();
	l_finalconstants.clear();
	l_finalcolorsets.clear();
	l_finalvariables.clear();
	l_finalFunSel.clear();
	l_finalObservers.clear();
	m_mGroup2Const.clear();
	m_mType2Declaration.clear();
	if (p_pcDlg)
	{
		m_pcTextconstants1->SetValue("");
		wxDELETE(p_pcDlg);
	}
	m_bIsNewDoc = true;
	//m_mDeclaration2Overwritten.clear();
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

	if (p_Net.observers_)
		AppendObservers(*p_Net.observers_);
	if (m_mType2Declaration.size() > 0)
	{

		SP_DLG_OverwriteDeclarations* l_pcOverwriteDlg = new SP_DLG_OverwriteDeclarations(NULL, wxT("Overwrite Declarations"), m_mType2Declaration);

		if (l_pcOverwriteDlg->ShowModal() == wxID_OK)
		{
			m_mType2Declaration.clear();
			m_mType2Declaration = l_pcOverwriteDlg->GetSelectedItems();
		}
	}

	if (m_mType2Declaration.size()>0)
	{
		Overwritedeclarations(m_mType2Declaration, p_Net);
	}
	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

	return true;

}

bool SP_ImportCANDL::Overwritedeclarations(const std::map<wxString, std::set<wxString>>& p_mType2Dec, const dssd::andl::Net& p_Net)
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
		if (itMap->first == wxT("colorset"))
		{
			for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
			{
				OverwriteExistColorSet(*itSet, p_Net);

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

		if (itMap->first == wxT("variable"))
		{
			for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
			{
				OverwriteExistVariable(*itSet, p_Net);

			}
		}
	}
	return true;
}

bool SP_ImportCANDL::OverwriteExistVariable(const wxString& p_sVar, const dssd::andl::Net& p_Net)
{
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if (!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if (!l_pcNewMetadata)
		return false;
	SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if (!l_pcColList)
		return false;
	for (int i = 0; i < l_pcColList->GetRowCount(); ++i)
	{
		if (p_sVar == l_pcColList->GetCell(i, 0).c_str())
		{
			for (auto& var : *p_Net.variables_)
			{
				if (p_sVar == var->name_.c_str())
				{
					l_pcColList->SetCell(i, 1, var->colorset_.c_str());
					return true;
				}
			}
		}
	}

	return false;
}

bool SP_ImportCANDL::OverwriteExistObserver(const wxString& p_sObserver, const dssd::andl::Net& p_Net)
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

bool SP_ImportCANDL::OverwriteExistFunction(const wxString& p_sFunction, const dssd::andl::Net& p_Net)
{
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


	for (int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		if (l_pcColList->GetCell(i, 1) == p_sFunction)
		{
			for (auto& f : *p_Net.color_functions_)
			{
				dssd::andl::ColorFunction::Params params = f->params_;
				wxString param;
				for (auto& p : params)
				{
					if (!param.IsEmpty())
						param << wxT(",");
					param << p.first << wxT(" ") << p.second;
				}
				wxString body = f->body_.c_str();
				AdaptColorExpression(body);
				l_pcColList->SetCell(i, 0, f->ret_.c_str());
				l_pcColList->SetCell(i, 2, param);
				l_pcColList->SetCell(i, 3, body);
				return true;
			}

		}
	}
	return false;
}

bool SP_ImportCANDL::OverwriteExistColorSet(const wxString& p_sColorset, const dssd::andl::Net& p_Net)
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

	for (int i = 0; i < l_pcColListSimple->GetRowCount(); i++)
	{
		if (p_sColorset == l_pcColListSimple->GetCell(i, 0))
		{
			for (auto& cs : *p_Net.colorsets_)
			{
				if (cs->name_.c_str() != p_sColorset) continue;

				wxString colors = cs->definition_.c_str();
				if (cs->type_ == dssd::andl::ColorsetType::INT_T)
				{
					l_pcColListSimple->SetCell(i, 1, wxT("int"));
				}
				else if (cs->type_ == dssd::andl::ColorsetType::ENUM_T)
				{
					l_pcColListSimple->SetCell(i, 1, wxT("enum"));
				}

				colors.Replace(wxT(".."), wxT("-"));
				colors.Replace(wxT("{"), wxT(""));
				colors.Replace(wxT("}"), wxT(""));

				l_pcColListSimple->SetCell(i, 2, colors);
				return true;
			}
		}
	}

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return false;
	SP_DS_Metadata* l_pcNewCompound = *(l_pcMetadataclass->GetElements()->begin());
	if (!l_pcNewCompound)
		return false;
	SP_DS_ColListAttribute* l_pcColListCompound = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewCompound->GetAttribute(wxT("StructuredColorsetList")));
	if (!l_pcColListCompound)
		return false;

	for (int i = 0; i < l_pcColListCompound->GetRowCount(); i++)
	{
		if (p_sColorset == l_pcColListCompound->GetCell(i, 0))
		{
			for (auto& cs : *p_Net.colorsets_)
			{
				if (cs->name_.c_str() != p_sColorset) continue;

				wxString colors = cs->definition_.c_str();
				wxString type;
				bool l_bSimple = true;
				if (colors.StartsWith(wxT("PROD")))
				{
					l_bSimple = false;
					type = wxT("product");
					colors = colors.AfterFirst('(').BeforeLast(')');
				}

				if (l_bSimple)
				{
					if (colors.Contains(wxT("[")) && colors.Contains(wxT("]")))
					{
						wxString l_sExpression = colors;
						colors = colors.AfterFirst(wxChar('['));
						colors = colors.BeforeLast(wxChar(']'));
						type = l_sExpression.BeforeFirst(wxChar('['));
						l_bSimple = false;

					}
				}
				l_pcColListSimple->SetCell(i, 1, type);
				AdaptColorExpression(colors);
				l_pcColListSimple->SetCell(i, 2, colors);
				return true;
			}
		}
	}


	return false;
}


bool SP_ImportCANDL::OverwriteExistConstant(const wxString& p_sConstant, const dssd::andl::Net& p_Net)
{

	SP_DS_Metadataclass*	l_pcConstantsOrgin = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);

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

void SP_ImportCANDL::ComputeInitMarking()
{
	SP_DS_Nodeclass *nodeClass = NULL;
	wxString l_sNodeType;
	if (m_eNetType == dssd::andl::NetType::COL_CPN_T)
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
		l_sNodeType << SP_DS_CONTINUOUS_PLACE;
	}
	else
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
		l_sNodeType << SP_DS_DISCRETE_PLACE;
	}


	SP_CPN_SyntaxChecking l_cSyntaxChecking;
	if (!l_cSyntaxChecking.Initialize())
		return ;

	SP_ListNode::const_iterator  l_itElem;
	for (l_itElem = nodeClass->GetElements()->begin(); l_itElem != nodeClass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcPlaceNode = (*l_itElem);

		wxString l_sMainMarking = wxT("");
		map<wxString, vector<SP_CPN_TokenNum> > l_mColorToMarkingMap;
		if (!l_cSyntaxChecking.ComputeInitialMarking(l_pcPlaceNode, l_mColorToMarkingMap, false))
			 continue;
		map<wxString, vector<SP_CPN_TokenNum> >::iterator itMap;

		if (l_sNodeType == SP_DS_CONTINUOUS_PLACE)
		{
			double l_dMarking = 0;
			for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_dMarking = l_dMarking + itMap->second[0].m_DoubleMultiplicity;
			}
			l_sMainMarking << l_dMarking;
		}
		else
		{
			long l_nMarking = 0;
			for (itMap = l_mColorToMarkingMap.begin(); itMap != l_mColorToMarkingMap.end(); itMap++)
			{
				l_nMarking = l_nMarking + itMap->second[0].m_intMultiplicity;
			}
			l_sMainMarking << l_nMarking;
		}

		///////////////////////////////////////////////////////////


		SP_DS_Attribute* l_pcMarkingAttr = l_pcPlaceNode->GetAttribute(wxT("Marking"));
		if (l_pcMarkingAttr)
		{
			l_pcMarkingAttr->SetValueString(l_sMainMarking);
		}

	}
	/////////////////////////////////////////////////////////
	SP_Core::Instance()->GetRootDocument()->GetGraph()->SqueezeIdAttributes();
	SP_Core::Instance()->GetRootDocument()->GetGraph()->CheckIntegrity();
	SP_Core::Instance()->GetRootDocument()->Modify(true);
	SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->Modify(true);
	SP_Core::Instance()->GetRootDocument()->GetGraph()->GetParentDoc()->Refresh();
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
				m_mType2Declaration[wxT("constant")].insert(constant->name_);
				if (l_finalconstants.Index(constant->name_) != wxNOT_FOUND)
				{

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
	 
		//TODO
		/*
		if(constant->type_ == dssd::andl::ConstType::STRING_T)
		{
		type = wxT("string");
		}
		*/
		if (constant->type_ == dssd::andl::ConstType::DOUBLE_T || constant->type_ == dssd::andl::ConstType::INT_T
				|| constant->type_ == dssd::andl::ConstType::BOOL_T
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
			else if (constant->type_ == dssd::andl::ConstType::BOOL_T)
			{
				bool l_bValue = l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(wxT("bool"));
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

			for (unsigned int i = 0; i < p_Valuesets.size(); i++)
			{
				value = constant->values_[i];
				int l_nRowCol = l_pcColList1->AppendEmptyRow();

				if(p_Valuesets.size()>0 && i<p_Valuesets.size())
				l_pcColList1->SetCell(l_nRowCol, 0, p_Valuesets[i]);

				l_pcColList1->SetCell(l_nRowCol, 1, value);
			}

			if (p_Valuesets.size() == 0 && constant->values_.size()>0)
			 {
				int l_nRowCol = l_pcColList1->AppendEmptyRow();
				value = constant->values_[0];
				l_pcColList1->SetCell(l_nRowCol, 0,wxT("Main"));
				l_pcColList1->SetCell(l_nRowCol, 1, value);
			 }

			m_pcGraph->GetFunctionRegistry()->registerFunction(name, l_sMain);
			l_pcColList1->UpdateActiveListColumnPtr();
			l_pcConstant->Update();
			lookupConstants[constant->name_] = *l_itElem;
			 ++l_itElem;
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
				m_mType2Declaration[wxT("colorset")].insert(name);
				continue;
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

			if (l_bIsUsed)
			{//ask user about his decision
				m_mType2Declaration[wxT("colorset")].insert(name);
				continue;
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

		if (l_bIsExist)
		{//ask user about his decision
			m_mType2Declaration[wxT("variable")].insert(name);
			continue;
		}
		unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();
		l_pcColList->SetCell(l_nNewRow, 0, name);
		l_pcColList->SetCell(l_nNewRow, 1, type);
		lookupVariables[v->name_] = l_pcNewMetadata;

	}
	return true;
}

bool SP_ImportCANDL::AppendObservers(const dssd::andl::Observers& p_observers)
{
	for (auto& o : p_observers)
	{
		if (!o) continue;

		if (lookupObservers.find(o->name_) != lookupObservers.end())
		{
			continue;
		}

		if (l_finalObservers.Index(o->name_, true) == wxNOT_FOUND) { continue; }

		bool l_bIsExist = false;
		bool l_bOverwrite = false;

		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
		if (mc)
		{
			for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
			{
				wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
				wxString l_sMetadataComment = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Comment")))->GetValue();
				//wxString l_sMetadataBody = dynamic_cast<SP_DS_EquationAttribute*>(l_pcMetadata->GetAttribute(wxT("Body")))->GetValue();
				if (l_sMetadataName == o->name_)
				{
					l_bIsExist = true;
					break;
				}
			}
		}
			if (l_bIsExist)
			{
			//	m_mDeclaration2Overwritten[wxT("observer")].insert(o->name_);
				m_mType2Declaration[wxT("observer")].insert(o->name_);
				continue;
			}

			SP_DS_Metadata* l_Obs = mc->NewElement(1);
			l_Obs->GetAttribute(wxT("Name"))->SetValueString(o->name_);
			wxString type;

				if (o->type_ == dssd::andl::ObserverType::PLACE_T)
				{
					type = wxT("Plce");
				}
				else
				{
					type = wxT("Transition");
				}

			l_Obs->GetAttribute(wxT("Type"))->SetValueString(type);
			l_Obs->GetAttribute(wxT("Body"))->SetValueString(o->function_.c_str());
			lookupObservers[o->name_] = l_Obs;

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

			///m_mDeclaration2Overwritten[wxT("function")].insert(name);
			m_mType2Declaration[wxT("function")].insert(name);
			continue;

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
		else if (colors.Contains(wxT("+")))
		{
		    l_bSimple = false;
			type = wxT("union");
			colors.Replace(wxT("+"), wxT(","));
		}
		else
		{
			colors.Replace(wxT(".."), wxT("-"));
			colors.Replace(wxT("{"), wxT(""));
			colors.Replace(wxT("}"), wxT(""));
			colors.Replace(wxT("\""), wxT(""));//string colour sets case
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

			if (colors.Contains(wxT("true")) || colors.Contains(wxT("false")))
		    {
							type = wxT("bool");
		     }

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
			if (l_finalObservers.Index(name, true) == wxNOT_FOUND) { continue; }//by george

			wxString body = obs->function_;
			SP_DS_Metadata* l_Obs = mc->NewElement(1);
			l_Obs->GetAttribute(wxT("Name"))->SetValueString(name);
			l_Obs->GetAttribute(wxT("Type"))->SetValueString(type);
			l_Obs->GetAttribute(wxT("Body"))->SetValueString(body);
			lookupObservers[obs->name_] = l_Obs;
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
			bool l_bIscolouredDependentRate=false;
			for(size_t i = function.find(wxT("++")); i != wxNOT_FOUND; i = function.find(wxT("++"), i+2))
			{
				l_Functions.Add(function.Mid(prev,i-prev));
				prev = i;
				l_bIscolouredDependentRate=true;
			}
			if (l_bIscolouredDependentRate)
			{
				wxString l_sLastRate = function.Mid(prev + 2, function.length()-1);
				l_Functions.Add(l_sLastRate);
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
	m_Options_observers.clear();
	m_Options_observers_order.clear();
	m_Options_vars.clear();
	m_Options_vars_order.Clear();
	l_finalFunSel.clear();
	l_finalcolorsets.clear();
	l_finalvariables.clear();
	l_finalconstants.clear();
	l_finalObservers.clear();


	AddConstants();
	AddColorSets();
	AddFunctions();
	AddVariables();
	AddObservers();

	return true;
}

void SP_ImportCANDL::ONselectAll(wxCommandEvent& p_cEvent)
{//select/unselect all declarations of the foucesd tab only taking dependencies into consideration
	if (m_pcCheckAll->IsChecked())
	{
		if (m_pcNotebookPageConstants)
		{
			if (m_pcNotebookPageConstants->IsFocusable())
			{
				set<wxString> l_setDec;

				for (auto& constant : *p_pcDoc->constants_)
				{
					wxString l_selectedConstant = constant->name_.c_str();
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

		if (m_pcNotebookPagecolorsets)
		{
			if (m_pcNotebookPagecolorsets->IsFocusable())
			{
				for (auto& cs : *p_pcDoc->colorsets_)
				{
					wxString l_selectedCs = cs->name_.c_str();

					set<wxString> l_setDec;

					m_CheckDec.ComputeBackwardDependency(l_selectedCs, NODE_TYPE::COLOR_SET, l_setDec);

					for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
					{
						if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_constants.Index(*itSet);
							m_pcRearrangelist_constants->Check(index, true);
						}

						if (m_Options_cs.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}

						if (m_Options_vars.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_vars.Index(*itSet);
							m_pcRearrangelist_variables->Check(index, true);
						}
					}
				}
			}
		}


		if (m_pcNotebookPagevariables)
		{
			if (m_pcNotebookPagevariables->IsFocusable())
			{
				for (auto& var : *p_pcDoc->variables_)
				{
					wxString l_selectedVar = var->name_.c_str();


					if (m_Options_vars.Index(l_selectedVar) != wxNOT_FOUND)
					{
						int index = m_Options_vars.Index(l_selectedVar);
						m_pcRearrangelist_variables->Check(index, true);
					}

					set<wxString> l_setDec;
					m_CheckDec.ComputeBackwardDependency(l_selectedVar, NODE_TYPE::VAR, l_setDec);

					for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
					{
						if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_constants.Index(*itSet);
							m_pcRearrangelist_constants->Check(index, true);

						}

						if (m_Options_cs.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}


					}
				}
			}
		}

		if (m_pcNotebookPageFunctions)
		{
			if (m_pcNotebookPageFunctions->IsFocusable())
			{
				for (auto& fun : *p_pcDoc->color_functions_)
				{
					wxString l_selectedfun = fun->name_.c_str();
					set<wxString> l_setDec;
					m_CheckDec.ComputeBackwardDependency(l_selectedfun, NODE_TYPE::FUN_COLOOR, l_setDec);

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
						else if(m_Options_cs.Index(*itSet) != wxNOT_FOUND)
						{
							int index = m_Options_cs.Index(*itSet);
							m_pcRearrangelist_colorsets->Check(index, true);
						}

					}
				}
			}
		}

		PrintChosenConstants();
		PrintChosenColorSets();
		PrintColorFunctions();
		PrintVariables();

	}
	else
	{
		if (m_pcNotebookPageConstants)
		{
			if (m_pcNotebookPageConstants->IsFocusable())
			{
				for (int j = 0; j < m_vConstDependenciesVector.size(); j++)
				{
					for (auto& constant : *p_pcDoc->constants_)
					{
						wxString l_selectedConstant = constant->name_.c_str();
						int index = m_Options_constants.Index(l_selectedConstant);
						if (index != wxNOT_FOUND)
						{
							m_pcRearrangelist_constants->Check(index, false);
						}

						if (m_vConstDependenciesVector[j]->key == l_selectedConstant)
						{
							std::map<NODE_TYPE, set<wxString>> l_DepMap;

							LevelOrderTraversal(m_vConstDependenciesVector[j], l_DepMap);

							if (l_DepMap.size() > 0)
							{
								for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
								{
									if (itMap->first == NODE_TYPE::COLOR_SET)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_cs.Index(*itSet);
											m_pcRearrangelist_colorsets->Check(index, false);

										}

									}
									else if (itMap->first == NODE_TYPE::VAR)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_vars.Index(*itSet);
											m_pcRearrangelist_variables->Check(index, false);
										}
									}
									else if (itMap->first == NODE_TYPE::FUN_COLOOR)
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
									else if (itMap->first == NODE_TYPE::OBSERVER)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_observers.Index(*itSet);
											m_pcRearrangelist_observers->Check(index, false);

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
			}
		}

		if (m_pcNotebookPagecolorsets)
		{
			if (m_pcNotebookPagecolorsets->IsFocusable())
			{
				for (auto& cs : *p_pcDoc->colorsets_)
				{
					wxString l_selectedCs = cs->name_.c_str();

					int index = m_Options_cs.Index(l_selectedCs);
					if (index != wxNOT_FOUND)
					{
						m_pcRearrangelist_colorsets->Check(index, false);
					}

					for (int j = 0; j < m_vColorsetDependecncyTrees.size(); j++)
					{
						if (m_vColorsetDependecncyTrees[j]->key == l_selectedCs)
						{
							std::map<NODE_TYPE, set<wxString>> l_DepMap;

							LevelOrderTraversal(m_vColorsetDependecncyTrees[j], l_DepMap);

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
									else if (itMap->first == NODE_TYPE::VAR)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_vars.Index(*itSet);
											m_pcRearrangelist_variables->Check(index, false);
										}

									}
									else if (itMap->first == NODE_TYPE::COLOR_SET)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_cs.Index(*itSet);
											m_pcRearrangelist_colorsets->Check(index, false);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (m_pcNotebookPagevariables)
		{
			if (m_pcNotebookPagevariables->IsFocusable())
			{
				for (auto& var : *p_pcDoc->variables_)
				{
					wxString l_selectedVar = var->name_.c_str();


					int index = m_Options_vars.Index(l_selectedVar);
					if (index != wxNOT_FOUND)
					{
						m_pcRearrangelist_variables->Check(index, false);
					}

					for (int j = 0; j < m_vVariablesDependecncyTrees.size(); j++)
					{
						if (m_vVariablesDependecncyTrees[j]->key == l_selectedVar)
						{
							std::map<NODE_TYPE, set<wxString>> l_DepMap;

							LevelOrderTraversal(m_vVariablesDependecncyTrees[j], l_DepMap);

							if (l_DepMap.size() > 0)
							{
								for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
								{
									if (itMap->first == NODE_TYPE::COLOR_SET)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_cs.Index(*itSet);
											m_pcRearrangelist_colorsets->Check(index, false);
										}
									}

									if (itMap->first == NODE_TYPE::CONSTANT)
									{
										for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
										{
											int index = m_Options_constants.Index(*itSet);
											m_pcRearrangelist_constants->Check(index, false);
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
				for (auto& fun : *p_pcDoc->color_functions_)
				{
					wxString l_selectedfun = fun->name_.c_str();

					int index = m_Options_funs.Index(l_selectedfun);
					if (index != wxNOT_FOUND)
					{
						m_pcRearrangelist_function->Check(index, false);
					}

					for (int j = 0; j < m_vColFunDependenciesVector.size(); j++)
					{
						if (m_vColFunDependenciesVector[j]->key == l_selectedfun)
						{
							std::map<NODE_TYPE, set<wxString>> l_DepMap;

							LevelOrderTraversal(m_vColFunDependenciesVector[j], l_DepMap);

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
								}
							}
						}
					}
				}
			}
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
		m_pcRearrangelist_observers->Enable(true);
    }
	else
	{
		m_pcCheckAll->Enable(false);
		m_pcRearrangelist_function->Enable(false);
		m_pcRearrangelist_constants->Enable(false);
		m_pcRearrangelist_variables->Enable(false);
		m_pcRearrangelist_colorsets->Enable(false);
		m_pcRearrangelist_observers->Enable(false);
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


void SP_ImportCANDL::AddObservers()
{
	m_pcNotebookPageobservers = p_pcDlg->AddPage(wxT("Observers"));

	m_pcMainSizer_observers = new wxBoxSizer(wxHORIZONTAL);
	m_pcLeftSizer_observers = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageobservers, -1, wxT("Observer names")), wxVERTICAL);
	m_pcRightSizer_observers = new wxBoxSizer(wxVERTICAL);
	m_pcRightSizer_observers = new wxStaticBoxSizer(new wxStaticBox(m_pcNotebookPageobservers, -1, wxT("Observer defintions")), wxVERTICAL);

	m_pcTextobservers = new wxTextCtrl(m_pcNotebookPageobservers, SP_ID_OBS_TESXT, "", wxDefaultPosition, wxSize(400, 300), wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

	m_pcMainSizer_observers->Add(m_pcLeftSizer_observers, 1, wxALL | wxEXPAND, 5);
	m_pcMainSizer_observers->Add(m_pcRightSizer_observers, 1, wxALL | wxEXPAND, 5);

	AddObservers_Att();

	m_pcRearrangelist_observers = new wxRearrangeList(m_pcNotebookPageobservers, SP_ID_DECLARATIONS_OBSERVERS, wxDefaultPosition,
		wxSize(50, 200), m_Options_observers_order, m_Options_observers);
	m_pcRearrangelist_observers->Enable(false);
	m_pcLeftSizer_observers->Add(m_pcRearrangelist_observers, 1, wxALL | wxEXPAND, 5);

	m_pcRightSizer_observers->Add(m_pcTextobservers);

	m_pcRearrangelist_observers->Bind(wxEVT_CHECKLISTBOX, &SP_ImportCANDL::OnSelChange_observers, this, SP_ID_DECLARATIONS_OBSERVERS);

	m_pcNotebookPageobservers->AddControl(m_pcMainSizer_observers, 1, wxALL | wxEXPAND, 5);
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

	wxString l_selectedCs = m_Options_cs[p_cEvent.GetInt()];

	if (!b)
	{
		for (int j = 0; j < m_vColorsetDependecncyTrees.size(); j++)
		{
			if (m_vColorsetDependecncyTrees[j]->key == l_selectedCs)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vColorsetDependecncyTrees[j], l_DepMap);

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
						else if(itMap->first == NODE_TYPE::VAR)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_vars.Index(*itSet);
								m_pcRearrangelist_variables->Check(index, false);
							}

						}
						else if (itMap->first == NODE_TYPE::COLOR_SET)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_cs.Index(*itSet);
								m_pcRearrangelist_colorsets->Check(index, false);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		set<wxString> l_setDec;

		m_CheckDec.ComputeBackwardDependency(l_selectedCs, NODE_TYPE::COLOR_SET, l_setDec);

		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);
			}

			if (m_Options_cs.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_cs.Index(*itSet);
				m_pcRearrangelist_colorsets->Check(index, true);
			}

			if (m_Options_vars.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_vars.Index(*itSet);
				m_pcRearrangelist_variables->Check(index, true);
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
						if (itMap->first == NODE_TYPE::COLOR_SET)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_cs.Index(*itSet);
								m_pcRearrangelist_colorsets->Check(index, false);

							}

						}
						else if (itMap->first == NODE_TYPE::VAR)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_vars.Index(*itSet);
								m_pcRearrangelist_variables->Check(index, false);
							}
						}
						else if (itMap->first == NODE_TYPE::FUN_COLOOR)
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
						else if (itMap->first == NODE_TYPE::OBSERVER)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_observers.Index(*itSet);
								m_pcRearrangelist_observers->Check(index, false);

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
	///do printing on the right hand panel
	PrintChosenConstants();
	PrintChosenColorSets();
	PrintVariables();
	PrintColorFunctions();
	PrintObservers();
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

void SP_ImportCANDL::OnSelChange_observers(wxCommandEvent& p_cEvent)
{
	bool b = m_pcRearrangelist_observers->IsChecked(p_cEvent.GetInt());
	m_pcCheckAll->SetValue(false);
	if (!b)
	{
		wxString l_sObservers;
		wxString observerName = m_Options_observers[p_cEvent.GetInt()];
		for (auto& observer : *p_pcDoc->observers_)
		{
			int ind = m_Options_observers.Index(observer->name_);
			if (!m_pcRearrangelist_observers->IsChecked(ind)) { continue; }
			if (observer->name_ == observerName) { continue; }

			if (observer->type_ == dssd::andl::ObserverType::PLACE_T && !l_sObservers.Contains(wxT("Place:\n")))
			{
				l_sObservers << wxT("Place:\n");
			}
			else if (observer->type_ == dssd::andl::ObserverType::TRANS_T && !l_sObservers.Contains(wxT("Transition:\n")))
			{
				l_sObservers << wxT("Transition:\n");
			}
			l_sObservers << observer->name_ << wxT(" = ") << observer->function_ << wxT(";") << wxT("\n");
		}

		m_pcTextobservers->SetValue(l_sObservers);
	}

	wxString l_selectedObserver = m_Options_observers[p_cEvent.GetInt()];
	if (b)
	{
		set<wxString> l_setDec;
		m_CheckDec.ComputeBackwardDependency(l_selectedObserver, NODE_TYPE::OBSERVER, l_setDec);

		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);

			}

			if (m_Options_observers.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_observers.Index(*itSet);
				m_pcRearrangelist_observers->Check(index, true);
			}
		}
	}
	else
	{
		for (int j = 0; j < m_vObserversDependenciesVector.size(); j++)
		{
			if (m_vObserversDependenciesVector[j]->key == l_selectedObserver)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vObserversDependenciesVector[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{
						if (itMap->first == NODE_TYPE::CONSTANT)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, false);
							}
						}

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
	PrintChosenConstants();
	PrintObservers();
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
	wxString l_selectedVar = m_Options_vars[p_cEvent.GetInt()];
	if (b)
	{
		set<wxString> l_setDec;
		m_CheckDec.ComputeBackwardDependency(l_selectedVar, NODE_TYPE::VAR, l_setDec);

		for (auto itSet = l_setDec.begin(); itSet != l_setDec.end(); ++itSet)
		{
			if (m_Options_constants.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_constants.Index(*itSet);
				m_pcRearrangelist_constants->Check(index, true);

			}

			if (m_Options_cs.Index(*itSet) != wxNOT_FOUND)
			{
				int index = m_Options_cs.Index(*itSet);
				m_pcRearrangelist_colorsets->Check(index, true);
			}
		}
	}
	else
	{
		for (int j = 0; j < m_vVariablesDependecncyTrees.size(); j++)
		{
			if (m_vVariablesDependecncyTrees[j]->key == l_selectedVar)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vVariablesDependecncyTrees[j], l_DepMap);

				if (l_DepMap.size() > 0)
				{
					for (auto itMap = l_DepMap.begin(); itMap != l_DepMap.end(); ++itMap)
					{
						if (itMap->first == NODE_TYPE::COLOR_SET)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_cs.Index(*itSet);
								m_pcRearrangelist_colorsets->Check(index, false);
							}
						}

						if (itMap->first == NODE_TYPE::CONSTANT)
						{
							for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
							{
								int index = m_Options_constants.Index(*itSet);
								m_pcRearrangelist_constants->Check(index, false);
							}
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

	wxString l_selectedfun = m_Options_funs[p_cEvent.GetInt()];
	if (!b)
	{
		for (int j = 0; j < m_vColFunDependenciesVector.size(); j++)
		{
			if (m_vColFunDependenciesVector[j]->key == l_selectedfun)
			{
				std::map<NODE_TYPE, set<wxString>> l_DepMap;

				LevelOrderTraversal(m_vColFunDependenciesVector[j], l_DepMap);

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
					}
				}
			}
		}
	}
	else
	{
		set<wxString> l_setDec;
		m_CheckDec.ComputeBackwardDependency(l_selectedfun, NODE_TYPE::FUN_COLOOR, l_setDec);

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
			else if (m_Options_cs.Index(*itSet) != wxNOT_FOUND)//31.10.20
			{
				int index = m_Options_cs.Index(*itSet);
				m_pcRearrangelist_colorsets->Check(index, true);
			}
			else
			{//observers are not use colored functions

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

void SP_ImportCANDL::AddObservers_Att()
{
	wxString l_sObserver;
	int l_nOrder = 0;
	for (auto& obs : *p_pcDoc->observers_)
	{
		if (obs->type_ == dssd::andl::ObserverType::PLACE_T)
		{
			if(!l_sObserver.Contains("Place:\n"))
			l_sObserver << wxT("Place:\n");
			wxString l_sFun(obs->function_.c_str(), wxConvUTF8);

			l_sObserver << obs->name_ << wxT(" = ") << l_sFun << wxT("\n");

			m_Options_observers.Add(obs->name_);

			m_Options_observers_order.Add(l_nOrder);
			l_nOrder++;
		}
	}
	for (auto& obs : *p_pcDoc->observers_)
	{
		wxString l_sObsType;
		if (obs->type_ == dssd::andl::ObserverType::TRANS_T)
		{
			if (!l_sObserver.Contains("Transition:\n"))
			l_sObserver << wxT("Transition:\n");
			wxString l_sFun(obs->function_.c_str(), wxConvUTF8);

			l_sObserver << obs->name_ << wxT(" = ") << l_sFun << wxT("\n");

			m_Options_observers.Add(obs->name_);

			m_Options_observers_order.Add(l_nOrder);
			l_nOrder++;
		}
	}
	m_pcTextobservers->SetValue(l_sObserver);
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

void SP_ImportCANDL::PrintObservers()
{
	wxString l_sObserver;
	int l_nOrder = 0;
	for (auto& obs : *p_pcDoc->observers_)
	{

		if (obs->type_ == dssd::andl::ObserverType::PLACE_T)
		{
			if (!l_sObserver.Contains("Place:\n"))
				l_sObserver << wxT("Place:\n");
			wxString l_sFun(obs->function_.c_str(), wxConvUTF8);

			int ind = m_Options_observers.Index(obs->name_);
			if (!m_pcRearrangelist_observers->IsChecked(ind)) { continue; }

			l_sObserver << obs->name_ << wxT(" = ") << l_sFun << wxT("\n");

		}
	}
	for (auto& obs : *p_pcDoc->observers_)
	{
		wxString l_sObsType;
		if (obs->type_ == dssd::andl::ObserverType::TRANS_T)
		{
			if (!l_sObserver.Contains("Transition:\n"))
				l_sObserver << wxT("Transition:\n");
			wxString l_sFun(obs->function_.c_str(), wxConvUTF8);

			int ind = m_Options_observers.Index(obs->name_);
			if (!m_pcRearrangelist_observers->IsChecked(ind)) { continue; }

			l_sObserver << obs->name_ << wxT(" = ") << l_sFun << wxT("\n");


		}
	}
	m_pcTextobservers->SetValue(l_sObserver);
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

void  SP_ImportCANDL::LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_mapResultKey)
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
			l_mapResultKey[p->type].insert(p->key);
			// Enqueue all children of the dequeued item
			for (int i = 0; i<p->child.size(); i++)
				q.push(p->child[i]);
			n--;
		}
	}
}
