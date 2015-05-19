/*
 * SP_DS_ResultViewer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 18.03.2011
 * Short Description:
 */ //=================================================
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/extensions/ResultViewer/AttributeTypes/SP_DS_ViewerAttributeText.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

using namespace spsa;

SP_DS_ResultViewer::SP_DS_ResultViewer(wxWindow* p_pcParent, wxSizer* p_pcSizer) :
		m_pcParent(p_pcParent), m_pcSizer(p_pcSizer), m_pcXAxisValues(NULL), m_sTitle(wxT(""))
{
	SP_DS_ViewerAttributeText* l_pcTextAttribute;

	m_aPlotCurves.clear();

	m_apcShownCurvesList.clear();

	m_asAttributeCategories.clear();

	m_pcAttributeList = new SP_ViewerAttributeList();

	l_pcTextAttribute = new SP_DS_ViewerAttributeText(wxT("TITLE"), wxT("General"), wxT("Title"));
	l_pcTextAttribute->SetValueString(m_sTitle);

	m_pcAttributeList->push_back(l_pcTextAttribute);

	//Add some categories just for test
	m_asAttributeCategories.push_back(wxT("General"));

	//Add default save types and extensions
	m_sSupportedSaveExtensions = wxT("Comma-separated values(*.csv)|*.csv");
}

SP_DS_ResultViewer::~SP_DS_ResultViewer()
{
	SP_ViewerAttributeList::iterator l_itAttribute;

	//delete individual attributes
	for (l_itAttribute = m_pcAttributeList->begin(); l_itAttribute != m_pcAttributeList->end(); l_itAttribute++)
	{
		wxDELETE((*l_itAttribute));
	}

	//delete the attribute list
	wxDELETE(m_pcAttributeList);

	ClearCurves();
}
void SP_DS_ResultViewer::Destroy()
{
	m_pcSizer->Clear(true);
}

SP_ViewerAttributeList SP_DS_ResultViewer::GetAttributesByCategory(const wxString& p_sCategory)
{
	SP_ViewerAttributeList::iterator l_itAttribute;

	SP_ViewerAttributeList l_aAttributeList;

	l_aAttributeList.clear();
	for (l_itAttribute = m_pcAttributeList->begin(); l_itAttribute != m_pcAttributeList->end(); l_itAttribute++)
	{
		if ((*l_itAttribute)->GetCategory() == p_sCategory)
		{
			l_aAttributeList.push_back((*l_itAttribute));
		}
	}

	return l_aAttributeList;
}

void SP_DS_ResultViewer::UpdateAttributes()
{
	if (GetAttributeByName(wxT("TITLE")) != NULL)
	{
		m_sTitle = (dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("TITLE"))))->GetValueString();
	}
	//TODO: Add here general viewer properties
}
void SP_DS_ResultViewer::LoadCurrentAttributeValues()
{
	if (GetAttributeByName(wxT("TITLE")) != NULL)
	{
		(dynamic_cast<SP_DS_ViewerAttributeText*>(GetAttributeByName(wxT("TITLE"))))->SetValueString(m_sTitle);
	}
}

SP_DS_ViewerAttribute* SP_DS_ResultViewer::GetAttributeByName(const wxString& p_sName)
{
	SP_ViewerAttributeList::iterator l_itAttribute;

	for (l_itAttribute = m_pcAttributeList->begin(); l_itAttribute != m_pcAttributeList->end(); l_itAttribute++)
	{
		if ((*l_itAttribute)->GetName() == p_sName)
		{
			return (*l_itAttribute);
		}
	}

	return NULL;
}
void SP_DS_ResultViewer::AddCurve(const wxString& p_sCurveName, const unsigned long& p_nDataColumn, const SP_Vector2DDouble* p_pcResultMatrix, const wxString& p_sColor, const int& p_nLineWidth,
		const int& p_nLineStyle)
{
	SP_DS_ViewerCurve* l_pcCurve;
	wxString l_sCurveColor = p_sColor;

	if (l_sCurveColor == wxT(""))
	{
		l_sCurveColor = GetColourString(p_nDataColumn);
	}
	//create a curve
	l_pcCurve = new SP_DS_ViewerCurve(p_sCurveName, p_nDataColumn, l_sCurveColor, p_nLineWidth, p_nLineStyle, p_pcResultMatrix);

	//add the position of this curve as the current number of curves
	m_mCurveName2Postion[p_sCurveName] = m_aPlotCurves.size();

	//add this curve to the list
	m_aPlotCurves.push_back(l_pcCurve);
}

void SP_DS_ResultViewer::ClearCurves()
{
	for (unsigned long l_nCurve = 0; l_nCurve < m_aPlotCurves.size(); l_nCurve++)
	{
		wxDELETE(m_aPlotCurves[l_nCurve]);
	}

	//clear name map
	m_mCurveName2Postion.clear();

	//clear pointers
	m_aPlotCurves.clear();

	//clear shown curves
	m_apcShownCurvesList.clear();
}

wxString SP_DS_ResultViewer::GetColourString(unsigned int p_nIndex)
{
	wxColour l_defaultColours[] =
	{ wxColour(255, 0, 0), wxColour(0, 255, 0), wxColour(0, 0, 255), wxColour(200, 100, 100), wxColour(200, 200, 100), wxColour(0, 200, 100), wxColour(200, 0, 100), wxColour(200, 200, 0), wxColour(
			200, 200, 200), wxColour(0, 0, 0),
	// TODO: add more colors
			};
	if (p_nIndex >= WXSIZEOF(l_defaultColours))
	{
		p_nIndex = p_nIndex % WXSIZEOF(l_defaultColours);
	}

	return l_defaultColours[p_nIndex].GetAsString(wxC2S_HTML_SYNTAX);
}

void SP_DS_ResultViewer::SetCurveLineWidth(const unsigned long& p_nIndex, const int& p_nLineWidth)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		m_aPlotCurves[p_nIndex]->SetLineWidth(p_nLineWidth);
	}
}
void SP_DS_ResultViewer::SetCurveLineWidth(const wxString& l_nName, const int& p_nLineWidth)
{
	SP_VectorCurve::iterator l_itCurve;

	for (l_itCurve = m_aPlotCurves.begin(); l_itCurve != m_aPlotCurves.end(); l_itCurve++)
	{
		if ((*l_itCurve)->GetName() == l_nName)
		{
			(*l_itCurve)->SetLineWidth(p_nLineWidth);

			return;
		}
	}
}

void SP_DS_ResultViewer::SetCurveLineStyle(const unsigned long& p_nIndex, const int& p_nLineSyle)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		m_aPlotCurves[p_nIndex]->SetLineStyle(p_nLineSyle);
	}
}
void SP_DS_ResultViewer::SetCurveLineStyle(const wxString& l_nName, const int& p_nLineSyle)
{
	SP_VectorCurve::iterator l_itCurve;

	for (l_itCurve = m_aPlotCurves.begin(); l_itCurve != m_aPlotCurves.end(); l_itCurve++)
	{
		if ((*l_itCurve)->GetName() == l_nName)
		{
			(*l_itCurve)->SetLineStyle(p_nLineSyle);

			return;
		}
	}

}

void SP_DS_ResultViewer::SetCurveColor(const unsigned long& p_nIndex, const wxString& p_sColor)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		m_aPlotCurves[p_nIndex]->SetColor(p_sColor);
	}
}

wxString SP_DS_ResultViewer::GetCurveColor(const unsigned long& p_nIndex)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		return m_aPlotCurves[p_nIndex]->GetColor();
	}
	else
	{
		return wxT("rgb(0,0,0)");
	}
}
int SP_DS_ResultViewer::GetCurveLineWidth(const unsigned long& p_nIndex)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		return m_aPlotCurves[p_nIndex]->GetLineWidth();
	}
	else
	{
		return 2;
	}
}
int SP_DS_ResultViewer::GetCurveLineStyle(const unsigned long& p_nIndex)
{
	if (p_nIndex < m_aPlotCurves.size())
	{
		return m_aPlotCurves[p_nIndex]->GetLineStyle();
	}
	else
	{
		return wxSOLID;
	}
}

void SP_DS_ResultViewer::SetCurveColor(const wxString& l_nName, const wxString& p_sColor)
{
	SP_VectorCurve::iterator l_itCurve;

	for (l_itCurve = m_aPlotCurves.begin(); l_itCurve != m_aPlotCurves.end(); l_itCurve++)
	{
		if ((*l_itCurve)->GetName() == l_nName)
		{
			(*l_itCurve)->SetColor(p_sColor);

			return;
		}
	}

}

void SP_DS_ResultViewer::ShowCurve(const unsigned long& p_nIndex, const bool& p_bShow)
{
	SP_ListCurve::iterator l_itCurve;

	if (p_nIndex >= m_aPlotCurves.size())
	{
		return;
	}
	if (p_bShow == true)
	{
		m_apcShownCurvesList.push_back(p_nIndex);
	}
	else
	{
		m_apcShownCurvesList.remove(p_nIndex);
	}

}

void SP_DS_ResultViewer::ShowCurve(const wxString& p_sName, const bool& p_bShow)
{
	SP_ListCurve::iterator l_itCurve;
	unsigned long l_nIndex = 0;

	//get the curve position using its name
	l_nIndex = m_mCurveName2Postion[p_sName];

	if (l_nIndex >= m_aPlotCurves.size())
	{
		return;
	}
	if (p_bShow == true)
	{
		m_apcShownCurvesList.push_back(l_nIndex);
	}
	else
	{
		m_apcShownCurvesList.remove(l_nIndex);
	}

}
void SP_DS_ResultViewer::SaveOutput(const wxString& p_sFileName, const int& p_nFileType)
{
	/*wxString l_sSpacer=wxT(" ");
	 wxString l_sOutput;

	 if(p_nFileType==0)
	 {
	 //open the file
	 wxFileOutputStream* l_pcExportFileOutputStream = new wxFileOutputStream(p_sFileName);
	 if(!l_pcExportFileOutputStream->IsOk())
	 {
	 return;
	 }
	 wxTextOutputStream*  l_pcExport = new wxTextOutputStream(*l_pcExportFileOutputStream);
	 for (unsigned long l_nRow=0; l_nRow < m_pcXAxisValues.size(); l_nRow++)
	 {
	 l_sOutput=wxT("");
	 l_sOutput << wxString::Format(wxT("%g"), m_pcXAxisValues[l_nRow]);

	 for(unsigned long l_nItem=0;l_nItem<m_apcShownCurvesList.size();l_nItem++)
	 {
	 l_sOutput<<l_sSpacer;
	 double l_nValue=(*m_pcResultMatrix)[l_nRow][m_apcShownCurvesList[l_nItem]];
	 l_sOutput<<wxString::Format(wxT("%.16g"),l_nValue);

	 }
	 *l_pcExport << l_sOutput << wxT("\n");
	 }
	 }*/

}

void SP_DS_ResultViewer::SaveViewToServerFormat(spsa::ModelView* p_pcServerSideView)
{
	std::list<unsigned int>::const_iterator l_itShownCurve;

	if (p_pcServerSideView == NULL)
	{
		return;
	}

	//set view title
	p_pcServerSideView->SetTitle(m_sTitle);

}
void SP_DS_ResultViewer::LoadViewFromServerFormat(spsa::ModelView* p_pcServerSideView)
{

	if (p_pcServerSideView == NULL)
	{
		return;
	}

	//clear old shown curves
	m_apcShownCurvesList.clear();

	m_sTitle = p_pcServerSideView->GetTitle();
}

void SP_DS_ResultViewer::SaveViewToSnoopyFormat(SP_DS_Metadata* p_pcView)
{

	CHECK_POINTER(p_pcView, return);

	//title
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("ViewTitle"));
	CHECK_POINTER(l_pcAttribute, return)
	l_pcAttribute->SetValueString(m_sTitle);

}
void SP_DS_ResultViewer::LoadViewFromSnoopyFormat(SP_DS_Metadata* p_pcView)
{
	CHECK_POINTER(p_pcView, return);

	//title
	SP_DS_Attribute* l_pcAttribute = p_pcView->GetAttribute(wxT("ViewTitle"));
	CHECK_POINTER(l_pcAttribute, return)
	m_sTitle = l_pcAttribute->GetValueString();
}
