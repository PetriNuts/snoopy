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

#include "sp_defines.h"

#include "dssz/andl/andl_reader.h"
#include "dssz/misc/exception.h"

#include <wx/spinctrl.h>

#include <algorithm>

SP_ImportCANDL::SP_ImportCANDL()
{

}

SP_ImportCANDL::~SP_ImportCANDL()
{

}

bool
SP_ImportCANDL::ReadFile(const wxString& p_sFile)
{
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
	catch(const dsszmc::andl::parse_error& e)
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
	SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
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
}



/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////


void
SP_ImportCANDL::CreateGraph(const wxString& p_sFile, const dsszmc::andl::Net& p_Net)
{
	x = 350.0;
	y = 250.0;
	SP_LOGDEBUG(wxT("Creating Graph"));

	m_eNetType = p_Net.type_;
	m_pcGraph = CreateDocument(p_sFile, p_Net.type_);

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
}


bool
SP_ImportCANDL::CreatePlaces(const dsszmc::andl::Places& p_Places)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if(m_eNetType == dsszmc::andl::NetType::COL_CPN_T)
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);
	}
	else
	{
		nodeClass = m_pcGraph->GetNodeclass(SP_DS_DISCRETE_PLACE);
	}
	wxString colorset = wxT("Dot");
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
		wxString colorset = p->colorset_;
		l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
		l_node->GetAttribute(wxT("Name"))->SetValueString(name);
		l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
		l_pcAttr = l_node->GetAttribute(SP_DS_CPN_COLORSETNAME);
		l_pcAttr->SetValueString(colorset);
		SP_DS_ColListAttribute* l_pcColAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("MarkingList")));
		l_pcColAttr->Clear();

		l_node->ShowOnCanvas(l_pcCanvas, FALSE, x, y, 0);
		lookupPlaces[p->name_] = l_node;
	}
	return true;
}

bool
SP_ImportCANDL::CreateConst(const dsszmc::andl::Constants& p_Constants, const dsszmc::andl::Valuesets& p_Valuesets)
{
	for(auto& constant : p_Constants)
	{
		if(!constant) continue;

		if(lookupParameters.find(constant->name_) != lookupParameters.end())
		{
			continue;
			//return false;
		}
		if(lookupConstants.find(constant->name_) != lookupConstants.end())
		{
			continue;
			//return false;
		}

		wxString name = constant->name_;
		wxString type = wxT("int");
		//TODO
/*
		if(constant->type_ == dsszmc::andl::ConstType::STRING_T)
		{
			type = wxT("string");
		}
*/
		if(constant->type_ == dsszmc::andl::ConstType::DOUBLE_T
			&& (m_eNetType == dsszmc::andl::NetType::COL_SPN_T
					|| m_eNetType == dsszmc::andl::NetType::COL_CPN_T
					|| m_eNetType == dsszmc::andl::NetType::COL_HPN_T))
		{
			SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Parameter"));
			SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
			SP_DS_Node *l_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
			l_node->GetAttribute(wxT("Name"))->SetValueString(name);
			l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("ParameterList")));
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
			l_node->ShowOnCanvas(l_pcCanvas, FALSE, x , y ,0 );
			lookupParameters[constant->name_] = l_node;
		}
		else
		{
			SP_DS_Metadataclass* l_pcMetadataclass;
			l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
			if(!l_pcMetadataclass)
				return false;
			SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
			if(!l_pcNewMetadata)
				return false;
			SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
			if(!l_pcColList)
				return false;
			unsigned int l_nNewRow = l_pcColList->AppendEmptyRow();

			wxString value = wxT("0");
			if(!constant->values_.empty())
			{
				value = constant->values_.front();
			}
			l_pcColList->SetCell(l_nNewRow,0,name);
			l_pcColList->SetCell(l_nNewRow,1,type);
			l_pcColList->SetCell(l_nNewRow,2,value);
			lookupConstants[constant->name_] = l_pcNewMetadata;
		}

	}
	return true;
}

bool SP_ImportCANDL::CreateColorsets(const dsszmc::andl::Colorsets& p_Colorsets)
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

		if(lookupColorsets.find(cs->name_) != lookupColorsets.end())
		{
			continue;
			//return false;
		}

		wxString name = cs->name_;
		//TODO: add more
		wxString type = wxT("int");
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
			colors.Replace(wxT("{"), wxT("("));
			colors.Replace(wxT("}"), wxT(")"));
		}
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

bool SP_ImportCANDL::CreateVariables(const dsszmc::andl::Variables& p_Variables)
{
	for(auto& v : p_Variables)
	{
		if(!v) continue;

		if(lookupVariables.find(v->name_) != lookupVariables.end())
		{
			continue;
			//return false;
		}

		wxString name = v->name_;
		wxString type = v->colorset_;

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

bool SP_ImportCANDL::CreateFunctions(const dsszmc::andl::ColorFunctions& p_Functions)
{
	for(auto& f : p_Functions)
	{
		if(!f) continue;

		if(lookupFunctions.find(f->name_) != lookupFunctions.end())
		{
			continue;
			//return false;
		}

		wxString name = f->name_;
		wxString type = f->ret_;
		dsszmc::andl::ColorFunction::Params params = f->params_;
		wxString param;
		for(auto& p : params)
		{
			if(!param.IsEmpty())
				param << wxT(",");
			param << p.first << wxT(" ") << p.second;
		}
		wxString body = f->body_;

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

bool SP_ImportCANDL::CreateObservers(const dsszmc::andl::Observers& p_Observers)
{
	//TODO
	return true;
}

bool
SP_ImportCANDL::CreateTransitions(const dsszmc::andl::Transitions& p_Transitions)
{
	SP_DS_Nodeclass *nodeClass = NULL;
	if (m_eNetType == dsszmc::andl::NetType::COL_CPN_T)
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

		wxString guard = t->guard_;
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(SP_DS_CPN_GUARDLIST));
		l_pcAttr->SetCell(0,1,guard);

		if (m_eNetType == dsszmc::andl::NetType::COL_SPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_GSPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_XSPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_CPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_HPN_T)
		{
			wxString function = wxT("1");
			if (!t->function_.empty())
			{
				function = t->function_;
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
			wxArrayString l_Functions = wxStringTokenize(function, wxT("++"));

			for(size_t i = 0; i < l_Functions.Count(); ++i)
			{
				wxString predicate = l_Functions[i].AfterFirst('[').BeforeFirst(']');
				wxString function = l_Functions[i].AfterFirst('[');
				unsigned int l_nNewRow = l_pcAttr->AppendEmptyRow();
				l_pcAttr->SetCell(l_nNewRow,0, predicate);
				if(nodeClass->GetName() == SP_DS_DETERMINISTIC_TRANS)
				{
					l_pcAttr->SetCell(l_nNewRow,1, function);
				}
				else if(nodeClass->GetName() == SP_DS_SCHEDULED_TRANS)
				{
					wxArrayString vals = wxStringTokenize(function,  wxT(","));
					l_pcAttr->SetCell(l_nNewRow,1, vals[0]);
					l_pcAttr->SetCell(l_nNewRow,2, vals[1]);
					l_pcAttr->SetCell(l_nNewRow,3, vals[2]);
				}
				else
				{
					l_pcAttr->SetCell(l_nNewRow,1, function);
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
				case dsszmc::andl::UpdateType::DECREASE_T:
					w.vpt_ = u->value_;
					break;
				case dsszmc::andl::UpdateType::INCREASE_T:
					w.vtp_ = u->value_;
					break;
				case dsszmc::andl::UpdateType::ASSIGN_T:
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

void
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
}

SP_DS_Graph*
SP_ImportCANDL::CreateDocument(const wxString& p_sFile, dsszmc::andl::NetType p_eType)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
    wxString netName;
	if(m_eNetType == dsszmc::andl::NetType::COL_PN_T)
	{
		netName = SP_DS_COLPN_CLASS;
	}
	else if(m_eNetType == dsszmc::andl::NetType::COL_XPN_T)
	{
		netName = SP_DS_COLEPN_CLASS;
	}
	else if(m_eNetType == dsszmc::andl::NetType::COL_SPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_GSPN_T
			|| m_eNetType == dsszmc::andl::NetType::COL_XSPN_T)
	{
		netName = SP_DS_COLSPN_CLASS;
	}
	else if(m_eNetType == dsszmc::andl::NetType::COL_CPN_T)
	{
		netName = SP_DS_COLCPN_CLASS;
	}
	else if(m_eNetType == dsszmc::andl::NetType::COL_HPN_T)
	{
		netName = SP_DS_COLHPN_CLASS;
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

void
SP_ImportCANDL::CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight, const wxString& type)
{
	CHECK_POINTER(source, SP_LOGDEBUG(wxString(wxT("source is NULL"))));
	CHECK_POINTER(target, SP_LOGDEBUG(wxString(wxT("target is NULL"))));

		SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(type);
		SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
		SP_DS_Edge *l_edge =  l_pcEC->GetPrototype()->Clone();
		l_edge->SetNetnumber(l_pcCanvas->GetNetnumber());
		l_edge->SetNodes(source,target);
		l_edge->SetNodes(*(source->GetGraphics()->begin()),*(target->GetGraphics()->begin()));
		SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_edge->GetAttribute(SP_DS_CPN_INSCRIPTION));
		if(l_pcAttr)
			l_pcAttr->SetCell(0,1,weight);
		l_pcEC->AddElement(l_edge);
		l_edge->ShowOnCanvas(l_pcCanvas, FALSE);
		CHECK_POINTER(l_edge->GetGraphics(),SP_LOGDEBUG(wxT("no graphics")));

}


