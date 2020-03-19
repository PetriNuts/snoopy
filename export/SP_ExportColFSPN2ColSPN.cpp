//////////////////////////////////////////////////////////////////////
// $Author: G. Assaf $
// $Version: 0.2 $
// $Date: 2020/3/14 $
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColFSPN2ColSPN.h"
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
bool SP_ExportColFSPN2ColSPN::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_FUZZY_ColSPN_CLASS);
}

bool SP_ExportColFSPN2ColSPN::Write(SP_MDI_Doc* p_doc,
	const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();

	m_nTransitionNumber = m_graph->GetNodeclass(wxT("Transition"))->GetElements()->size();
//	m_nTransitionNumber += m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements()->size();

	//m_nPlaceNumber = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements()->size();
	m_nPlaceNumber += m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements()->size();

	m_pcTransitionNodeclass = NULL;
	m_pcPlaceNodeclass = NULL;


	m_fileName = p_fileName;
	return SP_XmlWriter::Write(m_graph, m_fileName);
}

bool SP_ExportColFSPN2ColSPN::WriteNetclass(SP_DS_Netclass* p_pcVal,
	wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);


	wxXmlNode* l_pcElem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("netclass"));
	l_pcElem->AddAttribute(wxT("name"), SP_DS_COLSPN_CLASS);
	p_pcRoot->AddChild(l_pcElem);

	return TRUE;
}


bool SP_ExportColFSPN2ColSPN::WriteMetadataclass(SP_DS_Metadataclass* p_pcVal, wxXmlNode* p_pcRoot)
{
	CHECK_POINTER(p_pcVal, return FALSE);
	CHECK_POINTER(p_pcRoot, return FALSE);

	wxString l_sName = p_pcVal->GetName();
	if (l_sName == SP_DS_CPN_CONSTANT_HARMONIZING)
	{
		//do nothing
	}
	else if (l_sName == wxT("Constant Class"))//by george
	{

		SP_DS_Metadataclass* l_pcMC = m_graph->GetMetadataclass(SP_DS_META_CONSTANT);
		SP_DS_Metadataclass* l_pcMCNew = m_graph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
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
			wxString l_sType = l_pcConst->GetAttribute(wxT("Type"))->GetValueString();
			l_pcConstant->GetAttribute(wxT("Group"))->SetValueString(l_pcConst->GetAttribute(wxT("Group"))->GetValueString());
			if (l_sType == wxT("TFN"))
			{
				l_pcConstant->GetAttribute(wxT("Type"))->SetValueString("double");
			}
			else
			{
				l_pcConstant->GetAttribute(wxT("Type"))->SetValueString(l_pcConst->GetAttribute(wxT("Type"))->GetValueString());
			}

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
				if (l_sType == wxT("TFN"))
				{
					int l_nRowCol = l_pcColList->AppendEmptyRow();
					wxString l_sVal = l_pcSourceColList->GetCell(l_nRowCol, 1);
					wxString l_sCrisp = GetCrispValue(l_sVal, l_sType);
					wxString l_sCol = l_pcSourceColList->GetCell(l_nRowCol, 0);
					l_pcColList->SetCell(l_nRowCol, 0, l_sCol);
					l_pcColList->SetCell(l_nRowCol, 1, l_sCrisp);
					continue;
				}
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
		if (l_sName != wxT("Constant Class1"))//this line by george
			SP_XmlWriter::WriteMetadataclass(p_pcVal, p_pcRoot);
	}
	return true;
}

wxString  SP_ExportColFSPN2ColSPN::GetCrispValue(wxString& param, wxString& type)
{
	wxString crispValue;
	if (type == wxT("TFN") || type == wxT("BFN"))
	{
		SP_VectorDouble fn_param = GetFNConstants(param);
		if (fn_param.size() > 0)
		{
			crispValue << fn_param[1];
		}
	}
	return crispValue;

}

SP_VectorDouble   SP_ExportColFSPN2ColSPN::GetFNConstants(wxString &val)
{
	SP_VectorDouble v_dConstants;
	std::string s = val;
	wxString delimiter = ",";
	bool isOk = true;
	size_t pos = 0;
	wxString token;
	int numConst = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		numConst++;
		token = s.substr(0, pos);
		double value;
		if (token.ToDouble(&value)) { v_dConstants.push_back(value); }
		s.erase(0, pos + delimiter.length());

	}
	wxString ss = s;
	double d;

	if (ss.ToDouble(&d)) { v_dConstants.push_back(d); }


	return v_dConstants;
}

