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

#include "dssz/andl/andl_builder.h"
#include "dssz/auxi/auxi.h"

#include <wx/filename.h>
#include <wx/tokenzr.h>

//////////////////////////////////////////////////////////////////////

bool SP_SimpleNetBuilder::operator ()(SP_DS_Graph* p_pcGraph)
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;
	bool l_bFinish = true;
	try
	{
		dsszmc::andl::simple_net_builder b;
		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_PN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::PN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_EXTPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::XPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_SPN_CLASS)
		{
			if(!m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
				|| !m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
			{
				b.setType(dsszmc::andl::NetType::XSPN_T);
			}
			else if(!m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
			{
				b.setType(dsszmc::andl::NetType::GSPN_T);
			}
			else
			{
				b.setType(dsszmc::andl::NetType::SPN_T);

			}
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_CONTINUOUSPED_CLASS)
		{
			b.setType(dsszmc::andl::NetType::CPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_HYBRIDPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::HPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_TIMEPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::TPN_T);
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

bool SP_SimpleNetBuilder::CreatePlaces(dsszmc::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Place")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dsszmc::andl::PlType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("discrete"))
				{
					l_Type = dsszmc::andl::PlType::DISCRETE_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dsszmc::andl::PlType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				std::string l_sMarking;
				if(l_pcNode->GetAttribute(wxT("MarkingList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("MarkingList")));
					l_sMarking = l_pcColList->GetActiveCellValue(1);
				}
				else
				{
					l_sMarking = l_pcNode->GetAttribute(wxT("Marking"))->GetValueString();
				}
				auto p = std::make_shared<dsszmc::andl::Place>(l_Type, l_sName, l_sMarking);
				b.addPlace(p);
			}
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateTransitions(dsszmc::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Transition")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dsszmc::andl::TrType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("stochastic"))
				{
					l_Type = dsszmc::andl::TrType::STOCHASTIC_T;
				}
				else if(l_sType == wxT("immediate"))
				{
					l_Type = dsszmc::andl::TrType::IMMEDIATE_T;
				}
				else if(l_sType == wxT("deterministic"))
				{
					l_Type = dsszmc::andl::TrType::DETERMINISTIC_T;
				}
				else if(l_sType == wxT("scheduled"))
				{
					l_Type = dsszmc::andl::TrType::SCHEDULED_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dsszmc::andl::TrType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				auto t = make_shared<dsszmc::andl::Transition>(l_Type, l_sName);

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetSourceEdges()))
				{
					SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
					std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
					auto u = make_shared<dsszmc::andl::Update>(l_sTarget, '+', l_sMult);
					t->updates_->push_back(u);
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetTargetEdges()))
				{
					SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					std::string l_sSource = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if(l_pcEdge->GetClassName() == SP_DS_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto u = make_shared<dsszmc::andl::Update>(l_sSource, '-', l_sMult);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_RESET_EDGE)
					{
						bool found = false;
						for(auto u : *(t->updates_))
						{
							if(u->place_ == l_sSource && u->type_ == dsszmc::andl::UpdateType::INCREASE_T)
							{
								u->op_ = '=';
								u->type_ = dsszmc::andl::UpdateType::ASSIGN_T;
								found = true;
							}
						}
						if(!found)
						{
							auto u = make_shared<dsszmc::andl::Update>(l_sSource, '=', "0");
							t->updates_->push_back(u);
						}
					}
					else if(l_pcEdge->GetClassName() == SP_DS_READ_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sMult, dsszmc::andl::CondType::READ_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_INHIBITOR_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sMult, dsszmc::andl::CondType::INHIBITOR_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_EQUAL_EDGE)
					{
						std::string l_sMult = l_pcEdge->GetAttribute(wxT("Multiplicity"))->GetValueString();
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sMult, dsszmc::andl::CondType::EQUAL_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_MODIFIER_EDGE)
					{
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, "", dsszmc::andl::CondType::MODIFIER_T);
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

				b.addTransition(t);
			}
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateConstants(dsszmc::andl::simple_net_builder& b)
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
			auto type = dsszmc::andl::ConstType::INT_T;
			wxString l_sType = l_pcMetadata->GetAttribute(wxT("Type"))->GetValueString();
			if(l_sType == wxT("double"))
			{
				type = dsszmc::andl::ConstType::DOUBLE_T;
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
			auto c = make_shared<dsszmc::andl::Constant>(type, l_sName, l_sGroup, values);
			b.addConstant(c);
		}
	}

	return TRUE;
}

bool SP_SimpleNetBuilder::CreateFunctions(dsszmc::andl::simple_net_builder& b)
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
			auto f = make_shared<dsszmc::andl::Function>(l_sName, l_sParam, l_sBody);
			b.addFunction(f);
		}
	}
	return true;
}

bool SP_SimpleNetBuilder::CreateObservers(dsszmc::andl::simple_net_builder& b)
{
	//TODO
	return true;
}

//////////////////////////////////////////////////////////////////////

bool SP_ColoredNetBuilder::operator ()(SP_DS_Graph* p_pcGraph)
{
	if(!p_pcGraph) return false;

	m_pcGraph = p_pcGraph;
	bool l_bFinish = true;

	try
	{
		dsszmc::andl::simple_net_builder b;
		if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::COL_PN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLEPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::COL_XPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLSPN_CLASS)
		{
			if(!m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
				|| !m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
			{
				b.setType(dsszmc::andl::NetType::COL_XSPN_T);
			}
			else if(!m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
			{
				b.setType(dsszmc::andl::NetType::COL_GSPN_T);
			}
			else
			{
				b.setType(dsszmc::andl::NetType::COL_SPN_T);

			}
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLCPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::COL_CPN_T);
		}
		else if(m_pcGraph->GetNetclass()->GetName() == SP_DS_COLHPN_CLASS)
		{
			b.setType(dsszmc::andl::NetType::COL_HPN_T);
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

bool SP_ColoredNetBuilder::CreateVariables(dsszmc::andl::simple_net_builder& b)
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
		auto v = std::make_shared<dsszmc::andl::Variable>(name, colorset);
		b.addVariable(v);
	}

	return true;
}

bool SP_ColoredNetBuilder::CreateColorsets(dsszmc::andl::simple_net_builder& b)
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
		std::string value = "{" + l_pcColList->GetCell(i,2) + "}";
		dsszmc::aux::replaceAll(value, "-", "..");
		auto cs = std::make_shared<dsszmc::andl::Colorset>(name, value);
		if(type == wxT("dot"))
		{
			cs->type_ = dsszmc::andl::ColorsetType::DOT_T;
		}
		else if(type == wxT("int"))
		{
			cs->type_ = dsszmc::andl::ColorsetType::INT_T;
		}
		else if(type == wxT("enum"))
		{
			cs->type_ = dsszmc::andl::ColorsetType::ENUM_T;
		}
		else if(type == wxT("string"))
		{
			cs->type_ = dsszmc::andl::ColorsetType::STRING_T;
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
			value = dsszmc::aux::toUpper(type) + "(" + value + ")";
		}
		else if(type == "union")
		{
			dsszmc::aux::replaceAll(value, ",", "+");
		}
		else
		{
			value = type + "[" + value + "]";
		}
		dsszmc::aux::replaceAll(value, "<>", "!=");
		auto cs = std::make_shared<dsszmc::andl::Colorset>(name, value);
		b.addColorset(cs);
	}

	return true;
}

bool SP_ColoredNetBuilder::CreateColorFunctions(dsszmc::andl::simple_net_builder& b)
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
		dsszmc::aux::tokenize(param, tmp, ",");
		dsszmc::andl::ColorFunction::Params params;
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
		dsszmc::aux::replaceAll(body, "<>", "!=");
		auto f = std::make_shared<dsszmc::andl::ColorFunction>(name, type, params, body);
		b.addColorFunction(f);
	}
	return true;
}

bool SP_ColoredNetBuilder::CreatePlaces(dsszmc::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Place")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dsszmc::andl::PlType::DISCRETE_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("continuous"))
				{
					l_Type = dsszmc::andl::PlType::CONTINUOUS_T;
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
					}
				}
				else
				{
					l_sMarking = l_pcNode->GetAttribute(wxT("Marking"))->GetValueString();
				}
				dsszmc::aux::replaceAll(l_sMarking, "all()", "all");
				dsszmc::aux::replaceAll(l_sMarking, "auto()", "auto");
				dsszmc::aux::replaceAll(l_sMarking, "<>", "!=");
				auto p = std::make_shared<dsszmc::andl::Place>(l_Type, l_sName, l_sMarking, l_sColorset);
				b.addPlace(p);
			}
		}
	}

	return TRUE;
}

bool SP_ColoredNetBuilder::CreateTransitions(dsszmc::andl::simple_net_builder& b)
{
	for(SP_DS_Nodeclass* NC : *(m_pcGraph->GetNodeclasses()))
	{
		wxString l_sNCName = NC->GetDisplayName();
		if(l_sNCName.Find(wxT("Transition")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			auto l_Type = dsszmc::andl::TrType::DEFAULT_T;
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !NC->GetElements()->empty())
			{
				wxString l_sType = l_sNCName.BeforeFirst(' ').Lower();
				if(l_sType == wxT("stochastic"))
				{
					l_Type = dsszmc::andl::TrType::STOCHASTIC_T;
				}
				else if(l_sType == wxT("immediate"))
				{
					l_Type = dsszmc::andl::TrType::IMMEDIATE_T;
				}
				else if(l_sType == wxT("deterministic"))
				{
					l_Type = dsszmc::andl::TrType::DETERMINISTIC_T;
				}
				else if(l_sType == wxT("scheduled"))
				{
					l_Type = dsszmc::andl::TrType::SCHEDULED_T;
				}
				else if(l_sType == wxT("continuous"))
				{
					l_Type = dsszmc::andl::TrType::CONTINUOUS_T;
				}
			}
			for(SP_DS_Node* l_pcNode : *(NC->GetElements()))
			{
				std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				auto t = make_shared<dsszmc::andl::Transition>(l_Type, l_sName);

				//guard
				SP_DS_Attribute* l_pcAttr = l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST);
				if(l_pcAttr)
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcAttr);
					wxString l_sGuard = l_pcColList->GetActiveCellValue(1);
					l_sGuard.Trim(true).Trim(false);
					if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
						t->guard_ = "[" + l_sGuard + "]";
					dsszmc::aux::replaceAll(t->guard_, "all()", "all");
					dsszmc::aux::replaceAll(t->guard_, "auto()", "auto");
					dsszmc::aux::replaceAll(t->guard_, "<>", "!=");
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetSourceEdges()))
				{
					SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
					std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
					std::string l_sInscr = l_pcColList->GetActiveCellValue(1);
					dsszmc::aux::replaceAll(l_sInscr, "all()", "all");
					dsszmc::aux::replaceAll(l_sInscr, "auto()", "auto");
					dsszmc::aux::replaceAll(l_sInscr, "<>", "!=");

					auto u = make_shared<dsszmc::andl::Update>(l_sTarget, '+', l_sInscr);
					t->updates_->push_back(u);
				}

				for(SP_DS_Edge* l_pcEdge : *(l_pcNode->GetTargetEdges()))
				{
					SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					std::string l_sSource = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
					std::string l_sInscr = l_pcColList->GetActiveCellValue(1);
					dsszmc::aux::replaceAll(l_sInscr, "all()", "all");
					dsszmc::aux::replaceAll(l_sInscr, "auto()", "auto");
					dsszmc::aux::replaceAll(l_sInscr, "<>", "!=");

					if(l_pcEdge->GetClassName() == SP_DS_EDGE)
					{
						auto u = make_shared<dsszmc::andl::Update>(l_sSource, '-', l_sInscr);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_RESET_EDGE)
					{
						auto u = make_shared<dsszmc::andl::Update>(l_sSource, '=', l_sInscr);
						t->updates_->push_back(u);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_READ_EDGE)
					{
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sInscr, dsszmc::andl::CondType::READ_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_INHIBITOR_EDGE)
					{
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sInscr, dsszmc::andl::CondType::INHIBITOR_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_EQUAL_EDGE)
					{
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sInscr, dsszmc::andl::CondType::EQUAL_T);
						t->conditions_->push_back(c);
					}
					else if(l_pcEdge->GetClassName() == SP_DS_MODIFIER_EDGE)
					{
						auto c = make_shared<dsszmc::andl::Condition>(l_sSource, l_sInscr, dsszmc::andl::CondType::MODIFIER_T);
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
						if(i>0)
						{
							l_sFunction << wxT(" ++ ");
						}
						wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
						l_sGuard.Trim(true).Trim(false);
						if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
						{
							l_sFunction << wxT("[") << l_sGuard << wxT("] ");
						}
						l_sFunction << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
					}
				}
				else if(l_pcNode->GetAttribute(wxT("DelayList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						if(i>0)
						{
							l_sFunction << wxT(" ++ ");
						}
						wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
						l_sGuard.Trim(true).Trim(false);
						if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
						{
							l_sFunction << wxT("[") << l_sGuard << wxT("] ");
						}
						l_sFunction << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
					}
				}
				else if(l_pcNode->GetAttribute(wxT("PeriodicList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")));
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						if(i>0)
						{
							l_sFunction << wxT(" ++ ");
						}
						wxString l_sGuard = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
						l_sGuard.Trim(true).Trim(false);
						if(!l_sGuard.IsEmpty() && l_sGuard.CmpNoCase(wxT("true")) != 0)
						{
							l_sFunction << wxT("[") << l_sGuard << wxT("] ");
						}
						l_sFunction << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1)
								<< wxT(",") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+2)
								<< wxT(",") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+3);
					}
				}
				t->function_ = l_sFunction;
				dsszmc::aux::replaceAll(t->function_, "all()", "all");
				dsszmc::aux::replaceAll(t->function_, "auto()", "auto");
				dsszmc::aux::replaceAll(t->function_, "<>", "!=");

				b.addTransition(t);
			}
		}
	}

	return TRUE;
}

bool SP_ColoredNetBuilder::CreateConstants(dsszmc::andl::simple_net_builder& b)
{
	if(!m_pcGraph)
		return false;

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
		auto type = dsszmc::andl::ConstType::INT_T;
		if(l_sType == wxT("double"))
		{
			type = dsszmc::andl::ConstType::DOUBLE_T;
		}
		else if(l_sType != "int")
		{
			continue;
		}
		std::vector<std::string> values = {value};
		auto c = std::make_shared<dsszmc::andl::Constant>(type, name, "all", values);
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
			auto type = dsszmc::andl::ConstType::DOUBLE_T;
			std::string name = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
			std::vector<std::string> values;
			values.reserve(l_pcColList->GetRowCount());
			for (unsigned i = 0; i < l_pcColList->GetRowCount(); ++i)
			{
				std::string l_sValue = l_pcColList->GetCell(i, 1);
				values.push_back(l_sValue);
			}
			auto c = make_shared<dsszmc::andl::Constant>(type, name, "param", values);
			b.addConstant(c);
		}
	}

	return TRUE;
}

bool SP_ColoredNetBuilder::CreateFunctions(dsszmc::andl::simple_net_builder& b)
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
			auto f = make_shared<dsszmc::andl::Function>(l_sName, l_sParam, l_sBody);
			b.addFunction(f);
		}
	}
	return true;
}

bool SP_ColoredNetBuilder::CreateObservers(dsszmc::andl::simple_net_builder& b)
{
	//TODO
	return true;
}

//////////////////////////////////////////////////////////////////////
