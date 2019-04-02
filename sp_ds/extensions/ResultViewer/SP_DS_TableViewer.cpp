/*
 * SP_DS_TableViewer.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.06.2011
 * Short Description:
 *///=================================================
#include <dssd/auxi/auxi.h>
#include "sp_utilities.h"
#include "sp_core/SP_Core.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_ResultViewer.h"
#include "sp_ds/extensions/ResultViewer/SP_DS_TableViewer.h"

SP_DS_TableViewer::SP_DS_TableViewer(wxWindow* p_pcParrent,wxSizer* p_pcSizer)
:SP_DS_ResultViewer(p_pcParrent,p_pcSizer)
{
	           m_sViewerName=wxT("Tabular");
}

SP_DS_TableViewer::~SP_DS_TableViewer()
{

}

void SP_DS_TableViewer::Create()
{
	        m_pcStaticTextBox=new wxStaticText(m_pcParent,wxID_ANY,m_sTitle);

			m_pcSizer->Add(m_pcStaticTextBox , 0, wxALL, 5);

	        //Create the table Ctrl
			m_pcTableViewGrid = new wxGrid( m_pcParent, wxID_ANY,	wxDefaultPosition, wxSize( 600, 600 ), wxSUNKEN_BORDER );
			m_pcTableViewGrid->CreateGrid( 0, 0 );
			m_pcTableViewGrid->EnableEditing( false );
			m_pcTableViewGrid->Show( true );

		    m_pcSizer->Add(m_pcTableViewGrid, 1, wxEXPAND | wxALL, 5);

		    m_pcSizer->Layout();

		   // m_pcTableViewGrid->SetEventHandler(OnLabelDClick);
}

void SP_DS_TableViewer::Destroy()
{
			 SP_DS_ResultViewer::Destroy();
}
//Added By G.A
  void  SP_DS_TableViewer::UpdateMembershipViewer(double TimePoint)
{}
void SP_DS_TableViewer::Update()
{
	std::list<unsigned int>::iterator l_itCurve;
	int l_nCol = 0;

	wxString m_sNetClassName = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNetclass()->GetName();
	CHECK_POINTER(m_pcXAxisValues, return);

	CHECK_POINTER(m_pcTableViewGrid, return);

	m_pcStaticTextBox->SetLabel(m_sTitle);

	//delete the old rows if there are any
	if (m_pcTableViewGrid->GetNumberRows() > 0)
	{
		m_pcTableViewGrid->DeleteRows(0, m_pcTableViewGrid->GetNumberRows());
	}

	//delete the old cols if there are any
	if (m_pcTableViewGrid->GetNumberCols() > 0)
	{
		m_pcTableViewGrid->DeleteCols(0, m_pcTableViewGrid->GetNumberCols());
	}

	//Add some column equal to the item counts
	if (!m_sNetClassName.Contains(wxT("Fuzzy")))
	{
		m_pcTableViewGrid->AppendCols(m_apcShownCurvesList.size());
	}

	m_pcTableViewGrid->AppendRows(m_pcXAxisValues->size());

	if (!m_sNetClassName.Contains(wxT("Fuzzy")))
	{
	//Add the column labels
	for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
	{

		m_pcTableViewGrid->SetColLabelValue(l_nCol++, m_aPlotCurves[(*l_itCurve)]->GetName());
	}
    }
	else
	{//fuzzy curve
		for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
		{//Add Columns
			unsigned int index = (*l_itCurve);
			SP_DS_Viewer_Fuzzy_Curve* m_fcurve = m_aPlotFuzzyCurves[index];
			//std::vector<SP_Vector2DDouble> m_lfResultMatrices = m_fcurve->GetFuzzyResultMatrix();
			//m_pcTableViewGrid->AppendCols(m_lfResultMatrices.size()*m_apcShownCurvesList.size());
			break;
		}
		m_pcTableViewGrid->AppendCols(m_apcShownCurvesList.size()*2);
		
		for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
		{
			unsigned int index = (*l_itCurve);
			SP_DS_Viewer_Fuzzy_Curve* m_fcurve = m_aPlotFuzzyCurves[index];
			
			wxString sColName = m_aPlotFuzzyCurves[(*l_itCurve)]->GetName();
			unsigned int numOfFn = 0;
			 
			wxString sCol;
			sCol << sColName << "_Min";
			 
			m_pcTableViewGrid->SetColLabelValue(l_nCol++, sCol);
			wxString sCol1;
			sCol1 << sColName << "_Max";
			m_pcTableViewGrid->SetColLabelValue(l_nCol++, sCol1);
		 
			
		}

	}

	//change row label
	for (int l_nRow = 0; l_nRow < m_pcTableViewGrid->GetNumberRows(); l_nRow++)
	{
		wxString l_sRowValue = wxString::Format(wxT("%d"), l_nRow);

		m_pcTableViewGrid->SetRowLabelValue(l_nRow, l_sRowValue);
	}

	

	if (!m_sNetClassName.Contains(wxT("Fuzzy")))//Added by G.A
	{
	
	//Fill in the table with data
	l_nCol = 0;
	for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
	{
		for (int l_nRow = 0; l_nRow < m_pcTableViewGrid->GetNumberRows(); l_nRow++)
		{
			wxString l_sYValue = dssd::aux::toString(m_aPlotCurves[(*l_itCurve)]->GetValue(l_nRow));
			m_pcTableViewGrid->SetCellValue(l_nRow, l_nCol, l_sYValue);
		}
		l_nCol++;
	}

   }
	else {
		//Fuzzy Curves
		l_nCol = 0;
		int colSize = 0;
		bool binit = false;
		int num = 0;
		for (l_itCurve = m_apcShownCurvesList.begin(); l_itCurve != m_apcShownCurvesList.end(); l_itCurve++)
		{
			num++;
			//binit = true;
			unsigned int index = (*l_itCurve);
			SP_DS_Viewer_Fuzzy_Curve* m_fcurve = m_aPlotFuzzyCurves[index];
			SP_Compressed_Fuzzy_Band p_pcCompressedBand = m_fcurve->GetResultMatrix();
			// m_lfResultMatrices = m_fcurve->GetResultMatrix();
			std::vector<SP_Vector2DDouble> m_minMaxBands;
			m_minMaxBands.clear();
			std::vector<vector<double>> minCurve = p_pcCompressedBand.GetMinimumCurve();
			std::vector<vector<double>> maxCurve = p_pcCompressedBand.GetMaximumCurve();
			
			m_minMaxBands.push_back(minCurve);
			m_minMaxBands.push_back(maxCurve);

			long m_lnumofMatricies = m_minMaxBands.size();
			colSize = m_lnumofMatricies;
			for (int l_nRow = 0; l_nRow < m_pcTableViewGrid->GetNumberRows(); l_nRow++)
			{
				for (int i = 0; i < m_lnumofMatricies; i++)
				{
					const SP_Vector2DDouble m_nResMat = m_minMaxBands[i];
					
					wxString l_sYValue = dssd::aux::toString(m_aPlotFuzzyCurves[(*l_itCurve)]->GetValue(l_nRow, m_nResMat));
					 
					m_pcTableViewGrid->SetCellValue(l_nRow, l_nCol, l_sYValue);
					l_nCol++;
					//binit = true;
				}
				if (binit == false)
				{
					l_nCol = 0;
				}
				else {
					l_nCol = (num - 1)*colSize ;
				}
			}
			l_nCol = num*colSize ;
			binit = true;
		}
		 
	}
}

void SP_DS_TableViewer::UpdateAttributes()
{
	SP_DS_ResultViewer::UpdateAttributes();
	//TODO: update here general table viewer properties
}

void SP_DS_TableViewer::SaveOutput(const wxString& p_sFileName,const int& p_sFileType)
{

}

