//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2011/09/06 $
// Short Description: unfold a net from a file
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_ColUnfoldFile__
#define __SP_DS_ColUnfoldFile__

#include "snoopy.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include <wx/ffile.h>
#include "sp_ds/extensions/Col_SPN/SP_DS_ColPN_Unfolding.h"

#include <wx/tokenzr.h>

class SP_DS_ColUnfoldFile
{
private:
	 wxFFile m_fFile;
	 wxString m_fileName;
	 SP_DS_Graph* m_pcGraph;
	 SP_DS_ColPN_Unfolding* m_pcColPN_Unfolding;

	 //read
	 wxStringTokenizer* m_pTokenizer;
	 wxString m_sCurrentToken;

private:
	bool WritePlaceClass();
	bool WritePlaces(SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces);
	bool WriteTransClass();
	bool WriteTransitions(SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions);		
	bool WriteArcs(vector<SP_CPN_UnfoldedArcInfo>& p_vArcs, wxString& p_sBuffer, bool p_bInput);	
  

	wxString GetNetName(const wxString& p_fName);

private:
	bool Parse();
	bool parsePlace();
	bool parseColor(SP_CPN_UnfoldedPlace& p_mUnfoldedPlaceInfo);
	bool parseTransition();
	bool Parsebinding(SP_CPN_UnfoldedTransition& p_nUnfoldedTransInfo);
	bool ParseArc(vector<SP_CPN_UnfoldedArcInfo>& p_vInputArcs);
	bool CheckValue();
 
public:
  SP_DS_ColUnfoldFile(SP_DS_ColPN_Unfolding* p_pcColPN_Unfolding);
  virtual ~SP_DS_ColUnfoldFile(); 

public:
	bool DoWrite(const wxString& p_sFile);
	bool DoRead(const wxString& p_sFile);
	
    
    

};

#endif // __SP_DS_ColUnfoldFile__

