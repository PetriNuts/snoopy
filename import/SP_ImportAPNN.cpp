//////////////////////////////////////////////////////////////////////
// $Author:ms$
// $Version: 0.1 $
// $Date: 2007/07/12 $
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportAPNN.h"
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

#include <wx/spinctrl.h>

#include <algorithm>

////////////////////////////////Data Structures//////////////////////////////////////////////7

SP_ImportAPNN::PlaceAPNNImp::PlaceAPNNImp(unsigned long p_id, const wxString& p_name, const wxString& p_marking, double x, double y) :
		id(p_id),
		m_name(p_name),
		m_marking(p_marking),
		m_node(NULL),
		m_x(x),
		m_y(y)
{
}

SP_ImportAPNN::ConstAPNNImp::ConstAPNNImp(unsigned long p_id, const wxString& p_name, const wxString& p_value, double x, double y) :
		id(p_id),
		m_name(p_name),
		m_value(p_value),
		m_node(NULL),
		m_x(x),
		m_y(y)
{
}

SP_ImportAPNN::TransitionAPNNImp::TransitionAPNNImp(unsigned long t_id, const wxString& t_name, const wxString& t_function, double x, double y) :
		id(t_id),
		m_name(t_name),
		m_function(t_function),
		m_node(NULL),
		m_x(x),
		m_y(y)
{
}

SP_ImportAPNN::ArcAPNNImp::ArcAPNNImp(const wxString& a_from, const wxString& a_to, const wxString& a_weight, const wxString& a_type) :
		fromID(a_from),
		toID(a_to),
		weight(a_weight),
		type(a_type)
{
}

void SP_ImportAPNN::ClearPlaceIds()
{
	PlaceIdentifierMap::const_iterator pIt;
	PlaceAPNNImp *p;
	for (pIt = m_placeIds.begin(); pIt != m_placeIds.end(); pIt++)
	{
		p = (*pIt).second;
		//SP_LOGDEBUG(wxString::Format(wxT("deleting place: %s"), p->m_name));
		wxDELETE(p);
	}
	m_placeIds.clear();
}
void SP_ImportAPNN::ClearConstIds()
{
	ConstIdentifierMap::const_iterator pIt;
	ConstAPNNImp *p;
	for (pIt = m_constIds.begin(); pIt != m_constIds.end(); pIt++)
	{
		p = (*pIt).second;
		//SP_LOGDEBUG(wxString::Format(wxT("deleting const: %s"), p->m_name));
		wxDELETE(p);
	}
	m_constIds.clear();
}
void SP_ImportAPNN::ClearTransitionIds()
{
	TransitionIdentifierMap::const_iterator tIt;
	TransitionAPNNImp *t;
	for (tIt = m_transitionIds.begin(); tIt != m_transitionIds.end(); tIt++)
	{
		t = (*tIt).second;
		//SP_LOGDEBUG(wxString::Format(wxT("deleting transition: %s"), t->m_name));
		wxDELETE(t);
	}
	m_transitionIds.clear();
}
void SP_ImportAPNN::ClearArcList()
{
	ArcList::const_iterator aIt;
	ArcAPNNImp *a;
	for (aIt = m_arcs.begin(); aIt != m_arcs.end(); aIt++)
	{
		a = (*aIt);
		//SP_LOGDEBUG(wxString::Format(wxT("deleting arc from : %s to %s"), a->fromID, a->toID));
		wxDELETE(a);
	}
	m_arcs.clear();
}
void SP_ImportAPNN::ClearAll()
{
	ClearPlaceIds();
	ClearConstIds();
	ClearTransitionIds();
	ClearArcList();
}

SP_ImportAPNN::SP_ImportAPNN() :
		m_eNetType(SIMPLE_PT)
{

}

SP_ImportAPNN::~SP_ImportAPNN()
{

}

bool SP_ImportAPNN::Close()
{
	m_fFile.Close();
	if (m_fFile.IsOpened())
	{
		return false;
	}
	return true;
}

bool SP_ImportAPNN::Open(const wxString& p_sFile)
{
	m_fFile.Open(p_sFile.c_str());
	if (m_fFile.IsOpened())
	{
		return true;
	}
	return false;
}
bool SP_ImportAPNN::ReadFile(const wxString& p_sFile)
{
	m_eNetType = SIMPLE_PT;
	wxString input;
	Open(p_sFile);
	m_fFile.ReadAll(&input);
	wxStringTokenizer st(input, wxT(" {}\n\r\t"), wxTOKEN_STRTOK);
	stz = &st;
	//while(stz.HasMoreTokens()){
	//	SP_LOGDEBUG(stz.GetNextToken());
	//}

	if (parse())
	{
		CreateGraph(p_sFile);
		doLayout();
		ClearAll();
		Close();
	}
	else
	{
		SP_MESSAGEBOX(wxT("Error while parsing  ") + p_sFile + wxT(": ") + errorString);
		ClearAll();
		Close();
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////Parsing APNN/////////////////////////////////////////////////////////

bool SP_ImportAPNN::parse()
{
	wxString placeT = wxT("\\place");
	places = 0;
	consts = 0;
	transitions = 0;
	arcs = 0;
	SP_LOGDEBUG(wxString(wxT("parsing")));
	if (stz->HasMoreTokens())
	{
		currentToken = stz->GetNextToken();
	}
	else
	{
		return FALSE;
	}

	if (currentToken.Cmp(wxT("\\beginnet")) == 0)
	{
		if (stz->HasMoreTokens())
		{
			netName = stz->GetNextToken();
			SP_LOGDEBUG(netName);
		}
		else
		{
			return FALSE;
		}
	}
	currentToken = stz->GetNextToken();

	while (currentToken.Cmp(wxT("")))
	{
		SP_LOGDEBUG(currentToken);
		if (currentToken.Cmp(wxT("\\place")) == 0)
		{
			SP_LOGDEBUG(currentToken);
			if (!parsePlace())
				return FALSE;
		}
		else if (currentToken.Cmp(wxT("\\const")) == 0)
		{
			if (!parseConst())
				return FALSE;
		}
		else if (currentToken.Cmp(wxT("\\transition")) == 0)
		{
			if (!parseTransition())
				return FALSE;
		}
		else if (currentToken.Cmp(wxT("\\arc")) == 0)
		{
			if (!parseArc())
				return FALSE;
		}
		else if (currentToken.Cmp(wxT("\\endnet")) == 0)
		{
			return TRUE;

		}
		else
		{
			return FALSE;
		}
	} //while
	return TRUE;
} //parse

bool SP_ImportAPNN::parsePlace()
{
	SP_LOGDEBUG(wxString(wxT("parsePlace")));
	wxString name = wxT("P_") + wxString::Format(wxT("lu%"), places);
	wxString init = wxT("0");
	wxString cap = wxT("0");
	wxString value = wxT("0");
	double x = 0.0;
	double y = 0.0;
	wxString id;

	if (stz->HasMoreTokens())
	{
		id = stz->GetNextToken();
	}
	else
	{
		return FALSE;
	}
	while (stz->HasMoreTokens())
	{
		currentToken = stz->GetNextToken();
		if (currentToken.Cmp(wxT("\\name")) == 0)
		{
			if (CheckValue())
			{
				name = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for name in place ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\init")) == 0)
		{
			if (CheckValue())
			{
				init = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for init in place ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\capacity")) == 0)
		{
			if (CheckValue())
			{
				cap = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for capacity in place ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\coords")) == 0)
		{
			if (CheckValue())
			{
				currentToken.ToDouble(&x);
				if (CheckValue())
				{
					currentToken.ToDouble(&y);
				}
				else
				{
					errorString = wxT("missing value for coord y in place ") + id;
					return FALSE;
				}
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for coord x in place ") + id;
				return FALSE;
			}
		}
		else
		{
			PlaceAPNNImp* p = new PlaceAPNNImp(places, name, init, x, y);
			places++;
			m_placeIds[id] = p;
			return TRUE;
		}

	}
	errorString = name;
	return FALSE;
}

bool SP_ImportAPNN::parseConst()
{
	SP_LOGDEBUG(wxString(wxT("parseConst")));
	wxString name = wxT("");
	wxString type = wxT("");
	wxString value = wxT("0");
	double x = 10.0;
	double y = 10.0;
	wxString id = wxT("C_") + wxString::Format(wxT("lu%"), consts);

	/*
	 if(stz->HasMoreTokens()){
	 id = stz->GetNextToken();

	 }else{
	 return FALSE;
	 }
	 */
	while (stz->HasMoreTokens())
	{
		currentToken = stz->GetNextToken();
		if (currentToken.Cmp(wxT("\\name")) == 0)
		{
			if (CheckValue())
			{
				name = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for name in const ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\type")) == 0)
		{
			if (CheckValue())
			{
				type = currentToken;
				SP_LOGDEBUG(currentToken + name);

			}
			else
			{
				errorString = wxT("missing value for type in const ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\value")) == 0)
		{
			if (CheckValue())
			{
				value = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for value in const ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\coords")) == 0)
		{
			if (CheckValue())
			{
				currentToken.ToDouble(&x);
				if (CheckValue())
				{
					currentToken.ToDouble(&y);
				}
				else
				{
					errorString = wxT("missing value for coord y in const ") + id;
					return FALSE;
				}
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for coord x in const ") + id;
				return FALSE;
			}
		}
		else
		{
			ConstAPNNImp* p = new ConstAPNNImp(consts, name, value, x, y);
			consts++;
			m_constIds[id] = p;
			return TRUE;
		}
	}
	errorString = name;
	return FALSE;
}

bool SP_ImportAPNN::parseTransition()
{
	SP_LOGDEBUG(wxString(wxT("parseTransition")));
	wxString name = wxT("T_") + wxString::Format(wxT("lu%"), transitions);
	wxString function = wxEmptyString;
	double x = 0.0;
	double y = 0.0;
	wxString id;

	if (stz->HasMoreTokens())
	{
		id = stz->GetNextToken();

	}
	else
	{
		return FALSE;
	}
	while (stz->HasMoreTokens())
	{
		currentToken = stz->GetNextToken();
		SP_LOGDEBUG(currentToken);

		if (currentToken.Cmp(wxT("\\name")) == 0)
		{
			if (CheckValue())
			{
				name = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for name in transition ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\function")) == 0)
		{
			if (CheckValue())
			{
				m_eNetType = STOCHASTIC_PT;
				function = currentToken;
				SP_LOGDEBUG(currentToken + function);
			}
			else
			{
				errorString = wxT("missing value for name in transition ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\coords")) == 0)
		{
			if (CheckValue())
			{
				currentToken.ToDouble(&x);
				if (CheckValue())
				{
					currentToken.ToDouble(&y);
				}
				else
				{
					errorString = wxT("missing value for coord y in transition ") + id;
					return FALSE;
				}
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for coord x in transition ") + id;
				return FALSE;
			}
		}
		else
		{
			SP_LOGDEBUG(wxString(wxT("transition finished currentToken")) + currentToken);

			transitions++;
			TransitionAPNNImp* t = new TransitionAPNNImp(transitions, name, function, x, y);
			m_transitionIds[id] = t;
			return TRUE;
		}
	}
	errorString = name;
	return FALSE;

}

bool SP_ImportAPNN::parseArc()
{
	SP_LOGDEBUG(wxString(wxT("parseArc")));
	wxString name;
	wxString from = wxT("");
	wxString to = wxT("");
	wxString weight = wxT("1");
	wxString type = wxT("Edge");
	wxString id;

	if (stz->HasMoreTokens())
	{
		id = stz->GetNextToken();
	}
	else
	{
		errorString = wxT("missing identifier for arc");
		return FALSE;
	}
	while (stz->HasMoreTokens())
	{
		currentToken = stz->GetNextToken();
		if (currentToken.Cmp(wxT("\\from")) == 0)
		{
			if (CheckValue())
			{
				from = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for from in arc ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\to")) == 0)
		{
			if (CheckValue())
			{
				to = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for to in arc ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\weight")) == 0)
		{
			if (CheckValue())
			{
				weight = currentToken;
				SP_LOGDEBUG(currentToken + name);
			}
			else
			{
				errorString = wxT("missing value for multiplicity in arc ") + id;
				return FALSE;
			}
		}
		else if (currentToken.Cmp(wxT("\\type")) == 0)
		{
			if (CheckValue())
			{
				wxString val = currentToken;
				if (val.Cmp(wxT("inhibitor")) == 0)
				{
					type = wxT("Inhibitor Edge");
				}
				else if (val.Cmp(wxT("readarc")) == 0)
				{
					type = wxT("Read Edge");
				}
				else if (val.Cmp(wxT("resetarc")) == 0)
				{
					type = wxT("Reset Edge");
				}
				else if (val.Cmp(wxT("equalarc")) == 0)
				{
					type = wxT("Equal Edge");
				}
			}
			else
			{
				errorString = wxT("wrong or missing type of arc ") + id;
				return FALSE;
			}
		}
		else
		{
			SP_LOGDEBUG(wxString(wxT("arc finished currentToken")) + currentToken);
			if (from.Cmp(wxT("")) == 0 || to.Cmp(wxT("")) == 0)
			{
				return FALSE;
			}
			if (m_placeIds.find(from) != m_placeIds.end())
			{
				if (m_transitionIds.find(to) == m_transitionIds.end())
				{
					errorString = wxT("unknown target ") + to + wxT("in arc ") + id;
					return FALSE;
				}
			}
			else if (m_transitionIds.find(from) != m_transitionIds.end())
			{
				if (m_placeIds.find(to) == m_placeIds.end())
				{
					errorString = wxT("unknown target ") + to + wxT(" in arc ") + id;
					return FALSE;
				}
			} //*/
			arcs++;

			ArcAPNNImp* a = new ArcAPNNImp(from, to, weight, type);

			m_arcs.push_back(a);
			if (m_eNetType == SIMPLE_PT)
			{
				if (a->type.Cmp(wxT("Edge")))
				{
					m_eNetType = EXTENDED_PT;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

bool SP_ImportAPNN::CheckValue()
{
	if (!stz->HasMoreTokens())
		return FALSE;
	currentToken = stz->GetNextToken();
	if (currentToken.IsSameAs(wxT("\\name")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\function")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\init")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\capacity")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\weight")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\value")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\type")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\from")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\to")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\place")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\arc")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\transition")))
		return FALSE;
	if (currentToken.IsSameAs(wxT("\\endnet")))
		return FALSE;
	return TRUE;
}

void SP_ImportAPNN::doLayout()
{
	SP_DLG_LayoutProperties* l_pcDlg = new SP_DLG_LayoutProperties(NULL, m_pcMyDoc);
	l_pcDlg->ShowModal();
	l_pcDlg->Destroy();
}

/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////

void SP_ImportAPNN::CreateGraph(const wxString& p_sFile)
{
	x = 350.0;
	y = 250.0;
	SP_LOGDEBUG(wxT("Creating Graph"));
	m_pcGraph = CreateDocument(p_sFile);

	CreateConst();
	CreatePlaces();
	CreateTransitions();
	CreateArcs();

}

void SP_ImportAPNN::CreatePlaces()
{
	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Place"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_node;
	PlaceIdentifierMap::const_iterator pIt;
	PlaceAPNNImp *p;
	for (pIt = m_placeIds.begin(); pIt != m_placeIds.end(); pIt++)
	{
		p = (*pIt).second;
		if (p)
		{
			p->m_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
			if (p->m_node)
			{
				l_node = p->m_node;
				l_node->GetAttribute(wxT("Name"))->SetValueString(p->m_name);
				l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
				l_node->GetAttribute(wxT("Marking"))->SetValueString(p->m_marking);

				l_node->ShowOnCanvas(l_pcCanvas, FALSE, p->m_x, p->m_y, 0);
			}
		}
	}

}
void SP_ImportAPNN::CreateConst()
{

	SP_DS_Metadataclass* mc = m_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	SP_DS_Metadata* l_constant;
	ConstIdentifierMap::const_iterator pIt;
	ConstAPNNImp *p;
	for (pIt = m_constIds.begin(); pIt != m_constIds.end(); pIt++)
	{
		p = (*pIt).second;
		if (p)
		{
			l_constant = mc->NewElement(1);
			l_constant->GetAttribute(wxT("Name"))->SetValueString(p->m_name);
			l_constant->GetAttribute(wxT("Group"))->SetValueString(wxT("all"));
			if (m_eNetType == SIMPLE_PT || m_eNetType == EXTENDED_PT)
			{
				l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("int"));
			}
			if (m_eNetType == STOCHASTIC_PT)
			{
				l_constant->GetAttribute(wxT("Type"))->SetValueString(wxT("double"));
			}
			SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_constant->GetAttribute(wxT("ValueList")));
			l_pcAttr->SetCell(0, 1, p->m_value);
		}
	}
}
void SP_ImportAPNN::CreateTransitions()
{
	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Transition"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_node;
	TransitionIdentifierMap::const_iterator tIt;
	TransitionAPNNImp *t;
	for (tIt = m_transitionIds.begin(); tIt != m_transitionIds.end(); tIt++)
	{
		t = (*tIt).second;
		if (t)
		{
			t->m_node = nodeClass->NewElement(l_pcCanvas->GetNetnumber());
			if (t->m_node)
			{
				l_node = t->m_node;
				l_node->GetAttribute(wxT("Name"))->SetValueString(t->m_name);
				l_node->GetAttribute(wxT("Name"))->SetShow(TRUE);
				if (m_eNetType == STOCHASTIC_PT)
				{
					SP_DS_ColListAttribute* l_pcAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_node->GetAttribute(wxT("FunctionList")));
					l_pcAttr->SetCell(0, 1, t->m_function);
				}
				l_node->ShowOnCanvas(l_pcCanvas, FALSE, t->m_x, t->m_y, 0);
			}
		}
	}
}
void SP_ImportAPNN::CreateArcs()
{
	ArcList::const_iterator aIt;
	PlaceIdentifierMap::const_iterator pIt;
	TransitionIdentifierMap::const_iterator tIt;
	ArcAPNNImp *a;
	SP_DS_Node *from;
	SP_DS_Node *to;

	for (aIt = m_arcs.begin(); aIt != m_arcs.end(); aIt++)
	{
		a = (*aIt);
		from = NULL;
		to = NULL;

		pIt = m_placeIds.find(a->fromID);
		tIt = m_transitionIds.find(a->fromID);
		if (pIt != m_placeIds.end())
		{
			from = pIt->second->m_node;
			tIt = m_transitionIds.find(a->toID);
			if (tIt != m_transitionIds.end())
			{
				to = tIt->second->m_node;
			}
		}
		else if (tIt != m_transitionIds.end())
		{
			from = tIt->second->m_node;
			pIt = m_placeIds.find(a->toID);
			if (pIt != m_placeIds.end())
			{
				to = pIt->second->m_node;
			}
		}
		if (from && to)
		{
			CreateEdge(from, to, a->weight, a->type);
		}
		else
		{
			SP_LOGDEBUG(wxT("from or to is NULL"));
		}
	}
}

SP_DS_Graph*
SP_ImportAPNN::CreateDocument(const wxString& p_sFile)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxString netName;
	if (m_eNetType == SIMPLE_PT)
	{
		netName = SP_DS_PN_CLASS;
	}
	else if (m_eNetType == EXTENDED_PT)
	{
		netName = SP_DS_EXTPN_CLASS;
	}
	else if (m_eNetType == STOCHASTIC_PT)
	{
		netName = SP_DS_SPN_CLASS;
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

void SP_ImportAPNN::CreateEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& weight, const wxString& type)
{
	CHECK_POINTER(source, SP_LOGDEBUG(wxString(wxT("source is NULL"))));
	CHECK_POINTER(target, SP_LOGDEBUG(wxString(wxT("target is NULL"))));

	SP_DS_Edgeclass* l_pcEC = m_pcGraph->GetEdgeclass(type);
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Edge *l_edge = l_pcEC->GetPrototype()->Clone();
	l_edge->SetNetnumber(l_pcCanvas->GetNetnumber());
	l_edge->SetNodes(source, target);
	l_edge->SetNodes(*(source->GetGraphics()->begin()), *(target->GetGraphics()->begin()));

	l_edge->GetAttribute(wxT("Multiplicity"))->SetValueString(weight);
	l_pcEC->AddElement(l_edge);
	l_edge->ShowOnCanvas(l_pcCanvas, FALSE);
	CHECK_POINTER(l_edge->GetGraphics(), SP_LOGDEBUG(wxT("no graphics")));

}

