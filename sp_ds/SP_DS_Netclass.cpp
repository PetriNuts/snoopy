//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/mdi/SP_MDI_CoarseView.h"

#include "sp_defines.h"

SP_DS_Netclass::SP_DS_Netclass(const wxString& p_pchName)
:SP_Type(SP_ELEMENT_NETCLASS),
 SP_Name(p_pchName),
 m_pcDocTemplate(NULL)
{
	m_sExtension = wxT("net");
}

SP_DS_Netclass::~SP_DS_Netclass()
{
}

SP_DS_Graph*
SP_DS_Netclass::CreateGraph(SP_DS_Graph* p_pcGraph)
{
	CHECK_POINTER(p_pcGraph, return NULL);

    p_pcGraph->SetName(GetName());

    return p_pcGraph;
}

bool
SP_DS_Netclass::AddToGui(SP_GM_Docmanager* p_pcDocmanager)
{
    CHECK_POINTER(p_pcDocmanager, return FALSE);

    if(m_pcDocTemplate) return FALSE;


    m_pcDocTemplate = new SP_GM_DocTemplate(p_pcDocmanager,
        this, GetName(), GetFilter(), wxT(""), GetExtension(),
        GetName() + wxT(" Document"), GetName() + wxT(" View"),
        CLASSINFO(SP_MDI_Doc), CLASSINFO(SP_MDI_View));


    return TRUE;
}

bool
SP_DS_Netclass::CreateCoarse()
{
    CHECK_POINTER(m_pcDocTemplate, return FALSE);
    CHECK_POINTER(m_pcDocTemplate->GetDocumentManager(), return FALSE);

    SP_GM_Docmanager* l_pcDM = dynamic_cast<SP_GM_Docmanager*>(
        m_pcDocTemplate->GetDocumentManager());

    CHECK_POINTER(l_pcDM, return FALSE);

    if (!l_pcDM->HasTemplate(GetName() + wxT(" Coarse")))
    {
        new SP_GM_DocTemplate(l_pcDM,
            this, GetName() + wxT(" Coarse"), wxT("*.") + GetExtension() + wxT("coarse"), wxT(""), GetExtension() + wxT("coarse"),
            GetName() + wxT(" Coarse Document"), GetName() + wxT("Coarse View"),
            CLASSINFO(SP_MDI_CoarseDoc), CLASSINFO(SP_MDI_CoarseView), wxTEMPLATE_INVISIBLE);
    }

    return TRUE;
}

bool
SP_DS_Netclass::NodeRequirement(SP_DS_Node* p_pcNode)
{
    CHECK_POINTER(p_pcNode, return FALSE);

    return TRUE;
}

bool
SP_DS_Netclass::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
                                SP_Data* p_pcSrc,
                                SP_Data* p_pcTrg)
{
    CHECK_POINTER(p_pcClass, return FALSE);
    CHECK_POINTER(p_pcSrc, return FALSE);
    CHECK_POINTER(p_pcTrg, return FALSE);

    return TRUE;
}

bool
SP_DS_Netclass::MetadataRequirement(SP_DS_Metadata* p_pcMetadata)
{
    CHECK_POINTER(p_pcMetadata, return FALSE);

    return TRUE;
}
