//////////////////////////////////////////////////////////////////////
// $Author: G. Assaf $
// $Version: 0.2 $
// $Date: 2020/3/14 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColHPN2ColFHPN.h"
#include "sp_core/base/SP_Graphic.h"

#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gr/attributes/SP_GR_DoubleMarkAttribute.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
bool SP_ExportColHPN2ColFHPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportColHPN2ColFHPN::Write(SP_MDI_Doc* p_doc,
	const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	m_nTransitionNumber = m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size();
	m_nTransitionNumber += m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();

	m_nPlaceNumber = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceNumber += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	m_pcTransitionNodeclass = NULL;
	m_pcPlaceNodeclass = NULL;


	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportColHPN2ColFHPN::WriteNetclass(SP_DS_Netclass* p_pcVal,
	wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_FUZZY_ColHPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}

 
bool SP_ExportColHPN2ColFHPN::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	wxString l_sName = p_pcVal->GetName();
	if (l_sName == SP_DS_META_CONSTANT)
	{
		//do nothing
	}
	else if (l_sName == wxT("Constant Class1"))//by george
	{

		SP_DS_Metadataclass* l_pcMC = m_graph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
		SP_DS_Metadataclass* l_pcMCNew = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
		l_pcMCNew->RemoveAll();
		SP_DS_Metadataclass* l_pcCOnstants = p_pcVal;
		SP_ListMetadata::const_iterator l_itElem;
		l_itElem = l_pcCOnstants->GetElements()->begin();
		for (size_t l_nCon = 0; l_nCon < l_pcCOnstants->GetElements()->size(); l_nCon++)
		{
			SP_DS_Metadata* l_pcConstant = l_pcMCNew->NewElement(1);
			///l_pcConstant = *l_itElem;
			SP_DS_Metadata* l_pcConst;
			l_pcConst = *l_itElem;
			l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_pcConst->GetAttribute(wxT("Group"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_pcConst->GetAttribute(wxT("Type"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_pcConst->GetAttribute(wxT("Name"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Comment"))->SetValueString(l_pcConst->GetAttribute(wxT("Comment"))->GetValueString());
			l_pcConstant->GetAttribute(wxT("Name"))->SetValueString(l_pcConst->GetAttribute(wxT("Name"))->GetValueString());
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConstant->GetAttribute(wxT("ValueList")));
			SP_DS_ColListAttribute * l_pcSourceColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcConst->GetAttribute(wxT("ValueList")));


			l_pcColList->Clear();
			//l_pcColList->SetCell(0, 0, wxT("Main"));
			//l_pcColList->SetCell(0, 1, l_pcSourceColList->GetCell(0, 1));

			for (size_t l_nRow = 0; l_nRow < l_pcSourceColList->GetRowCount(); l_nRow++)
			{
				int l_nRowCol = l_pcColList->AppendEmptyRow();
				l_pcColList->SetCell(l_nRowCol, 0, l_pcSourceColList->GetCell(l_nRowCol, 0));
				l_pcColList->SetCell(l_nRowCol, 1, l_pcSourceColList->GetCell(l_nRowCol, 1));
			}
			++l_itElem;

		}

		SP_XmlWriter::WriteMetadataclass(l_pcMCNew, p_pcRoot);
	}
	//else if (l_sName == SP_DS_META_FUNCTION)
	//{
	//TODO
	//}
	else if (m_graph->GetMetadataclass(l_sName))
	{
		if (l_sName != wxT("Constant Class"))//this line by george
			SP_XmlWriter::WriteMetadataclass(p_pcVal, p_pcRoot);
	}
	return true;
}

