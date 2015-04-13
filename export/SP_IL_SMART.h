//////////////////////////////////////////////////////////////////////
// $Author: sliese $
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: CTL Logik with grammar and CTL - check
//////////////////////////////////////////////////////////////////////
#ifndef __SP_IL_SMART__
#define __SP_IL_SMART__

#include "sp_ds/SP_DS_TreeNode.h"
#include "sp_core/TParser/SP_Parser.h"
#include <wx/tokenzr.h>


class SP_IL_SMART
{

	SP_Parser* Parser;
	wxArrayString placename_list;
	wxString netname_parameter_string;
	bool time_prob_at_default;

	void Transform_Token_Identifier_tk(TreeNode* root);
	//place check in tokenlist
	wxString Tokenlist_TIdentifier_Place(TreeNode* Tokenliste);
	
	//IL
	TreeNode* IL_root;
	TreeNode* IL_Tokenlist;
	wxArrayString Get_IL_Tokendef();
	wxArrayString Get_IL_grammar();

	public:
	SP_IL_SMART();
	~SP_IL_SMART();

	wxString IL_Check(wxString data);
	//SteadyState
	wxString Transform_IL_prob_inner(wxString num);
	wxString Transform_IL_prob_outer(wxString num);
	//Transiente Analyse
	wxString Transform_IL_probat_inner(wxString num,wxString at);
	wxString Transform_IL_probat_outer(wxString num,wxString at);

	void SetPlacenamelist(wxArrayString list);
	void Set_netname_parameter_string(wxString p);
	bool Get_time_prob_at_default();
	//wxString Transform_to_SMART(wxString data);

};
#endif// __SP_IL_SMART__
