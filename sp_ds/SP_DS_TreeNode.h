//////////////////////////////////////////////////////////////////////
// $Author: sliese $
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: node for a tree,  or Parser
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_TREENODE__
#define __SP_DS_TREENODE__

#include <wx/wx.h>

struct TreeNode
{
    wxString data;
	wxString type;
    TreeNode *left;
	TreeNode *right;
};
#endif // __SP_DS_TREENODE__
