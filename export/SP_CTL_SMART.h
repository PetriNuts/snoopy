//////////////////////////////////////////////////////////////////////
// $Author: sliese $
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: CTL Logik with grammar and CTL - check
//////////////////////////////////////////////////////////////////////
#ifndef __SP_CTL_SMART__
#define __SP_CTL_SMART__

#include "sp_ds/SP_DS_TreeNode.h"
#include "sp_core/TParser/SP_Parser.h"
#include <wx/tokenzr.h>


class SP_CTL_SMART
{

	SP_Parser* Parser;
	wxArrayString placename_list;
	wxString netname_parameter_string;



	void Transform_Token_Identifier_tk(TreeNode* root);
	//place check in tokenlist
	wxString Tokenlist_TIdentifier_Place(TreeNode* Tokenliste);
	
	//CTL
	TreeNode* CTL_root;
	TreeNode* CTL_Tokenlist;
	wxArrayString Get_CTL_Tokendef();
	wxArrayString Get_CTL_grammar();

	
	long CTL_Tokenpos;
	//CTL-Transform
	bool Transform_Token_ADDandrule(TreeNode* root);
	bool Transform_Token_CTL_formula1(TreeNode* root);
	bool Transform_Token_CTL_formula(TreeNode* root);
	bool Transform_Token_ORBracket(TreeNode* root);
	bool Transform_Token_ANDBracket(TreeNode* root);
	bool Transform_Token_TNOT_factor(TreeNode* root);
	bool Transform_Token_atomic_formula(TreeNode* root);
	bool Transform_Token_EU_AU(TreeNode* root);
	bool Transform_Token_EX_AX_EF_AF_EG_AG(TreeNode* root);



	public:
	SP_CTL_SMART();
	~SP_CTL_SMART();

	wxString CTL_Check(wxString data);
	wxString Transform_CTL_SMART_inner(wxString num);
	wxString Transform_CTL_SMART_outer(wxString num);
	void SetPlacenamelist(wxArrayString list);
	void Set_netname_parameter_string(wxString p);
	wxString Transform_to_SMART(wxString data);

};
#endif// __SP_CTL_SMART__
