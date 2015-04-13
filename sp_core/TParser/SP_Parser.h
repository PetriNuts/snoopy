//////////////////////////////////////////////////////////////////////
// $Author: sliese $
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: simple Parser, Top Down use LL(1) Grammatik
//////////////////////////////////////////////////////////////////////
#ifndef __SP_PARSER__
#define __SP_PARSER__

#include "sp_ds/SP_DS_TreeNode.h"
#include <wx/tokenzr.h>
class SP_Parser
{
public:
	wxArrayString SplitwxString(wxString data,wxString separator);
	//TreeNode
	TreeNode* TreeNode_delete(TreeNode* root);
	TreeNode* GetnewTreeNode();
	long GetTreeNode_Count(TreeNode* root,bool all);
	long GetTreeNodeToken_Count(TreeNode* root);
	//Token
	TreeNode* TreeNode_remove(TreeNode* liste,TreeNode* element);
	bool Replace_Tokendata(TreeNode* root,wxString tokentype,wxString Tokendata);
	
	//Tokenlist
	TreeNode* Tokenlist_generate(wxString& data,wxArrayString Tokendef);
	long Tokenlist_count;
	int CheckToken(wxString Token,wxString Tokendefinition);
	wxString Tokendefbereich(wxString& Tokendefinition);
	TreeNode* Tokenlist_copy(TreeNode* root_from,TreeNode* root_to);
	TreeNode* Tokenlist_RemoveTokentype(TreeNode *liste,wxString Tokentype);
	//long Tokenlist_Count(TreeNode *wurzel);
	
	int Tokenlist_GetCountElementatPos(TreeNode* root,long pos);
	TreeNode* Tokenlist_GetElementatPos(TreeNode* root,long pos,int alternativeelementpos=1);
	
	//Parser
	wxString Parser_error;
	long Parser_error_TPos;
	bool Parse(TreeNode *root,TreeNode *parent,wxArrayString grammar,TreeNode *Tokenlist,long& Tokenpos);
	wxString GetParse_error();
	//Tree
	wxString Tokentree_tostring(TreeNode* root);
public:
	SP_Parser();
	~SP_Parser();
};
#endif// __SP_PARSER__
