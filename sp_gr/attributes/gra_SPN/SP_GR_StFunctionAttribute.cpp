//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/09/23 15:35:00 $
//////////////////////////////////////////////////////////////////////

#include "sp_gr/attributes/gra_SPN/SP_GR_StFunctionAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_gr/base/SP_GR_BaseText.h"
#include "sp_defines.h"
#include "sp_core/SP_Core.h"

#include "snoopy.h"

#include "sp_gui/windows/SP_GUI_Canvas.h"

//---------------------------------------------------------------
//
// Creation (c'tor and clone)
//
//---------------------------------------------------------------
SP_GR_StFunctionAttribute::SP_GR_StFunctionAttribute(SP_DS_Attribute* p_pcParent,
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
}

SP_GR_StFunctionAttribute::~SP_GR_StFunctionAttribute()
{
    if (m_pcPrimitive)
    {
        SP_Core::Instance()->RemoveExtern(m_pcPrimitive);
        wxDELETE(m_pcPrimitive);
    }
    m_pcPrimitive = NULL;
}

SP_Graphic*
SP_GR_StFunctionAttribute::Clone(SP_Data* p_pcParent)
{
    if (!p_pcParent || p_pcParent->GetElementType() != SP_ELEMENT_ATTRIBUTE)
        return NULL;

    SP_DS_Attribute* l_pcParent = dynamic_cast<SP_DS_Attribute*>(p_pcParent);

    SP_GR_StFunctionAttribute* l_pcNew = new SP_GR_StFunctionAttribute(l_pcParent,
        m_sFormat,
        this->GetWidth(),
        this->GetHeight());

    CloneBase(l_pcNew);

    return l_pcNew;
}

wxString
SP_GR_StFunctionAttribute::FormatText()
{

    if (!GetParent() || (GetParent()->GetElementType() != SP_ELEMENT_ATTRIBUTE))
        return wxT("");

    SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( GetParent() );

    wxString l_pchVal = l_pcColList->GetActiveCellValue( 1);


	///////////////////////////////////////////////////////////////////////////////
	//colored stochastic PN: show rate functions	
	if( l_pcColList->GetName()==wxT("FunctionList"))
	{
		//stochastic PN
		if( l_pcColList->GetCell(0,0) == wxT("Main") )
		{
			l_pchVal = l_pcColList->GetActiveCellValue( 1);
		}
		//colored stochastic PN
		else
		{
			l_pchVal=wxT("");
			for(unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{	
				wxString l_sPredicate=l_pcColList->GetCell(i,0).c_str();
				wxString l_sFunction=l_pcColList->GetCell(i,1).c_str();
				l_pchVal << l_sPredicate<<wxT(" : ")<<l_sFunction<< wxT("\n");
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////


    if ( 200 < l_pchVal.Len() )
    {
       l_pchVal[196] = ' ';
       l_pchVal[197] = '.';
       l_pchVal[198] = '.';
       l_pchVal[199] = '.';
       l_pchVal[200] = '\0';
    }

    if (m_pcPrimitive)
        m_pcPrimitive->Show(m_bShow && (l_pchVal.Len() != 0));

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
SP_GR_StFunctionAttribute::Update(bool p_bLocalOnly)
{
    if (!GetCanvas() || !m_pcPrimitive)
        return TRUE;

    wxString l_sVal = FormatText();

    m_pcPrimitive->AddText(l_sVal);
    m_pcPrimitive->CalculateDimensions(l_sVal);

    return TRUE;
}

wxShape*
SP_GR_StFunctionAttribute::GetPrimitive()
{
    return m_pcPrimitive;
}

