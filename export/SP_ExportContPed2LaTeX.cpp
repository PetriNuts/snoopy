//////////////////////////////////////////////////////////////////////
// $Author: dscheibl $
// $Version: 0.2 $
// $Date: 2005/08/09 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportContPed2LaTeX.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"
#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_EquationAttribute.h"

#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include <wx/filename.h>

/**
 * constructing ODE using equations at all pre and post transitions
 * and equations at edges to this transitions
 */
SP_DS_EquationParser* SP_ExportContPed2LaTeX::ConstructODE(SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return FALSE);

	wxString l_sRet = wxT("");
	SP_ListEdge::const_iterator l_itEdge;
	for (l_itEdge = p_pcNode->GetTargetEdges()->begin(); l_itEdge != p_pcNode->GetTargetEdges()->end(); ++l_itEdge)
	{
		l_sRet = l_sRet + EquationEdge(p_pcNode, (*l_itEdge));
	}
	for (l_itEdge = p_pcNode->GetSourceEdges()->begin(); l_itEdge != p_pcNode->GetSourceEdges()->end(); ++l_itEdge)
	{
		l_sRet = l_sRet + EquationEdge(p_pcNode, (*l_itEdge));
	}
	if (wxT("") == l_sRet)
	{
		return NULL;
	}
	else
	{
		return new SP_DS_EquationParser(p_pcNode, l_sRet);
	}

}

wxString SP_ExportContPed2LaTeX::EquationEdge(SP_DS_Node* p_pcNode,
		SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcNode, return wxT(""));
	CHECK_POINTER(p_pcEdge, return wxT(""));
	wxString l_sName = p_pcEdge->GetEdgeclass()->GetName();
	if (SP_DS_EDGE != l_sName)
		return wxT("");

	wxString l_sRet = wxT("");
	SP_DS_Attribute* l_pcAttr;
	SP_DS_Node* l_pcTrans = NULL;

	l_pcAttr = p_pcEdge->GetAttribute(wxT("Multiplicity"));
	if (l_pcAttr)
	{
		if (p_pcEdge->GetTarget() == p_pcNode)
		{
			l_sRet = l_sRet + wxT(" +(");
		}
		if (p_pcEdge->GetSource() == p_pcNode)
		{
			l_sRet = l_sRet + wxT(" -(");
		}
		wxString l_sEquation = l_pcAttr->GetValueString();
		if (l_sEquation != wxT("1"))
		{
			l_sRet = l_sRet + l_sEquation + wxT(")*(");
		}
		if (p_pcEdge->GetTarget() == p_pcNode)
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(p_pcEdge->GetSource());
		}
		if (p_pcEdge->GetSource() == p_pcNode)
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(p_pcEdge->GetTarget());
		}
		if (l_pcTrans)
		{
			if (!m_bFull)
			{
				SP_DS_Attribute* l_pcTransAttr =
						l_pcTrans->GetAttribute(wxT("Multiplicity"));
				l_sEquation = l_pcTransAttr->GetValueString();
				l_sRet = l_sRet + l_sEquation;
			}
			else
			{
				if(!l_pcTrans->GetAttribute(wxT("Name"))->GetValueString().IsEmpty())
				{
					l_sRet = l_sRet + l_pcTrans->GetAttribute(wxT("Name"))->GetValueString();
				}
				else
				{
					l_sRet = l_sRet + wxT("\\_t") + l_pcTrans->GetAttribute(wxT("ID"))->GetValueString() + wxT("\\_");
				}
			}
		}
		l_sRet = l_sRet + wxT(")");
	}
	return l_sRet;
}

SP_DS_EquationParser* SP_ExportContPed2LaTeX::ConstructEducts(
		SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return FALSE);

	wxString l_sRet = wxT("");
	SP_ListEdge::const_iterator l_itEdge;
	for (l_itEdge = p_pcNode->GetTargetEdges()->begin(); l_itEdge != p_pcNode->GetTargetEdges()->end(); ++l_itEdge)
	{
		l_sRet = l_sRet + ProductEdge(p_pcNode, (*l_itEdge));
	}
	if (wxT("") == l_sRet)
	{
		return NULL;
	}
	else
	{
		return new SP_DS_EquationParser(p_pcNode, l_sRet);
	}

}

SP_DS_EquationParser* SP_ExportContPed2LaTeX::ConstructProducts(
		SP_DS_Node* p_pcNode)
{
	CHECK_POINTER(p_pcNode, return FALSE);

	wxString l_sRet = wxT("");
	SP_ListEdge::const_iterator l_itEdge;
	for (l_itEdge = p_pcNode->GetSourceEdges()->begin(); l_itEdge != p_pcNode->GetSourceEdges()->end(); ++l_itEdge)
	{
		l_sRet = l_sRet + ProductEdge(p_pcNode, (*l_itEdge));
	}
	if (wxT("") == l_sRet)
	{
		return NULL;
	}
	else
	{
		return new SP_DS_EquationParser(p_pcNode, l_sRet);
	}
}

wxString SP_ExportContPed2LaTeX::ProductEdge(SP_DS_Node* p_pcNode,
		SP_DS_Edge* p_pcEdge)
{
	CHECK_POINTER(p_pcNode, return wxT(""));
	CHECK_POINTER(p_pcEdge, return wxT(""));
	wxString l_sName = p_pcEdge->GetEdgeclass()->GetName();
	if (SP_DS_EDGE != l_sName)
		return wxT("");

	wxString l_sRet = wxT("");
	SP_DS_Attribute* l_pcAttr;
	SP_DS_Node* l_pcTrans = NULL;

	l_pcAttr = p_pcEdge->GetAttribute(wxT("Multiplicity"));
	if (l_pcAttr)
	{
		l_sRet = l_sRet + wxT(" +(");
		wxString l_sEquation = l_pcAttr->GetValueString();
		if (l_sEquation != wxT("1"))
		{
			l_sRet = l_sRet + l_sEquation + wxT(")*(");
		}
		if (p_pcEdge->GetTarget() == p_pcNode)
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(p_pcEdge->GetSource());
		}
		if (p_pcEdge->GetSource() == p_pcNode)
		{
			l_pcTrans = dynamic_cast<SP_DS_Node*>(p_pcEdge->GetTarget());
		}
		if (l_pcTrans)
		{
			if(!l_pcTrans->GetAttribute(wxT("Name"))->GetValueString().IsEmpty())
			{
				l_sRet = l_sRet + l_pcTrans->GetAttribute(wxT("Name"))->GetValueString();
			}
			else
			{
				l_sRet = l_sRet + wxT("\\_p") + l_pcTrans->GetAttribute(wxT("ID"))->GetValueString() + wxT("\\_");
			}
		}
		l_sRet = l_sRet + wxT(")");
	}
	return l_sRet;
}

wxString SP_ExportContPed2LaTeX::DoWrite(const wxString& p_sODEstr,const wxString& p_sName, const wxString& p_sValue, bool p_bTrans)
{
	wxString l_sTemp;
	bool l_bConst=false;

	if(p_sODEstr!=wxT(""))
		l_bConst=true;

	switch (m_nSaveEquationProperties)
	{
	case 0:
	case 4:
		l_sTemp = EquationToTexString(p_sODEstr, p_sName, m_bGreek,
				m_bTabular, m_bBold, p_bTrans, m_bParentheses);
		break;
	case 1:
	case 5:
		if (!p_bTrans)
		{
			l_sTemp = ValueToTexString(p_sName, p_sValue, l_bConst, m_bGreek,
					m_bTabular, m_bBold, p_bTrans, m_bParentheses);
		}
		break;
	case 2:
		break;
	case 3:
	case 6:
		if (!p_bTrans)
		{
			l_sTemp = ValueToTexString(p_sName, p_sValue, l_bConst, m_bGreek,
					m_bTabular, m_bBold, p_bTrans, m_bParentheses);
		}
		l_sTemp.Append(EquationToTexString(p_sODEstr, p_sName, m_bGreek,
				m_bTabular, m_bBold, p_bTrans, m_bParentheses) );
		break;
	default:
		l_sTemp = EquationToTexString(p_sODEstr, p_sName, m_bGreek,
				m_bTabular, m_bBold, p_bTrans, m_bParentheses);
		break;
	}
	return l_sTemp;
}

wxString SP_ExportContPed2LaTeX::DoWrite(SP_DS_EquationParser* p_pcEductParser,
		SP_DS_EquationParser* p_pcProductParser, const wxString& p_sName,
		bool p_bReversible)
{
	SP_DS_EquationParserVariable* l_pcVarName =
			new SP_DS_EquationParserVariable(p_sName);
	wxString l_chName = l_pcVarName->ToTexString(m_bGreek, m_bParentheses);
	wxString l_sReversible = wxT("\\rightarrow");
	wxString l_sPattern = wxT("");
	wxString l_sResult = wxT("");

	wxString l_sEductResult = wxT("");
	wxString l_sProductResult = wxT("");

	switch (m_nSaveEquationProperties)
	{
	case 2:
	case 4:
	case 5:
	case 6:
		if (p_bReversible)
		{
			l_sReversible = wxT("\\rightleftharpoons");
		}
		if (m_bTabular)
		{
			l_sPattern = wxT("%s: & %s & %s & %s\\\\ \n");
		}
		else
		{
			l_sPattern = wxT("$$%s: %s %s %s$$ \n");
		}
		if (p_pcEductParser)
		{
			l_sEductResult = p_pcEductParser->ToTexString(m_bGreek,
					m_bParentheses);
		}
		else
		{
			l_sEductResult = wxT(""); //"\\mathbf{\\mathit{in}}";
		}
		if (p_pcProductParser)
		{
			l_sProductResult = p_pcProductParser->ToTexString(m_bGreek,
					m_bParentheses);
		}
		else
		{
			l_sEductResult = wxT(""); //"\\mathbf{\\mathit{out}}";
		}
		l_sResult.Printf(l_sPattern.c_str(), l_chName.c_str(), l_sEductResult.c_str(),
				l_sReversible.c_str(), l_sProductResult.c_str());
		break;
	default:
		break;
	}
	wxDELETE(l_pcVarName);
	return l_sResult;
}

bool SP_ExportContPed2LaTeX::DoWrite()
{

	CHECK_POINTER(m_graph, return FALSE);

	// gets the iterator of nodeclass
	SP_ListNodeclass::const_iterator ncIt;
	const SP_ListNodeclass* ncList = m_graph->GetNodeclasses();

	CHECK_POINTER(ncList, return FALSE);

	SP_DS_Nodeclass* nc;
	SP_ListNode::const_iterator nIt;
	const SP_ListNode* nList;


	SP_DS_ColListAttribute* l_pcListAttr;

	wxString l_sId;
	wxString l_sName;
	wxString l_sMarking;
	wxString l_sEquation;

	unsigned int l_nMaxParameterChoices = 0;
	unsigned int l_nMaxPlaceChoices = 0;

	wxStringList l_pcPlaceNames;
	wxStringList l_pcTransNames;
	wxStringList l_pcReactionNames;
	wxStringList l_pcParamNames;
	SP_MapString2String l_pcPlaceStrings;
	SP_MapString2String l_pcTransStrings;
	SP_MapString2String l_pcReactionStrings;
	SP_MapString2String l_pcParamStrings;

    SP_DS_PlaceODE * l_pcPlaceODE=NULL;

	wxString l_sResult = wxT("");
	unsigned int i = 0;

	//iterate the nodeclasses
	for (ncIt = ncList->begin(); ncIt != ncList->end(); ncIt++)
	{
		nc = (*ncIt);
		nList = nc->GetElements();

		if ((nc->GetName()) == SP_DS_PARAM)
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				l_sId = wxT("\\_pa");
				l_sId.Append((*nIt)->GetAttribute(wxT("ID"))->GetValueString());
				l_sId.Append(wxT("\\_"));
				l_sName = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();

				l_pcListAttr = dynamic_cast<SP_DS_ColListAttribute*>((*nIt)->GetAttribute(wxT("ParameterList")));

				l_sMarking = FormatListValues(l_pcListAttr);

				if (l_nMaxParameterChoices < l_pcListAttr->GetCountLists())
				{
						l_nMaxParameterChoices = l_pcListAttr->GetCountLists();
				}

				if (wxT("") == l_sName)
				{
					l_pcParamStrings[l_sId]
							= DoWrite(wxT(""),l_sId, l_sMarking, FALSE);
					l_pcParamNames.Add(l_sId.c_str());
				}
				else
				{
					l_pcParamStrings[l_sName]
							= DoWrite(wxT(""),l_sName, l_sMarking, FALSE);
					l_pcParamNames.Add(l_sName.c_str());
				}

			}
			if ((0 < l_pcParamNames.GetCount()) && (2
					!= m_nSaveEquationProperties))
			{
				if (m_bSort)
				{
					l_pcParamNames.Sort();
				}
				if (m_bTabular)
				{
					l_sResult.Append(wxT("\\begin{displaymath}\\begin{array}{rl"));
					if (0 < l_nMaxParameterChoices)
					{
						l_sResult.Append(wxT("l"));
					}
					for (unsigned long j = 1; j < l_nMaxParameterChoices; j++)
					{
						l_sResult.Append(wxT("|l"));
					}
					l_sResult.Append(wxT("}\n\n"));
					for (i = 0; i < l_pcParamNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcParamStrings[l_pcParamNames[i]]);
					}
					l_sResult.Append(wxT("\\end{array}\\end{displaymath}\n\n"));
				}
				else
				{
					for (i = 0; i < l_pcParamNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcParamStrings[l_pcParamNames[i]]);
					}
				}
			}
		}

		if (nc->GetName() == SP_DS_CONTINUOUS_PLACE)
		{
			long l_nPlacePos=0;
			l_pcPlaceODE = new SP_DS_PlaceODE(m_graph);

			if(!l_pcPlaceODE->Initialize())
				return false;

			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				l_sId = wxT("\\_p");
				l_sId.Append((*nIt)->GetAttribute(wxT("ID"))->GetValueString());
				l_sId.Append(wxT("\\_"));
				l_sName = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();
				l_sMarking = (*nIt)->GetAttribute(wxT("Marking"))->GetValueString();

				wxString l_sPlaceODEstr=l_pcPlaceODE->ConstructODE(l_nPlacePos);


				if (l_sPlaceODEstr.Contains(wxT("_")))//by george, fixing the layout of _
				{
					l_sPlaceODEstr.Replace(wxT("_"), wxT("\\_"));
				}

				if (wxT("") == l_sName)
				{
					l_pcPlaceStrings[l_sId] = DoWrite(l_sPlaceODEstr, l_sId,
							l_sMarking, FALSE);
					l_pcPlaceNames.Add(l_sId.c_str());
				}
				else
				{
					l_pcPlaceStrings[l_sName] = DoWrite(l_sPlaceODEstr, l_sName,
							l_sMarking, FALSE);
					l_pcPlaceNames.Add(l_sName.c_str());
				}


				l_nPlacePos++;
			}

			if (l_pcPlaceODE)
				{
					wxDELETE(l_pcPlaceODE);
				}
				l_pcPlaceODE = NULL;


			if ((0 < l_pcPlaceNames.GetCount()) && (2
					!= m_nSaveEquationProperties))
			{
				if (m_bSort)
				{
					l_pcPlaceNames.Sort();
				}
				if (m_bTabular)
				{
					l_sResult.Append(wxT("\\begin{displaymath}\\begin{array}{rl"));
					if (0 < l_nMaxPlaceChoices)
					{
						l_sResult.Append(wxT("l"));
					}
					for (unsigned long j = 1; j < l_nMaxPlaceChoices; j++)
					{
						l_sResult.Append(wxT("|l"));
					}
					l_sResult.Append(wxT("}\n\n"));
					for (i = 0; i < l_pcPlaceNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcPlaceStrings[l_pcPlaceNames[i]]);
					}
					l_sResult.Append(wxT("\\end{array}\\end{displaymath}\n\n"));
					/*
					 l_sResult.Append(wxT("\\begin{eqnarray*}\n"));
					 for (i = 0; i < l_pcPlaceNames.GetCount(); i++)
					 {
					 l_sResult.Append(l_pcPlaceStrings[l_pcPlaceNames[i]]);
					 }
					 l_sResult.Append(wxT("\\end{eqnarray*}\n\n"));
					 */
				}
				else
				{
					for (i = 0; i < l_pcPlaceNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcPlaceStrings[l_pcPlaceNames[i]]);
					}
				}
			}
		}

		/*if (nc->GetName() == SP_DS_CONTINUOUSPED_TRANS)
		{
			for (nIt = nList->begin(); nIt != nList->end(); nIt++)
			{
				l_sId = wxT("\\_t");
				l_sId.Append((*nIt)->GetAttribute(wxT("ID"))->GetValueString());
				l_sId.Append(wxT("\\_"));
				l_sName = (*nIt)->GetAttribute(wxT("Name"))->GetValueString();
				if (m_bFull)
				{
					l_sEquation = wxT("");

					l_pcListAttr= dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr)->GetAttribute(wxT("FunctionList"));

					l_sEquation=l_pcListAttr->GetActiveCellValue(1);

					l_pcParser = new SP_DS_EquationParser((*nIt), l_sEquation);
					if (wxT("") == l_sName)
					{
						l_pcTransStrings[l_sId] = DoWrite(l_pcParser, l_sId,
								wxT(""), TRUE);
						l_pcTransNames.Add(l_sId);
					}
					else
					{
						l_pcTransStrings[l_sName] = DoWrite(l_pcParser,
								l_sName, wxT(""), TRUE);
						l_pcTransNames.Add(l_sName);
					}
					if (l_pcParser)
					{
						wxDELETE(l_pcParser);
					}
					l_pcParser = NULL;
				}

				l_pcEductParser = ConstructEducts((*nIt));
				l_pcProductParser = ConstructProducts((*nIt));

				l_bReversible = false;
				if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_BOOL == (*nIt)->GetAttribute(wxT("Reversible"))->GetAttributeType())
				{
					l_pcBoolAttr = dynamic_cast<SP_DS_BoolAttribute*>((*nIt)->GetAttribute(wxT("Reversible")));
					l_bReversible = l_pcBoolAttr->GetValue();
				}

				if (wxT("") == l_sName)
				{
					l_pcReactionStrings[l_sId] = DoWrite(l_pcEductParser,
							l_pcProductParser, l_sId, l_bReversible);
					l_pcReactionNames.Add(l_sId);
				}
				else
				{
					l_pcReactionStrings[l_sName] = DoWrite(l_pcEductParser,
							l_pcProductParser, l_sName, l_bReversible);
					l_pcReactionNames.Add(l_sName);
				}

				if (l_pcEductParser)
				{
					wxDELETE(l_pcEductParser);
				}
				l_pcEductParser = NULL;
				if (l_pcProductParser)
				{
					wxDELETE(l_pcProductParser);
				}
				l_pcProductParser = NULL;
			}
			if ((0 < l_pcTransNames.GetCount()) && (2
					!= m_nSaveEquationProperties))
			{
				if (m_bSort)
				{
					l_pcTransNames.Sort();
				}
				if (m_bTabular)
				{
					l_sResult.Append(wxT("\\begin{eqnarray*}\n"));
					for (i = 0; i < l_pcTransNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcTransStrings[l_pcTransNames[i]]);
					}
					l_sResult.Append(wxT("\\end{eqnarray*}\n\n"));
				}
				else
				{
					for (i = 0; i < l_pcTransNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcTransStrings[l_pcTransNames[i]]);
					}
				}
			}
			if ((0 < l_pcReactionNames.GetCount()) && (0
					!= m_nSaveEquationProperties) && (1
					!= m_nSaveEquationProperties) && (3
					!= m_nSaveEquationProperties))
			{
				if (m_bSort)
				{
					l_pcReactionNames.Sort();
				}
				if (m_bTabular)
				{
					l_sResult.Append(wxT("\\begin{displaymath}\\begin{array}{lrcl}\n\n"));
					for (i = 0; i < l_pcReactionNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcReactionStrings[l_pcReactionNames[i]]);
					}
					l_sResult.Append(wxT("\\end{array}\\end{displaymath}\n\n"));
				}
				else
				{
					for (i = 0; i < l_pcReactionNames.GetCount(); i++)
					{
						l_sResult.Append(l_pcReactionStrings[l_pcReactionNames[i]]);
					}
				}
			}
		}*/
	}
	m_file.Write(l_sResult);
	return FALSE;
}

SP_ExportContPed2LaTeX::SP_ExportContPed2LaTeX() :
	m_pcParent(NULL), m_bGreek(FALSE), m_bTabular(FALSE), m_bFull(FALSE),
			m_bBold(FALSE), m_bConst(FALSE), m_bSort(FALSE),
			m_bParentheses(FALSE), m_bConvSpecialChar(FALSE),
			m_pcSaveLaTeXTabularCheckBox(NULL),
			m_pcSaveLaTeXGreekCheckBox(NULL), m_pcSaveLaTeXFullCheckBox(NULL),
			m_pcSaveLaTeXBoldCheckBox(NULL), m_pcSaveLaTeXConstCheckBox(NULL),
			m_pcSaveLaTeXSortCheckBox(NULL),
			m_pcSaveLaTeXParenthesesCheckBox(NULL),
			m_nSaveEquationProperties(0),
			m_pcSaveEquationPropertiesChoice(NULL), m_doc(NULL), m_graph(NULL),
			m_fileName(wxT(""))
{
}

SP_ExportContPed2LaTeX::~SP_ExportContPed2LaTeX()
{
}

bool SP_ExportContPed2LaTeX::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (SP_DS_CONTINUOUSPED_CLASS == cName);
}

bool SP_ExportContPed2LaTeX::Write(SP_MDI_Doc* p_doc, const wxString& p_fileName)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	CHECK_BOOL((!p_fileName.IsEmpty()), return false);

	m_doc = p_doc;
	m_graph = m_doc->GetGraph();
	m_fileName = p_fileName;

	if (!m_file.Open(m_fileName.c_str(), wxT("wt")) )
	{
		return false;
	}

	SP_DLG_SaveContinuousEquationProperties l_cDlg(this, m_pcParent);
	int l_bModalResult = l_cDlg.ShowModal();
	l_cDlg.Destroy();

	if (wxID_CANCEL == l_bModalResult)
	{
		return false;
	}

	// will be implemented in successors
	bool res = DoWrite();

	// close file handle and check if this was ok
	CHECK_BOOL((m_file.Close()), return false);
	return res && !m_bError;
}

wxString SP_ExportContPed2LaTeX::EquationToTexString(
		const wxString& p_sODEstr , const wxString& p_sName, bool p_bGreek,
		bool p_bTabular, bool p_bBold, bool p_bTrans, bool p_bParentheses)
{

	wxString l_chName = ToTexString(p_sName,p_bGreek, p_bParentheses,
			m_bConvSpecialChar);

	wxString l_sResult;
	if (p_bTrans)
	{
		if (p_bTabular)
		{
			l_sResult.Printf(wxT("%s & = \n& %s\\\\ \n\\\\\n"), l_chName.c_str(),
					ToTexString(p_sODEstr, p_bGreek, p_bParentheses,
							m_bConvSpecialChar).c_str());
		}
		else
		{
			l_sResult.Printf(wxT("$$%s = \n%s$$ \n\\\\\n"), l_chName.c_str(),
					ToTexString(p_sODEstr,p_bGreek, p_bParentheses,
							m_bConvSpecialChar).c_str());
		}
	}
	else
	{
		if (p_bTabular)
		{
			l_sResult.Printf(
					wxT("\\frac{d%s}{d\\mathrm{t}} & = &\n %s\\\\ \n\\\\\n"),
					l_chName.c_str(),ToTexString(p_sODEstr,p_bGreek, p_bParentheses,
							m_bConvSpecialChar).c_str());
		}
		else
		{
			l_sResult.Printf(wxT("$$\\frac{d%s}{d\\mathrm{t}} = \n%s$$ \n\\\\\n"),
					l_chName.c_str(), ToTexString(p_sODEstr,p_bGreek, p_bParentheses,
							m_bConvSpecialChar).c_str());
		}
	}
	return l_sResult;
}

wxString SP_ExportContPed2LaTeX::ValueToTexString(const wxString& p_sName,
		const wxString& p_sValue, bool p_bConst, bool p_bGreek, bool p_bTabular,
		bool p_bBold, bool p_bTrans, bool p_bParentheses)
{
	wxString l_sResult = wxT("");
	if ((wxT("") != p_sValue) && (!p_bTrans))
	{
		const wxString l_chName =ToTexString(p_sName,p_bGreek,
				p_bParentheses,false);
		if (p_bTabular)
		{
			if (p_bConst)
			{
				l_sResult.Printf(
						wxT("%s & =\n & %s\\quad\\left(\\text{const.}\\right)\\\\ \n"),
						l_chName.c_str(), p_sValue.c_str());
			}
			else
			{
				l_sResult.Printf(wxT("%s & =\n & %s\\\\ \n"), l_chName.c_str(),
						p_sValue.c_str());
			}
		}
		else
		{
			if (p_bConst)
			{
				l_sResult.Printf(
						wxT("$$%s =\n %s\\quad\\left(\\text{const.}\\right)$$ \n"),
						l_chName.c_str(), p_sValue.c_str());
			}
			else
			{
				l_sResult.Printf(wxT("$$%s =\n %s$$ \n"), l_chName.c_str(), p_sValue.c_str());
			}
		}
	}
	return l_sResult;
}

wxString SP_ExportContPed2LaTeX::FormatListValues(SP_DS_ColListAttribute* p_pcListAttr)
{
	CHECK_POINTER(p_pcListAttr, return wxT(""));

	wxString l_sResult = wxT("");
	for (unsigned int i = 0; i < p_pcListAttr->GetCountLists(); i++)
	{
		if (i > 0)
		{
			if (m_bTabular)
			{
				l_sResult.Append(wxT(" & "));
			}
			else
			{
				l_sResult.Append(wxT("; "));
			}
		}
		if ((m_bBold) && (i == p_pcListAttr->GetActiveList()))
		{
			l_sResult.Append(wxT("\\mathbf{"));
		}
		l_sResult.Append(p_pcListAttr->GetCell(i,1));
		if ((m_bBold) && (i == p_pcListAttr->GetActiveList()))
		{
			l_sResult.Append(wxT("}"));
		}
	}
	return l_sResult;
}

bool SP_ExportContPed2LaTeX::AddToSaveContinuousEquationDialog(
		SP_DLG_SaveContinuousEquationProperties* p_pcDlg, wxSizer* p_pcSizer)
{
	CHECK_POINTER(p_pcDlg, return FALSE);
	CHECK_POINTER(p_pcSizer, return FALSE);

	wxString l_sSaveEquationPropertiesChoices[] =
	{ wxT("LaTeX Formula Output"), wxT("LaTeX Start Value Output"),
			wxT("LaTeX Reaction Formula Output"),
			wxT("LaTeX Formula + Start Value Output"),
			wxT("LaTeX Formula + Reaction Formula Output"),
			wxT("LaTeX Start Value + Reaction Formula Output"),
			wxT("LaTeX Formula + Start Value + Reaction Formula Output") };
	m_pcSaveEquationPropertiesChoice = new wxRadioBox(
			p_pcDlg,
			-1,
			wxT("Choose Equation Output Format:"),
			wxDefaultPosition,
			wxDefaultSize,
			7,
			l_sSaveEquationPropertiesChoices,
			1,
			wxRA_SPECIFY_COLS);//wxRA_SPECIFY_ROWS);
	m_pcSaveEquationPropertiesChoice->SetSelection(m_nSaveEquationProperties);
	p_pcSizer->Add(m_pcSaveEquationPropertiesChoice, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXTabularCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Save LaTeX output in tabular format"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bTabular));
	p_pcSizer->Add(m_pcSaveLaTeXTabularCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXGreekCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Convert Greek Letter into LaTeX commands"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bGreek));
	p_pcSizer->Add(m_pcSaveLaTeXGreekCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXFullCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Save places and transitions separated"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bFull));
	p_pcSizer->Add(m_pcSaveLaTeXFullCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXConstCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Mark constant places and parameters with '(const.)'"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bConst));
	p_pcSizer->Add(m_pcSaveLaTeXConstCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXBoldCheckBox
			= new wxCheckBox(
					p_pcDlg,
					-1,
					wxT("Mark selected place and parameter values with bold letters"),
					wxDefaultPosition,
					wxDefaultSize,
					0,
					wxGenericValidator(&m_bBold));
	p_pcSizer->Add(m_pcSaveLaTeXBoldCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXSortCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Sort by Name"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bSort));
	p_pcSizer->Add(m_pcSaveLaTeXSortCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXParenthesesCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Put Variable Name in Parentheses"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bParentheses));
	p_pcSizer->Add(m_pcSaveLaTeXParenthesesCheckBox, 0, wxEXPAND | wxALL, 5);

	m_pcSaveLaTeXConvSpecialCharCheckBox = new wxCheckBox(
			p_pcDlg,
			-1,
			wxT("Convert special Characters"),
			wxDefaultPosition,
			wxDefaultSize,
			0,
			wxGenericValidator(&m_bConvSpecialChar));
	p_pcSizer->Add(m_pcSaveLaTeXConvSpecialCharCheckBox, 0, wxEXPAND | wxALL, 5);

	return TRUE;
}

bool SP_ExportContPed2LaTeX::OnSaveContinuousEquationPropertiesOK()
{
	m_nSaveEquationProperties
			= m_pcSaveEquationPropertiesChoice->GetSelection();
	return TRUE;
}


const wxString
SP_ExportContPed2LaTeX::ToTexString(const wxString l_pVarName,bool p_bGreek,bool p_bParentheses,bool p_bConvSpecialChar)
{
  wxString result = wxT("");

  SP_DS_Nodeclass* l_pcContPlaceNodeClass = m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

  const SP_ListNode* l_pcNodeList = l_pcContPlaceNodeClass->GetElements();
  SP_ListNode::const_iterator l_Iter;
  bool l_bisPlace= false;
  for (l_Iter = l_pcNodeList->begin(); l_Iter != l_pcNodeList->end(); ++l_Iter){
	  wxString tt = (*l_Iter)->GetAttribute(wxT("Name"))->GetValueString();
	  if (l_pVarName.IsSameAs(tt)){
		 l_bisPlace=true;
		}
  }

  if (p_bParentheses &&  l_bisPlace)
  {

    result.Append(wxT("\\left["));
  }
  result.Append(wxT("\\mathrm{"));
  if (p_bGreek)
  {
    result.Append(ConvertGreek(l_pVarName,  p_bConvSpecialChar));
  } else
  {
    result.Append(FormatTexString(l_pVarName, p_bConvSpecialChar));
  }
  result.Append(wxT("}"));
  if (p_bParentheses && l_bisPlace )
  {
    result.Append(wxT("\\right]"));
  }
  return result;
}

wxString
SP_ExportContPed2LaTeX::FormatTexString(const wxString& p_sName, bool p_bConvSpecialChar)
{
	wxString l_sName = p_sName;
	wxString l_sQuoteUnderscore=wxT("");
	if (p_bConvSpecialChar){
		l_sQuoteUnderscore=wxT("\\");

		l_sName.Replace(wxT("$"), wxT("\\$ "));
		l_sName.Replace(wxT("&"), wxT("\\& "));
		l_sName.Replace(wxT("#"), wxT("\\# "));

	}

	 int l_nReplaced = l_sName.Replace(wxT("_"),
		 wxString::Format(wxT("%s_\\mathrm{"),l_sQuoteUnderscore.c_str()));

	  for (int  i = 0; i < l_nReplaced; i++)
	  {
		l_sName.Append(wxT("}"));
	  }







  /* Here all wxT("_" were quoted, only when a number comes after "_")
   then wxT("_") is not quoted

  int l_nUndod=0;
  int i;
  for ( i =0; i<10; i++)
  {
	  l_nUndod += l_sName.Replace(
		  wxString::Format(wxT("\\_\\mathrm{%d"),i).c_str(),
		  wxString::Format(wxT("_%d"),i).c_str());
  }
  for ( i = 0; i < l_nReplaced - l_nUndod; i++)
  {
    l_sName.Append(wxT("}"));
  }

  */
  return l_sName;
}

wxString
SP_ExportContPed2LaTeX::ConvertGreek(const wxString& p_sName, bool p_bConvSpecialChar)
{
  wxString result = FormatTexString(p_sName, p_bConvSpecialChar);
  result.Replace(wxT("alpha"), wxT("\\alpha "));
  result.Replace(wxT("gamma"), wxT("\\gamma "));
  result.Replace(wxT("delta"), wxT("\\delta "));
  result.Replace(wxT("epsilon"), wxT("\\epsilon "));
  result.Replace(wxT("var\\epsilon "), wxT("\\varepsilon "));
  result.Replace(wxT("eta"), wxT("\\eta "));
  result.Replace(wxT("b\\eta "), wxT("\\beta "));
  result.Replace(wxT("z\\eta "), wxT("\\zeta "));
  result.Replace(wxT("th\\eta "), wxT("\\theta "));
  result.Replace(wxT("var\\theta "), wxT("\\vartheta "));
  result.Replace(wxT("iota"), wxT("\\iota "));
  result.Replace(wxT("kappa"), wxT("\\kappa "));
  result.Replace(wxT("lambda"), wxT("\\lambda "));
  result.Replace(wxT("mu"), wxT("\\mu "));
  result.Replace(wxT("nu"), wxT("\\nu "));
  result.Replace(wxT("xi"), wxT("\\xi "));
  result.Replace(wxT("pi"), wxT("\\pi "));
  result.Replace(wxT("var\\pi "), wxT("\\varpi "));
  result.Replace(wxT("rho"), wxT("\\rho "));
  result.Replace(wxT("var\\rho "), wxT("\\varrho "));
  result.Replace(wxT("sigma"), wxT("\\sigma "));
  result.Replace(wxT("var\\sigma "), wxT("\\varsigma "));
  result.Replace(wxT("tau"), wxT("\\tau "));
  result.Replace(wxT("upsilon"), wxT("\\upsilon "));
  result.Replace(wxT("phi"), wxT("\\phi "));
  result.Replace(wxT("var\\phi "), wxT("\\varphi "));
  result.Replace(wxT("chi"), wxT("\\chi "));
  result.Replace(wxT("psi"), wxT("\\psi "));
  result.Replace(wxT("omega"), wxT("\\omega "));

  result.Replace(wxT("Alpha"), wxT("\\Alpha "));
  result.Replace(wxT("Gamma"), wxT("\\Gamma "));
  result.Replace(wxT("Delta"), wxT("\\Delta "));
  result.Replace(wxT("Epsilon"), wxT("\\Epsilon "));
  result.Replace(wxT("Var\\epsilon"), wxT("\\Varepsilon "));
  result.Replace(wxT("Eta"), wxT("\\Eta "));
  result.Replace(wxT("B\\eta "), wxT("\\Beta "));
  result.Replace(wxT("Z\\eta "), wxT("\\Zeta "));
  result.Replace(wxT("Th\\eta "), wxT("\\Theta "));
  result.Replace(wxT("var\\theta "), wxT("\\Vartheta "));
  result.Replace(wxT("Iota"), wxT("\\Iota "));
  result.Replace(wxT("Kappa"), wxT("\\Kappa "));
  result.Replace(wxT("Lambda"), wxT("\\Lambda "));
  result.Replace(wxT("Mu"), wxT("\\Mu "));
  result.Replace(wxT("Nu"), wxT("\\Nu "));
  result.Replace(wxT("Xi"), wxT("\\Xi "));
  result.Replace(wxT("Pi"), wxT("\\Pi "));
  result.Replace(wxT("Var\\pi "), wxT("\\Varpi "));
  result.Replace(wxT("Rho"), wxT("\\Rho "));
  result.Replace(wxT("Var\\rho "), wxT("\\Varrho "));
  result.Replace(wxT("Sigma"), wxT("\\Sigma "));
  result.Replace(wxT("Var\\sigma "), wxT("\\Varsigma "));
  result.Replace(wxT("Tau"), wxT("\\Tau "));
  result.Replace(wxT("Upsilon"), wxT("\\Upsilon "));
  result.Replace(wxT("Phi"), wxT("\\Phi "));
  result.Replace(wxT("Var\\phi "), wxT("\\Varphi "));
  result.Replace(wxT("Chi"), wxT("\\Chi "));
  result.Replace(wxT("Psi"), wxT("\\Psi "));
  result.Replace(wxT("Omega"), wxT("\\Omega "));

  result.Replace(wxT(" \\"), wxT("\\"));
  return result;
}

