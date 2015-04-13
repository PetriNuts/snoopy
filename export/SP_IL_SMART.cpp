//////////////////////////////////////////////////////////////////////
// $Author: sliese
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: IL Logik with grammar and IL - check
//////////////////////////////////////////////////////////////////////

#include "sp_core/TParser/SP_Parser.h"
#include "export/SP_IL_SMART.h"

SP_IL_SMART::SP_IL_SMART()
{
	Parser=new SP_Parser;
	IL_Tokenlist=NULL;
	IL_root=NULL;
} 

SP_IL_SMART::~SP_IL_SMART()
{
	Parser->TreeNode_delete(IL_root);
	Parser->TreeNode_delete(IL_Tokenlist);
	delete Parser;
} 
wxArrayString
SP_IL_SMART::Get_IL_Tokendef()
{
	wxArrayString d;
	d.Add(wxT("Token_Number|[0123456789]+"));
	d.Add(wxT("Token_Trenner|[ ]|[\t]|[\r]|[\n]"));
	d.Add(wxT("Token_TRUE|[t][t]|[tT][rR][uU][eE]"));
	d.Add(wxT("Token_FALSE|[f][f]|[fF][aA][lL][sS][eE]"));
	d.Add(wxT("Token_OR|[oO][rR]|[|][|]|[+|]"));
	d.Add(wxT("Token_AND|[aA][nN][dD]|[&][&]|[&^*\\]"));
	d.Add(wxT("Token_NOT|[[nN][oO][tT]|[~!]"));
	d.Add(wxT("Token_Comment|[/][/]-[\n]"));
	d.Add(wxT("Token_EQ|[=][=]"));
	d.Add(wxT("Token_NE|[!][=]"));
	d.Add(wxT("Token_GE|[>][=]"));
	d.Add(wxT("Token_LE|[<][=]"));
	d.Add(wxT("Token_L|[<]"));
	d.Add(wxT("Token_G|[>]"));
	d.Add(wxT("Token_ORBracket|[(]"));
	d.Add(wxT("Token_CRBracket|[)]"));
	d.Add(wxT("Token_Identifier|[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*"));
	return d;
}
wxArrayString
SP_IL_SMART::Get_IL_grammar()
{
	wxArrayString g;
	g.Add(wxT("start|IL_formula"));
	g.Add(wxT("IL_formula|term Token_OR IL_formula|term"));
	g.Add(wxT("term|factor Token_AND term|factor"));
	g.Add(wxT("factor|Token_NOT factor|atomic_formula"));
	g.Add(wxT("atomic_formula|Token_FALSE|Token_TRUE|Token_Identifier Token_EQ Token_Number|Token_Identifier Token_NE Token_Number|Token_Identifier Token_GE Token_Number|Token_Identifier Token_LE Token_Number|Token_Identifier Token_L Token_Number|Token_Identifier Token_G Token_Number|Token_ORBracket IL_formula Token_CRBracket"));
	return g;
}





wxString
SP_IL_SMART::IL_Check(wxString data)
{
	time_prob_at_default=false;
	wxString erg=wxT("");
	if(data.Len()>0)
	{
		Parser->TreeNode_delete(IL_Tokenlist);
		IL_Tokenlist=Parser->Tokenlist_generate(data,Get_IL_Tokendef());
		if(IL_Tokenlist->type==wxT("Token_error"))
		{
			//fehler bei Tokenerzeugung
			erg=IL_Tokenlist->type + wxT(" ")+ IL_Tokenlist->data + wxT(" at:")+IL_Tokenlist->left->data;
		}else
		{
			//entferne nicht ben�tigte Token
			IL_Tokenlist=Parser->Tokenlist_RemoveTokentype(IL_Tokenlist,wxT("Token_Trenner"));
			IL_Tokenlist=Parser->Tokenlist_RemoveTokentype(IL_Tokenlist,wxT("Token_Comment"));
			//checke alle ident auf Placename
			wxString erg1=Tokenlist_TIdentifier_Place(IL_Tokenlist);
			
			if(erg1==wxT(""))
			{
				//parser start
				IL_root= Parser->GetnewTreeNode();
				IL_root->type=wxT("start");
				long pos=1;
				bool error=!Parser->Parse(IL_root,IL_root,Get_IL_grammar(),IL_Tokenlist,pos);
				if (error)
					erg=Parser->GetParse_error();
			}else
			{
				erg=erg1;
			}
		}
	}
	return erg;
}
wxString
SP_IL_SMART::Tokenlist_TIdentifier_Place(TreeNode* Tokenliste)
{
	wxString erg=wxT("");
	TreeNode* w=Tokenliste;
	while(w!=NULL)
	{
		if(w->type==wxT("Token_Identifier"))
		{
			bool find=false;
			for(unsigned long i=0;i<placename_list.Count();i++)
			{
				if(placename_list.Item(i)==w->data)
				{
					find=true;
					break;
				}

			}
			if(find)
				w=w->right;
			else
			{
				erg=wxT("Place '")+w->data+wxT("' not in graph");
				w=NULL;
			}
		}else
			w=w->right;
	}
	return erg;
}
void
SP_IL_SMART::Transform_Token_Identifier_tk(TreeNode* root)
{
	if(root->type==wxT("Token_Identifier"))
		root->data=wxT("tk(") + root->data + wxT(")");
	if(root->left!=NULL)
		Transform_Token_Identifier_tk(root->left);
	if(root->right!=NULL)
		Transform_Token_Identifier_tk(root->right);
}
wxString
SP_IL_SMART::Transform_IL_prob_inner(wxString num)
{
	TreeNode* wurzel=IL_root;
	Parser->Replace_Tokendata(wurzel,wxT("Token_AND"),wxT("&"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_OR"),wxT("|"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_NOT"),wxT("!"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_EQ"),wxT("=="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_NE"),wxT("!="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_G"),wxT(">"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_GE"),wxT(">="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_L"),wxT("<"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_LE"),wxT("<="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_TRUE"),wxT("true"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_FALSE"),wxT("false"));
	Transform_Token_Identifier_tk(wurzel);

	wxString erg=wxT("  real il_") + num + wxT(" := prob_ss(")+ Parser->Tokentree_tostring(wurzel)+wxT(");\n");
	return erg;
}
wxString
SP_IL_SMART::Transform_IL_prob_outer(wxString num)
{
	wxString erg=wxT("");
	erg=wxT("print(\"\\t il_") + num + wxT("(") + Parser->Tokentree_tostring(IL_root) + wxT(") = \",") + netname_parameter_string + wxT(".il_") + num +wxT(", \"\\n\");\n");
	return erg;
}
wxString
SP_IL_SMART::Transform_IL_probat_inner(wxString num,wxString at)
{
	TreeNode* wurzel=IL_root;
	Parser->Replace_Tokendata(wurzel,wxT("Token_AND"),wxT("&"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_OR"),wxT("|"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_NOT"),wxT("!"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_EQ"),wxT("=="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_NE"),wxT("!="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_G"),wxT(">"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_GE"),wxT(">="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_L"),wxT("<"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_LE"),wxT("<="));
	Parser->Replace_Tokendata(wurzel,wxT("Token_TRUE"),wxT("true"));
	Parser->Replace_Tokendata(wurzel,wxT("Token_FALSE"),wxT("false"));
	Transform_Token_Identifier_tk(wurzel);
	if (at.Len()==0)
	{
		time_prob_at_default=true;
		at=wxT("time_prob_at_default");
	}
	wxString erg=wxT("  real il_") + num + wxT(" := prob_at(")+ Parser->Tokentree_tostring(wurzel)+wxT(",") + at +wxT(");\n");
	return erg;
}
wxString
SP_IL_SMART::Transform_IL_probat_outer(wxString num,wxString at)
{
	return Transform_IL_prob_outer(num);
}
void 
SP_IL_SMART::SetPlacenamelist(wxArrayString list)
{
	placename_list=list;
}
void
SP_IL_SMART::Set_netname_parameter_string(wxString p)
{
	netname_parameter_string=p;
}
bool
SP_IL_SMART::Get_time_prob_at_default()
{
	return time_prob_at_default;
}
