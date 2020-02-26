/*
 * SP_SimpleNetBuilder.cpp
 *
 *  Created on: Mar 18, 2015
 *      Author: crohr
 */

#include "sp_core/tools/SP_SimpleNetBuilder.h"

#include "sp_utilities.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"

#include "dssd/andl/andl_builder.h"
#include "dssd/auxi/auxi.h"

#include <wx/filename.h>
#include <wx/tokenzr.h>

//george
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/extensions/SP_DS_FunctionEvaluator.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_ColorProcessing.h"//george
bool SP_SimpleNetBuilder::operator ()(SP_DS_Graph* p_pcGraph)
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;
	bool l_bFinish = true;
	try
	{
		dssd::andl::simple_net_builder b;
		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_PN_CLASS)
		{
			b.setType(dssd::andl::NetType::PN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_EXTPN_CLASS)
		{
			b.setType(dssd::andl::NetType::XPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		{
			if(!m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
				|| !m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
			{
				b.setType(dssd::andl::NetType::XSPN_T);
			}
			else if(!m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
			{
				b.setType(dssd::andl::NetType::GSPN_T);
			}
			else
			{
				b.setType(dssd::andl::NetType::SPN_T);

			}
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_CONTINUOUSPED_CLASS)
		{
			b.setType(dssd::andl::NetType::CPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_HYBRIDPN_CLASS)
		{
			b.setType(dssd::andl::NetType::HPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_TIMEPN_CLASS)
		{
			b.setType(dssd::andl::NetType::TPN_T);
		}
		else
		{
			SP_LOGERROR(wxT("netclass: '") + m_pcGraph->GetNetclass()->GetName() + wxT("' not supported"));
			return false;
		}

		SP_DS_Metadata* l_pcMeta = m_pcGraph->GetMetadataclass(wxT("General"))->GetElements()->front();
		wxString netName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();
		if(netName.IsEmpty())
		{
			wxFileName fn = wxFileName(m_pcGraph->GetParentDoc()->GetUserReadableName());
			netName = fn.GetName();
		}

		b.setName(SP_NormalizeString(netName, false).ToStdString());

		l_bFinish &= CreateFunctions(b);
		l_bFinish &= CreateConstants(b);
		l_bFinish &= CreatePlaces(b);
		l_bFinish &= CreateObservers(b);
		l_bFinish &= CreateTransitions(b);

		m_Net = b.get();
	}
	catch(std::exception& e)
	{
		SP_LOGERROR(e.what());
		l_bFinish = false;
	}
	catch(...)
	{
		l_bFinish = false;
		SP_LOGERROR(wxT("unkown exception!"));
	}
	return l_bFinish;
}

bool SP_SimpleNetBuilder::CreatePlaces(dssd::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Place")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dssd::andl::PlType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("discrete"))
				{
					l_Type = dssd::andl::PlType::DISCRETE_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dssd::andl::PlType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				std::string l_sMarking;
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("MarkingList"));
				if(l_pcAttr)
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
					l_sMarking = l_pcColList->GetActiveCellValue(1);
					
				}
				else
				{
					l_sMarking = l_pcNode->GetAttribute(wxT("Marking"))->GetValueString();
				}
				auto p = std::make_shared<dssd::andl::Place>(l_Type, l_sName, l_sMarking);
				l_pcAttr = l_pcNode->GetAttribute(wxT("Fixed"));
				if(l_pcAttr)
				{
					p->fixed_ = dynamic_cast< SP_DS_BoolAttribute* >(l_pcAttr)->GetValue();
				}
				b.addPlace(p);
			}
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateTransitions(dssd::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Transition")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dssd::andl::TrType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("stochastic"))
				{
					l_Type = dssd::andl::TrType::STOCHASTIC_T;
				}
				else if(l_sType == wxT("immediate"))
				{
					l_Type = dssd::andl::TrType::IMMEDIATE_T;
				}
				else if(l_sType == wxT("deterministic"))
				{
					l_Type = dssd::andl::TrType::DETERMINISTIC_T;
				}
				else if(l_sType == wxT("scheduled"))
				{
					l_Type = dssd::andl::TrType::SCHEDULED_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dssd::andl::TrType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				auto t = make_shared<dssd::andl::Transition>(l_Type, l_sName);

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetSourceEdges()))
				{
					SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
					std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
					auto u = make_shared<dssd::andl::Update>(l_sTarget, '+', l_sMult);
					t->updates_->push_back(u);
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetTargetEdges()))
				{
					SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					std::string l_sSource = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if(l_pcEdge->GetClassName() == SP_DS_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto u = make_shared<dssd::andl::Update>(l_sSource, '-', l_sMult);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_RESET_EDGE)
					{
						bool found = false;
						for(auto u : *(t->updates_))
						{
							if(u->place_ == l_sSource && u->type_ == dssd::andl::UpdateType::INCREASE_T)
							{
								u->op_ = '=';
								u->type_ = dssd::andl::UpdateType::ASSIGN_T;
								found = true;
							}
						}
						if(!found)
						{
							auto u = make_shared<dssd::andl::Update>(l_sSource, '=', "0");
							t->updates_->push_back(u);
						}
					}
					else if(l_pcEdge->GetClassName() == SP_DS_READ_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sMult, dssd::andl::CondType::READ_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_INHIBITOR_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sMult, dssd::andl::CondType::INHIBITOR_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_EQUAL_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sMult, dssd::andl::CondType::EQUAL_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_MODIFIER_EDGE)
					{
						auto c = make_shared<dssd::andl::Condition>(l_sSource, "", dssd::andl::CondType::MODIFIER_T);
						t->conditions_->push_back(c);
					}

				}

				// function
				wxString l_sFunction;
				if(l_pcNode->GetAttribute(wxT("FunctionList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")));
					l_sFunction << l_pcColList->GetActiveCellValue(1);
				}
				else if(l_pcNode->GetAttribute(wxT("DelayList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")));
					l_sFunction << l_pcColList->GetActiveCellValue(1);
				}
				else if(l_pcNode->GetAttribute(wxT("PeriodicList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")));
					l_sFunction << l_pcColList->GetActiveCellValue(1);
					l_sFunction << wxT(",") << l_pcColList->GetActiveCellValue(2);
					l_sFunction << wxT(",") << l_pcColList->GetActiveCellValue(3);
				}
				t->function_ = l_sFunction;

				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Reversible"));
				if(l_pcAttr)
				{
					t->reversible_ = dynamic_cast< SP_DS_BoolAttribute* >(l_pcAttr)->GetValue();
				}

				b.addTransition(t);
			}
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateConstants(dssd::andl::simple_net_builder& b)
{
	// for the new constants
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(mc)
	{
		//create valuesets
		if(!mc->GetElements()->empty())
		{
			SP_DS_Metadata* l_pcMetadata = mc->GetElements()->front();
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcMetadata->GetAttribute(wxT("ValueList")));
			if(l_pcColList->GetRowCount() > 1)
			{
				for (unsigned i = 0; i < l_pcColList->GetRowCount(); ++i)
				{
					wxString valueSet = SP_NormalizeString(l_pcColList->GetCell(i,0));
					b.registerValueSet(valueSet.ToStdString());
				}
			}
		}
		//create constants
		for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
		{
			auto type = dssd::andl::ConstType::INT_T;
			wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
			if(l_sType == wxT("double"))
			{
				type = dssd::andl::ConstType::DOUBLE_T;
			}
			std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			std::string l_sGroup = l_pcMetadata->GetAttribute(wxT("Group"))->GetValueString();

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcMetadata->GetAttribute(wxT("ValueList")));
			std::vector<std::string> values;
			values.reserve(l_pcColList->GetRowCount());
			for (unsigned i = 0; i < l_pcColList->GetRowCount(); ++i)
			{
				std::string l_sValue = l_pcColList->GetCell(i, 1);
				values.push_back(l_sValue);
			}
			auto c = make_shared<dssd::andl::Constant>(type, l_sName, l_sGroup, values);
			b.addConstant(c);
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateFunctions(dssd::andl::simple_net_builder& b)
{
	// for the functions
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if(mc)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
		{
			std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			std::string l_sParam = l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString();
			std::string l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
			auto f = make_shared<dssd::andl::Function>(l_sName, l_sParam, l_sBody);
			b.addFunction(f);
		}
	}
	return true;
}

bool SP_SimpleNetBuilder::CreateObservers(dssd::andl::simple_net_builder& b)
{
    // for the observers
    SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
    if(mc)
    {
        for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
        {
            std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
            std::string l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
            dssd::andl::ObserverType l_eType = dssd::andl::ObserverType::PLACE_T;
            if(l_sType == "Transition") {
                l_eType = dssd::andl::ObserverType::TRANS_T;
            }
            std::string l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
            auto o = make_shared<dssd::andl::Observer>(l_eType, l_sName, l_sBody);
            b.addObserver(o);
        }
    }
	return true;
}

//////////////////////////////////////////////////////////////////////

bool SP_ColoredNetBuilder::operator ()(SP_DS_Graph* p_pcGraph )
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;
	bool l_bFinish = true;

	try
	{
		dssd::andl::simple_net_builder b;
		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLPN_CLASS)
		{
			b.setType(dssd::andl::NetType::COL_PN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS)
		{
			b.setType(dssd::andl::NetType::COL_XPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
		{
			if(!m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
				|| !m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
			{
				b.setType(dssd::andl::NetType::COL_XSPN_T);
			}
			else if(!m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
			{
				b.setType(dssd::andl::NetType::COL_GSPN_T);
			}
			else
			{
				b.setType(dssd::andl::NetType::COL_SPN_T);

			}
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
		{
			b.setType(dssd::andl::NetType::COL_CPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS)
		{
			b.setType(dssd::andl::NetType::COL_HPN_T);
		}
		else
		{
			return false;
		}

		SP_DS_Metadata* l_pcMeta = m_pcGraph->GetMetadataclass(wxT("General"))->GetElements()->front();
		wxString netName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();
		if(netName.IsEmpty())
		{
			wxFileName fn = wxFileName(m_pcGraph->GetParentDoc()->GetUserReadableName());
			netName = fn.GetName();
		}

		b.setName(SP_NormalizeString(netName, false).ToStdString());

		//l_bFinish &= CreateFunctions(b);
		l_bFinish &= CreateConstants(b);
		l_bFinish &= CreateColorsets(b);
		l_bFinish &= CreateVariables(b);
		l_bFinish &= CreateColorFunctions(b);
		l_bFinish &= CreatePlaces(b);
		l_bFinish &= CreateObservers(b);
		l_bFinish &= CreateTransitions(b);

		m_Net = b.get();
	}
	catch(std::exception& e)
	{
		l_bFinish = false;
		SP_LOGERROR(e.what());
	}
	catch(...)
	{
		l_bFinish = false;
		SP_LOGERROR(wxT("unkown exception!"));
	}
	return l_bFinish;
}

bool SP_ColoredNetBuilder::CreateVariables(dssd::andl::simple_net_builder& b)
{
	if(!m_pcGraph)
		return false;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;

	if(l_pcMetadataclass->GetElements()->empty())
		return false;

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute *l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		std::string name = l_pcColList->GetCell(i,0);
		std::string colorset = l_pcColList->GetCell(i,1);
		auto v = std::make_shared<dssd::andl::Variable>(name, colorset);
		b.addVariable(v);
	}

	return true;
}

bool SP_ColoredNetBuilder::CreateColorsets(dssd::andl::simple_net_builder& b)
{
	if(!m_pcGraph)
		return false;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	if(l_pcMetadataclass->GetElements()->empty())
		return false;

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		std::string name = l_pcColList->GetCell(i,0);
		std::string type = l_pcColList->GetCell(i,1);
		std::string value = "{" + l_pcColList->GetCell(i, 2) + "}";
		if(type=="enum"&& value.find('-')!=std::string::npos)//george
		{
			SP_LOGMESSAGE("The type " + type + " with colour range is not supported by CANDL format");
			wxString l_ColValues = l_pcColList->GetCell(i, 2);
			SP_CPN_ColorProcessing l_cpn_process;
			std::vector<wxString> l_vColors;

			l_cpn_process.ProcessingString(name, l_ColValues, l_vColors);
			wxString l_sColors;
			for (auto i : l_vColors)
			{
				l_sColors << "," << i;
			}
			l_sColors = l_sColors.AfterFirst(',');
			value = "{" + l_sColors.ToStdString() + "}";
		}

		if (type == "string" )//george
		{
			wxString l_ColValues = l_pcColList->GetCell(i, 2);
			SP_CPN_ColorProcessing l_cpn_process;
			std::vector<wxString> l_vColors;

			l_cpn_process.ProcessingString(name, l_ColValues, l_vColors);
			wxString l_sColors;
			wxString l_sQutCol;
			for (auto i : l_vColors)
			{
				l_sQutCol << wxT("\"") << i << wxT("\"");
				l_sColors <<","<< l_sQutCol ;
				l_sQutCol.Empty();
			}
			l_sColors = l_sColors.AfterFirst(',');
			value = "{" + l_sColors.ToStdString() + "}";
		}
		if (type == "union" || type=="index")//george
		{
			SP_LOGMESSAGE("The type "+ type+" is not supported by CANDL format");
		}
		dssd::aux::replaceAll(value, "-", "..");
		auto cs = std::make_shared<dssd::andl::Colorset>(name, value);
		if(type == wxT("dot"))
		{
			cs->type_ = dssd::andl::ColorsetType::DOT_T;
		}
		else if(type == wxT("int"))
		{
			cs->type_ = dssd::andl::ColorsetType::INT_T;
		}
		else if(type == wxT("enum"))
		{
			cs->type_ = dssd::andl::ColorsetType::ENUM_T;
		}
		else if(type == wxT("string"))
		{
			cs->type_ = dssd::andl::ColorsetType::STRING_T;
		}
		b.addColorset(cs);
	}

	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	if(l_pcMetadataclass->GetElements()->empty())
		return false;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		std::string name = l_pcColList->GetCell(i,0);
		std::string type = l_pcColList->GetCell(i,1);
		std::string value = l_pcColList->GetCell(i,2);
		if(type == "product")
		{
			type = type.substr(0,4);
			value = dssd::aux::toUpper(type) + "(" + value + ")";
		}
		else if(type == "union")
		{
			dssd::aux::replaceAll(value, ",", "+");
			SP_LOGMESSAGE("The type " + type + " is not supported by CANDL format");
		}
		else
		{
			value = type + "[" + value + "]";
		}
		dssd::aux::replaceAll(value, "<>", "!=");
		auto cs = std::make_shared<dssd::andl::Colorset>(name, value);
		b.addColorset(cs);
	}

	return true;
}

bool SP_ColoredNetBuilder::CreateColorFunctions(dssd::andl::simple_net_builder& b)
{
	if(!m_pcGraph)
		return false;

	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;

	if(l_pcMetadataclass->GetElements()->empty())
		return false;

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		std::string type = l_pcColList->GetCell(i,0);
		std::string name = l_pcColList->GetCell(i,1);
		std::string param = l_pcColList->GetCell(i,2);
		std::vector<std::string> tmp;
		dssd::aux::tokenize(param, tmp, ",");
		dssd::andl::ColorFunction::Params params;
		for(auto& s : tmp)
		{
			// different member versions of find in the same order as above:
			std::string::size_type found = s.find(" ");
			if (found != std::string::npos)
			{
				params.push_back(std::make_pair(s.substr(0,found), s.substr(found+1)));
			}
		}
		std::string body = l_pcColList->GetCell(i,3);
		dssd::aux::replaceAll(body, "<>", "!=");
		auto f = std::make_shared<dssd::andl::ColorFunction>(name, type, params, body);
		b.addColorFunction(f);
	}
	return true;
}

bool SP_ColoredNetBuilder::CreatePlaces(dssd::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Place")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dssd::andl::PlType::DISCRETE_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("continuous"))
				{
					l_Type = dssd::andl::PlType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				std::string l_sColorset = l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME)->GetValueString();
				std::string l_sMarking;
				if(l_pcNode->GetAttribute(wxT("MarkingList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sColors = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
						if(l_sColors.IsEmpty())
						{
							l_sColors = "all";
						}
						wxString l_sToken = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
						if(l_sToken.IsEmpty())
						{
							l_sToken = "0";
						}
						if(i>0)
						{
							l_sMarking += "++";
						}
						l_sMarking += "" + l_sToken
								+ "`" + l_sColors
								+ "";
						wxString l_sOld = l_sMarking;//george
						wxString l_sNew;//george
						 
						PrePareMarkingString(l_sOld, l_sNew);//george
						l_sMarking = l_sNew.ToStdString();//george
					    
					 
					}
				}
				else
				{
					l_sMarking = l_pcNode->GetAttribute(wxT("Marking"))->GetValueString();
					wxString l_sOld = l_sMarking;//george
					wxString l_sNew;//
					PrePareMarkingString(l_sOld, l_sNew);//
					l_sMarking=l_sNew.ToStdString();//
				}
				dssd::aux::replaceAll(l_sMarking, "all()", "all");
				dssd::aux::replaceAll(l_sMarking, "auto()", "auto");
				dssd::aux::replaceAll(l_sMarking, "<>", "!=");
				auto p = std::make_shared<dssd::andl::Place>(l_Type, l_sName, l_sMarking, l_sColorset);
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(wxT("Fixed"));
				if(l_pcAttr)
				{
					p->fixed_ = dynamic_cast< SP_DS_BoolAttribute* >(l_pcAttr)->GetValue();
				}
				b.addPlace(p);
			}
		}
	}

	return TRUE;
}

bool SP_ColoredNetBuilder::CreateTransitions(dssd::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Transition")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dssd::andl::TrType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("stochastic"))
				{
					l_Type = dssd::andl::TrType::STOCHASTIC_T;
				}
				else if(l_sType == wxT("immediate"))
				{
					l_Type = dssd::andl::TrType::IMMEDIATE_T;
				}
				else if(l_sType == wxT("deterministic"))
				{
					l_Type = dssd::andl::TrType::DETERMINISTIC_T;
				}
				else if(l_sType == wxT("scheduled"))
				{
					l_Type = dssd::andl::TrType::SCHEDULED_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dssd::andl::TrType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				auto t = make_shared<dssd::andl::Transition>(l_Type, l_sName);

				//guard
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST);
				if(l_pcAttr)
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
					wxString l_sGuard = l_pcColList->GetActiveCellValue(1);
					l_sGuard.Trim(true).Trim(false);
					if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
						t->guard_ = "[" + l_sGuard + "]";
					dssd::aux::replaceAll(t->guard_, "all()", "all");
					dssd::aux::replaceAll(t->guard_, "auto()", "auto");
					dssd::aux::replaceAll(t->guard_, "<>", "!=");
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetSourceEdges()))
				{
					SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
					std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
					std::string l_sInscr = l_pcColList->GetActiveCellValue(1);
					dssd::aux::replaceAll(l_sInscr, "all()", "all");
					dssd::aux::replaceAll(l_sInscr, "auto()", "auto");
					dssd::aux::replaceAll(l_sInscr, "<>", "!=");

					auto u = make_shared<dssd::andl::Update>(l_sTarget, '+', l_sInscr);
					t->updates_->push_back(u);
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetTargetEdges()))
				{
					SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					std::string l_sSource = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
					std::string l_sInscr = l_pcColList->GetActiveCellValue(1);
					dssd::aux::replaceAll(l_sInscr, "all()", "all");
					dssd::aux::replaceAll(l_sInscr, "auto()", "auto");
					dssd::aux::replaceAll(l_sInscr, "<>", "!=");

					if(l_pcEdge->GetClassName() == SP_DS_EDGE)
					{
						auto u = make_shared<dssd::andl::Update>(l_sSource, '-', l_sInscr);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_RESET_EDGE)
					{
						auto u = make_shared<dssd::andl::Update>(l_sSource, '=', l_sInscr);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_READ_EDGE)
					{
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sInscr, dssd::andl::CondType::READ_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_INHIBITOR_EDGE)
					{
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sInscr, dssd::andl::CondType::INHIBITOR_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_EQUAL_EDGE)
					{
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sInscr, dssd::andl::CondType::EQUAL_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_MODIFIER_EDGE)
					{
						auto c = make_shared<dssd::andl::Condition>(l_sSource, l_sInscr, dssd::andl::CondType::MODIFIER_T);
						t->conditions_->push_back(c);
					}
				}

				// function
				wxString l_sFunction;
				if(l_pcNode->GetAttribute(wxT("FunctionList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
						l_sGuard.Trim(true).Trim(false);
                        wxString l_sFunc = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
                        l_sFunc.Trim(true).Trim(false);
                        if(!l_sFunc.IsEmpty())
                        {
                            if(!l_sFunction.IsEmpty())
                            {
                                l_sFunction << wxT(" ++ ");
                            }
                            if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
                            {
                                l_sFunction << wxT("[") << l_sGuard << wxT("] ");
                            }
                            l_sFunction << l_sFunc;
                        }
					}
				}
				else if(l_pcNode->GetAttribute(wxT("DelayList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
                        wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
                        l_sGuard.Trim(true).Trim(false);
                        wxString l_sFunc = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
                        l_sFunc.Trim(true).Trim(false);
                        if(!l_sFunc.IsEmpty())
                        {
                            if(!l_sFunction.IsEmpty())
                            {
                                l_sFunction << wxT(" ++ ");
                            }
                            if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
                            {
                                l_sFunction << wxT("[") << l_sGuard << wxT("] ");
                            }
                            l_sFunction << l_sFunc;
                        }
					}
				}
				else if(l_pcNode->GetAttribute(wxT("PeriodicList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
                        wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
                        l_sGuard.Trim(true).Trim(false);
                        wxString l_sFunc = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1)
                                + wxT(",") + l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+2)
                                + wxT(",") + l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+3);
                        l_sFunc.Trim(true).Trim(false);
                        if(!l_sFunc.IsEmpty())
                        {
                            if(!l_sFunction.IsEmpty())
                            {
                                l_sFunction << wxT(" ++ ");
                            }
                            if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
                            {
                                l_sFunction << wxT("[") << l_sGuard << wxT("] ");
                            }
                            l_sFunction << l_sFunc;
                        }
					}
				}
				t->function_ = l_sFunction;
				dssd::aux::replaceAll(t->function_, "all()", "all");
				dssd::aux::replaceAll(t->function_, "auto()", "auto");
				dssd::aux::replaceAll(t->function_, "<>", "!=");

				l_pcAttr = l_pcNode->GetAttribute(wxT("Reversible"));
				if(l_pcAttr)
				{
					t->reversible_ = dynamic_cast< SP_DS_BoolAttribute* >(l_pcAttr)->GetValue();
				}

				b.addTransition(t);
			}
		}
	}

	return TRUE;
}

bool SP_ColoredNetBuilder::CreateConstants(dssd::andl::simple_net_builder& b)
{
	if(!m_pcGraph)
		return false;
  
	bool l_bForUpdateMarking = m_bIsExport;
	/*************** by george*************/
	if (!l_bForUpdateMarking)
	{
		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_ListMetadata::const_iterator it;
		int i = 0;
		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{

			SP_DS_Metadata* l_pcConstant = *it;
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();
			wxString l_sGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcConstant->GetAttribute(wxT("Group")))->GetValue();
			SP_DS_ColListAttribute * l_pcSourceColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			if (i == 0) {

				for (unsigned i = 0; i < l_pcSourceColList->GetRowCount(); ++i)
				{
					wxString valueSet = SP_NormalizeString(l_pcSourceColList->GetCell(i, 0));
					b.registerValueSet(valueSet.ToStdString());
				}
			}
			i++;

			std::vector<string> l_sVsets;
			for (unsigned i = 0; i < l_pcSourceColList->GetRowCount(); ++i)
			{
				l_sVsets.push_back(l_pcSourceColList->GetCell(i, 1));

			}

			SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();


			SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
			if (l_FE.IsOk())
			{
				SP_FunctionPtr l_Function = l_FE.getFunction();
				wxString l_sConstVal;

				if (l_Function->isValue())
				{
					if (l_sType == wxT("double"))
					{
						double l_nValue = 0.0;
						l_nValue = l_Function->getValue();


						l_sConstVal << l_nValue;
					}
					else {
						long   l_nValue = 0.0;
						l_nValue = l_Function->getValue();


						l_sConstVal << l_nValue;
					}


					//l_pcFR->registerFunction(l_sName, to_string(l_nValue));
				}
				else
				{
					//evaluate string
					wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
					if (l_sType == wxT("int"))
					{
						long l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();

						l_sConstVal << l_nValue;

						//l_pcFR->registerFunction(l_sName, to_string(l_nValue));
					}
					else if (l_sType == wxT("double"))
					{
						double l_dval;
						l_dval = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
						l_sConstVal << l_dval;
						//l_pcFR->registerFunction(l_sName, to_string(l_dval));
					}

				}

				if (l_sType == wxT("int"))
				{
					auto type = dssd::andl::ConstType::INT_T;
					std::vector<std::string> values = l_sVsets;// { l_sConstVal };
					auto c = std::make_shared<dssd::andl::Constant>(type, l_sName, l_sGroup, values);//l_sGroup was "all"
					b.addConstant(c);
				}
				if (l_sType == wxT("double"))
				{
					auto type = dssd::andl::ConstType::DOUBLE_T;
					std::vector<std::string> values = l_sVsets;// { l_sConstVal };
					auto c = std::make_shared<dssd::andl::Constant>(type, l_sName, l_sGroup, values);//l_sGroup was "param"
					b.addConstant(c);
				}
				if (l_sType != wxT("double") && l_sType != wxT("int"))
				{
					continue;
				}
			}
		}
	}
	else
	{//for updating marking

		SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_ListMetadata::const_iterator it;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcConstant = *it;
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcConstant->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcConstant->GetAttribute(wxT("Type")))->GetValue();///Added by G.A

			SP_DS_FunctionRegistry* l_pcFR = m_pcGraph->GetFunctionRegistry();

			wxString l_sConstVal;
			SP_DS_FunctionRegistryEntry l_FE = l_pcFR->lookUpFunction(l_sName);
			if (l_FE.IsOk())
			{
				SP_FunctionPtr l_Function = l_FE.getFunction();
				long l_nValue = 0;
				if (l_Function->isValue())
				{
					l_nValue = l_Function->getValue();

				
					l_sConstVal << l_nValue;
 
				}
				else
				{
					//evaluate string
					wxString l_sType = l_pcConstant->GetAttribute(wxT("Type"))->GetValueString();
					if (l_sType == wxT("int"))
					{
						l_nValue = SP_DS_FunctionEvaluatorLong{ l_pcFR, l_Function }();
						//wxString l_sConstVal;
						l_sConstVal << l_nValue;
 

					}
					else if (l_sType == wxT("double"))
					{
						l_nValue = SP_DS_FunctionEvaluatorDouble{ l_pcFR, l_Function }();
						l_sConstVal << l_nValue;
					}
					//l_pcFR->registerFunction(l_sName, to_string(l_nValue));
				}

				auto type = dssd::andl::ConstType::INT_T;
				if (l_sType == wxT("double"))
				{
					type = dssd::andl::ConstType::DOUBLE_T;
				}
				else if (l_sType != "int")
				{
					continue;
				}
				std::string l_sVal = l_sConstVal.ToStdString();
				std::vector<std::string> values = { l_sVal };
				auto c = std::make_shared<dssd::andl::Constant>(type, l_sName, "all", values);
				b.addConstant(c);
			}
		}

	}
	/***************************************/

	/**
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return false;

	if(l_pcMetadataclass->GetElements()->empty())
		return false;

	SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		std::string name = l_pcColList->GetCell(i,0);
		wxString l_sType = l_pcColList->GetCell(i,1);
		std::string value = l_pcColList->GetCell(i,2);
		auto type = dssd::andl::ConstType::INT_T;
		if(l_sType == wxT("double"))
		{
			type = dssd::andl::ConstType::DOUBLE_T;
		}
		else if(l_sType != "int")
		{
			continue;
		}
		std::vector<std::string> values = {value};
		auto c = std::make_shared<dssd::andl::Constant>(type, name, "all", values);
		b.addConstant(c);
	}


	SP_DS_Nodeclass* l_pcNC = m_pcGraph->GetNodeclass(SP_DS_PARAM);
	if(l_pcNC)
	{
		if(!l_pcNC->GetElements()->empty())
		{
			SP_DS_Node* l_pcNode = l_pcNC->GetElements()->front();
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
			if(l_pcColList->GetRowCount() > 1)
			{
				for (unsigned i = 0; i < l_pcColList->GetRowCount(); ++i)
				{
					wxString valueSet = SP_NormalizeString(l_pcColList->GetCell(i,0));
					b.registerValueSet(valueSet.ToStdString());
				}
			}
		}
		for(SP_DS_Node* l_pcNode : *l_pcNC->GetElements())
		{
			auto type = dssd::andl::ConstType::DOUBLE_T;
			std::string name = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
			std::vector<std::string> values;
			values.reserve(l_pcColList->GetRowCount());
			for (unsigned i = 0; i < l_pcColList->GetRowCount(); ++i)
			{
				std::string l_sValue = l_pcColList->GetCell(i, 1);
				values.push_back(l_sValue);
			}
			auto c = make_shared<dssd::andl::Constant>(type, name, "param", values);
			b.addConstant(c);
		}
	}
	*/
	return TRUE;
}

bool SP_ColoredNetBuilder::CreateFunctions(dssd::andl::simple_net_builder& b)
{
	// for the functions
	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if(mc)
	{
		for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
		{
			std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			std::string l_sParam = l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString();
			std::string l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
			auto f = make_shared<dssd::andl::Function>(l_sName, l_sParam, l_sBody);
			b.addFunction(f);
		}
	}
	return true;
}

bool SP_ColoredNetBuilder::CreateObservers(dssd::andl::simple_net_builder& b)
{
    // for the observers
    SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
    if(mc)
    {
        for (SP_DS_Metadata* l_pcMetadata : *(mc->GetElements()))
        {
            std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
            std::string l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
            dssd::andl::ObserverType l_eType = dssd::andl::ObserverType::PLACE_T;
            if(l_sType == "Transition") {
                l_eType = dssd::andl::ObserverType::TRANS_T;
            }
            std::string l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
            auto o = make_shared<dssd::andl::Observer>(l_eType, l_sName, l_sBody);
            b.addObserver(o);
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////

void SP_ColoredNetBuilder::PrePareMarkingString(wxString& p_sMarkingExpression, wxString& l_sResultExp)
{
	l_sResultExp = p_sMarkingExpression;
	wxString l_sProcessed;
	std::vector<wxString> l_vList;
	if(l_sResultExp.Contains("++"))
	{
		char_separator<char> sep("++");
		tokenizer<char_separator<char>> tokens(l_sResultExp.ToStdString(), sep);

		for (const auto& t : tokens)
		{
			wxString xx;
			xx =wxString( t);
			l_vList.push_back(t);
		}
	}
	else 
	{
		l_vList.push_back(l_sResultExp);
	}
	for (int i = 0; i < l_vList.size(); i++)
	{
		l_sResultExp = l_vList[i];
		//deal with all
		if (l_sResultExp.Contains(wxT("all()")) || l_sResultExp.Contains(wxT("All")) ||
			l_sResultExp.Contains(wxT("ALL")))
		{
			l_sResultExp.Replace(wxT("all()"), wxT("all"));
			l_sResultExp.Replace(wxT("All()"), wxT("all"));
			l_sResultExp.Replace(wxT("ALL()"), wxT("all"));

		}
		std::map<wxString, wxString> l_mVar2Type;
		bool                         l_bIsColurFunction = false;
		///////////////////////////////////
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
		if (!l_pcMetadataclass)
			return;
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if (!l_pcNewMetadata)
			return;
		SP_DS_ColListAttribute*  l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
		if (!l_pcColList)
			return;
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			SP_CPN_Collist_Declarations l_scDeclaration;
			wxString l_sName = wxString(l_pcColList->GetCell(i, 0).c_str());
			wxString  l_sType = wxString(l_pcColList->GetCell(i, 1).c_str());
			l_mVar2Type[l_sName] = l_sType;

		}
		//////////////////////
		l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
		if (!l_pcMetadataclass)
			return;
		l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		if (!l_pcNewMetadata)
			return;
		l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		if (!l_pcColList)
			return;

		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sFunName = wxString(l_pcColList->GetCell(i, 1).c_str());
			if (l_sResultExp.Contains(l_sFunName))
			{
				l_bIsColurFunction = true;
				break;
			}
		}


		/////////////////////////////////////

		SP_CPN_SyntaxChecking l_sCheck;

		std::string l_sPred = l_sResultExp.ToStdString();
		if (l_sResultExp.Contains(","))
		{
			l_sProcessed = l_sResultExp;
			continue;
		}
		//deal with predicates
		if (l_sResultExp.Contains(wxT("&")) || l_sResultExp.Contains(wxT("|")) || l_sResultExp.Contains(wxT(">")) ||
			l_sResultExp.Contains(wxT(">=")) || l_sResultExp.Contains(wxT("<")) || l_sResultExp.Contains(wxT(">=")) ||
			l_sResultExp.Contains(wxT("=")) || l_sResultExp.Contains(wxT("!")) || l_bIsColurFunction
			)
		{
			std::set<std::string> l_setVar;
			std::string tupel = "";
			char_separator<char> sep("&|=)(-*+`%!<>,/ ");
			tokenizer<char_separator<char>> tokens(l_sPred, sep);

			for (const auto& t : tokens)
			{

				auto it = l_mVar2Type.find(t);
				if (it != l_mVar2Type.end())
				{
					tupel += t + ",";
				}
			}

			if (tupel[tupel.length() - 1] == ',')
			{
				tupel[tupel.length() - 1] = ' ';
				tupel = "(" + tupel + ")";
			}

			std::size_t pos = l_sPred.find("`");
			std::string num_tokens = l_sPred.substr(0, pos);
			std::string color = l_sPred.substr(pos + 1, l_sPred.length());

			std::string preparedMArkingExp = "[" + color + "]" + num_tokens + "`" + tupel;
			l_sPred = preparedMArkingExp;
			l_sResultExp = l_sPred;
		}

		if (l_vList.size() > 1)
		{
			l_sProcessed << wxT("++")<< l_sResultExp ;
		}
	}
	if (l_vList.size() > 1)
	{
		l_sResultExp = l_sProcessed;
	}
}