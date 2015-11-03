//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2011/08/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_PlacesSelection_H__
#define __SP_DLG_PlacesSelection_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/listbox.h>
#include <wx/event.h>
#include <vector>
#include "sp_gui/dialogs/dia_ContinuousPN/SP_DLG_CPNSimulationResults.h"

typedef enum
{
  SP_COLPN_OUTPUTTYPE_UNFOLDED,   //Unfolded
  SP_COLPN_OUTPUTTYPE_COLORED,    //Colored
  SP_COLPN_OUTPUTTYPE_AUXILIARY   //Auxiliary variables

} SP_ColPNOutputType;

struct SP_ColPN_Position_OutputType
{
	long m_nPosition;
	wxString m_OutputType;
	
	wxString m_sName;
};


class SP_DLG_PlacesSelection : public wxDialog
{

protected:	
	
	wxListBox * m_pListBoxOut;	
	wxChoice* m_pcComboBox;

	wxBoxSizer* m_pcBoxSizer;

	wxSizer*  m_pcRadioRowSizer;

	wxBoxSizer* m_pcMainSizer;
	wxStaticBox* m_pcPlaceChoiceStaticBox;
	wxStaticBox* m_pcFourthColumStaticBox;
	wxString m_sPlaceTransition;    //get the string for the place or tansition list box

	wxTextCtrl* m_pcNameTextCtrl;
	wxRadioBox* m_pcPlaceTransitionRadioBox;
	wxRadioBox* m_pcPlaceResultsRadioBox;

	bool m_bComSingleRun;

	wxListBox * m_pListBoxIn;

	wxRadioBox* m_pcOutputTypeRadioBox;
	
	wxChoice* m_pcChooseColoredPlace;

	wxString m_sPlaceType;

	wxArrayString m_ArrayString;

	map<wxString, wxArrayString> m_msaCurLBInStrings;

	wxArrayString m_ArrayUnPlaces;
	wxArrayString m_ArrayColPlaces;
	wxArrayString m_ArrayUnTranstions;
	wxArrayString m_ArrayColTransitions;

	wxArrayString m_ArrayPlaceAuxVar;
	wxArrayString m_ArrayTransAuxVar;	

	map<long, wxString> m_mPlaceID2Name;
	map<wxString,long> m_mPlaceName2ID;
	map<long, wxString> m_mTransID2Name;
	map<wxString,long> m_mTransName2ID;

	wxString m_sNodeType;

	SP_DS_Graph* m_pcGraph;
			
	vector<SP_ColPN_Position_OutputType> m_vmSelectedNodes;

	SP_DS_Metadata* m_pcEditMetadata;

	wxString m_sPlotName;
public:
	wxTextCtrl *m_pcPlaceChoiceOutRegex;
	wxCheckBox *m_pcPlaceChoiceRegexInvert;
	wxCheckBox *m_pcPlaceChoiceColour;

	 wxString m_sOutputType;
protected:

	 //load Node colours
	 void LoadNodeColours(const wxString& p_nNodeName,SP_VectorString& p_asColours);

public:
	
    SP_DLG_PlacesSelection(SP_DS_Metadata* p_pcEditMetadata, wxWindow* p_pcParent,
    const wxString& p_sTitle = wxT("Edit"),
    long p_nStyle = wxDEFAULT_DIALOG_STYLE );

public:
	vector< SP_ColPN_Position_OutputType > GetSelectedNodes() 
	{
		return m_vmSelectedNodes;
	}
	virtual void GetSelResults();

protected:
	//void SetCommonLayout();
	void SetCommonLayout1();
	virtual void LoadData();

	virtual void OnColPlaceSel( wxCommandEvent& p_cEvent ){}

	void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

	void OnLeftToRight(wxCommandEvent& p_cEvent );
	void OnRightToLeft(wxCommandEvent& p_cEvent );

	void OnLeftToRightAll(wxCommandEvent& p_cEvent );
	void OnRightToLeftAll(wxCommandEvent& p_cEvent );

	virtual void OnEditAuxVariable(wxCommandEvent& p_cEvent );

	virtual void AddtoCurLBInStrings(wxString p_sName);
	virtual void RemovefromCurLBInStrings(wxString p_sName);
	
	virtual void SaveMetaData();
	
	virtual void Initialize();
	virtual void InitilizeFromMetaData();

	virtual void OnChangedPlaceTransition( wxCommandEvent& p_cEvent );
  
	virtual void OnChangedOutType( wxCommandEvent& p_cEvent );

	void OnRegexSelection(wxCommandEvent& p_cEvent );
	void NodesThroughRegEx();
	DECLARE_CLASS( SP_DLG_PlacesSelection )
	DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_PlacesSelection_H__

