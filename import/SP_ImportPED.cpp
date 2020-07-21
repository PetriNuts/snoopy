//////////////////////////////////////////////////////////////////////
// $Author:ms$
// $Version: 0.1 $
// $Date: 2007/07/12 $
//////////////////////////////////////////////////////////////////////

#include "import/SP_ImportPED.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_core/SP_Core.h"

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

SP_ImportPED::SP_ImportPED()
{

}

SP_ImportPED::~SP_ImportPED()
{

}

bool SP_ImportPED::ReadFile(const wxString& p_sFile,SP_ImportRoutine* p_sIR)
{
	g_bError = FALSE;
	g_sMessage = wxT("");
	m_pcGraph = NULL;
	m_mPlaces.clear();
	m_mTransitions.clear();

	wxTextFile l_cFile;
	if(l_cFile.Open(p_sFile))
	{
		CreateGraph(p_sFile);
		if(m_pcGraph)
		{
			Parse(l_cFile);
		}
		l_cFile.Close();
	}

	return TRUE;
}

/////////////////////////////////////Parsing PED/////////////////////////////////////////////////////////

bool SP_ImportPED::Parse(wxTextFile& p_cFile)
{
	wxString l_sLog = wxT("");
	SP_LOGDEBUG(wxString(p_cFile.GetName()).AfterLast(wxFILE_SEP_PATH));

	wxString l_sLine = p_cFile.GetFirstLine();
	wxString l_sMatch;
	wxRegEx l_cExp(wxT("# PED V4.3 petri net"));
	if (!l_cExp.Matches(l_sLine))
	{
		SP_LOGDEBUG(wxT(" No V4.3 ped file!"));
		SP_MESSAGEBOX(wxT(" No V4.3 ped file!"));
		return FALSE;
	}

	for (; !g_bError && (p_cFile.GetCurrentLine()
			< (p_cFile.GetLineCount() - 2)); l_sLine = p_cFile.GetNextLine())
	{
		//       if (l_sLine.Cmp(wxT("V { 7")) == 0)
		// 	int a = 1;

		l_cExp.Compile(wxT("([CVPTA]) \\{(.*)"));
		if (!l_cExp.Matches(l_sLine))
		{
			continue;
		}
		l_sMatch = l_cExp.GetMatch(l_sLine, 0);
		wxChar c = l_sMatch.GetChar(0);
		switch (c)
		{
		case 80: // 'P'
			g_bError = !ParsePlace(p_cFile, l_cExp.GetMatch(l_sLine, 2).Trim(FALSE));
			break;
		case 84: // 'T'
			g_bError = !ParseTrans(p_cFile, l_cExp.GetMatch(l_sLine, 2).Trim(FALSE));
			break;
		case 86: // 'V'
			g_bError = !ParseCoarse(p_cFile, l_cExp.GetMatch(l_sLine, 2).Trim(FALSE));
			break;
		case 67: // 'C'
			g_bError = !ParseComment(p_cFile);
			break;
		case 65: // 'A'
			g_bError = !ParseArc(p_cFile);
		};

		l_sMatch = l_cExp.GetMatch(l_sLine, 1);
	}
	if (g_bError)
		SP_LOGDEBUG(wxString::Format(wxT(" error in line %lu"), p_cFile.GetCurrentLine()));

	return g_bError;
}

bool SP_ImportPED::ParseComment(wxTextFile& p_cFile)
{
	wxString l_sLine = p_cFile.GetNextLine();
	//     net x  y
	//     N 1 75 75
	wxRegEx l_cExp(wxT(".+ N ([0-9]+) ([0-9]+) ([0-9]+)"));
	if (!l_cExp.Matches(l_sLine))
		return FALSE;

	wxString l_sNet = l_cExp.GetMatch(l_sLine, 1);
	wxString l_sX = l_cExp.GetMatch(l_sLine, 2);
	wxString l_sY = l_cExp.GetMatch(l_sLine, 3);

	long l_nNet = ToLong(l_sNet)+1;

	SP_DS_Metadataclass *metaClass = m_pcGraph->GetMetadataclass(wxT("Comment"));
	SP_DS_Metadata *l_pcNode = metaClass->NewElement(l_nNet);
	SP_Graphic *l_pcNodeGr = l_pcNode->AddGraphicInSubnet(l_nNet);
	long l_nX = ToLong(l_sX);
	long l_nY = ToLong(l_sY);
	l_pcNodeGr->SetPosXY(l_nX, l_nY);

	l_sLine = p_cFile.GetNextLine();
	wxString l_sText = wxT("");
	while (l_sLine.Cmp(wxT("}")) != 0)
	{
		l_sLine.Replace(wxT("    "), wxT(""), FALSE);
		if (l_sLine.StartsWith(wxT("\"")))
			l_sLine.Replace(wxT("\""), wxT(""), FALSE);
		if (l_sLine.EndsWith(wxT("\"")))
			l_sLine.RemoveLast();

		l_sText.Append(wxT("\n"));
		l_sText.Append(l_sLine);

		l_sLine = p_cFile.GetNextLine();
	}

	l_pcNode->GetAttribute(wxT("Comment"))->SetValueString(l_sText);
	l_pcNode->ShowOnCanvas(m_pcCanvas, FALSE);
	return TRUE;
}

bool SP_ImportPED::ParsePlace(wxTextFile& p_cFile, wxString& p_sId)
{
	long l_nId = ToLong(p_sId);
	wxString l_sLine = p_cFile.GetNextLine();
	//     name        capacity mark delay interval type (0 = normal, 1 = logic)
	//     wxT("pos1_free") 0        1    0     [0,0]    1
	wxRegEx l_cExp(wxT(".+ \"(.*)\" [0-9]+ ([0-9]+) [0-9]+ \\[.+\\] ([01])"));
	if (!l_cExp.Matches(l_sLine))
		return false;

	wxString l_sName = l_cExp.GetMatch(l_sLine, 1);
	wxString l_sMark = l_cExp.GetMatch(l_sLine, 2);
	wxString l_sLogic = l_cExp.GetMatch(l_sLine, 3);

	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Place"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_pcPlace = NULL;

	if(l_sLogic == wxT("1"))
	{
		l_pcPlace = m_mPlaces[l_nId];
	}

	if(l_pcPlace == NULL)
	{
		l_pcPlace = nodeClass->NewElement(0);
	}

	if (!l_pcPlace)
		return FALSE;

	l_pcPlace->GetAttribute(wxT("Name"))->SetValueString(l_sName);
	l_pcPlace->GetAttribute(wxT("Name"))->SetShow(TRUE);
	l_pcPlace->GetAttribute(wxT("Marking"))->SetValueString(l_sMark);
	l_pcPlace->GetAttribute(wxT("Logic"))->SetValueString(l_sLogic);

	// parse next lines to comment
	wxString l_sComment = wxT("");
	l_sLine = p_cFile.GetNextLine();
	while (!l_sLine.Trim(FALSE).Trim().StartsWith(wxT("M ")))
	{
		if (!l_sLine.IsEmpty())
		{
			l_sLine.Replace(wxT("\\\""), wxT("\""));
			l_sLine.Replace(wxT("\\\\"), wxT("\\"));
			if (l_sLine.StartsWith(wxT("\"")))
				l_sLine.Replace(wxT("\""), wxT(""), FALSE);
			if (l_sLine.EndsWith(wxT("\"")))
				l_sLine.RemoveLast();

			if (!l_sComment.IsEmpty())
				l_sComment.Append(wxT("\n"));
			l_sComment.Append(l_sLine);
		}

		l_sLine = p_cFile.GetNextLine();
	}
	// comment attribute
	l_pcPlace->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);

	// extract graphics
	// mode id    net x   y   attribute
	// M 1  I 263 N 0 140 200 -44 27 35 -40
	//                            id = 1     net = 2   x = 3     y = 4     ax = 5    ay = 6
	l_cExp.Compile(wxT("M ([012]) I ([0-9]+) N ([0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+)"));

	//wxString l_sLine = p_cFile.GetNextLine();
	while (l_sLine.Trim(FALSE).Trim().Cmp(wxT("}")) != 0)
	{
		if (l_cExp.Matches(l_sLine))
		{
			long l_nMode = ToLong(l_cExp.GetMatch(l_sLine, 1));
			wxString l_sId = l_cExp.GetMatch(l_sLine, 2);
			long l_nIdGr = ToLong(l_sId);
			wxString l_sNet = l_cExp.GetMatch(l_sLine, 3);
			wxString l_sX = l_cExp.GetMatch(l_sLine, 4);
			wxString l_sY = l_cExp.GetMatch(l_sLine, 5);
			wxString l_sAX = l_cExp.GetMatch(l_sLine, 6);
			wxString l_sAY = l_cExp.GetMatch(l_sLine, 7);
			wxString l_sAlgX = l_cExp.GetMatch(l_sLine, 8);
			wxString l_sAlgY = l_cExp.GetMatch(l_sLine, 9);
			long l_nNet = ToLong(l_sNet)+1;
			long l_nX = ToLong(l_sX);
			long l_nY = ToLong(l_sY);
			long l_nAX = ToLong(l_sAX);
			long l_nAY = ToLong(l_sAY);
			long l_nAlgX = ToLong(l_sAlgX);
			long l_nAlgY = ToLong(l_sAlgY);
			SP_Graphic* l_pcPlaceGr = NULL;
			if(!l_pcPlace->GetNetnumber())
			{
				l_pcPlace->SetNetnumber(l_nNet);
				l_pcPlaceGr = l_pcPlace->GetGraphicInSubnet(l_nNet);
			}
			else
			{
				l_pcPlaceGr = l_pcPlace->NewGraphicInSubnet(l_nNet);
			}
			l_pcPlaceGr->SetPosXY(l_nX, l_nY);
			if (l_nMode == 1)
				l_pcPlaceGr->SetGraphicState(SP_STATE_COARSEBORDERTOP);
			if (l_nMode == 2)
				l_pcPlaceGr->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
			for(SP_ListGraphic::iterator itAttrGr = l_pcPlaceGr->GetGraphicChildren()->begin();
				itAttrGr != l_pcPlaceGr->GetGraphicChildren()->end(); itAttrGr++)
			{
				SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>((*itAttrGr)->GetParent());
				if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Name")))
				{
					(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
				}else if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("ID")))
				{
					(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
				}else if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Comment")))
				{
					(*itAttrGr)->SetPosXY(l_nAlgX, l_nAlgY);
				}
			}
			m_mPlacesGr[l_nIdGr] = l_pcPlaceGr;
		}
		l_sLine = p_cFile.GetNextLine();
	}
	l_pcPlace->ShowOnCanvas(l_pcCanvas, FALSE);
	m_mPlaces[l_nId] = l_pcPlace;
	return TRUE;
}

bool SP_ImportPED::ParseTrans(wxTextFile& p_cFile, wxString& p_sId)
{
	long l_nId = ToLong(p_sId);
	wxString l_sLine = p_cFile.GetNextLine();
	//     name        priority delay interval type shape
	//     wxT("pos1_free") 0        1     [0,0]    0    0
	wxRegEx l_cExp(wxT(".+ \"(.*)\" [0-9]+ [0-9]+ \\[.+\\] ([012345]) [0-9]+"));
	if (!l_cExp.Matches(l_sLine))
		return false;

	wxString l_sName = l_cExp.GetMatch(l_sLine, 1);
	wxString l_sLogic = l_cExp.GetMatch(l_sLine, 2);

	SP_DS_Nodeclass *nodeClass = m_pcGraph->GetNodeclass(wxT("Transition"));
	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_pcTrans = NULL;

	if (l_sLogic.Cmp(wxT("1")) == 0 || // logic
			l_sLogic.Cmp(wxT("3")) == 0 || // fact and logic
			l_sLogic.Cmp(wxT("4")) == 0) // filled and logic
	{
		l_pcTrans = m_mTransitions[l_nId];
	}

	if(l_pcTrans == NULL)
	{
		l_pcTrans = nodeClass->NewElement(0);
	}

	if (!l_pcTrans)
		return FALSE;

	l_pcTrans->GetAttribute(wxT("Name"))->SetValueString(l_sName);
	l_pcTrans->GetAttribute(wxT("Name"))->SetShow(TRUE);

	if (l_sLogic.Cmp(wxT("1")) == 0 || // logic
			l_sLogic.Cmp(wxT("3")) == 0 || // fact and logic
			l_sLogic.Cmp(wxT("4")) == 0) // filled and logic
	{
		l_pcTrans->GetAttribute(wxT("Logic"))->SetValueString(wxT("1"));
	}

	// parse next lines to comment
	wxString l_sComment = wxT("");
	l_sLine = p_cFile.GetNextLine();
	while (!l_sLine.Trim(FALSE).Trim().StartsWith(wxT("M ")))
	{
		if (!l_sLine.IsEmpty())
		{
			l_sLine.Replace(wxT("\\\""), wxT("\""));
			l_sLine.Replace(wxT("\\\\"), wxT("\\"));
			if (l_sLine.StartsWith(wxT("\"")))
				l_sLine.Replace(wxT("\""), wxT(""), FALSE);
			if (l_sLine.EndsWith(wxT("\"")))
				l_sLine.RemoveLast();

			if (!l_sComment.IsEmpty())
				l_sComment.Append(wxT("\n"));
			l_sComment.Append(l_sLine);
		}

		l_sLine = p_cFile.GetNextLine();
	}
	// comment attribute
	l_pcTrans->GetAttribute(wxT("Comment"))->SetValueString(l_sComment);


	// extract graphics
	// mode id    net x   y   attribute
	// M 1  I 263 N 0 140 200 -44 27 35 -40
	//                           id = 1     net = 2   x = 3     y = 4     ax = 5    ay = 6   alg x   alg y   angel
	l_cExp.Compile(wxT("M ([012]) I ([0-9]+) N ([0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) [-0-9]+"));

	//l_sLine = p_cFile.GetNextLine();
	while (l_sLine.Trim(FALSE).Trim().Cmp(wxT("}")) != 0)
	{
		if (l_cExp.Matches(l_sLine))
		{
			long l_nMode = ToLong(l_cExp.GetMatch(l_sLine, 1));
			wxString l_sId = l_cExp.GetMatch(l_sLine, 2);
			wxString l_sNet = l_cExp.GetMatch(l_sLine, 3);
			wxString l_sX = l_cExp.GetMatch(l_sLine, 4);
			wxString l_sY = l_cExp.GetMatch(l_sLine, 5);
			wxString l_sAX = l_cExp.GetMatch(l_sLine, 6);
			wxString l_sAY = l_cExp.GetMatch(l_sLine, 7);
			wxString l_sAlgX = l_cExp.GetMatch(l_sLine, 8);
			wxString l_sAlgY = l_cExp.GetMatch(l_sLine, 9);
			long l_nNet = ToLong(l_sNet)+1;
			long l_nX = ToLong(l_sX);
			long l_nY = ToLong(l_sY);
			long l_nAX = ToLong(l_sAX);
			long l_nAY = ToLong(l_sAY);
			long l_nAlgX = ToLong(l_sAlgX);
			long l_nAlgY = ToLong(l_sAlgY);
			SP_Graphic* l_pcTransGr = NULL;
			if(!l_pcTrans->GetNetnumber())
			{
				l_pcTrans->SetNetnumber(l_nNet);
				l_pcTransGr = l_pcTrans->GetGraphicInSubnet(l_nNet);
			}
			else
			{
				l_pcTransGr = l_pcTrans->NewGraphicInSubnet(l_nNet);
			}

			l_pcTransGr->SetPosXY(l_nX, l_nY);
			if (l_nMode == 1)
				l_pcTransGr->SetGraphicState(SP_STATE_COARSEBORDERTOP);
			if (l_nMode == 2)
				l_pcTransGr->SetGraphicState(SP_STATE_COARSEBORDERDOWN);
			for(SP_ListGraphic::iterator itAttrGr = l_pcTransGr->GetGraphicChildren()->begin();
				itAttrGr != l_pcTransGr->GetGraphicChildren()->end(); itAttrGr++)
			{
				SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>((*itAttrGr)->GetParent());
				if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Name")))
				{
					(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
				}else if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("ID")))
				{
					(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
				}else if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Comment")))
				{
					(*itAttrGr)->SetPosXY(l_nAlgX, l_nAlgY);
				}
			}
			m_mTransitionsGr[ToLong(l_sId)] = l_pcTransGr;
		}
		l_sLine = p_cFile.GetNextLine();
	}
	l_pcTrans->ShowOnCanvas(l_pcCanvas, FALSE);
	m_mTransitions[l_nId] = l_pcTrans;
	return TRUE;
}

bool SP_ImportPED::ParseCoarse(wxTextFile& p_cFile, wxString& p_sId)
{
	long l_nId = ToLong(p_sId);
	wxString l_sLine = p_cFile.GetNextLine();
	//     name       subnet type
	//     wxT("consumer") 1      T
	wxRegEx l_cExp(wxT(".+ \"(.*)\" ([0-9]+) ([PT])"));
	if (!l_cExp.Matches(l_sLine))
		return FALSE;

	wxString l_sName = l_cExp.GetMatch(l_sLine, 1);
	wxString l_sNet = l_cExp.GetMatch(l_sLine, 2);
	wxString l_sType = l_cExp.GetMatch(l_sLine, 3);

	long l_nCoarseNet = ToLong(l_sNet)+1;

	SP_DS_Nodeclass *nodeClass = NULL;
	if (l_sType.Cmp(wxT("T")) == 0)
		nodeClass = m_pcGraph->GetNodeclass(wxT("Coarse Transition"));
	else if (l_sType.Cmp(wxT("P")) == 0)
		nodeClass = m_pcGraph->GetNodeclass(wxT("Coarse Place"));
	else
		return FALSE;

	SP_GUI_Canvas* l_pcCanvas = m_pcView->GetCanvas();
	SP_DS_Node *l_pcCoarse = NULL;

	// extract graphics
	// mode id    net x   y   attribute
	// M 0  I 264 N 0 220 280 13 13
	//                         id = 1     net = 2  x = 3     y = 4     ax = 5    ay = 6
	l_cExp.Compile(wxT("M 0 I ([0-9]+) N ([0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+) ([-0-9]+)"));

	l_sLine = p_cFile.GetNextLine();
	while (l_sLine.Trim(FALSE).Trim().Cmp(wxT("}")) != 0)
	{
		if (l_cExp.Matches(l_sLine))
		{
			wxString l_sId = l_cExp.GetMatch(l_sLine, 1);
			wxString l_sNet = l_cExp.GetMatch(l_sLine, 2);
			wxString l_sX = l_cExp.GetMatch(l_sLine, 3);
			wxString l_sY = l_cExp.GetMatch(l_sLine, 4);
			wxString l_sAX = l_cExp.GetMatch(l_sLine, 5);
			wxString l_sAY = l_cExp.GetMatch(l_sLine, 6);
			long l_nNet = ToLong(l_sNet)+1;
			long l_nX = ToLong(l_sX);
			long l_nY = ToLong(l_sY);
			long l_nAX = ToLong(l_sAX);
			long l_nAY = ToLong(l_sAY);
			if(!l_pcCoarse)
			{
				l_pcCoarse = nodeClass->NewElement(l_nNet);
				l_pcCoarse->GetCoarse()->SetNetnumber(l_nCoarseNet);
				l_pcCoarse->GetAttribute(wxT("Name"))->SetValueString(l_sName);
			}
			SP_Graphic* l_pcCoarseGr = l_pcCoarse->AddGraphicInSubnet(l_nNet);
			l_pcCoarseGr->SetPosXY(l_nX, l_nY);
			l_pcCoarseGr->SetGraphicState(SP_STATE_COARSECOARSE);
			for(SP_ListGraphic::iterator itAttrGr = l_pcCoarseGr->GetGraphicChildren()->begin();
				itAttrGr != l_pcCoarseGr->GetGraphicChildren()->end(); itAttrGr++)
			{
				SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>((*itAttrGr)->GetParent());
				if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Name")))
				{
					(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
				}
			}
			m_mCoarseGr[ToLong(l_sId)] = l_pcCoarseGr;
		}
		else if (!l_sLine.Contains(wxT('"')))
			return FALSE;
		l_sLine = p_cFile.GetNextLine();
	}
	l_pcCoarse->ShowOnCanvas(l_pcCanvas,FALSE);
	m_mCoarse[l_nId] = l_pcCoarse;
	return TRUE;
}

bool SP_ImportPED::ParseArc(wxTextFile& p_cFile)
{

	wxString l_sLine = p_cFile.GetNextLine();
	// multiplicity delay interv type shape
	//                           0..3 0..4
	// 1            0     [0,0]  0    0
	wxRegEx l_cExp(wxT(".+ ([0-9]+) [0-9]+ \\[.+\\] [0123] [01234]"));
	if (!l_cExp.Matches(l_sLine))
		return FALSE;

	wxString l_sMulti = l_cExp.GetMatch(l_sLine, 1);

	SP_DS_Edgeclass *l_pcEC = m_pcGraph->GetEdgeclass(wxT("Edge"));
	SP_DS_Edge *l_pcEdge = NULL;
	// mode id   net src            target        ax ay gx gy #p px  py  px  py
	// M 0  I 74 N 4 P 16 I 134 N 4 T 8 I 119 N 4 10 10 0  40 2  140 550 140 580
	l_cExp.Compile(wxT("M ([0123]) I ([0-9]+) N ([0-9]+) ")
		wxT("([PTV]) ([0-9]+) I ([0-9]+) N ([0-9]+) ")
		wxT("([PTV]) ([0-9]+) I ([0-9]+) N ([0-9]+) ")
		wxT("([-0-9]+) ([-0-9]+) ")
		wxT("[-0-9]+ [-0-9]+ ")
		wxT("([0-9]+) ([ 0-9]+)"));

	p_cFile.GetNextLine();
	l_sLine = p_cFile.GetNextLine();
	while (l_sLine.Trim(FALSE).Trim().Cmp(wxT("}")) != 0)
	{
		if (l_cExp.Matches(l_sLine))
		{
			long l_nMode = ToLong(l_cExp.GetMatch(l_sLine, 1));
			wxString l_sId = l_cExp.GetMatch(l_sLine, 2);
			wxString l_sNet = l_cExp.GetMatch(l_sLine, 3);
			long l_nNet = ToLong(l_sNet)+1;
			wxString l_sSrcT = l_cExp.GetMatch(l_sLine, 4); // P T V
			wxString l_sSrc = l_cExp.GetMatch(l_sLine, 5);
			wxString l_sSrcGr = l_cExp.GetMatch(l_sLine, 6);
			SP_DS_Node *l_pcSrc = NULL;
			SP_DS_Node *l_pcTrg = NULL;
			SP_Graphic *l_pcSrcGr = NULL;
			SP_Graphic *l_pcTrgGr = NULL;
			if(l_sSrcT.IsSameAs(wxT("P")))
			{
				l_pcSrc = m_mPlaces[ToLong(l_sSrc)];
				l_pcSrcGr = m_mPlacesGr[ToLong(l_sSrcGr)];
			}
			else if(l_sSrcT.IsSameAs(wxT("T")))
			{
				l_pcSrc = m_mTransitions[ToLong(l_sSrc)];
				l_pcSrcGr = m_mTransitionsGr[ToLong(l_sSrcGr)];
			}
			else if(l_sSrcT.IsSameAs(wxT("V")))
			{
				l_pcSrc = m_mCoarse[ToLong(l_sSrc)];
				l_pcSrcGr = m_mCoarseGr[ToLong(l_sSrcGr)];
			}

			wxString l_sTrgT = l_cExp.GetMatch(l_sLine, 8); // P T V
			wxString l_sTrg = l_cExp.GetMatch(l_sLine, 9);
			wxString l_sTrgGr = l_cExp.GetMatch(l_sLine, 10);
			if(l_sTrgT.IsSameAs(wxT("P")))
			{
				l_pcTrg = m_mPlaces[ToLong(l_sTrg)];
				l_pcTrgGr = m_mPlacesGr[ToLong(l_sTrgGr)];
			}
			else if(l_sTrgT.IsSameAs(wxT("T")))
			{
				l_pcTrg = m_mTransitions[ToLong(l_sTrg)];
				l_pcTrgGr = m_mTransitionsGr[ToLong(l_sTrgGr)];
			}
			else if(l_sTrgT.IsSameAs(wxT("V")))
			{
				l_pcTrg = m_mCoarse[ToLong(l_sTrg)];
				l_pcTrgGr = m_mCoarseGr[ToLong(l_sTrgGr)];
			}

			wxString l_sAX = l_cExp.GetMatch(l_sLine, 12);
			wxString l_sAY = l_cExp.GetMatch(l_sLine, 13);
			long l_nAX = ToLong(l_sAX);
			long l_nAY = ToLong(l_sAY);

			wxString l_sNrP = l_cExp.GetMatch(l_sLine, 14);
			wxString l_sPoints = l_cExp.GetMatch(l_sLine, 15);



			if (l_nMode == 0 || l_nMode == 1)
			{
				l_pcEdge = l_pcEC->GetPrototype()->Clone();
				l_pcEdge->SetNetnumber(l_nNet);
				l_pcEdge->SetNodes(l_pcSrc, l_pcTrg);
				l_pcEdge->GetAttribute(wxT("Multiplicity"))->SetValueString(l_sMulti);
				l_pcEC->AddElement(l_pcEdge);
			}

			if (l_nMode != 1)
			{
				SP_GR_Edge *l_pcEdgeGr = dynamic_cast<SP_GR_Edge*>(l_pcEdge->AddGraphicInSubnet(l_nNet));
				l_pcEdgeGr->SetSource(l_pcSrcGr);
				l_pcEdgeGr->SetTarget(l_pcTrgGr);

				if (l_nMode == 3)
					l_pcEdgeGr->SetGraphicState(SP_STATE_COARSEBORDERTOP);
				if (l_nMode == 2)
					l_pcEdgeGr->SetGraphicState(SP_STATE_COARSEBORDERDOWN);

				wxStringTokenizer l_cTkn(l_sPoints, wxT(" "));
			    wxList l_lPoints;
			    while (l_cTkn.HasMoreTokens())
			    {
			    	wxString l_sX = l_cTkn.GetNextToken();
			    	wxString l_sY = l_cTkn.GetNextToken();
			        l_lPoints.Append((wxObject*)new wxRealPoint(ToLong(l_sX), ToLong(l_sY)));

			    }
				l_pcEdgeGr->SetControlPoints(&l_lPoints);

				for(SP_ListGraphic::iterator itAttrGr = l_pcEdgeGr->GetGraphicChildren()->begin();
					itAttrGr != l_pcEdgeGr->GetGraphicChildren()->end(); itAttrGr++)
				{
					SP_DS_Attribute* l_pcAttr = dynamic_cast<SP_DS_Attribute*>((*itAttrGr)->GetParent());
					if(wxString(l_pcAttr->GetName()).IsSameAs(wxT("Multiplicity")))
					{
						(*itAttrGr)->SetPosXY(l_nAX, l_nAY);
					}
				}
			}
		}
		else if (!l_sLine.Contains(wxT('"')))
			return FALSE;
		l_sLine = p_cFile.GetNextLine();
	}
	l_pcEdge->ShowOnCanvas(m_pcView->GetCanvas(), FALSE);
	return TRUE;
}

long SP_ImportPED::ToLong(const wxString& p_sVal)
{
	long l_nVal;
	if (p_sVal.ToLong(&l_nVal))
		return l_nVal;

	return -1;
}

/////////////////////////////////////////////////Graph Creation////////////////////////////////////////////////////


void SP_ImportPED::CreateGraph(const wxString& p_sFile)
{
	SP_LOGDEBUG(wxT("Creating Graph"));
	m_pcGraph = CreateDocument(p_sFile);
}

SP_DS_Graph* SP_ImportPED::CreateDocument(const wxString& p_sFile)
{
	SP_DS_Graph* l_pcGraph = NULL;
	SP_GM_Docmanager* l_pcDM = wxGetApp().GetDocmanager();
	wxString netName = SP_DS_PN_CLASS;

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

