//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2009/02/9 $
// Short Description: export to Prism
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTPRISM__
#define __SP_EXPORTPRISM__

#include <wx/wx.h>

#include "export/SP_ExportPT.h"
#include "sp_utilities.h"
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_Map.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"

class SP_ExportPRISM: public SP_ExportPT
{
 protected:
  bool WritePlaces();
  bool WriteTransitions();
  virtual bool DoWrite();
  bool ReadBoundedness();

  wxString GetPlaceName(const long);
  wxString GetMax(const wxString );
  int GetMark(const wxString);

 public:
  SP_ExportPRISM();
  virtual ~SP_ExportPRISM();

  virtual bool AcceptsDoc(SP_MDI_Doc*);

  inline virtual wxString GetName()
    { return wxT("Export to PRISM ");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Petri Net in PRISM Format");}
  virtual wxString GetExtension()
    { return wxT("sm");}
  virtual wxString GetFileDescr()
    { return wxT("PRISM Files");}

  virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);
  virtual bool OnDialogOk();





  private:

	wxRadioButton* m_pRadioButton1;
	wxRadioButton* m_pRadioButton2;
	wxRadioButton* m_pRadioButton3;
	wxRadioButton* m_pRadioButton4;
	wxRadioButton* m_pRadioButton5;
	wxRadioButton* m_pRadioButton6;

	wxFilePickerCtrl* m_pcFilePickerCtrl;
	wxFilePickerCtrl* m_pcFilePickerCtrlFromFile;

	int m_nMode;
    int m_nOrderMode;
    wxString m_FilePath;
	wxString m_FromFilePath;
    vector<wxString>  m_PlaceMax;
    vector<wxString>  m_PlaceName;
    vector<wxString>  m_Max;


	  unsigned m_nPlaceNr;
      unsigned m_nTransNr;
	  vector<bool> m_vPlacesUsed;
	  vector<SP_Place*> m_vPlaces;
	  vector<SP_Transition*> m_vTransitions;

	  map<SP_Transition*, float> m_mTransitionPrePlaceUsed;
      map<SP_Transition*, float> m_mTransitionPostPlaceUsed;
	  map<SP_Transition*, float> m_mTransitionPrePlaceUsed_2;
	  map<SP_Transition*, float> m_mTransitionPostPlaceUsed_2;

   public:
	  void Reverse(vector<wxString>& );
	  void Random(vector<wxString>& );
	  vector<wxString> Noack1();
	  vector<wxString> Noack2();

    private:
	  bool FromFile(vector<wxString>&);
	  bool OrdinaryWrite();
      bool IntelligentWrite();
	  wxString ReadConstant();

	   //////////////////////
	   //for Noark1 ordering
	   //////////////////////
	   void InitNoack1();
	   vector<wxString> OrderNoack1();
	   float CalcWeightforNoack1(unsigned );
	   void AssignedforNoack1(unsigned, unsigned );
	   unsigned FindIdxOfMax(const vector<float>& ,const vector<bool>& );
	   //////////////////////
	   //for Noark2 ordering
	   //////////////////////
	   void InitNoack2();
	   vector<wxString>  OrderNoack2();
	   float calcWeightforNoack2(unsigned );
	   void AssignedforNoack2(unsigned , unsigned );

};

#endif // __SP_EXPORTPRISM__


