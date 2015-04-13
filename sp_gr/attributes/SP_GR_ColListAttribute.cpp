//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: Textattribute implementation
//////////////////////////////////////////////////////////////////////
#include "sp_gr/attributes/SP_GR_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_GPR_Elements.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_ColListAttribute::SP_GR_ColListAttribute(SP_DS_Attribute* p_pcParent,
        const wxString& p_pchFormat,
        double p_nWidth,
        double p_nHeight)
:SP_GR_Attribute(p_pcParent),
m_pcPrimitive(NULL),
m_sFormat(p_pchFormat)
{
    // instance for displaying on the canvas
    m_pcPrimitive = new SP_GR_BaseText(FormatText(), p_nWidth, p_nHeight);

    SP_Core::Instance()->AssociateExtern(m_pcPrimitive, this);

    SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( p_pcParent );
	m_sPropertyName = l_pcColList->GetName();
	SP_Data* l_pcDataNode = l_pcColList->GetParent();

	if( l_pcDataNode )
	{
		if(l_pcDataNode->GetClassObject() )
		{
			m_sNodeClassName = l_pcDataNode->GetClassObject()->GetName();

			if(l_pcDataNode->GetClassObject()->GetParentGraph())
			{
				if(l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass())
				{
					m_sNetClassName = l_pcDataNode->GetClassObject()->GetParentGraph()->GetNetclass()->GetName();
				}
			}
		}
	}
}

SP_GR_ColListAttribute::~SP_GR_ColListAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_ColListAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_ColListAttribute* l_pcNew = new SP_GR_ColListAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_ColListAttribute::FormatText()
{
    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");

    wxString l_pchVal = wxEmptyString;
    SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( GetParent() );
    ////////////////////////////////////////////////////////////////////////////
	// add marking for colored Petri Nets
	if(m_sPropertyName == SP_DS_CPN_MARKINGLIST )
	{
		unsigned l_nActiveColumn = l_pcColList->GetActiveColumn();
		l_pchVal=wxT("");
		for(unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{		
			wxString l_sMarking = l_pcColList->GetCell(i,l_nActiveColumn*2);
			bool l_bNull = (l_sMarking != wxT(""));
			wxString l_nTokenNumber = l_pcColList->GetCell(i,l_nActiveColumn*2+1);
			if( !(l_nTokenNumber ==wxT ("0") || l_nTokenNumber == wxT("")) && l_bNull )
			{
				if(i != 0)
					l_pchVal << wxT("++\n");
				l_pchVal << l_pcColList->GetCell(i,l_nActiveColumn*2+1)
						 << wxT("`") << l_pcColList->GetCell(i,l_nActiveColumn*2);
			}
		}
		//liu, 19.05.2011
		if(l_pchVal.Len() > 100)
		{
			l_pchVal = l_pchVal.Mid(0,99) + wxT("...");
		}
	}
	//////////////////////////////////////////////////////////////////////////////////
	else
	////////////////////////////////////////////////////////////////////////////
	// Let the inscriptions show correctly
	if(m_sPropertyName == SP_DS_CPN_INSCRIPTION )
	{
		l_pchVal = wxT("");
		wxString l_sInscription = l_pcColList->GetCell(0,1);

		l_sInscription.Replace(wxT("++"), wxT("?"));
		while(l_sInscription.Find((wxT('?'))) != wxNOT_FOUND)
		{							
			l_pchVal << l_sInscription.BeforeFirst(wxT('?'));
			l_pchVal << wxT("++\n");
			l_sInscription = l_sInscription.AfterFirst(wxT('?'));			
		}
		l_pchVal << l_sInscription;
				
	}
	//////////////////////////////////////////////////////////////////////////////////
	else
	///////////////////////////////////////////////////////////////////////////////
	//colored stochastic PN: show rate functions	
	if( m_sPropertyName == wxT("FunctionList")||
		m_sPropertyName == wxT("DelayList") )
	{
		l_pchVal = wxT("");
		//stochastic PN
		if( l_pcColList->GetCell(0,0) == wxT("Main") )
		{
			 for(unsigned int i = 1; i < l_pcColList->GetColCount(); i++)
			{
    			if(i>1)
    				l_pchVal << wxT(",");
    			l_pchVal << l_pcColList->GetActiveCellValue(i);
			}
		}
		//colored stochastic PN
		else
		{
			for(unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{	
				wxString l_sPredicate = l_pcColList->GetCell(i,0);
				wxString l_sFunction = l_pcColList->GetCell(i,1);
				l_pchVal << l_sPredicate << wxT(" : ") << l_sFunction;
				if(i < l_pcColList->GetRowCount()-1)
					l_pchVal << wxT("\n");

			}
		}
	}
	else
	//colored stochastic PN: show rate functions	
	if( m_sPropertyName == wxT("PeriodicList") )
	{
		//stochastic PN
		l_pchVal = wxT("");
		if( l_pcColList->GetCell(0,0) == wxT("Main") )
		{
			 for(unsigned int i = 1; i < l_pcColList->GetColCount(); i++)
			{
    			if(i>1)
    				l_pchVal << wxT(",");
    			l_pchVal << l_pcColList->GetActiveCellValue(i);
			}
		}
		//colored stochastic PN
		else
		{
			for(unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{	
				wxString l_sPredicate = l_pcColList->GetCell(i,0);
				wxString l_sFunction = l_pcColList->GetCell(i,1)
						+ wxT(",") + l_pcColList->GetCell(i,2)
						+ wxT(",") + l_pcColList->GetCell(i,3);
				l_pchVal << l_sPredicate << wxT(" : ") <<  l_sFunction;
				if(i < l_pcColList->GetRowCount()-1)
					l_pchVal << wxT("\n");

			}
		}
	}
	else
	{
		if(l_pcColList->GetColCount() == 2)
		{
			l_pchVal << l_pcColList->GetCell(0, 1);
			for(unsigned int i = 1; i < l_pcColList->GetRowCount(); i++)
			{
				l_pchVal << wxT(":");
				l_pchVal << l_pcColList->GetCell(i,1);
			}
		}
		else
		{
			for(unsigned int i = 1; i < l_pcColList->GetColCount(); i++)
			{
				if(i>1)
					l_pchVal << wxT(",");
				l_pchVal << l_pcColList->GetActiveCellValue(i);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////

	if( m_sPropertyName == SP_DS_CPN_INSCRIPTION
		&& l_pcColList->GetCell(0,1).Find(wxT("dot")) != wxNOT_FOUND )
	{
		bool l_bDotExpressionShowOption;	
		l_bDotExpressionShowOption = wxGetApp().GetElementPrefs()->GetExpressionShowOption( m_sNetClassName );

		if (m_pcPrimitive)
			m_pcPrimitive->Show(m_bShow && (wxStrlen(l_pchVal) != 0) && l_bDotExpressionShowOption );		
	}
	else
	{	
		if (m_pcPrimitive)
			m_pcPrimitive->Show(m_bShow && (wxStrlen(l_pchVal) != 0));
	}

    wxString l_sVal = m_sFormat;
    l_sVal.Replace(wxT("%"), l_pchVal);

    return l_sVal;
}

//---------------------------------------------------------------
//
// Accessors
//
//---------------------------------------------------------------
bool
SP_GR_ColListAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    wxString l_sVal = FormatText();

	wxString l_sTextColor;

	if ((m_sNetClassName.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLEPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
			m_sNetClassName.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 ))
	{
		if( m_sPropertyName == SP_DS_CPN_MARKINGLIST)
		{
			if(m_sNetClassName.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 )
				l_sTextColor = wxGetApp().GetElementPrefs()->GetMarkingColor(m_sNetClassName,wxT("Continuous Place"));
			else
				l_sTextColor = wxGetApp().GetElementPrefs()->GetMarkingColor(m_sNetClassName,m_sNodeClassName);
		}
		if ( m_sPropertyName == SP_DS_CPN_GUARDLIST)
		{
			if(m_sNetClassName.CmpNoCase(SP_DS_COLCPN_CLASS) == 0 )
				l_sTextColor = wxGetApp().GetElementPrefs()->GetGuardColor(m_sNetClassName,wxT("Continuous Transition"));
			else
				l_sTextColor = wxGetApp().GetElementPrefs()->GetGuardColor(m_sNetClassName,m_sNodeClassName);
		}
		if ( m_sPropertyName == SP_DS_CPN_INSCRIPTION)
		{
			l_sTextColor = wxGetApp().GetElementPrefs()->GetExpressionColor(m_sNetClassName);
		}
	}

	if( l_sTextColor == wxT(""))
		l_sTextColor = m_sTextColor;

	m_pcPrimitive->SetTextColour(l_sTextColor);
	m_pcPrimitive->AddText(l_sVal);
	m_pcPrimitive->CalculateDimensions(l_sVal);

    return TRUE;
}

wxShape*
SP_GR_ColListAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

