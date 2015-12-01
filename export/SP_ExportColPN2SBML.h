//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2013/05/21 $
// Short Description: export to sbml
//////////////////////////////////////////////////////////////////////
#ifndef __SP_ExportColPN2SBML__
#define __SP_ExportColPN2SBML__

#include "export/SP_ExportPT.h"
#include <sbml/SBMLTypes.h>
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"

class SP_ExportColPN2SBML: public SP_ExportPT
{

	private:
	wxString m_sNetClass;
	wxRadioButton* m_pRadioButton1;
	wxRadioButton* m_pRadioButton2;
	int level;


 protected:

	SP_MDI_Doc* m_pcDoc;
	SP_DS_Graph* m_pcGraph;
	wxString m_fileName;
	SBMLDocument* m_pcSbmlDoc;
	Model* m_pcSbmlModel;

  bool WritePlaces(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WritePlaceClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces);
  bool WriteTransitions(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteTransitionClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions);
  bool WriteParameters(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding);
  bool WriteArcs(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding, const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs, wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput,Reaction* l_pcReaction );
  virtual bool DoWrite();
  bool WriteConstants();
  bool ValidateSBML();



 public:
  	//chiru
  	virtual bool OnDialogOk();
  		virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);
  //chiru

  SP_ExportColPN2SBML();
  virtual ~SP_ExportColPN2SBML();

  virtual bool AcceptsDoc(SP_MDI_Doc*);
	
  inline virtual wxString GetName()
    { return wxT("Export to SBML");}
  inline virtual wxString GetDescr()
    { return wxT("Export a Colored Petri Net in SBML Format");}
  virtual wxString GetExtension()
    { return wxT("xml");}
  virtual wxString GetFileDescr()
    { return wxT("SBML Files");}

    
    

};

#endif // __SP_ExportColPN2SBML__

