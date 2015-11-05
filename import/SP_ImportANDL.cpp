//////////////////////////////////////////////////////////////////////
// $Author: cr $
// $Version: 0.1 $
// $Date: 2011/10/27 $
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

#include "snoopy.h"
#include "sp_defines.h"

#include "dssz/andl/andl_reader.h"

#include <wx/spinctrl.h>

#include <algorithm>

SP_ImportANDL::SP_ImportANDL()
{
}

SP_ImportANDL::~SP_ImportANDL()
{
}

bool SP_ImportANDL::ReadFile(const wxString& p_sFile)
{
	ClearAll();

	m_fileName = p_sFile;

	dsszmc::andl::reader p;
	bool l_Return = false;
	try
	{
		l_Return = p(p_sFile.ToStdString());
		if (l_Return)
		{
			const dsszmc::andl::Net& l_Net = *(p.get());
			wxString l_sMsg = wxT("parse successful\n");
			SP_LOGMESSAGE(l_sMsg);
			CreateGraph(p_sFile, l_Net);
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
	SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();
}

void SP_ImportANDL::ClearAll()
{
	lookupParameters.clear();
	lookupPlaces.clear();
	lookupTransitions.clear();
	arcs.clear();
}

/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////

void SP_ImportANDL::CreateGraph(const wxString& p_sFile, const dsszmc::andl::Net& p_Net)
{
	x = 350.0;
	y = 250.0;
	SP_LOGDEBUG(wxT("Creating Graph"));

	m_eNetType = p_Net.type_;
	m_pcGraph = CreateDocument(p_sFile, p_Net.type_);

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
}

bool SP_ImportANDL::CreateFunc(const dsszmc::andl::Functions& p_Functions)
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
			SP_DS_Metadata* l_Func = mc->NewElement(1);
			l_Func->GetAttribute(wxT("Name"))->SetValueString(name);
			l_Func->GetAttribute(wxT("Parameter"))->SetValueString(param);
			l_Func->GetAttribute(wxT("Body"))->SetValueString(body);
		}
	}

	return true;
}

bool SP_ImportANDL::CreatePlaces(const dsszmc::andl::Places& p_Places)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dsszmc::andl::NetType::CPN_T)
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
		if (p->type_ == dsszmc::andl::PlType::DISCRETE_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
		}
		else if (p->type_ == dsszmc::andl::PlType::CONTINUOUS_T)
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
		l_node->ShowOnCanvas(l_pcCanvas, FALSE, x, y, 0);
		lookupPlaces[p->name_] = l_node;
	}
	return true;
}

bool SP_ImportANDL::CreateConst(const dsszmc::andl::Constants& p_Constants, const dsszmc::andl::Valuesets& p_Valuesets)
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
			if(constant->type_ == dsszmc::andl::ConstType::DOUBLE_T)
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
			for(size_t i = 0; i < constant->values_.size(); ++i)
			{
				wxString value = constant->values_[i];
				if(p_Valuesets.size() > i)
				{
					wxString vset = p_Valuesets[i];
					l_pcAttr->SetCell(i, 0, vset);
				}
				l_pcAttr->SetCell(i, 1, value);
			}
		}
	}

	return true;
}

bool SP_ImportANDL::CreateObservers(const dsszmc::andl::Observers & p_Observers)
{
	//TODO
	return true;
}

bool SP_ImportANDL::CreateTransitions(const dsszmc::andl::Transitions& p_Transitions)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dsszmc::andl::NetType::CPN_T)
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

		if (t->type_ == dsszmc::andl::TrType::STOCHASTIC_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_STOCHASTIC_TRANS);
		}
		else if (t->type_ == dsszmc::andl::TrType::IMMEDIATE_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_IMMEDIATE_TRANS);
		}
		else if (t->type_ == dsszmc::andl::TrType::DETERMINISTIC_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS);
		}
		else if (t->type_ == dsszmc::andl::TrType::SCHEDULED_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_SCHEDULED_TRANS);
		}
		else if (t->type_ == dsszmc::andl::TrType::CONTINUOUS_T)
		{
			nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_TRANS);
		}

		wxString name = t->name_;
		l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
		l_node->GetAttribute(wxT("Name"))->SetValueString(name);
		l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);

		if (m_eNetType == dsszmc::andl::NetType::SPN_T
			|| m_eNetType == dsszmc::andl::NetType::GSPN_T
			|| m_eNetType == dsszmc::andl::NetType::XSPN_T
			|| m_eNetType == dsszmc::andl::NetType::CPN_T
			|| m_eNetType == dsszmc::andl::NetType::HPN_T)
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
				case dsszmc::andl::CondType::READ_T:
					w.vr_ = c->value_;
					break;
				case dsszmc::andl::CondType::INHIBITOR_T:
					w.vi_ = c->value_;
					break;
				case dsszmc::andl::CondType::EQUAL_T:
					w.vr_ = c->value_;
					w.vi_ = c->value_;
					break;
				case dsszmc::andl::CondType::MODIFIER_T:
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
				case dsszmc::andl::UpdateType::DECREASE_T:
					w.vpt_ = u->value_;
					break;
				case dsszmc::andl::UpdateType::INCREASE_T:
					w.vtp_ = u->value_;
					break;
				case dsszmc::andl::UpdateType::ASSIGN_T:
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

void SP_ImportANDL::CreateArcs()
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
}

SP_DS_Graph*
SP_ImportANDL::CreateDocument(const wxString& p_sFile, dsszmc::andl::NetType p_eType)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxString netName;
	if (m_eNetType == dsszmc::andl::NetType::PN_T)
	{
		netName = SP_DS_PN_CLASS;
	}
	else if (m_eNetType == dsszmc::andl::NetType::XPN_T)
	{
		netName = SP_DS_EXTPN_CLASS;
	}
	else if (m_eNetType == dsszmc::andl::NetType::SPN_T
			|| m_eNetType == dsszmc::andl::NetType::GSPN_T
			|| m_eNetType == dsszmc::andl::NetType::XSPN_T)
	{
		netName = SP_DS_SPN_CLASS;
	}
	else if (m_eNetType == dsszmc::andl::NetType::CPN_T)
	{
		netName = SP_DS_CONTINUOUSPED_CLASS;
	}
	else if (m_eNetType == dsszmc::andl::NetType::HPN_T)
	{
		netName = SP_DS_HYBRIDPN_CLASS;
	}
	else if (m_eNetType == dsszmc::andl::NetType::TPN_T)
	{
		netName = SP_DS_TIMEPN_CLASS;
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

void SP_ImportANDL::CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight, const wxString& type)
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
}


