/*
 * SP_GUI_SteeringSubWindow.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 06.01.2012
 * Short Description:
 */
//=================================================
#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SelectEditingList.h"

#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_SteeringSubWindow.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"

#include "sp_gui/windows/SP_GUI_LogWindow.h"

enum
{
	SP_GUI_ID_SLIDERBAR=1000
};

BEGIN_EVENT_TABLE(SP_GUI_SteeringSubWindow,wxScrolledWindow)
   EVT_SPIN(wxID_ANY,SP_GUI_SteeringSubWindow::OnSpinValueChanged)
   EVT_TEXT_ENTER(wxID_ANY,SP_GUI_SteeringSubWindow::OnEnterPressed)
END_EVENT_TABLE()

SP_GUI_SteeringSubWindow::SP_GUI_SteeringSubWindow(wxWindow* p_pcParent,spsa::SteeringClient* p_pcGUIClient,const wxString& p_sSubWindowType)
:wxScrolledWindow( p_pcParent, wxID_ANY,	wxDefaultPosition, wxSize( 200, 200 ), wxSUNKEN_BORDER ),
 m_nSlideCount(0),
 m_sSubWindowType(p_sSubWindowType),
 m_nSpinIncrement(0.1),
 m_pcGUIClient(p_pcGUIClient)

{

	         m_apcSteeringVariables.clear();

	         l_pcMainSizer= new wxBoxSizer( wxVERTICAL);

	         l_pcMainSizer->SetMinSize(150,200);

	         SetSizerAndFit(l_pcMainSizer);

	         SetScrollbars(20, 20, 50, 50);

	         SetScrollRate(3,5);
}

SP_GUI_SteeringSubWindow::~SP_GUI_SteeringSubWindow()
{

}

void SP_GUI_SteeringSubWindow::AddVariable(const wxString& p_sName,const double& p_nCurrentValue,
	    		               const double& p_nMinValue,const double& p_nMaxValue)
{
	        wxBoxSizer* l_pcRowSizer=new wxStaticBoxSizer( new wxStaticBox( this,wxID_ANY,p_sName), wxVERTICAL);

	        SP_WDG_SpinCtrlDouble* l_pcSpinCtrl=new SP_WDG_SpinCtrlDouble( this,m_nSlideCount,m_nSpinIncrement);

	        l_pcSpinCtrl->SetId(m_nSlideCount);

	        l_pcSpinCtrl->SetValue(p_nCurrentValue);
	       
	        l_pcRowSizer->Add(l_pcSpinCtrl, 1, wxEXPAND | wxALIGN_CENTER,5);

	        l_pcMainSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALIGN_CENTER,5);

	        m_apcSteeringVariables.push_back(l_pcSpinCtrl);
}

void SP_GUI_SteeringSubWindow::OnSelectItemToEdit()
{
SP_VectorString l_asNames;
wxString l_sWindowTitle;
SP_VectorDouble  l_anCurrentValues;

                 if(m_sSubWindowType==wxT("Places"))
                 {
                      l_asNames=m_pcGUIClient->GetCurrentModel()->GetPlaceNames();
                      
                      l_anCurrentValues=m_pcGUIClient->GetCurrentModel()->GetCurrentMarking();
                      
                      l_sWindowTitle= wxT("select places to edit");
                 }
                 else
                 {
                    l_asNames=m_pcGUIClient->GetCurrentModel()->GetParameterNames();
                    
                    l_sWindowTitle=wxT("select parameters to edit");
                    
                    l_anCurrentValues=m_pcGUIClient->GetCurrentModel()->GetCurrentParameterValues();
                 }      

	             SP_GUI_SelectEditingList l_diag(this,l_sWindowTitle,&l_asNames,&m_anSelectedItems,
	                                             &l_anCurrentValues);

	             if(l_diag.ShowModal()==wxID_OK)
	             {
	            	 m_anSelectedItems=l_diag.GetSelectedList();

	            	 ShowSteeredItems(l_asNames,l_anCurrentValues);
	             }
}

void SP_GUI_SteeringSubWindow::ShowSteeredItems(const SP_VectorString& p_asNames,
		                                        const SP_VectorDouble&  p_anCurrentValues)
{
		 l_pcMainSizer->Clear(true);

		 m_apcSteeringVariables.clear();

		 m_nSlideCount=0;

		 for(unsigned int l_nPos=0;l_nPos<m_anSelectedItems.size();l_nPos++)
		 {
			   unsigned long l_nItemPos=m_anSelectedItems[l_nPos];

			   wxString l_sItemName=p_asNames[l_nItemPos];

			   AddVariable(l_sItemName,p_anCurrentValues[l_nItemPos],1,100);

			   m_nSlideCount++;
		 }

		 l_pcMainSizer->Layout();

         SetScrollbars(20, 20, 50, 50);

         SetScrollRate(3,5);
}

void SP_GUI_SteeringSubWindow::ChangeValue(const long& p_nPosition,const double& p_nValue)
{    
	if(m_sSubWindowType==wxT("Places"))
	{
	 m_pcGUIClient->ChangeCurrentPlaceValues(p_nPosition,p_nValue);
	}
	else
	{
	  m_pcGUIClient->ChangeCurrentParameterValues(p_nPosition,p_nValue);
	}

	SP_LOGMESSAGE(wxT("Value Changed to: ")+wxString::Format(wxT("%g"),p_nValue));

	SP_GUI_LogWindow* l_pclogWindow = wxGetApp().GetMainframe()->GetExportLogWindow();

	if(l_pclogWindow!=NULL)
	{
	 l_pclogWindow->Show(true);
	}
}

void SP_GUI_SteeringSubWindow::OnSpinValueChanged(wxSpinEvent & event)
{
	int l_nPosition=event.GetId();

	double l_nValue=m_apcSteeringVariables[l_nPosition]->GetValue();

	ChangeValue(m_anSelectedItems[l_nPosition],l_nValue);
}

void SP_GUI_SteeringSubWindow::OnEnterPressed(wxCommandEvent& event)
{
	 int l_nPosition=event.GetId();

	 double l_nValue=m_apcSteeringVariables[l_nPosition]->GetValue();

     ChangeValue(m_anSelectedItems[l_nPosition],l_nValue);
}

void SP_GUI_SteeringSubWindow::SaveSteeredPositions()
{
	    if(m_sSubWindowType==wxT("Places"))
		 {
			 m_pcGUIClient->SendSteeredPlacePositions(m_anSelectedItems);
		 }
		 else
		 {
			 m_pcGUIClient->SendSteeredParamPositions(m_anSelectedItems);
		 }
}
void SP_GUI_SteeringSubWindow::LoadSteeredPositions()
{
SP_VectorString l_asNames;
SP_VectorDouble  l_anCurrentValues;

         m_anSelectedItems.clear();

	     if(m_sSubWindowType==wxT("Places"))
		 {
			 m_pcGUIClient->ReceiveSteeredPlacePositions(m_anSelectedItems);

			 l_asNames=m_pcGUIClient->GetCurrentModel()->GetPlaceNames();

			 l_anCurrentValues=m_pcGUIClient->GetCurrentModel()->GetCurrentMarking();
		 }
		 else
		 {
			 m_pcGUIClient->ReceiveSteeredParamPositions(m_anSelectedItems);

			 l_asNames=m_pcGUIClient->GetCurrentModel()->GetParameterNames();

			 l_anCurrentValues=m_pcGUIClient->GetCurrentModel()->GetCurrentParameterValues();
		 }

		 //show the steered item on the window
		 ShowSteeredItems(l_asNames,l_anCurrentValues);
}

void SP_GUI_SteeringSubWindow::RefreshValues()
{
 std::vector<SP_WDG_SpinCtrlDouble*>::iterator l_itValueCtrl;

 SP_VectorDouble l_anCurrentValues;

        unsigned long l_nPos=0;

        //get the new value from the server
        if(m_sSubWindowType==wxT("Places"))
		 {
			 m_pcGUIClient->ReadCurrentPlaceValues(l_anCurrentValues);
		 }
		 else
		 {
			 m_pcGUIClient->ReadCurrentParameterValues(l_anCurrentValues);
		 }

        for(l_itValueCtrl=m_apcSteeringVariables.begin();l_itValueCtrl!=m_apcSteeringVariables.end();l_itValueCtrl++)
        {
        	  unsigned long l_nItemPos=m_anSelectedItems[l_nPos];

        	  double l_nValue= l_anCurrentValues[l_nItemPos];

        	  (*l_itValueCtrl)->SetValue(l_nValue);

        	  //Increase the item position
        	  l_nPos++;
        }
}
