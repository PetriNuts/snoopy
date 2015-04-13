//////////////////////////////////////////////////////////////////////
// $Author: sliese
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: CTL Logik with grammar and CTL - check
//////////////////////////////////////////////////////////////////////

#include "sp_core/TParser/SP_Parser.h"
#include "export/SP_CTL_SMART.h"

SP_CTL_SMART::SP_CTL_SMART()
{
	Parser=new SP_Parser;
	CTL_Tokenlist=NULL;
	CTL_root=NULL;
} 

SP_CTL_SMART::~SP_CTL_SMART()
{
	Parser->TreeNode_delete(CTL_root);
	Parser->TreeNode_delete(CTL_Tokenlist);
	delete Parser;
} 
//CTL Tokendefinition
wxArrayString
SP_CTL_SMART::Get_CTL_Tokendef()
{
	wxArrayString d;
	d.Add(wxT("Token_Number|[0123456789]+"));
	d.Add(wxT("Token_Trenner|[ ]|[\t]|[\r]|[\n]"));
	d.Add(wxT("Token_UNTIL|[U]"));
	d.Add(wxT("Token_AU|[A]"));
	d.Add(wxT("Token_EU|[E]"));
	d.Add(wxT("Token_AX|[A][X]"));
	d.Add(wxT("Token_EX|[E][X]"));
	d.Add(wxT("Token_AF|[A][F]"));
	d.Add(wxT("Token_EF|[E][F]"));
	d.Add(wxT("Token_AG|[A][G]"));
	d.Add(wxT("Token_EG|[E][G]"));
	d.Add(wxT("Token_TRUE|[t][t]|[tT][rR][uU][eE]"));
	d.Add(wxT("Token_FALSE|[f][f]|[fF][aA][lL][sS][eE]"));
	d.Add(wxT("Token_OR|[oO][rR]|[|][|]|[+|]"));
	d.Add(wxT("Token_AND|[aA][nN][dD]|[&][&]|[&^*\\]"));
	d.Add(wxT("Token_NOT|[[nN][oO][tT]|[~!]"));
	d.Add(wxT("Token_XOR|[xX][oO][rR]|[%]"));
	d.Add(wxT("Token_Comment|[/][/]-[\n]"));
	d.Add(wxT("Token_Implies|[iI][mM][pP][lL][iI][eE][sS]|[-][>]"));
	d.Add(wxT("Token_Implies_by|[iI][mM][pP][lL][iI][eE][sS][_][bB][yY]|[<][-]"));
	d.Add(wxT("Token_IFF|[iI][fF][iI]|[eE][qQ][uU][vV]|[<][-][>]"));
	d.Add(wxT("Token_EQ|[=][=]"));
	d.Add(wxT("Token_NE|[!][=]"));
	d.Add(wxT("Token_GE|[>][=]"));
	d.Add(wxT("Token_LE|[<][=]"));
	d.Add(wxT("Token_L|[<]"));
	d.Add(wxT("Token_G|[>]"));
	d.Add(wxT("Token_ORBracket|[(]"));
	d.Add(wxT("Token_CRBracket|[)]"));
	d.Add(wxT("Token_OEBracket|[[]"));
	d.Add(wxT("Token_CEBracket|[]]"));
	d.Add(wxT("Token_Identifier|[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*"));
	d.Add(wxT("Token_Semikolon|[;]"));
	return d;
}
//CTL grammar
wxArrayString
SP_CTL_SMART::Get_CTL_grammar()
{
	wxArrayString g;
	g.Add(wxT("start|CTL_input"));
	g.Add(wxT("CTL_input|CTL_formula Token_Semikolon CTL_input|CTL_formula Token_Semikolon"));
	g.Add(wxT("CTL_formula|formula Token_Implies CTL_formula|formula Token_Implies_by CTL_formula|formula Token_IFF CTL_formula|formula Token_XOR CTL_formula|formula"));
	g.Add(wxT("formula|term Token_OR formula|term"));
	g.Add(wxT("term|factor Token_AND term|factor term|factor"));
	g.Add(wxT("factor|Token_NOT factor|Token_EU Token_OEBracket factor Token_UNTIL factor Token_CEBracket|Token_AU Token_OEBracket factor Token_UNTIL factor Token_CEBracket|Token_EU Token_ORBracket factor Token_UNTIL factor Token_CRBracket|Token_AU Token_ORBracket factor Token_UNTIL factor Token_CRBracket|Token_EX factor|Token_AX factor|Token_EF factor|Token_AF factor|Token_EG factor|Token_AG factor|atomic_formula"));
	g.Add(wxT("atomic_formula|Token_FALSE|Token_TRUE|Token_Identifier arithmetical Token_Number|Token_ORBracket CTL_formula Token_CRBracket"));
	g.Add(wxT("arithmetical|Token_EQ|Token_NE|Token_GE|Token_LE|Token_L|Token_G"));
	return g;
}








wxString
SP_CTL_SMART::CTL_Check(wxString data)
{
	wxString erg=wxT("");
	if(data.Len()>0)
	{
		//TreeNode_delete(CTL_Tokenlist);
		CTL_Tokenlist=Parser->Tokenlist_generate(data,Get_CTL_Tokendef());
		if(CTL_Tokenlist->type==wxT("Token_error"))
		{
			//error at Tokengeneration
			erg=CTL_Tokenlist->type + wxT(" ")+ CTL_Tokenlist->data + wxT(" at:")+CTL_Tokenlist->left->data;
		}else
		{
			//remove not needed Token
			CTL_Tokenlist=Parser->Tokenlist_RemoveTokentype(CTL_Tokenlist,wxT("Token_Trenner"));
			CTL_Tokenlist=Parser->Tokenlist_RemoveTokentype(CTL_Tokenlist,wxT("Token_Comment"));
			//check all identifier if is a placename
			wxString erg1=Tokenlist_TIdentifier_Place(CTL_Tokenlist);
			if(erg1==wxT(""))
			{
				//parser start
				CTL_root= Parser->GetnewTreeNode();
				CTL_root->type=wxT("start");
				long pos=1;
				
				//time optimisation
				TreeNode* l_pc_from_Token=CTL_Tokenlist;
				TreeNode* l_pc_to_Token=CTL_Tokenlist;
				TreeNode* l_pc_CTL_Tokenlist_part=NULL;

				bool end=false;
				if(CTL_Tokenlist==NULL)
				{
					end=true;
					erg=wxT("empty input!");
				}
				while((!end))
				{
					while((l_pc_to_Token->right!=NULL)&&(l_pc_to_Token->type!=wxT("Token_Semikolon")))
						l_pc_to_Token=l_pc_to_Token->right;
					l_pc_CTL_Tokenlist_part=Parser->TreeNode_delete(l_pc_CTL_Tokenlist_part);
					l_pc_CTL_Tokenlist_part=Parser->Tokenlist_copy(l_pc_from_Token,l_pc_to_Token);
					pos=1;
					CTL_root=Parser->TreeNode_delete(CTL_root);
					CTL_root= Parser->GetnewTreeNode();
					CTL_root->type=wxT("start");
					bool error=!Parser->Parse(CTL_root,CTL_root,Get_CTL_grammar(),l_pc_CTL_Tokenlist_part,pos);
					if (error)
					{
						erg=Parser->GetParse_error();
						end=true;
					}else
					{
						l_pc_from_Token=l_pc_to_Token->right;
						if(l_pc_to_Token->right!=NULL)
							l_pc_to_Token=l_pc_to_Token->right;
						else
							end=true;
					}
				}
				
				
				/*bool error=!Parser->Parse(CTL_root,CTL_root,Get_CTL_grammar(),CTL_Tokenlist,pos);
				if (error)
					erg=Parser->GetParse_error();*/
			}else
			{
				erg=erg1;
			}
		}
	}else
		erg=wxT(" empty input!");
	return erg;
}



void 
SP_CTL_SMART::SetPlacenamelist(wxArrayString list)
{
	placename_list=list;
}
wxString
SP_CTL_SMART::Tokenlist_TIdentifier_Place(TreeNode* Tokenliste)
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
SP_CTL_SMART::Transform_Token_Identifier_tk(TreeNode* root)
{
	if(root->type==wxT("Token_Identifier"))
		root->data=wxT("tk(") + root->data + wxT(")");
	if(root->left!=NULL)
		Transform_Token_Identifier_tk(root->left);
	if(root->right!=NULL)
		Transform_Token_Identifier_tk(root->right);
}
bool
SP_CTL_SMART::Transform_Token_ADDandrule(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("term"))
		{
			TreeNode* child=root->left;
			if(child->right!=NULL)
			if(child->type==wxT("factor")&&
				child->right->type==wxT("term"))
			{
				erg=true;
				//term factor
				//SMART: add the missing Token_AND
				TreeNode* h1=Parser->GetnewTreeNode();
				h1->type=wxT("Token_AND");
				h1->data=wxT("&");
				h1->right=child->right;
				child->right=h1;
			}
		}
		Transform_Token_ADDandrule(root->left);
		Transform_Token_ADDandrule(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_CTL_formula1(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{		
		if(root->type==wxT("CTL_formula"))
			if(root->left->type==wxT("formula")&&(root->left->right==NULL))
			{
				TreeNode* h1=Parser->GetnewTreeNode();
				TreeNode* h2=Parser->GetnewTreeNode();
				TreeNode* h3=Parser->GetnewTreeNode();
				h1->type=wxT("Token_ORBracket");
				h1->data=wxT("(");
				h2->type=wxT("smCTL_formula1");
				h2->data=wxT("");
				h3->type=wxT("Token_CRBracket");
				h3->data=wxT(")");
				h1->right=h2;
				h2->right=h3;
				h3->right=root->left->right;
				h2->left=root->left->left;
				root->left->right=h1;
				root->left->left=NULL;
				root->left->type=wxT("Token_printset");
				root->left->data=wxT("printset");
			}
		Transform_Token_CTL_formula(root->left);
		Transform_Token_CTL_formula(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_CTL_formula(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("CTL_formula"))
		{
			TreeNode* child=root->left;
			if((child->right==NULL)&&(child->type==wxT("formula")))
			{	
				TreeNode* h1=Parser->GetnewTreeNode();
				TreeNode* h2=Parser->GetnewTreeNode();
				TreeNode* h3=Parser->GetnewTreeNode();
				h1->type=wxT("Token_printset");
				h1->data=wxT("printset");
				h2->type=wxT("Token_ORBracket");
				h2->data=wxT("(");
				h3->type=wxT("Token_CRBracket");
				h3->data=wxT(")");
				h1->right=h2;
				h2->right=child;
				child->right=h3;
				root->left=h1;
			}else
			{
				erg=true;
				////-> ; <-; <->; xor// x -> y
				//SMART: includes( x , y )
				TreeNode* t1=child;
				TreeNode* t2=child->right;
				TreeNode* t3=child->right->right;
				t1->right=NULL;
				t2->right=NULL;
				t3->right=NULL;
				TreeNode* h1=Parser->GetnewTreeNode();
				if(t2->type==wxT("Token_IFF"))
				{
					h1->type=wxT("Token_IFF");
					h1->data=wxT("equal");
				}else
				if(t2->type==wxT("Token_Implies"))
				{
					h1->type=wxT("Token_Implies");
					h1->data=wxT("includes");
				}else
				if(t2->type==wxT("Token_Implies_by"))
				{
					h1->type=wxT("Token_Implies_by");
					h1->data=wxT("includes");
					TreeNode* ht=t1;
					t1=t3;
					t3=ht;
				}else
				if(t2->type==wxT("Token_XOR"))
				{// xor
					h1->type=wxT("Token_XOR");
					h1->data=wxT("neq");
					TreeNode* ht=t1;
					t1=t3;
					t3=ht;	
				}
				t2->data=wxT(",");
				t2->type=wxT("Token_Comma");

				TreeNode* h2=Parser->GetnewTreeNode();
				h2->type=wxT("Token_ORBracket");
				h2->data=wxT("(");
				TreeNode* h3=Parser->GetnewTreeNode();
				h3->type=wxT("Token_CRBracket");
				h3->data=wxT(")");
				h1->right=h2;
				h2->right=t1;
				t1->right=t2;
				t2->right=t3;
				t3->type=wxT("sm_CTL_formula");
				t3->right=h3;
				root->left=h1;
			}
			root->type=wxT("sm_CTL_formula");
		}
		Transform_Token_CTL_formula(root->left);
		Transform_Token_CTL_formula(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_ORBracket(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("formula"))
		{
			TreeNode* child=root->left;
			if(child->right!=NULL)
			if((child->type==wxT("term"))&&
				(child->right->type==wxT("Token_OR"))&&
				(child->right->right->type==wxT("formula"))
				)
			{
				erg=true;
				TreeNode* t1=child;
				TreeNode* t2=child->right;
				TreeNode* t3=child->right->right;
				// set Bracket // x OR y
				//SMART: union( x , y )
				TreeNode* h1=Parser->GetnewTreeNode();
				h1->type=wxT("Token_union");
				h1->data=wxT("union");
				TreeNode* h2=Parser->GetnewTreeNode();
				h2->type=wxT("Token_ORBracket");
				h2->data=wxT("(");
				TreeNode* h3=Parser->GetnewTreeNode();
				h3->type=wxT("Token_Comma");
				h3->data=wxT(",");
				TreeNode* h4=Parser->GetnewTreeNode();
				h4->type=wxT("Token_ORBracket");
				h4->data=wxT(")");

				h1->right=h2;
				h2->right=t1;
				t1->right=h3;
				h3->right=t3;
				t3->right=h4;
				
				t2->left=NULL;
				t2->right=NULL;
				Parser->TreeNode_delete(t2);
				
				root->left=h1;
			}
		}
		Transform_Token_ORBracket(root->left);
		Transform_Token_ORBracket(root->right);	
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_ANDBracket(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("term"))
		{
			TreeNode* child=root->left;
			if(child->right!=NULL)
			{
				if(child->type==wxT("factor")&&
					child->right->type==wxT("Token_AND")&&
					child->right->right->type==wxT("term")
					)
				{
					erg=true;
					// set Bracket // x AND y
					//SMART: intersection ( x , y )
					TreeNode* t1=child;
					TreeNode* t2=child->right;
					TreeNode* t3=child->right->right;
					
					TreeNode* h1=Parser->GetnewTreeNode();
					h1->type=wxT("Token_intersection");
					h1->data=wxT("intersection");
					TreeNode* h2=Parser->GetnewTreeNode();
					h2->type=wxT("Token_ORBracket");
					h2->data=wxT("(");
					TreeNode* h3=Parser->GetnewTreeNode();
					h3->type=wxT("Token_Comma");
					h3->data=wxT(",");
					TreeNode* h4=Parser->GetnewTreeNode();
					h4->type=wxT("Token_ORBracket");
					h4->data=wxT(")");

					h1->right=h2;
					h2->right=t1;
					t1->right=h3;
					h3->right=t3;
					t3->right=h4;
					
					t2->left=NULL;
					t2->right=NULL;
					Parser->TreeNode_delete(t2);
					
					root->left=h1;
				}
			}
		}
		Transform_Token_ANDBracket(root->left);
		Transform_Token_ANDBracket(root->right);	
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_TNOT_factor(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("factor"))
		{
			TreeNode* child=root->left;

			if(child->type==wxT("Token_NOT")&&
				child->right->type==wxT("factor"))
			{
				erg=true;
				//Token_NOT factor
				//SMART: complement( x )  // x=factor
				TreeNode* h1=Parser->GetnewTreeNode();
				h1->type=wxT("Token_complement");
				h1->data=wxT("complement");
				TreeNode* h2=Parser->GetnewTreeNode();
				h2->type=wxT("Token_ORBracket");
				h2->data=wxT("(");
				TreeNode* h3=child->right;
				
				TreeNode* h4=Parser->GetnewTreeNode();
				h4->type=wxT("Token_CRBracket");
				h4->data=wxT(")");
				h1->right=h2;
				h2->right=h3;
				h3->right=h4;
				h3->type=wxT("afactor");
				/*TreeNode* rchild=child->right;
				child->left=NULL;
				child->right=NULL;
				rchild->left=NULL;
				rchild->right=NULL;*/
				root->left=h1;

				//Parser->TreeNode_delete(child);
				//Parser->TreeNode_delete(rchild);
			}
		}
		Transform_Token_TNOT_factor(root->left);
		Transform_Token_TNOT_factor(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_atomic_formula(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("atomic_formula"))
		{
			erg=true;
			//atomic_formula
			//SMART: potential( bool b)  // bool b= (tk(place)>=2)
			TreeNode* h1=Parser->GetnewTreeNode();
			h1->type=wxT("Token_potential");
			h1->data=wxT("potential");
			TreeNode* h2=Parser->GetnewTreeNode();
			h2->type=wxT("Token_ORBracket");
			h2->data=wxT("(");
			TreeNode* h3=Parser->GetnewTreeNode();
			h3->type=wxT("atom");
			h3->data=wxT("");
			TreeNode* h4=Parser->GetnewTreeNode();
			h4->type=wxT("Token_CRBracket");
			h4->data=wxT(")");
			//verbinden
			h1->right=h2;
			h2->right=h3;
			h3->right=h4;
			h3->left=root->left;
			root->type=wxT("aformula");
			root->left=h1;
		}
		Transform_Token_atomic_formula(root->left);
		Transform_Token_atomic_formula(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_EU_AU(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("Token_AU")||
			root->type==wxT("Token_EU"))
		{
			erg=true;	
			//Token_AU/EU '[' factor Token_UNTIL factor ']'
			// A( x U y ) //SMART: AU( x , y )
			root->data=root->data +wxT("U");
			//'['
			root->right->type=wxT("Token_ORBracket");
			root->right->data=wxT("(");
			//Token_UNTIL
			root->right->right->right->type=wxT("Token_Comma");
			root->right->right->right->data=wxT(",");
			//']'
			root->right->right->right->right->right->type=wxT("Token_CRBracket");
			root->right->right->right->right->right->data=wxT(")");
		}
		Transform_Token_EU_AU(root->left);
		Transform_Token_EU_AU(root->right);
	}
	return erg;
}
bool
SP_CTL_SMART::Transform_Token_EX_AX_EF_AF_EG_AG(TreeNode* root)
{
	bool erg=false;
	if(root!=NULL)
	{
		if(root->type==wxT("Token_EX")||
			root->type==wxT("Token_AX")||
			root->type==wxT("Token_EF")||
			root->type==wxT("Token_AF")||
			root->type==wxT("Token_EG")||
			root->type==wxT("Token_AG"))
		{
			erg=true;	
			//Token_EX factor
			//SMART: EX( x )
			TreeNode* h1=Parser->GetnewTreeNode();
			h1->data=wxT("(");
			h1->type=wxT("Token_ORBracket");
			TreeNode* h2=Parser->GetnewTreeNode();
			h2->data=wxT(")");
			h2->type=wxT("Token_CRBracket");
			root->right->right=h2;
			h1->right=root->right;
			root->right=h1;
		}
		Transform_Token_EX_AX_EF_AF_EG_AG(root->left);
		Transform_Token_EX_AX_EF_AF_EG_AG(root->right);
	}
	return erg;
}

wxString
SP_CTL_SMART::Transform_CTL_SMART_inner(wxString num)
{
	TreeNode* root=CTL_root;
	wxString erg=wxT("");
	//smart character
	Transform_Token_ADDandrule(root);
	Parser->Replace_Tokendata(root,wxT("Token_AND"),wxT("&"));
	Parser->Replace_Tokendata(root,wxT("Token_OR"),wxT("|"));
	Parser->Replace_Tokendata(root,wxT("Token_NOT"),wxT("!"));
	Parser->Replace_Tokendata(root,wxT("Token_EQ"),wxT("=="));
	Parser->Replace_Tokendata(root,wxT("Token_NE"),wxT("!="));
	Parser->Replace_Tokendata(root,wxT("Token_G"),wxT(">"));
	Parser->Replace_Tokendata(root,wxT("Token_GE"),wxT(">="));
	Parser->Replace_Tokendata(root,wxT("Token_L"),wxT("<"));
	Parser->Replace_Tokendata(root,wxT("Token_LE"),wxT("<="));
	Parser->Replace_Tokendata(root,wxT("Token_TRUE"),wxT("true"));
	Parser->Replace_Tokendata(root,wxT("Token_FALSE"),wxT("false"));
	
	Transform_Token_Identifier_tk(root);
	//Transform_Token_CTL_logic(root);
	Transform_Token_ORBracket(root);
	Transform_Token_ANDBracket(root);
	Transform_Token_TNOT_factor(root);
	Transform_Token_atomic_formula(root);
	Transform_Token_EU_AU(root);
	Transform_Token_EX_AX_EF_AF_EG_AG(root);
	Transform_Token_CTL_formula(root);

	erg=wxT("  bool CTL_") + num + wxT(" := ")+ Parser->Tokentree_tostring(root)+wxT("\n");
	return erg;
}
wxString
SP_CTL_SMART::Transform_CTL_SMART_outer(wxString num)
{
	wxString erg=wxT("");
	erg=wxT("print(\"\\t CTL_") + num + wxT("(") + Parser->Tokentree_tostring(CTL_root)+ wxT(") = \",")  + netname_parameter_string + wxT(".CTL_") + num +wxT(", \"\\n\");\n");
	return erg;
}
void
SP_CTL_SMART::Set_netname_parameter_string(wxString p)
{
	netname_parameter_string=p;
}
wxString
SP_CTL_SMART::Transform_to_SMART(wxString data)
{
	wxString erg=wxT("");
	erg=CTL_Check(data);
	if(erg==wxT(""))
	{
		erg=Parser->Tokentree_tostring(CTL_root);
	}else
	{
		erg=wxT("error");
	}
	return erg;
}
