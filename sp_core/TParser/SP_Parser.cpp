//////////////////////////////////////////////////////////////////////
// $Author: sliese
// $Version: 0.1 $
// $Date: 2009/03/22 $
// Short Description: simple Parser, Top Down use LL(1) Grammatik
//////////////////////////////////////////////////////////////////////

#include "sp_core/TParser/SP_Parser.h"

SP_Parser::SP_Parser()
{
} 

SP_Parser::~SP_Parser()
{
} 

wxArrayString
SP_Parser::SplitwxString(wxString data,wxString separator)
{
	wxArrayString liste;
	wxStringTokenizer tkz(data,separator);
	while ( tkz.HasMoreTokens() )
	{				
		wxString element=tkz.GetNextToken();
		liste.Add(element);
	}
	return liste;
}
/////////////
//Token
/////////////
TreeNode*
SP_Parser::GetnewTreeNode()
{
	TreeNode* root=new TreeNode ;
	root->data=wxT("");
	root->type=wxT("");
	root->left=NULL;
	root->right=NULL;
	return root;
}

/////////////
//Tokennizer
/////////////
wxString
SP_Parser::Tokendefbereich(wxString& Tokendefinition)
{
	wxString bereich=wxT("");
	wxString def=Tokendefinition;
	wxString zeichen=wxT("");;
	if(def.SubString(0,0)==wxT("["))
	{
		def=def.SubString(1,def.Len());
		zeichen=def.SubString(0,0);
		unsigned long pos=0;
		do{
			bereich=bereich+zeichen;
			pos++;
			zeichen=def.SubString(pos,pos);
		}while(((pos)<def.Len())&&(zeichen!=wxT("]")));
		Tokendefinition=Tokendefinition.SubString(bereich.Len()+2,Tokendefinition.Len());
	}
	return bereich;
}
int
SP_Parser::CheckToken(wxString Token,wxString Tokendefinition)
{
	int erg=0;
	//[0123456789]+
	//
	wxString zeichen=wxT("");

	wxString bereich=Tokendefbereich(Tokendefinition);
	wxString op=wxT("");
	wxString bis=wxT("");
	op=Tokendefinition.SubString(0,0);
	if((op==wxT("+"))||(op==wxT("*")))
	{
		Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
	}else if (op==wxT("-"))
	{
		Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
		bis=Tokendefbereich(Tokendefinition);
	}else
	{
		op=wxT("");
	}

	for(unsigned int p=0;/*static_cast<unsigned int>*/(p)<Token.Len();p++)
	{
		zeichen=Token.SubString(p,p);
		if(bereich.Len()==0)
		{
			erg=-1;
			break;
		}
		if(op==wxT("-"))
		{
			//bereichstest
			if(bereich.First(zeichen)==-1)
			{
				erg=-1;
				break;
			}
			op=wxT("--");
		}else if(op==wxT("--"))
		{
			//bis suchen wenn nicht am ende break;
			long fp=Token.First(bis);
			if(fp!=-1)
			{
				if(fp==(Token.Len()-bis.Len()))
				{
					
					break;
				}else
				{
					erg=-1;
					break;
				}
			}
			break;
		}else if(op==wxT("+"))
		{
			//bereichsttest
			if(bereich.First(zeichen)==-1)
			{
				erg=-1;
				break;
			}
			op=wxT("*");
		}else if(op==wxT("*"))
		{
			//bereichstest
			if(bereich.First(zeichen)==-1)
			{
				//erg=-1;
				//break;
			
				p--;
				//nextbereich
				bereich=Tokendefbereich(Tokendefinition);
				op=wxT("");
				bis=wxT("");
				op=Tokendefinition.SubString(0,0);
				if((op==wxT("+"))||(op==wxT("*")))
				{
					Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
				}else if (op==wxT("-"))
				{
					Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
					bis=Tokendefbereich(Tokendefinition);
				}else
				{
					op=wxT("");
				}
			}
		}else
		{
			//bereichstest
			if(bereich.First(zeichen)==-1)
			{
				erg=-1;
				break;
			}
			//nextbereich
			bereich=Tokendefbereich(Tokendefinition);
			op=wxT("");
			bis=wxT("");
			op=Tokendefinition.SubString(0,0);
			if((op==wxT("+"))||(op==wxT("*")))
			{
				Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
			}else if (op==wxT("-"))
			{
				Tokendefinition=Tokendefinition.SubString(1,Tokendefinition.Len());
				bis=Tokendefbereich(Tokendefinition);
			}else
			{
				op=wxT("");
			}
		}
	}
	if(erg==0)
	{
		bereich=Tokendefbereich(Tokendefinition);
		if(bereich.Len()==0)
			erg=1;
	}
	return erg;
}

TreeNode*
SP_Parser::Tokenlist_generate(wxString& data,wxArrayString Tokendef)
{
	 //SP_LOGMESSAGE(wxT("start generate Tokens"));
	wxString formel=data;
	//position variable of token
	long line_number=1;
	long line_pos=0;
	long next_line_number=1;
	long next_line_pos=0;
	//help pointer
	TreeNode *rroot=NULL;
	TreeNode *hroot=NULL;
	TreeNode *h1root=NULL;
	wxArrayString def=Tokendef;
	//checkible Tokentypes
	wxArrayString Tokentypes;
	//checked Tokentypes
	wxArrayString newTokentypes;
	//correct Tokentypes
	wxArrayString newkorrektTokentypes;
	wxArrayString Tokenrule;
	
	Tokentypes.Clear();
	
	wxString Tokendata=wxT("");
	wxString oldTokendata=wxT("");
	wxString zeichen=wxT("");
	wxString rule=wxT("");
	wxString checkedTokentype=wxT("");
	int imbereichcheck=0;
	bool find_rule=false;
	long pos=-1;
	bool end=false;
	bool error=false;
	while((formel.Len()>0)&&(!end))
	{
		//read next Token
		Tokentypes.Clear();
		//at start the Token can be from every Tokentype
		for(unsigned int i=0;(i)<def.Count();i++)
		{
			//Tokenrule.Clear();
			Tokenrule=SplitwxString(def.Item(i),wxT("|"));
			newTokentypes.Add(Tokenrule.Item(0));
		}
		Tokendata=wxT("");
		pos=-1;
		
		newkorrektTokentypes.Clear();
		line_number=next_line_number;
		line_pos=next_line_pos;
		do{
			pos++;
			if(static_cast<unsigned long>(pos) >= formel.Len())
			{
				end=true;
			}else
			{
			//copy tokentypes;
			Tokentypes.Clear();
			for(unsigned int a=0;(a)<newTokentypes.Count();a++)
				Tokentypes.Add(newTokentypes.Item(a));
			//read necht char to add it to the token
			zeichen=formel.SubString(pos,pos);
			Tokendata=Tokendata + zeichen;
			if(pos!=0)
			{
				if(zeichen==wxT("\n"))
				{
					next_line_number=line_number+1;
					next_line_pos=0;
				}else
				{
					line_pos++;
					next_line_pos++;
				}
			}
			newTokentypes.Clear();
			//test all tokentypes if correct or can be correct
			for(unsigned int a1=0;(a1)<Tokentypes.Count();a1++)
			{
				checkedTokentype=Tokentypes.Item(a1);
				//find Tokenrule in Tokendefinition
				find_rule=false;
				for(unsigned int a2=0;(a2)<def.Count();a2++)
				{	
					wxArrayString Tokenrule=SplitwxString(def.Item(a2),wxT("|"));
					if(Tokenrule.Item(0)==checkedTokentype)
					{
						find_rule=true;
						for(unsigned int a3=1;(a3)<Tokenrule.Count();a3++)
						{

							rule=Tokenrule.Item(a3);
							imbereichcheck=CheckToken(Tokendata,rule);
							if(imbereichcheck>-1){
								if(imbereichcheck==1)
								{
									if(oldTokendata.Len()!=Tokendata.Len())
										newkorrektTokentypes.Clear();
									newkorrektTokentypes.Add(checkedTokentype);
									oldTokendata=Tokendata;
								}
								newTokentypes.Add(checkedTokentype);
								break;
							}
						}
						break;
					}
				}
				if(!find_rule)
				{
					//error can't find Tokenrule!
				}
			}
			}
		}while(!((newTokentypes.Count()==0)||(end)));
		if(newkorrektTokentypes.Count()==0)
		{
			//unknown Token
			TreeNode* hw=GetnewTreeNode();
			hw->type=wxT("Token_error");
			hw->data=wxT("unknown Token: '") + formel.SubString(0,pos) + wxT("'");
			hw->right=rroot;
			TreeNode* hw1=GetnewTreeNode();
			hw->left=hw1;
			hw1->type=wxT("Token_Position");
			hw1->data=wxT("line:") + wxString::Format(wxT("%u"),line_number) + wxT(" position:") + wxString::Format(wxT("%u"),line_pos);
			rroot=hw;
			error=true;
			end=true;
		}else
		{
			//save token
			for(unsigned int a=0;(a)<newkorrektTokentypes.Count();a++)
			{
				TreeNode* hw=GetnewTreeNode();
				if(hroot==NULL)
				{
					hroot=hw;
					rroot=hroot;
					h1root=hroot;
				}else
				{
					if(a==0)
					{
						hroot->right=hw;
						hroot=hroot->right;
						h1root=hroot;
					}else
					{
						h1root->left=hw;
						h1root=h1root->left;	
					}
				}
				h1root->type=newkorrektTokentypes.Item(a);
				h1root->data=oldTokendata;
			}
			//write Token_Position alter Token_x
			TreeNode* hw=GetnewTreeNode();
			h1root->left=hw;
			hw->type=wxT("Token_Position");
			if(!error)
			{
				hw->data=wxT("line: ") + wxString::Format(wxT("%u"),line_number) + wxT("position: ") + wxString::Format(wxT("%u"),line_pos-oldTokendata.Len());
				//cut formel
				formel=formel.SubString(oldTokendata.Len(),formel.Len());
			}
		}
		
	}
	//calulate Tokenlist_count
	Tokenlist_count=GetTreeNode_Count(rroot,false);
	//SP_LOGMESSAGE(wxT("end generate Tokens"));
	//rroot zurueckgeben
	return rroot;	
}

////////////
//Tokenlist
////////////
TreeNode *
SP_Parser::Tokenlist_RemoveTokentype(TreeNode *liste,wxString Tokentype)
{
	bool find=false;
	TreeNode *h=liste;
	TreeNode *prev=NULL;
	//TreeNode *next=NULL;
	//TreeNode *parent=NULL;
	//TreeNode* child=NULL;
	while(h!=NULL)
	{
		if(h->type==Tokentype)
		{
			find=true;
			if(h==liste)
			{//first element
				liste=liste->right;
				h->right=NULL;
				TreeNode_delete(h);
				h=liste;
			}else
			{//next or last element
				prev->right=prev->right->right;
				h->right=NULL;
				TreeNode_delete(h);
				h=prev->right;
			}
		}else
		{
			prev=h;
			h=h->right;
		}
	}
	//while(h!=NULL)
	//{
	//	if(h->type==Tokentype)
	//	{
	//		find=true;
	//		//if first element
	//		if(h==liste)
	//		{
	//			next=h->right;
	//			h->right=NULL;
	//			child=h->left;
	//			h->left=NULL;
	//			if(child!=NULL)
	//			{
	//				child->right=next;
	//				liste=child;
	//				if(child->type=wxT("Token_Position"))
	//				{
	//					liste=liste->right;
	//					child->right=NULL;
	//					child->left=NULL;
	//					TreeNode_delete(child);
	//				}
	//			}else
	//			{
	//				liste=next;
	//			}
	//			TreeNode_delete(h);
	//			h=liste;

	//		}else //mittle or end element
	//		{
	//			//prev->right=h->right;
	//			next=h->right;
	//			
	//			child=h->left;
	//			if(child!=NULL)
	//			{
	//				child->right=next;
	//				prev->right=child;
	//				if(child->type=wxT("Token_Position"))
	//				{
	//					prev->right=next;
	//					child->left=NULL;
	//					child->right=NULL;
	//					TreeNode_delete(child);
	//				}
	//			}else
	//			{
	//				prev->right=next;
	//			}
	//			h->right=NULL;
	//			h->left=NULL;
	//			TreeNode_delete(h);
	//			h=prev->right;
	//		}

	//	}
	//	//childs ueberpruefen



	//	prev=h;
	//	if(h!=NULL)
	//		h=h->right;
	//	
	//}
	return liste;
}


int
SP_Parser::Tokenlist_GetCountElementatPos(TreeNode* root,long pos)
{
	TreeNode* h=Tokenlist_GetElementatPos(root,pos);
	int anz=0;
	while(h!=NULL)
	{
		anz++;
		h=h->left;
	}
	return anz;
}
TreeNode*
SP_Parser::Tokenlist_GetElementatPos(TreeNode* root,long pos,int alternativeelementpos)
{
	TreeNode* h=root;
	long p=1;
	while((p!=pos)&&(h!=NULL))
	{
		p++;
		h=h->right;
	}
	int aep=1;
	while((aep!=alternativeelementpos)&&(h!=NULL))
	{
		aep++;
		h=h->left;
	}
	return h;
}
TreeNode*
SP_Parser::Tokenlist_copy(TreeNode* root_from,TreeNode* root_to)
{
	TreeNode* hroot=GetnewTreeNode();
	TreeNode* rroot=NULL;
	if(root_from!=NULL)
	{
		hroot->data=root_from->data;
		hroot->type=root_from->type;
		if(root_from==root_to)
		{
			rroot=hroot;
		}else
		{
			if(root_from->right!=NULL)
				hroot->right=Tokenlist_copy(root_from->right,root_to);
			if(root_from->left!=NULL)
				hroot->left=Tokenlist_copy(root_from->left,NULL);
			rroot=hroot;
		}
	}
	return rroot;
}


/////////////
//Parser
/////////////
bool
SP_Parser::Parse(TreeNode *root,TreeNode *parent,wxArrayString grammar,TreeNode* Tokenlist,long& Tokenpos)
{	
	if(root==parent)
	{
		Tokenlist_count=GetTreeNode_Count(Tokenlist,false);
		//SP_LOGMESSAGE(wxT("start Parser"));
		Parser_error=wxT("");
		Parser_error_TPos=-1;
	}
	//if(parent->type==wxT("CTL_input"))
	//	SP_LOGMESSAGE(wxT("ctl_input"));

	wxArrayString rules;
	wxArrayString rule;
	wxArrayString hToken;
	wxString elementtype=parent->type;
	
	wxString helement=wxT("");
	TreeNode* hroot=NULL;
	TreeNode* rroot=hroot;
	
	long TPos=0;
	long TreeToken_count=GetTreeNodeToken_Count(root);
	bool find_rule=false;
	bool k=false;
	if(parent->type==wxT("wert1"))
						{
						}
	//check if wrong way
	if(TreeToken_count>(Tokenlist_count))
	{
		if(Parser_error_TPos<TPos)
		{
			//TreeNode* t1=
					Tokenlist_GetElementatPos(Tokenlist,TPos);
			//TreeNode* t2=
					Tokenlist_GetElementatPos(Tokenlist,TPos,Tokenlist_GetCountElementatPos(Tokenlist,TPos));
			Parser_error=wxT("Parser error 5 \n more Token needed");// at:'") + t1->data + wxT("' \n at ")+t2->data;
			Parser_error_TPos=TPos;
		}
		return k;
	}
	else
	for(unsigned int i=0;(i)<grammar.Count();i++)
	{
		wxString grammar_line=grammar.Item(i);
		wxString et=grammar_line.SubString(0,grammar_line.First(wxT("|"))-1);
		if(elementtype==et)
		{
			find_rule=true;
			rules.clear();
			rules=SplitwxString(grammar_line,wxT("|"));
			for(unsigned int a=1;(a)<rules.Count();a++)
			{
				k=true;
				rule.clear();
				rule=SplitwxString(rules.Item(a),wxT(" "));
				//create all elements and runaway and check it
				hroot=GetnewTreeNode();
				rroot=hroot;
				parent->left=rroot;
				hroot->type=rule.Item(0);
				for(unsigned int pos=1;(pos)<rule.Count();pos++)
				{
					TreeNode* h=GetnewTreeNode();
					hroot->right=h;
					h->type=rule.Item(pos);
					hroot=hroot->right;
				}
				
				hroot=rroot;
				TPos=Tokenpos;
				while((hroot!=NULL)&&(k))
				{
						
						if(hroot->type.SubString(0,5)==wxT("Token_"))
						{
							//Token must come
							TreeNode* troot=Tokenlist_GetElementatPos(Tokenlist,TPos);
							//check Token 
							if (troot==NULL)
							{
								k=false;
								if(Parser_error_TPos<TPos)
								{
									TreeNode* t1=Tokenlist_GetElementatPos(Tokenlist,TPos-1);
									TreeNode* t2=Tokenlist_GetElementatPos(Tokenlist,TPos-1,Tokenlist_GetCountElementatPos(Tokenlist,TPos-1));
									Parser_error=wxT("Parser error 3 \n sufficient Token after:'") + t1->data + wxT("' \n at ")+t2->data;
									Parser_error_TPos=TPos;
								}							
							}else
							if(hroot->type==troot->type)
							{
								hroot->data=troot->data;
								TPos++;
							}else
							{
								//error not correct token come
								if(rroot!=hroot)
								{
									if(Parser_error_TPos<TPos)
									{
										TreeNode* t1=Tokenlist_GetElementatPos(Tokenlist,TPos);
										TreeNode* t2=Tokenlist_GetElementatPos(Tokenlist,TPos,Tokenlist_GetCountElementatPos(Tokenlist,TPos));
										Parser_error=wxT("Parser error 1 \n wrong Token:'") + t1->data + wxT("' \n at ")+t2->data;
										Parser_error_TPos=TPos;
									}
								}
								k=false;
							}
						}else
						{	
							
							//if no Token then go deeper for the element
							if(!Parse(root,hroot,grammar,Tokenlist,TPos))
							{
								if(rroot!=hroot)
								{	
									if(Parser_error_TPos<TPos)
									{
										TreeNode* t1=Tokenlist_GetElementatPos(Tokenlist,TPos);
										TreeNode* t2=Tokenlist_GetElementatPos(Tokenlist,TPos,Tokenlist_GetCountElementatPos(Tokenlist,TPos));
										Parser_error=wxT("Parser error 2 \n no longer convertible at:'") + t1->data + wxT("' \n at ")+t2->data;
										Parser_error_TPos=TPos;
									}
								}
								k=false;
							}
						}
						hroot=hroot->right;
					
					if(!k)
					{
						//go next rule if this rule is inapplicable 
						parent->left=NULL;
						TreeNode_delete(rroot);
						rroot=NULL;
						hroot=NULL;
					}
				}
				//check if wrong way
				if((GetTreeNodeToken_Count(root))>(Tokenlist_count))
				{
					//error: Tree has more Token as in Tokenlist
					if(Parser_error_TPos<TPos)
					{
						//TreeNode* t1=
							Tokenlist_GetElementatPos(Tokenlist,TPos);
						//TreeNode* t2=
							Tokenlist_GetElementatPos(Tokenlist,TPos,Tokenlist_GetCountElementatPos(Tokenlist,TPos));
						Parser_error=wxT("Parser error 4 \n more Token needed");// at:'") + t1->data + wxT("' \n at ")+t2->data;
						Parser_error_TPos=TPos;
					}
					k=false;
				}else
				{
				}
				if(k)
				{
					Tokenpos=TPos;
					break;
				}
			}
			break;
		}
		
	}
	if(!find_rule)
	{
		//error: cannot find the rule
		k=false;
		if(Parser_error==wxT(""))
			Parser_error=wxT("no correct rule found, to come from '") + elementtype +wxT("' to '") +Tokenlist_GetElementatPos(Tokenlist,Tokenpos)->data +wxT("'");
	}
	if(parent==root)
	{
		if(GetTreeNodeToken_Count(rroot)!=(Tokenlist_count))
		{
			k=false;
			if(Parser_error==wxT(""))
				Parser_error=wxT(" no Token leftover");
		}
		//SP_LOGMESSAGE(wxT("end Parser"));
	}
	return k;
}
wxString 
SP_Parser::GetParse_error()
{
	return Parser_error;
}
////////////
//Tree
////////////
wxString
SP_Parser::Tokentree_tostring(TreeNode* root)
{
	if(root==NULL)
		return wxT("");
	else
		if(root->type.SubString(0,5)==wxT("Token_"))
		{
			wxString txt=Tokentree_tostring(root->right);
			if(txt==wxT(""))
				return root->data;
			else
				//return root->data+wxT(" ")+txt;
				if((txt.SubString(0,0)==wxT("("))||((txt.SubString(0,0)==wxT("{"))||((txt.SubString(0,0)==wxT("[")))))
					return root->data + txt;
				else
					return root->data+wxT(" ")+txt;
		}
		else
		{
			wxString txt=Tokentree_tostring(root->right);
			wxString txt1=	Tokentree_tostring(root->left);
			if((txt!=wxT(""))&&(txt1!=wxT("")))
				if((txt.SubString(0,0)==wxT("("))||((txt.SubString(0,0)==wxT("{"))||((txt.SubString(0,0)==wxT("[")))))
					return txt1 + txt;
				else
					return txt1 +wxT(" ")+ txt;
			else
				return txt1  + txt;
		}
}



bool
SP_Parser::Replace_Tokendata(TreeNode* root,wxString Tokentype,wxString Tokendata)
{	
	bool erg=false;
	if(root->type==Tokentype)
		root->data=Tokendata;
	if(root->left!=NULL)
		Replace_Tokendata(root->left,Tokentype,Tokendata);
	if(root->right!=NULL)
		Replace_Tokendata(root->right,Tokentype,Tokendata);
	return erg;
}
TreeNode*
SP_Parser::TreeNode_delete(TreeNode* root)
{
	if (root!=NULL)
	{
		TreeNode*lw=root->left;
		TreeNode*rw=root->right;
		root->left=NULL;
		root->right=NULL;
		if(lw!=NULL)
			TreeNode_delete(lw);
		if(rw!=NULL)
			TreeNode_delete(rw);
		delete root;
	}
	root=NULL;
	return root;
}

long
SP_Parser::GetTreeNodeToken_Count(TreeNode *root)
{
	long anz=0;
	if(root==NULL)
		return 0;
	if((root->left==NULL)&&(root->right==NULL))
	{
		anz=1;
	}else 
	{
		if(root->left!=NULL)
		{
			anz=anz+GetTreeNodeToken_Count(root->left);
		}
		if(root->right!=NULL)
		{
			anz=anz+GetTreeNodeToken_Count(root->right);
		}
		if((root->left==NULL) && (root->right!=NULL))
			anz=anz+1;
	}
	return anz;
}
long
SP_Parser::GetTreeNode_Count(TreeNode *root,bool all)
{
	long anz=0;
	if (root==NULL)
		return 0;
	if((root->left==NULL)&&(root->right==NULL))
	{
		anz=1;
	}else 
	{
		if(root->left!=NULL)
		{
			anz=anz+GetTreeNode_Count(root->left,all);
		}
		if(root->right!=NULL)
		{
			anz=anz+GetTreeNode_Count(root->right,all);
		}
		if (all)
			anz=anz+1;
	}
	return anz;
}
