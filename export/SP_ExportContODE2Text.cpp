/*
 * SP_ExportContODE2Text.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 12.8.2010
 * Short Description:
 */
 //==============================================================================

#include "export/SP_ExportContODE2Text.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_PlaceODE.h"

SP_ExportContODE2Text::SP_ExportContODE2Text(): SP_ExportPT()
{

}

SP_ExportContODE2Text::~SP_ExportContODE2Text()
{

}

bool SP_ExportContODE2Text::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_CONTINUOUSPED_CLASS || cName ==SP_DS_HYBRIDPN_CLASS);
}
bool SP_ExportContODE2Text::DoWrite()
{

	    SP_DS_PlaceODE* l_pcPlaceODEConstructor;
		wxString l_sResult = wxT("");
	    const SP_ListNode *l_places=m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();
	    SP_ListNode::const_iterator l_itPlace;

	    l_pcPlaceODEConstructor=new SP_DS_PlaceODE(m_graph);

	    if(!l_pcPlaceODEConstructor->Initialize())
		{
			SP_MESSAGEBOX(wxT("Error: Can not Initialize Place ODEs"));
			return false;
		}

	    long l_nPlacePos=0;
	    //Iterate for all the places
	    for(l_itPlace=l_places->begin();l_itPlace!=l_places->end();l_itPlace++)
		{
			wxString l_sName=(dynamic_cast<SP_DS_NameAttribute*>((*l_itPlace)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)))->GetValue();
			l_sResult<<wxT("d")<<l_sName<<wxT("/dt = ");
			l_sResult <<l_pcPlaceODEConstructor->ConstructODE(l_nPlacePos)<<wxT("\n\n");

			l_nPlacePos++;
		}
		m_file.Write(l_sResult);

		wxDELETE(l_pcPlaceODEConstructor);

		return true;
}

