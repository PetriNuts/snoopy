//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/05/01 $
//////////////////////////////////////////////////////////////////////

#ifndef __SP_ExportSBML_H__
#define __SP_ExportSBML_H__
#include <sbml/SBMLTypes.h>

#include "export/SP_ExportPT.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"

class SP_DS_Graph;
class SP_DS_Nodeclass;
class SP_DS_Node;
class SP_DS_Edgeclass;
class SP_DS_Metadataclass;
class SP_DS_Metadata;

class SP_ExportSBML : public SP_ExportPT
{

private:
		wxRadioButton* m_pRadioButton1;
		wxRadioButton* m_pRadioButton2;

		//int m_nMode;
		int level;



protected:

	SP_MDI_Doc* m_pcDoc;
	SP_DS_Graph* m_pcGraph;
	wxString m_fileName;
	SBMLDocument* m_pcSbmlDoc;
	Model* m_pcSbmlModel;

	bool WriteConstants();
	bool WritePlaces();
	bool WriteTransitions();
	virtual bool DoWrite();

	bool ValidateSBML();


public:
	virtual bool OnDialogOk();
	virtual bool AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc);


	virtual bool AcceptsDoc(SP_MDI_Doc*);

	inline virtual wxString GetName()
	{
		return wxT("Export to SBML");
	}

	inline virtual wxString GetDescr()
	{
		return wxT("Export a Petri Net into SBML Format");
	}

	virtual wxString GetExtension()
	{
		return wxT("xml");
	}

	virtual wxString GetFileDescr()
	{
		return wxT("SBML Files");
	}


};

#endif // __SP_ExportSBML_H__
