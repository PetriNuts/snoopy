//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Elements.cpp
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global elements preferences
//////////////////////////////////////////////////////////////////////

#include "sp_core/SP_GPR_Elements.h"

#include "sp_revision.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"

#include "sp_gui/windows/SP_GUI_Childframe.h"

#include <wx/spinctrl.h>
#include <wx/checkbox.h>

#include <wx/clrpicker.h>

enum
{
	SP_ID_SPINCTRL_WIDTH, SP_ID_SPINCTRL_HEIGHT
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SP_GPR_Elements::SP_GPR_Elements()
{
	m_ListBoxValues.Clear();
	m_ListBoxValues.Add(wxT("undirected edge"));
	m_ListBoxValues.Add(wxT("directed edge"));

   //Nodestyle
   m_asListBoxValues.Clear();
/* changed by ckruege2 17.06.2009 */
   m_asListBoxValues.Add(wxT("solid line"));
   m_asListBoxValues.Add(wxT("double solid line"));
   m_asListBoxValues.Add(wxT("dotted line"));
/*
GTK have problems with both designs.to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic, SP_GR_ExtendedEdge, shapes/SP_GR_Extended*, SP_GPR_Elements
*/
#ifndef linux
   m_asListBoxValues.Add(wxT("dotted-dashed line"));
   m_asListBoxValues.Add(wxT("dashed line"));
#endif
/* changing end */

	m_LineListBoxValue.Clear();
	m_LineListBoxValue.Add(wxT("solid line"));
    m_LineListBoxValue.Add(wxT("dotted line"));
/* 
GTK have problems with both designs.
to approve tests with newer GTK version
the ifdefs are in following classes
SP_WDG_DialogExtendedGraphic
SP_GR_ExtendedEdge
shapes/SP_GR_Extended*
SP_GPR_Elements
*/
#ifndef linux
    m_LineListBoxValue.Add(wxT("dotted-dashed line"));
    m_LineListBoxValue.Add(wxT("dashed line"));
#endif

	m_ARListBoxValue.Clear();
	m_ARListBoxValue.Add(wxT("None"));
    m_ARListBoxValue.Add(wxT("Empty Circle"));
    m_ARListBoxValue.Add(wxT("Filled Circle"));
    m_ARListBoxValue.Add(wxT("Arrow"));
    m_ARListBoxValue.Add(wxT("Single Oblique"));
    m_ARListBoxValue.Add(wxT("Double Oblique"));

	m_ALListBoxValue.Clear();
	m_ALListBoxValue.Add(wxT("None"));
    m_ALListBoxValue.Add(wxT("Empty Circle"));
    m_ALListBoxValue.Add(wxT("Filled Circle"));
    m_ALListBoxValue.Add(wxT("Arrow"));
    m_ALListBoxValue.Add(wxT("Double Arrow"));
    m_ALListBoxValue.Add(wxT("Double Circle"));
    m_ALListBoxValue.Add(wxT("Circle Arrow"));
}

SP_GPR_Elements::~SP_GPR_Elements()
{
}

//////////////////////////////////////////////////////////////////////
// Setter / Getter
//////////////////////////////////////////////////////////////////////

void SP_GPR_Elements::SetNodeWidth(const wxString& p_sNetClass, const wxString& p_sNodeClass, int p_nVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			if (p_nVal >= NODE_DIM_MIN && p_nVal <= NODE_DIM_MAX)
			{
				m_mnWidth[l_sKey] = p_nVal;
			}
			else
				m_mnWidth[l_sKey] = DEFAULT_WIDTH;
		}
	}
}

void SP_GPR_Elements::SetColorSetColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			if( wxTheColourDatabase->Find(p_sVal).Ok() )
				m_msColorSetColor[l_sKey] = p_sVal;
			else
				m_msColorSetColor[l_sKey] = wxT("BROWN");
		}
	}
}

wxString SP_GPR_Elements::GetColorSetColor(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{
	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sNodeClass.c_str());
	map<wxString, wxString>::iterator l_it = m_msColorSetColor.find(l_sKey);
	if(l_it != m_msColorSetColor.end())
	{
		if( wxTheColourDatabase->Find(l_it->second).Ok() )
			return l_it->second;		
	}

	return wxT("BROWN");

}



void SP_GPR_Elements::SetMarkingColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			if( wxTheColourDatabase->Find(p_sVal).Ok() )
				m_msMarkingColor[l_sKey] = p_sVal;
			else
				m_msMarkingColor[l_sKey] = wxT("RED");
		}
	}
}
wxString SP_GPR_Elements::GetMarkingColor(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{
	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sNodeClass.c_str());
	map<wxString, wxString>::iterator l_it = m_msMarkingColor.find(l_sKey);
	if(l_it != m_msMarkingColor.end())
	{
		if( wxTheColourDatabase->Find(l_it->second).Ok() )
			return l_it->second;		
	}

	return wxT("RED");
}

void SP_GPR_Elements::SetGuardColor(const wxString& p_sNetClass, const wxString& p_sNodeClass, wxString p_sVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			if( wxTheColourDatabase->Find(p_sVal).Ok() )
				m_msGuardColor[l_sKey] = p_sVal;
			else
				m_msGuardColor[l_sKey] = wxT("PURPLE");
		}
	}
}

wxString SP_GPR_Elements::GetGuardColor(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{
	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sNodeClass.c_str());
	map<wxString, wxString>::iterator l_it = m_msGuardColor.find(l_sKey);
	if(l_it != m_msGuardColor.end())
	{
		if( wxTheColourDatabase->Find(l_it->second).Ok() )
			return l_it->second;		
	}

	return wxT("PURPLE");
}




void SP_GPR_Elements::SetExpressionColor(const wxString& p_sNetClass, wxString p_sVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if( wxTheColourDatabase->Find(p_sVal).Ok() )
		{
			m_msExpressionColor[p_sNetClass] = p_sVal;
		}
		else
			m_msExpressionColor[p_sNetClass] = wxT("BLUE");
	}

}

wxString SP_GPR_Elements::GetExpressionColor(const wxString& p_sNetClass)
{
	map<wxString, wxString>::iterator l_it = m_msExpressionColor.find(p_sNetClass);
	if(l_it != m_msExpressionColor.end())
	{
		//first to last
		if( wxTheColourDatabase->Find(l_it->second).Ok() )	
			return l_it->second;
		
	}
	//last
	return wxT("BLUE");

}


bool SP_GPR_Elements::GetExpressionShowOption(const wxString& p_sNetClass)
{	
	SP_MapString2Bool::iterator l_it = m_mbDotExpressionShow.find(p_sNetClass);
	if(l_it != m_mbDotExpressionShow.end())
	{
		return l_it->second;
	}

	return true;
}


void SP_GPR_Elements::SetExpressionShowOption(const wxString& p_sNetClass, bool p_bVal)
{	
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{		
		m_mbDotExpressionShow[p_sNetClass] = p_bVal;
		
	}
	
}


void SP_GPR_Elements::SetNodeHeight(const wxString& p_sNetClass, const wxString& p_sNodeClass, int p_nVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			if (p_nVal >= NODE_DIM_MIN && p_nVal <= NODE_DIM_MAX)
			{
				m_mnHeight[l_sKey] = p_nVal;
			}
			else
				m_mnHeight[l_sKey] = DEFAULT_HEIGHT;
		}
	}
}

void SP_GPR_Elements::SetNodeFixed(const wxString& p_sNetClass, const wxString& p_sNodeClass, bool p_bVal)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (((*itNC).second).find(p_sNodeClass) != ((*itNC).second).end())
		{
			m_mbFixedSize[l_sKey] = p_bVal;
		}
	}
}


void SP_GPR_Elements::SetNodeDesignType(const wxString& p_sNetClass, SP_EXTENDED_TYPE p_nVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (p_nVal > 0)
		{
			m_mnDesignType[p_sNetClass] = p_nVal;
		}
		else
			// from thick auf default, find no pos who use this function
			m_mnDesignType[p_sNetClass] = SP_EXTENDED_TYPE_DEFAULT;
	}
}
void SP_GPR_Elements::SetLineEdgeDesignType(const wxString& p_sNetClass, SP_EXTENDED_TYPE_EDGE p_nVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (p_nVal > 0)
		{
			m_mnLineDesignType[p_sNetClass] = p_nVal;
		}
		else
			m_mnLineDesignType[p_sNetClass] = SP_EXTENDED_TYPE_DEFAULT_EDGE;
	}
}
void SP_GPR_Elements::SetArrowEdgeTypeR(const wxString& p_sNetClass, SP_EXTENDED_ARROW_TYPE_EDGE p_nVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (p_nVal > 0)
		{
			m_mnARLineDesignType[p_sNetClass] = p_nVal;
		}
		else
			m_mnARLineDesignType[p_sNetClass] = SP_EXTENDED_ARROW_TYPE_NONE_EDGE;
	}
}
void SP_GPR_Elements::SetArrowEdgeTypeL(const wxString& p_sNetClass, SP_EXTENDED_ARROW_TYPE_EDGE p_nVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (p_nVal > 0)
		{
			m_mnALLineDesignType[p_sNetClass] = p_nVal;
		}
		else
			m_mnALLineDesignType[p_sNetClass] = SP_EXTENDED_ARROW_TYPE_NONE_EDGE;
	}
}
void SP_GPR_Elements::SetEdgeDesignType(const wxString& p_sNetClass, SP_EXTENDED_ARC_TYPE p_nVal)
{
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC != m_mlClassNodes.end())
	{
		if (p_nVal > 0)
		{
			m_edgeDesignType = p_nVal;
		}
		else
			m_edgeDesignType = SP_EXTENDED_TYPE_UNDIRECTED;
	}

}

void SP_GPR_Elements::SetEdgeSpline(const wxString& p_sNetClass, bool p_bVal)
{
	m_mbEdgeSpline[p_sNetClass] = p_bVal;
}

int SP_GPR_Elements::GetNodeWidth(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{

	wxString l_sKey = wxString::Format(wxT("%s|%s"), p_sNetClass.c_str(), p_sNodeClass.c_str());
	SP_MapString2Int::iterator l_it = m_mnWidth.find(l_sKey);
	if(l_it != m_mnWidth.end())
	{
		if (l_it->second >= NODE_DIM_MIN)
		{
			return l_it->second;
		}
	}

	return DEFAULT_WIDTH;
}

int SP_GPR_Elements::GetNodeHeight(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	SP_MapString2Int::iterator l_it = m_mnHeight.find(l_sKey);
	if(l_it != m_mnHeight.end())
	{
		if (l_it->second >= NODE_DIM_MIN)
		{
			return l_it->second;
		}
	}

	return DEFAULT_HEIGHT;
}

bool SP_GPR_Elements::GetNodeFixed(const wxString& p_sNetClass, const wxString& p_sNodeClass)
{
	wxString l_sKey = p_sNetClass + wxT("|") + p_sNodeClass;
	SP_MapString2Bool::iterator l_it = m_mbFixedSize.find(l_sKey);
	if(l_it != m_mbFixedSize.end())
	{
		return l_it->second;
	}

	return DEFAULT_FIXED;
}


SP_EXTENDED_TYPE SP_GPR_Elements::GetNodeDesignType(const wxString& p_sNetClass)
{
	map<wxString, SP_EXTENDED_TYPE>::iterator l_it = m_mnDesignType.find(p_sNetClass);
	if(l_it != m_mnDesignType.end())
	{
		if (l_it->second > SP_EXTENDED_TYPE_DEFAULT)
		{
			return l_it->second;
		}
	}
   //last index...
	return SP_EXTENDED_TYPE_DASHED;
}
SP_EXTENDED_TYPE_EDGE SP_GPR_Elements::GetLineEdgeDesignType(const wxString& p_sNetClass)
{
	map<wxString, SP_EXTENDED_TYPE_EDGE>::iterator l_it = m_mnLineDesignType.find(p_sNetClass);
	if(l_it != m_mnLineDesignType.end())
	{
		//first to last
		if (l_it->second > SP_EXTENDED_TYPE_DEFAULT_EDGE)
		{
			return l_it->second;
		}
	}
	//last
	return SP_EXTENDED_TYPE_DEFAULT_EDGE;
}

SP_EXTENDED_ARROW_TYPE_EDGE SP_GPR_Elements::GetArrowEdgeTypeR(const wxString& p_sNetClass)
{
	map<wxString, SP_EXTENDED_ARROW_TYPE_EDGE>::iterator l_it = m_mnARLineDesignType.find(p_sNetClass);
	if(l_it != m_mnARLineDesignType.end())
	{
		//first to last
		if (l_it->second > SP_EXTENDED_ARROW_TYPE_NONE_EDGE)
		{
			return l_it->second;
		}
	}
	//last
	return SP_EXTENDED_ARROW_TYPE_NONE_EDGE;
}
SP_EXTENDED_ARROW_TYPE_EDGE SP_GPR_Elements::GetArrowEdgeTypeL(const wxString& p_sNetClass)
{
	map<wxString, SP_EXTENDED_ARROW_TYPE_EDGE>::iterator l_it = m_mnALLineDesignType.find(p_sNetClass);
	if(l_it != m_mnALLineDesignType.end())
	{
		//first to last
		if (l_it->second > SP_EXTENDED_ARROW_TYPE_NONE_EDGE)
		{
			return l_it->second;
		}
	}
	//last
	return SP_EXTENDED_ARROW_TYPE_NONE_EDGE;
}
SP_EXTENDED_ARC_TYPE SP_GPR_Elements::GetEdgeDesignType()
{

	if (m_edgeDesignType > SP_EXTENDED_TYPE_UNDIRECTED)
	{
		return m_edgeDesignType;
	}
	else
	{
		return SP_EXTENDED_TYPE_UNDIRECTED;
	}

}

bool SP_GPR_Elements::GetEdgeSpline(const wxString& p_sNetClass)
{
	bool l_bReturn = false;
	SP_MapString2Bool::iterator l_it = m_mbEdgeSpline.find(p_sNetClass);
	if (l_it != m_mbEdgeSpline.end())
		l_bReturn = l_it->second;
	return l_bReturn;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void SP_GPR_Elements::RegisterNodeclass(const wxString& p_sNetClass, const wxString& p_sNodeclass)
{

	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC == m_mlClassNodes.end())
	{
		m_mlClassNodes[p_sNetClass].insert(p_sNodeclass);
	}
	else
	{
		((*itNC).second).insert(p_sNodeclass);
	}

}

bool SP_GPR_Elements::AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage)
{

	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	//Todo: refactoring of this method is recommend!

	wxBoxSizer *helpSizer, *helpSizer2;
	wxStaticBoxSizer *l_pcNodeSectionSizer;

	SP_SetString::iterator itN;
	//get editable nodes for wxT("p_sNetClass") from map
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);
	wxString shapes[] =
	{ wxT("Rectangle"), wxT("Square"), wxT("H.Bar"), wxT("V.Bar") };
	wxString brushes[] =
	{ wxT("Solid gray"), wxT("Cross hatched") };
	wxString edgestyles[] =
	{ wxT("Line"), wxT("Spline") };

	//clear controls lists
	m_mscHeight.clear();
	m_mscWidth.clear();
	m_mcbFixed.clear();
	m_mcpColorSet.clear();
	m_mcpMarking.clear();
	m_mcpGuard.clear();
	m_mcpExpression.clear();
	m_mcbDotExpressionShow.clear();


	//add some space at top
	l_pcSizer->Add(0, 20, 0);

	//the following three net classes will appear like this...
	//you can add your new net class to this list,
	//or you can create a new else-if-branch if you need a different appearance
	if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_REACHABILITY_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_MTBDD_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_TIMEPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0||
		p_sNetClass.CmpNoCase(SP_DS_MTIDD_CLASS) == 0 ||
		p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0 )
	{

		for (itN = ((*itNC).second).begin(); itN != ((*itNC).second).end(); ++itN)
		{
			const wxString l_rString = *itN;
			l_pcNodeSectionSizer = new wxStaticBoxSizer(new wxStaticBox(p_pcPage, -1, *itN), wxVERTICAL);
			int SP_ID_WIDTH = -1;
			int SP_ID_HEIGHT = -1;

			//there are pre-defined shapes for transitions which are added here
			if ((p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			    p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0) &&
				l_rString.CmpNoCase(wxT("Transition")) == 0)
			{
				helpSizer = new wxBoxSizer(wxHORIZONTAL);
				m_rbTransShape = new wxRadioBox(p_pcPage, SP_ID_RADIOBOX_TRANSSHAPE, wxT("Predefined shapes"), wxDefaultPosition, wxDefaultSize, 4, shapes, 1, wxRA_SPECIFY_ROWS);
				helpSizer->Add(m_rbTransShape, 1);
				l_pcNodeSectionSizer->Add(helpSizer, 0, wxEXPAND | wxALL, 5);

				SP_ID_WIDTH = SP_ID_SPINCTRL_TRANSWIDTH;
				SP_ID_HEIGHT = SP_ID_SPINCTRL_TRANSHEIGHT;
			}
			

			helpSizer2 = new wxBoxSizer(wxHORIZONTAL);
			helpSizer = new wxBoxSizer(wxVERTICAL);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Width (pt):")), 0, wxALL, 4);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Height (pt):")), 0, wxALL, 4);
			helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Fixed Size:")), 0, wxALL, 4);

			if ((p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||			    
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ) &&
				l_rString.Find(wxT("Place")) != wxNOT_FOUND ) 
			{
				helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Color Set:")), 0, wxALL, 4);
				helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Marking:")), 0, wxALL, 4);
			}
			if ((p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||			    
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ) &&
				l_rString.Find(wxT("Transition")) != wxNOT_FOUND )				
			{
				helpSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Guard:")), 0, wxALL, 4);				
			}

			helpSizer2->Add(helpSizer, 0, wxLEFT | wxTOP, 5);

			helpSizer = new wxBoxSizer(wxVERTICAL);
			m_mscWidth[*itN] = new wxSpinCtrl(p_pcPage, SP_ID_WIDTH, wxString::Format(wxT("%d"), GetNodeWidth(p_sNetClass, *itN)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, NODE_DIM_MIN, NODE_DIM_MAX, GetNodeWidth(p_sNetClass, *itN));
			helpSizer->Add(m_mscWidth[*itN], 0, wxLEFT | wxRIGHT, 5);
			m_mscHeight[*itN] = new wxSpinCtrl(p_pcPage, SP_ID_HEIGHT, wxString::Format(wxT("%d"), GetNodeHeight(p_sNetClass, *itN)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, NODE_DIM_MIN, NODE_DIM_MAX, GetNodeHeight(p_sNetClass, *itN));
			helpSizer->Add(m_mscHeight[*itN], 0, wxLEFT | wxRIGHT, 5);
			wxCheckBox* l_pcCheckBox = new wxCheckBox(p_pcPage, -1, wxT(""));
			l_pcCheckBox->SetValue(GetNodeFixed(p_sNetClass, *itN));
			m_mcbFixed[*itN] = l_pcCheckBox;
			helpSizer->Add(m_mcbFixed[*itN], 0, wxLEFT | wxRIGHT, 5);

			if ((p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||			    
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ) &&
				l_rString.Find(wxT("Place")) != wxNOT_FOUND) 
			{
				wxColour l_ColorSetColor = wxColour( GetColorSetColor(p_sNetClass, *itN) );			

				wxColourPickerCtrl* l_pcColourPickerCtrl = new wxColourPickerCtrl(p_pcPage,  -1, l_ColorSetColor,
																wxDefaultPosition, wxDefaultSize,wxCLRP_DEFAULT_STYLE);
				m_mcpColorSet[*itN] = l_pcColourPickerCtrl;
				helpSizer->Add(m_mcpColorSet[*itN], 0, wxLEFT | wxRIGHT, 5);

				l_ColorSetColor = wxColour( GetMarkingColor(p_sNetClass, *itN) );	
				l_pcColourPickerCtrl = new wxColourPickerCtrl(p_pcPage, -1 , l_ColorSetColor,
																wxDefaultPosition, wxDefaultSize,wxCLRP_DEFAULT_STYLE);
				m_mcpMarking[*itN] = l_pcColourPickerCtrl;
				helpSizer->Add(m_mcpMarking[*itN], 0, wxLEFT | wxRIGHT, 5);
			}

			if ((p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||			    
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ) &&
				l_rString.Find(wxT("Transition")) != wxNOT_FOUND)				
			{
				wxColour l_GuardColor = wxColour( GetGuardColor(p_sNetClass, *itN) );	

				wxColourPickerCtrl* l_pcColourPickerCtrl = new wxColourPickerCtrl(p_pcPage,  -1, l_GuardColor,
																wxDefaultPosition, wxDefaultSize,wxCLRP_DEFAULT_STYLE);
				m_mcpGuard[*itN] = l_pcColourPickerCtrl;
				helpSizer->Add(m_mcpGuard[*itN], 0, wxLEFT | wxRIGHT, 5);
			}


			//in continuous petri nets places must be circles
			if (l_rString.CmpNoCase(SP_DS_CONTINUOUS_PLACE) == 0)
			{
				m_mscHeight[*itN]->Enable(false);
			}

			helpSizer2->Add(helpSizer, 1, wxALL, 5);
			l_pcNodeSectionSizer->Add(helpSizer2, 0, wxEXPAND | wxALL, 5);

			l_pcSizer->Add(l_pcNodeSectionSizer, 0, wxEXPAND | wxALL, 5);
		}

		if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
		{
			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Edge Design:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

			SP_EXTENDED_ARC_TYPE l_nType = GetEdgeDesignType();
			m_lbListBox = new wxListBox(p_pcPage, -1);
			m_lbListBox->InsertItems(m_ListBoxValues, 0);
			if (m_ListBoxValues.GetCount() > 0)
			{
				if (m_ListBoxValues.GetCount() >= (unsigned long) l_nType)
				{
					m_lbListBox->SetSelection((int) l_nType);
				}
				else
				{
					m_lbListBox->SetSelection((int) m_ListBoxValues.GetCount() - 1);
				}
			}
			l_pcSizer->Add(m_lbListBox, 0, wxEXPAND | wxALL, 5);

		}

		if (p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0)
		{
			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Node Design:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

			SP_EXTENDED_TYPE l_nType = GetNodeDesignType(p_sNetClass);

			m_lbNodeListBox = new wxListBox(p_pcPage, -1);
			m_lbNodeListBox->InsertItems(m_asListBoxValues, 0);
			if (m_asListBoxValues.GetCount() > 0)
			{
				if (m_asListBoxValues.GetCount() >= (unsigned long) l_nType)
				{
					m_lbNodeListBox->SetSelection((int) l_nType - 1);
				}
				else
				{
					m_lbNodeListBox->SetSelection((int) m_asListBoxValues.GetCount() - 1);
				}
			}
			l_pcSizer->Add(m_lbNodeListBox, 0, wxEXPAND | wxALL, 5);
		}

		/*Add ckruge2 20.07.09 */
		if (p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0
			)
		{
			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Line Design for Edges:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			SP_EXTENDED_TYPE_EDGE l_nType = GetLineEdgeDesignType(p_sNetClass);
			m_LineListBox = new wxListBox(p_pcPage, -1);
			m_LineListBox->InsertItems(m_LineListBoxValue, 0);
			if (m_LineListBoxValue.GetCount() > 0)
			{
				if (m_LineListBoxValue.GetCount() >= (unsigned long) l_nType)
				{
					m_LineListBox->SetSelection((int) l_nType - 1);
				}
				else
				{
					m_LineListBox->SetSelection((int) m_LineListBoxValue.GetCount() - 1);
				}
			}
			l_pcSizer->Add(m_LineListBox, 0, wxEXPAND | wxALL, 5);
		}
		if (p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0)
		{
			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Arrows:\n From - To:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			SP_EXTENDED_ARROW_TYPE_EDGE l_nType1 = GetArrowEdgeTypeR(p_sNetClass);
			m_ARListBox = new wxListBox(p_pcPage, -1);
			m_ARListBox->InsertItems(m_ARListBoxValue, 0);
			if (m_ARListBoxValue.GetCount() > 0)
			{
				if (m_ARListBoxValue.GetCount() >= (unsigned long) l_nType1)
				{
					m_ARListBox->SetSelection((int) l_nType1 - 1);
				}
				else
				{
					m_ARListBox->SetSelection((int) m_ARListBoxValue.GetCount() - 1);
				}
			}
			l_pcSizer->Add(m_ARListBox, 0, wxEXPAND | wxALL, 5);
			SP_EXTENDED_ARROW_TYPE_EDGE l_nType2 = GetArrowEdgeTypeL(p_sNetClass);
			m_ALListBox = new wxListBox(p_pcPage, -1);
			m_ALListBox->InsertItems(m_ALListBoxValue, 0);
			if (m_ALListBoxValue.GetCount() > 0)
			{
				if (m_ALListBoxValue.GetCount() >= (unsigned long) l_nType2)
				{
					m_ALListBox->SetSelection((int) l_nType2 - 1);
				}
				else
				{
					m_ALListBox->SetSelection((int) m_ALListBoxValue.GetCount() - 1);
				}
			}
			l_pcSizer->Add(m_ALListBox, 0, wxEXPAND | wxALL, 5);
		}

		//add option for choosing logical brush
		//TODO: if other brush/pen properties are to be modified in future development
		//			an extra tab might be more suitable
		if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0||
			p_sNetClass.CmpNoCase(SP_DS_MTBDD_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_TIMEPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_MTIDD_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0)
		{
			m_rbLogicBrush = new wxRadioBox(p_pcPage, -1, wxT("Logical Nodes (affecting all net classes)"), wxDefaultPosition, wxDefaultSize, 2, brushes, 1, wxRA_SPECIFY_ROWS);
			if (wxGetApp().GetCanvasLogicBrush()->GetStyle() == wxBRUSHSTYLE_CROSSDIAG_HATCH)
			{
				m_rbLogicBrush->SetSelection(1);
			}
			else
			{
				m_rbLogicBrush->SetSelection(0);
			}
			l_pcSizer->Add(m_rbLogicBrush, 0, wxEXPAND | wxALL, 5);
		}
		else
		{
			m_rbLogicBrush = NULL;
		}
		//Choose Edgestyle wxT("Line" oder "Spline")
		l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Edge")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
		m_rbEdgeSpline = new wxRadioBox(p_pcPage, -1, wxT("Choose Edge Style"), wxDefaultPosition, wxDefaultSize, 2, edgestyles, 1, wxRA_SPECIFY_ROWS);
		m_rbEdgeSpline->SetSelection(GetEdgeSpline(p_sNetClass));
		l_pcSizer->Add(m_rbEdgeSpline, 0, wxEXPAND | wxALL, 5);

		//////////for colored PN
		if ((p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||			    
				p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
				p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ) ) 
		{
			wxColour l_ColorSetColor = wxColour( GetExpressionColor(p_sNetClass) );	
			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, _T("Expression:")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			wxColourPickerCtrl* l_pcExpressionColourPickerCtrl = new wxColourPickerCtrl(p_pcPage,  -1, l_ColorSetColor,
																wxDefaultPosition, wxDefaultSize,wxCLRP_DEFAULT_STYLE);

			m_mcpExpression[p_sNetClass] = l_pcExpressionColourPickerCtrl;
			
			l_pcSizer->Add(l_pcExpressionColourPickerCtrl, 0, wxLEFT | wxRIGHT, 5);

			l_pcSizer->Add(new wxStaticText(p_pcPage, -1, wxT("Dot Expression Show:")), 0, wxALL, 4);
			wxCheckBox* l_pcCheckBox = new wxCheckBox(p_pcPage, -1, wxT(""));
			l_pcCheckBox->SetValue(GetExpressionShowOption(p_sNetClass));
			m_mcbDotExpressionShow[p_sNetClass] = l_pcCheckBox;
			l_pcSizer->Add(l_pcCheckBox, 0, wxLEFT | wxRIGHT, 5);
		}

		/////////////////////////////////////////////////////////

	}
	else /*DEFAULT*/
	{
		l_pcSizer->Add(new wxStaticText(p_pcPage, -1, wxT("No editable nodes for this class.")), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	}

	p_pcPage->AddControl(l_pcSizer,0,wxEXPAND);

	return true;
}

bool SP_GPR_Elements::OnDialogOk(const wxString& p_sNetClass)
{
	if (!p_sNetClass)
		return false;

	SP_SetString::iterator itN;
	//get editable nodes for <p_sNetClass> from map
	map<wxString, SP_SetString>::iterator itNC = m_mlClassNodes.find(p_sNetClass);

	if (itNC == m_mlClassNodes.end())
		return false;

	if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) != 0
		&& p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) != 0
		&& p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) != 0)
	{
		for (itN = ((*itNC).second).begin(); itN != ((*itNC).second).end(); ++itN)
		{
			SetNodeWidth(p_sNetClass, *itN, m_mscWidth[*itN]->GetValue());
			SetNodeHeight(p_sNetClass, *itN, m_mscHeight[*itN]->GetValue());
			SetNodeFixed(p_sNetClass, *itN, m_mcbFixed[*itN]->GetValue());

			if(m_mcpColorSet[*itN])
			{
				wxColour l_SelColour = m_mcpColorSet[*itN]->GetColour();
				wxString l_sColor = l_SelColour.GetAsString();			
				wxTheColourDatabase->AddColour(l_sColor, l_SelColour);
				SetColorSetColor(p_sNetClass, *itN, l_sColor);
			}
			if(m_mcpMarking[*itN])
			{		
				wxColour l_SelColour = m_mcpMarking[*itN]->GetColour();
				wxString l_sColor = l_SelColour.GetAsString();			
				wxTheColourDatabase->AddColour(l_sColor, l_SelColour);
				SetMarkingColor(p_sNetClass, *itN, l_sColor);
			}
			if(m_mcpGuard[*itN])
			{
				wxColour l_SelColour = m_mcpGuard[*itN]->GetColour();
				wxString l_sColor = l_SelColour.GetAsString();			
				wxTheColourDatabase->AddColour(l_sColor, l_SelColour);
				SetGuardColor(p_sNetClass, *itN, l_sColor);
			}
		}
		if (p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0)
		{
			SetNodeDesignType(p_sNetClass, (SP_EXTENDED_TYPE) (m_lbNodeListBox->GetSelection() + 1));
		}
		if (p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0
			)
		{
			SetLineEdgeDesignType(p_sNetClass, (SP_EXTENDED_TYPE_EDGE) (m_LineListBox->GetSelection() ));
		}
	}
	else
	{
		if(p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0) {
			for (itN = ((*itNC).second).begin(); itN != ((*itNC).second).end(); ++itN)
			{
				SetNodeWidth(p_sNetClass, *itN, m_mscWidth[*itN]->GetValue());
				SetNodeHeight(p_sNetClass, *itN, m_mscHeight[*itN]->GetValue());
				SetNodeFixed(p_sNetClass, *itN, m_mcbFixed[*itN]->GetValue());
			}
			SetLineEdgeDesignType(p_sNetClass, (SP_EXTENDED_TYPE_EDGE) (m_LineListBox->GetSelection() ));
			SetArrowEdgeTypeR(p_sNetClass, (SP_EXTENDED_ARROW_TYPE_EDGE) (m_ARListBox->GetSelection() ));
			SetArrowEdgeTypeL(p_sNetClass, (SP_EXTENDED_ARROW_TYPE_EDGE) (m_ALListBox->GetSelection() ));
		}
		else
		{
			for (itN = ((*itNC).second).begin(); itN != ((*itNC).second).end(); ++itN)
			{
				SetNodeWidth(p_sNetClass, *itN, m_mscWidth[*itN]->GetValue());
				SetNodeHeight(p_sNetClass, *itN, m_mscHeight[*itN]->GetValue());
				SetNodeFixed(p_sNetClass, *itN, m_mcbFixed[*itN]->GetValue());
			}
			SetEdgeDesignType(p_sNetClass, (SP_EXTENDED_ARC_TYPE) (m_lbListBox->GetSelection()));
		}

	}
	SetEdgeSpline(p_sNetClass, m_rbEdgeSpline->GetSelection());

	if( m_mcpExpression[p_sNetClass])
	{
		wxColour l_SelColour = m_mcpExpression[p_sNetClass]->GetColour();
		wxString l_sColor = l_SelColour.GetAsString();			
		wxTheColourDatabase->AddColour(l_sColor, l_SelColour);
		SetExpressionColor(p_sNetClass, l_sColor);
	}

	if( m_mcbDotExpressionShow[p_sNetClass])
	{
		SetExpressionShowOption(p_sNetClass, m_mcbDotExpressionShow[p_sNetClass]->GetValue());
	}

	if (m_rbLogicBrush)
	{
		switch (m_rbLogicBrush->GetSelection())
		{
		case 0://Solid gray
			wxGetApp().SetCanvasLogicBrush(wxSOLID);
			break;
		case 1://cross hatched
			wxGetApp().SetCanvasLogicBrush(wxCROSSDIAG_HATCH);
			break;
		}
	}

	return true;
}

void SP_GPR_Elements::OnUpdateTransShape()
{

	switch (m_rbTransShape->GetSelection())
	{
	case 1://Square
		m_mscHeight[wxT("Transition")]->SetValue(m_mscWidth[wxT("Transition")]->GetValue());
		break;
	case 2://H.Bar
		m_mscHeight[wxT("Transition")]->SetValue(int(m_mscWidth[wxT("Transition")]->GetValue() / BAR_RATIO));
		break;
	case 3://V.Bar
		m_mscWidth[wxT("Transition")]->SetValue(int(m_mscHeight[wxT("Transition")]->GetValue() / BAR_RATIO));
		break;
	}
}

void SP_GPR_Elements::OnUpdateTransWidth()
{

	switch (m_rbTransShape->GetSelection())
	{
	case 1://Square
		m_mscHeight[wxT("Transition")]->SetValue(m_mscWidth[wxT("Transition")]->GetValue());
		break;
	case 2://H.Bar
		m_mscHeight[wxT("Transition")]->SetValue(int(m_mscWidth[wxT("Transition")]->GetValue() / BAR_RATIO));
		break;
	case 3://V.Bar
		m_mscHeight[wxT("Transition")]->SetValue(m_mscWidth[wxT("Transition")]->GetValue() * BAR_RATIO);
		break;
	}
}

void SP_GPR_Elements::OnUpdateTransHeight()
{

	switch (m_rbTransShape->GetSelection())
	{
	case 1://Square
		m_mscWidth[wxT("Transition")]->SetValue(m_mscHeight[wxT("Transition")]->GetValue());
		break;
	case 2://H.Bar
		m_mscWidth[wxT("Transition")]->SetValue(m_mscHeight[wxT("Transition")]->GetValue() * BAR_RATIO);
		break;
	case 3://V.Bar
		m_mscWidth[wxT("Transition")]->SetValue(int(m_mscHeight[wxT("Transition")]->GetValue() / BAR_RATIO));
		break;
	}
}

bool SP_GPR_Elements::UpdateFromConfig(wxConfig& p_config)
{
	//temps for reading values from config
	map<wxString, SP_SetString>::iterator itCN;
	SP_SetString::iterator itN;
	wxString l_sCurrentClass, l_sCurrentNode, l_sKey;
	int tempInt;
	wxString tempString;

	for (itCN = m_mlClassNodes.begin(); itCN != m_mlClassNodes.end(); ++itCN)
	{
		l_sCurrentClass = (*itCN).first;

		for (itN = ((*itCN).second).begin(); itN != ((*itCN).second).end(); ++itN)
		{
			l_sCurrentNode = (*itN);
			l_sKey = wxString::Format(wxT("%s|%s"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str());

			p_config.Read(wxString::Format(wxT("%s//%s_Width"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), &tempInt, DEFAULT_WIDTH);
			m_mnWidth[l_sKey] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//%s_Height"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), &tempInt, DEFAULT_HEIGHT);
			m_mnHeight[l_sKey] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//%s_Fixed"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), &tempInt, DEFAULT_FIXED);
			m_mbFixedSize[l_sKey] = tempInt;

			if(l_sCurrentClass.Find(wxT("Colored")) != wxNOT_FOUND)
			{
				if( l_sCurrentNode.Find(wxT("Place")) != wxNOT_FOUND )
				{
					p_config.Read(wxString::Format(wxT("%s//%s_ColorSetColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  &tempString);
					m_msColorSetColor[l_sKey] = tempString;
					p_config.Read(wxString::Format(wxT("%s//%s_MarkingColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  &tempString);
					m_msMarkingColor[l_sKey] = tempString;
				}
				if(l_sCurrentNode.Find(wxT("Transition")) != wxNOT_FOUND)
				{
					p_config.Read(wxString::Format(wxT("%s//%s_GuardColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  &tempString);
					m_msGuardColor[l_sKey] = tempString;
				}
			}
		}

		p_config.Read(wxString::Format(wxT("%s//DesignType"), l_sCurrentClass.c_str()), &tempInt, SP_EXTENDED_TYPE_DEFAULT);
		m_mnDesignType[l_sCurrentClass] = (SP_EXTENDED_TYPE) tempInt;

		p_config.Read(wxString::Format(wxT("%s//LineDesignType"), l_sCurrentClass.c_str()), &tempInt, SP_EXTENDED_TYPE_DEFAULT_EDGE);
		m_mnLineDesignType[l_sCurrentClass] = (SP_EXTENDED_TYPE_EDGE) tempInt;
		p_config.Read(wxString::Format(wxT("%s//ARLineDesignType"), l_sCurrentClass.c_str()), &tempInt, SP_EXTENDED_ARROW_TYPE_NONE_EDGE);
		m_mnARLineDesignType[l_sCurrentClass] = (SP_EXTENDED_ARROW_TYPE_EDGE) tempInt;
		p_config.Read(wxString::Format(wxT("%s//ALLineDesignType"), l_sCurrentClass.c_str()), &tempInt, SP_EXTENDED_ARROW_TYPE_NONE_EDGE);
		m_mnALLineDesignType[l_sCurrentClass] = (SP_EXTENDED_ARROW_TYPE_EDGE) tempInt;

		p_config.Read(wxString::Format(wxT("%s//EdgeDesignType"), l_sCurrentClass.c_str()), &tempInt, SP_EXTENDED_TYPE_UNDIRECTED);
		m_edgeDesignType = (SP_EXTENDED_ARC_TYPE) tempInt;

		p_config.Read(wxString::Format(wxT("%s//EdgeSpline"), l_sCurrentClass.c_str()), &tempInt, 0);
		m_mbEdgeSpline[l_sCurrentClass] = (bool) tempInt;

		if(l_sCurrentClass.Find(wxT("Colored")) != wxNOT_FOUND)
		{
			p_config.Read(wxString::Format(wxT("%s//ExpressionColor"), l_sCurrentClass.c_str()), &tempString);
			m_msExpressionColor[l_sCurrentClass] = tempString;

			p_config.Read(wxString::Format(wxT("%s//DotExpressionShow"), l_sCurrentClass.c_str()), &tempInt);
			m_mbDotExpressionShow[l_sCurrentClass] = tempInt;
		}
	}

	return true;
}

bool SP_GPR_Elements::WriteToConfig(wxConfig& p_config)
{
	//temps for reading values from config
	map<wxString, SP_SetString>::iterator itCN;
	SP_SetString::iterator itN;
	wxString l_sCurrentClass, l_sCurrentNode, l_sKey;

	for (itCN = m_mlClassNodes.begin(); itCN != m_mlClassNodes.end(); ++itCN)
	{
		l_sCurrentClass = (*itCN).first;

		for (itN = ((*itCN).second).begin(); itN != ((*itCN).second).end(); ++itN)
		{
			l_sCurrentNode = (*itN);
			l_sKey = wxString::Format(wxT("%s|%s"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str());

			p_config.Write(wxString::Format(wxT("%s//%s_Width"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), (long) m_mnWidth[l_sKey]);
			p_config.Write(wxString::Format(wxT("%s//%s_Height"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), (long) m_mnHeight[l_sKey]);
			p_config.Write(wxString::Format(wxT("%s//%s_Fixed"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()), (long) m_mbFixedSize[l_sKey]);

			if(l_sCurrentClass.Find(wxT("Colored")) != wxNOT_FOUND)
			{
				if( l_sCurrentNode.Find(wxT("Place")) != wxNOT_FOUND )
				{
					p_config.Write(wxString::Format(wxT("%s//%s_ColorSetColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  m_msColorSetColor[l_sKey]);
					p_config.Write(wxString::Format(wxT("%s//%s_MarkingColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  m_msMarkingColor[l_sKey]);
				}
				if(l_sCurrentNode.Find(wxT("Transition")) != wxNOT_FOUND)
				{
					p_config.Write(wxString::Format(wxT("%s//%s_GuardColor"), l_sCurrentClass.c_str(), l_sCurrentNode.c_str()),  m_msGuardColor[l_sKey]);
				}
			}
		}

		p_config.Write(wxString::Format(wxT("%s//DesignType"), l_sCurrentClass.c_str()), (long) m_mnDesignType[l_sCurrentClass]);

		p_config.Write(wxString::Format(wxT("%s//LineDesignType"), l_sCurrentClass.c_str()), (long) m_mnLineDesignType[l_sCurrentClass]);
		p_config.Write(wxString::Format(wxT("%s//ARLineDesignType"), l_sCurrentClass.c_str()), (long) m_mnARLineDesignType[l_sCurrentClass]);
		p_config.Write(wxString::Format(wxT("%s//ALLineDesignType"), l_sCurrentClass.c_str()), (long) m_mnALLineDesignType[l_sCurrentClass]);

		p_config.Write(wxString::Format(wxT("%s//EdgeDesignType"), l_sCurrentClass.c_str()), (long) m_edgeDesignType);

		p_config.Write(wxString::Format(wxT("%s//EdgeSpline"), l_sCurrentClass.c_str()), (long) m_mbEdgeSpline[l_sCurrentClass]);

		if(l_sCurrentClass.Find(wxT("Colored")) != wxNOT_FOUND)
		{
			p_config.Write(wxString::Format(wxT("%s//ExpressionColor"), l_sCurrentClass.c_str()), m_msExpressionColor[l_sCurrentClass]);

			p_config.Write(wxString::Format(wxT("%s//DotExpressionShow"), l_sCurrentClass.c_str()), m_mbDotExpressionShow[l_sCurrentClass]);
		}
	}

	return true;
}
