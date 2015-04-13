//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2009/10/10 10:20:00 $
// Short Description: colored Placemultiset class
//////////////////////////////////////////////////////////////////////

#ifndef __SP_CPN_PlaceMultiSet__
#define __SP_CPN_PlaceMultiSet__

#include <wx/wx.h>
#include "sp_core/SP_Vector.h"
#include "sp_core/SP_List.h"
#include "sp_core/SP_Map.h"


struct SP_CPN_MultiSet
{
	int m_Appearance;
	wxString m_Color;
};

class SP_CPN_PlaceMultiSet
{
private:		
	SP_MapString2Int m_MultiSetMap;					// Store all the colors and its appearances 
	wxString m_sColorSetName;
	//vector<wxString> m_ColorsVector;

public:
	SP_CPN_PlaceMultiSet();
	~SP_CPN_PlaceMultiSet();

public:
	bool IsEmpty();									// Used to test if a multiset is empty 
	int  Size();									// Return all the appearances of all colors in the multiset
	void clear();
	
	
	void AddMultiSet(wxString p_Color, int p_Appearance);
	bool RemoveMultiSet(wxString p_Color, int p_Appearance);
	

	bool IsMarking(wxString p_Color, int p_Appearance);
	bool IsMarking( SP_MapString2Int p_mSubset );

	bool EqualArcTest( SP_MapString2Int p_mSubset );

	bool InhibitorArcTest( SP_MapString2Int p_mSubset );

	void SetColorSetName(wxString p_ColorSetName);
	wxString GetColorSetName();

	SP_MapString2Int* GetMultiSetMap();


	bool RemoveMultiSet(SP_MapString2Int p_ColorMultiset);
	void SetMultiSet(SP_MapString2Int p_ColorMultiset);

};

#endif // __SP_CPN_PlaceMultiSet__

