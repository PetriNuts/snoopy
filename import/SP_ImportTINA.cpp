//////////////////////////////////////////////////////////////////////
// $Author:ms$
// $Version: 0.1 $
// $Date: 2007/07/12 $
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportTINA.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gr/SP_GR_Edge.h"
#include "snoopy.h"

#include "sp_utilities.h"


#include <wx/spinctrl.h>

#include <algorithm>

// ============================================================================
// implementations
// ============================================================================

SP_ImportTINA::SP_ImportTINA()
{

}

SP_ImportTINA::~SP_ImportTINA()
{

}

bool SP_ImportTINA::ReadFile(const wxString& p_sFile)
{
	g_bError = FALSE;
	g_sMessage = wxT("");
	m_pcGraph = NULL;
	m_mPlaces.clear();
	m_mTransitions.clear();
	m_mCoarse.clear();
	m_mCoarseGr.clear();
	m_mPlacesGr.clear();
	m_mTransitionsGr.clear();

	wxTextFile l_cFile;
	if (l_cFile.Open(p_sFile))
	{
		CreateGraph(p_sFile);
		if (m_pcGraph)
		{
			Parse(l_cFile);
		}
		l_cFile.Close();
	}

	return TRUE;
}

/////////////////////////////////////Parsing TINA/////////////////////////////////////////////////////////

bool SP_ImportTINA::Parse(wxTextFile& p_cFile)
{
	wxString l_sLog = wxT("");
	SP_LOGDEBUG(wxString(p_cFile.GetName()).AfterLast(wxFILE_SEP_PATH));

	wxString l_sLine = p_cFile.GetFirstLine();
	wxString l_sMatch;
	wxRegEx l_cExp(wxT(".*"));

	for (; !g_bError && (p_cFile.GetCurrentLine()
			< (p_cFile.GetLineCount() - 2)); l_sLine = p_cFile.GetNextLine())
	{
		//SP_LOGDEBUG(l_sLine);
		l_cExp.Compile(wxT("(^[pteh]) "));
		if (!l_cExp.Matches(l_sLine))
		{
			continue;
		}
		l_sMatch = l_cExp.GetMatch(l_sLine, 0);
		wxChar c = l_sMatch.GetChar(0);
		switch (c)
		{
		case wxT('p'): // 'p'
			g_bError = !ParsePlace(l_sLine);
			break;
		case wxT('t'): // 't'
			g_bError = !ParseTrans(l_sLine);
			break;
		case wxT('h'): // 'h'
			g_bError = !ParseComment(l_sLine);
			break;
		case wxT('e'): // 'e'
			g_bError = !ParseArc(l_sLine);
		};

		l_sMatch = l_cExp.GetMatch(l_sLine, 1);
	}
	if (g_bError)
		SP_LOGDEBUG(wxString::Format(wxT(" error in line %lu"), p_cFile.GetCurrentLine()));

	return g_bError;
}

bool SP_ImportTINA::ParseComment(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	//
	//     h {celegans6cells-ac6-complete} small
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	if (!l_cST.CountTokens())
		return FALSE;

	long l_nNet = 1;

	SP_DS_Metadataclass *metaClass = m_pcGraph->GetMetadataclass(wxT("Comment"));
	SP_DS_Metadata *l_pcNode = metaClass->NewElement(l_nNet);
	SP_Graphic *l_pcNodeGr = l_pcNode->AddGraphicInSubnet(l_nNet);
	long l_nX = 10;
	long l_nY = 10;
	l_pcNodeGr->SetPosXY(l_nX, l_nY);

	l_cST.GetNextToken();
	wxString l_sText = l_cST.GetNextToken();

	l_pcNode->GetAttribute(wxT("Comment"))->SetValueString(l_sLine);
	l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE);
	return TRUE;
}

bool SP_ImportTINA::ParsePlace(wxString& p_sLine)
{

	wxString l_sLine = p_sLine;
	//     name posx posy id marks n comment w
	//     p 300.0 1010.0 P3p17 5 n {LET-60/P3.p} w
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	if (!l_cST.CountTokens())
		return false;

	l_cST.GetNextToken();
	wxString l_sX = l_cST.GetNextToken();
	wxString l_sY = l_cST.GetNextToken();
	wxString l_sName = l_cST.GetNextToken();
	wxString l_sMark = l_cST.GetNextToken();
	long l_nNet = 1;
	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Place"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_pcPlace = nodeClass->NewElement(l_nNet);
	l_pcPlace->GetAttribute(wxT("Name"))->SetValueString(l_sName);
	l_pcPlace->GetAttribute(wxT("Name"))->SetShow(TRUE);
	l_pcPlace->GetAttribute(wxT("Marking"))->SetValueString(l_sMark);

	if (!l_pcPlace)
		return FALSE;

	// parse next lines to comment
	wxString l_sComment = l_cST.GetString();

	// comment attribute
	l_pcPlace->GetAttribute(wxT("Comment"))->SetValueString(l_sLine);


	long l_nX = ToLong(l_sX);
	long l_nY = ToLong(l_sY);

	SP_Graphic* l_pcPlaceGr = l_pcPlace->AddGraphicInSubnet(l_nNet);
	l_pcPlaceGr->SetPosXY(l_nX, l_nY);

	m_mPlacesGr[l_sName] = l_pcPlaceGr;

	l_pcPlace->ShowOnCanvas(l_pcCanvas, FALSE);
	m_mPlaces[l_sName] = l_pcPlace;
	return TRUE;
}

bool SP_ImportTINA::ParseTrans(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	//     name x y id c 0 w n comment w
	//     t 260.0 970.0 P3t6 c 0 w n {LET-60 PRO/P3.p} w
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	if (!l_cST.CountTokens())
		return false;

	l_cST.GetNextToken();
	wxString l_sX = l_cST.GetNextToken();
	wxString l_sY = l_cST.GetNextToken();
	wxString l_sName = l_cST.GetNextToken();
	long l_nNet = 1;
	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Transition"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_pcTrans = nodeClass->NewElement(l_nNet);
	l_pcTrans->GetAttribute(wxT("Name"))->SetValueString(l_sName);
	l_pcTrans->GetAttribute(wxT("Name"))->SetShow(TRUE);

	if (!l_pcTrans)
		return FALSE;

	// parse next lines to comment
	wxString l_sComment = wxT("");
	//if (l_cST.GetNextToken() == wxT("c"))
	{
		l_sComment = l_cST.GetString();
		// comment attribute
		l_pcTrans->GetAttribute(wxT("Comment"))->SetValueString(l_sLine);
	}

	long l_nX = ToLong(l_sX);
	long l_nY = ToLong(l_sY);

	SP_Graphic* l_pcTransGr = l_pcTrans->AddGraphicInSubnet(l_nNet);
	l_pcTransGr->SetPosXY(l_nX, l_nY);

	m_mTransitionsGr[l_sName] = l_pcTransGr;

	l_pcTrans->ShowOnCanvas(l_pcCanvas, FALSE);
	m_mTransitions[l_sName] = l_pcTrans;
	return TRUE;
}

bool SP_ImportTINA::ParseArc(wxString& p_sLine)
{
	wxString l_sLine = p_sLine;
	// name src x y trg x y multiplicity
	// e P3p6 0.361 85.146 P3t23 0.510 150.332 5 n
	wxStringTokenizer l_cST(l_sLine,wxT(" \n\r\t"),wxTOKEN_STRTOK);
	if (!l_cST.CountTokens())
		return FALSE;

	l_cST.GetNextToken();
	wxString l_sSrc = l_cST.GetNextToken();
	wxString l_sTrg = l_cST.GetNextToken();
	double tmp;
	while (l_sTrg.ToDouble(&tmp))
	{
		l_sTrg = l_cST.GetNextToken();
	}
	wxString l_sMulti = l_cST.GetNextToken();
	long tmpl;
	while (l_sMulti.ToDouble(&tmp) && !l_sMulti.ToLong(&tmpl))
	{
		l_sMulti = l_cST.GetNextToken();
	}
	long l_nNet = 1;
	SP_DS_Edgeclass *l_pcEC = m_pcGraph->GetEdgeclass(wxT("Edge"));
	SP_DS_Edge *l_pcEdge= NULL;

	SP_DS_Node *l_pcSrc= NULL;
	SP_DS_Node *l_pcTrg= NULL;
	SP_Graphic *l_pcSrcGr= NULL;
	SP_Graphic *l_pcTrgGr= NULL;
	if (m_mPlaces.find(l_sSrc) != m_mPlaces.end())
	{
		l_pcSrc = m_mPlaces[l_sSrc];
		l_pcSrcGr = m_mPlacesGr[l_sSrc];
	}
	else if (m_mTransitions.find(l_sSrc) != m_mTransitions.end())
	{
		l_pcSrc = m_mTransitions[l_sSrc];
		l_pcSrcGr = m_mTransitionsGr[l_sSrc];
	}

	if (m_mPlaces.find(l_sTrg) != m_mPlaces.end())
	{
		l_pcTrg = m_mPlaces[l_sTrg];
		l_pcTrgGr = m_mPlacesGr[l_sTrg];
	}
	else if (m_mTransitions.find(l_sTrg) != m_mTransitions.end())
	{
		l_pcTrg = m_mTransitions[l_sTrg];
		l_pcTrgGr = m_mTransitionsGr[l_sTrg];
	}

	l_pcEdge = l_pcEC->GetPrototype()->Clone();
	l_pcEdge->SetNetnumber(l_nNet);
	l_pcEdge->SetNodes(l_pcSrc, l_pcTrg);
	l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(l_sMulti);
	l_pcEC->AddElement(l_pcEdge);

	SP_GR_Edge *l_pcEdgeGr =
			dynamic_cast<SP_GR_Edge*>(l_pcEdge->AddGraphicInSubnet(l_nNet));
	l_pcEdgeGr->SetSource(l_pcSrcGr);
	l_pcEdgeGr->SetTarget(l_pcTrgGr);

	l_pcEdge->ShowOnCanvas(m_pcView->GetCanvas(), FALSE);
	return TRUE;
}

long SP_ImportTINA::ToLong(const wxString& p_sVal)
{
	long l_nVal;
	if (p_sVal.ToLong(&l_nVal))
		return l_nVal;
	else if(p_sVal.BeforeFirst(wxT('.')).ToLong(&l_nVal))
		return l_nVal;
	else if(p_sVal.BeforeFirst(wxT(',')).ToLong(&l_nVal))
			return l_nVal;

	return -1;
}

/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////


void SP_ImportTINA::CreateGraph(const wxString& p_sFile)
{
	SP_LOGDEBUG(wxT("Creating Graph"));
	m_pcGraph = CreateDocument(p_sFile);
}

SP_DS_Graph* SP_ImportTINA::CreateDocument(const wxString& p_sFile)
{
	SP_DS_Graph* l_pcGraph= NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxString netName= SP_DS_PN_CLASS;

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

