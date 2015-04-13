//////////////////////////////////////////////////////////////////////
// $Author: sliese
// $Version: 0.1 $
// $Date: 2008/12/16 $
// Short Description: export to SMART
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportSMART.h"

#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/dialogs/SP_DLG_SMART.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/base/SP_Graphic.h"


SP_ExportSMART::SP_ExportSMART()
{
	m_pc_EC = new SP_ExportConfiguration;
	m_pc_Parser=new SP_Parser;
	CTL_SMART=new SP_CTL_SMART;
	IL_SMART=new SP_IL_SMART;
} 

SP_ExportSMART::~SP_ExportSMART()
{
	delete m_pc_EC;
	delete m_pc_Parser;
	delete CTL_SMART;
	delete IL_SMART;
} 

/*
void 
SP_ExportSMART :: SetMarkingSet( int p_nSetNumber )
{

  m_nMarkingSet = p_nSetNumber;

}


void 
SP_ExportSMART :: SetFunctionSet( int p_nSetNumber )
{

  m_nFunctionSet = p_nSetNumber;

}
void 
SP_ExportSMART :: SetParameterSet( int p_nSetNumber )
{

  m_nParameterSet = p_nSetNumber;

}
*/
bool
SP_ExportSMART::AcceptsDoc(SP_MDI_Doc* p_doc) 
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	g_wxS_nettype= cName;
	return (cName == wxT("Petri Net") ||
			cName == wxT("Extended PT Net") ||
			cName == wxT("Time PT Net") ||
			cName == wxT("Stochastic Petri Net") );
}




wxArrayString
SP_ExportSMART::DeleteDublicate(wxArrayString data){
	//this function delete Dublicates in a wxArrayString
	int l_int_count=0;
	for(unsigned int i=0;(i)<data.Count();i++)
	{
		l_int_count=0;
		for(unsigned int a=0;(a)<data.Count();a++)
		{
			if(data.Item(a)==data.Item(i))
			{
				if(l_int_count==1)
				{
					data.RemoveAt(data.Index(data.Item(a),true,true));
				}else
				{
					l_int_count++;
				}
			}
		}
	}
	return data;
}

wxString
SP_ExportSMART::Get_parameterhead_string()
{
	//this function get the parameter string for the head of a function in SMART
	//SPN Functionname'(parameter)' :={...
	wxString parameter=wxT("");
	if(m_pc_EC->GetConfigurationElement(wxT("init_option"))->checked){
			parameter=wxT(", int n");
	}
	if(m_pc_EC->GetConfigurationElement(wxT("scaling_factor"))->checked){
		parameter=parameter +wxT(", real scaling_factor") ;
	}
	if(m_pc_EC->GetConfigurationElement(wxT("time_prob_at_default"))->checked){
		parameter=parameter +wxT(", int time_prob_at_default") ;
	}
	//
	if(parameter.Len()>0)
	{
		parameter=wxT("(")+ parameter.SubString(1,parameter.Len()) + wxT(")");
	}
	return parameter;
}
wxString
SP_ExportSMART::Get_parameter_string()
{
	//this function get the parameter string for an function call in SMART
	//print("\t Number of embedded MC states.. = ",SPN_pusher4_neu_t1_p3'(parameter)'.numstates, "\n");
	wxString parameter=wxT("");
	if(m_pc_EC->GetConfigurationElement(wxT("init_option"))->checked){
			parameter=wxT(", n");
	}
	if(m_pc_EC->GetConfigurationElement(wxT("scaling_factor"))->checked){
		parameter=parameter +wxT(", scaling_factor") ;
	}
	if(m_pc_EC->GetConfigurationElement(wxT("time_prob_at_default"))->checked){
		parameter=parameter +wxT(", time_prob_at_default") ;
	}
	if(parameter.Len()>0)
	{
		parameter=wxT("(")+ parameter.SubString(1,parameter.Len()) + wxT(")");
	}
	return parameter;
}
wxArrayString
SP_ExportSMART::SplitwxString(wxString p_wxS_data,wxString p_wxS_separator)
{
	//this is a very nice function to split a wxString
	wxArrayString l_wxAS_liste;
	wxStringTokenizer tkz( p_wxS_data,p_wxS_separator);
	wxString l_wxS_element=wxT("");
	while ( tkz.HasMoreTokens() )
	{				
		l_wxS_element=tkz.GetNextToken();
		l_wxAS_liste.Add(l_wxS_element);
	}
	return l_wxAS_liste;
}
bool
SP_ExportSMART::DoWrite()
{
	//check the net
	CheckNetnumberInGraph(m_graph);
	//del old partitions
	int anz_part=m_pc_EC->GetPartition_count();
	for(int pn=0;pn<anz_part;pn++)
	{
		m_pc_EC->deletePartition(1);
	}
	//delete all useable places
	long anz_places=m_pc_EC->GetfromAllusableElementsforPartition_Count();
	for(int p=0;p<anz_places;p++)
	{
		m_pc_EC->DeletefromAllusableElementsforPartition(m_pc_EC->GetItemAllusableElementsforPartition(1));
	}
	//read net data
	Preprocessing();
	//if name problems
	if(!g_bool_Check_names_OK)
		return false;
	//show optimasation dialog
	SP_DLG_SMART l_cDialog(0);
	//m_l_SMArcList
	//set all standard values
	m_pc_EC->SetConfigurationElement(wxT("netname"),false, wxT("SPN_") + wxString::Format(GetNetName(m_fileName)));
	m_pc_EC->SetConfigurationElement(wxT("nettype"),false, wxString::Format(g_wxS_nettype));
	m_pc_EC->SetConfigurationElement(wxT("place_count"),false, wxString::Format(wxT("%u"),g_long_place_count));
	m_pc_EC->SetConfigurationElement(wxT("transitions_count"),false, wxString::Format(wxT("%u"),g_long_transitions_count));
	m_pc_EC->SetConfigurationElement(wxT("arcs_count"),false, wxString::Format(wxT("%u"),g_long_arcs_count));
	m_pc_EC->SetConfigurationElement(wxT("partition_depth_default"),false,wxT("1"));
	m_pc_EC->SetConfigurationElement(wxT("partition_min_number_default"),false,wxT("3"));
	m_pc_EC->SetConfigurationElement(wxT("CTL_data"),false,wxT(""));
	m_pc_EC->SetConfigurationElement(wxT("SteadyState_data"),false,wxT(""));
	m_pc_EC->SetConfigurationElement(wxT("Transient_data"),false,wxT(""));
	m_pc_EC->SetConfigurationElement(wxT("time_prob_at_default"),false,wxT(""));

	//set net data
	l_cDialog.SetConfiguration(m_pc_EC);
	l_cDialog.SetExportSMART(this);
	//create pages of Dialogbox
	l_cDialog.CreateNotebookPages();
	//show dialogbox
	if (l_cDialog.ShowModal() == wxID_OK)
	{
		// settings at SP_DLG_SMART ends
		// write file
		wxString ntype=wxT("");
		if (g_wxS_nettype==wxT("Stochastic Petri Net")){
				ntype=wxT("spn");
		}else if (g_wxS_nettype==wxT("Petri Net")){
				ntype=wxT("spn");
		}else if (g_wxS_nettype==wxT("Extended PT Net")){
				ntype=wxT("spn");
		}else if (g_wxS_nettype==wxT("Time PT Net")){
				ntype=wxT("spn");
		};	
		time_t systime;
		time(&systime);
		wxString TimeStamp(asctime(localtime(&systime)), wxConvUTF8);
		//asctime() puts a '\n' at the end of the time string which is deleted here:
		TimeStamp.erase(24);
		m_file.Write(wxT("// export time ")+TimeStamp+wxT("\n"));
		Transform_CTL_IL_toSMART();
		WritePre();
		m_file.Write(ntype + wxT(" ") + m_pc_EC->GetConfigurationElement(wxT("netname"))->value );
		wxString function_parameters=Get_parameterhead_string();
		if (function_parameters.Len()>0){
			m_file.Write( function_parameters );
		}
		m_file.Write( wxT(" := {\n"));
		if (ntype==wxT("spn")) 
		{
//place p, q;
			WritePlaces();
//init(p:1, q:4);
			WriteInit();
//partition
			WritePartitions();		
//trans t, u, v;
			WriteTransitions();
//arcs(p:t, t:q, q:u, u:p, p:v, v:q);
			WriteArcs();
//guards
			WriteGuards();
// inhibitor arcs
			WriteInhibitor();
//firing rules for Stochastic Petri Net
			if (g_wxS_nettype==wxT("Stochastic Petri Net"))
			{
				WriteFiring();
			}
//real m1 := avg_ss(tk(p));
			WriteIn();
		};
		m_file.Write(wxT("};\n"));
		WritePost();
	}else{
		m_file.Write(wxT("// abort settings-menue"));
		SP_LOGWARNING(wxString::Format(wxT("abort settings-menue")));
	}
	l_cDialog.Destroy();
	return !m_bError;
}






////////////////////
//Function Transform
wxArrayString
SP_ExportSMART::Get_Function_Tokendef()
{
	//this are all Token occurrences in a snoopy function( transition function)
	wxArrayString d;
	d.Add(wxT("Token_Number|[0123456789]+|[0123456789]+[.][0123456789]+"));
	d.Add(wxT("Token_Plus|[+]"));
	d.Add(wxT("Token_Minus|[-]"));
	d.Add(wxT("Token_Multi|[*]"));
	d.Add(wxT("Token_Division|[/]"));
	d.Add(wxT("Token_Comma|[,]"));

	d.Add(wxT("Token_ORBracket|[(]"));
	d.Add(wxT("Token_CRBracket|[)]"));

	d.Add(wxT("Token_Trenner|[ ]|[\t]|[\r]|[\n]"));
	d.Add(wxT("Token_FBioMassAction|[B][i][o][M][a][s][s][A][c][t][i][o][n]"));
	d.Add(wxT("Token_FBioLevelInterpretation|[B][i][o][L][e][v][e][l][I][n][t][e][r][p][r][e][t][a][t][i][o][n]"));
	d.Add(wxT("Token_FMultiplicity|[M][u][l][t][i][p][l][i][c][i][t][y]"));
	d.Add(wxT("Token_FSummation|[S][u][m][m][a][t][i][o][n]"));
	d.Add(wxT("Token_FProdukt|[P][r][o][d][u][k][t]"));
	
	d.Add(wxT("Token_FBinomialCoefficient|[B][i][n][o][m][i][a][l][C][o][e][f][f][i][c][i][e][n][t]"));
	d.Add(wxT("Token_FLigandBindingPotential|[L][i][g][a][n][d][B][i][n][d][i][n][g][P][o][t][e][n][t][i][a][l]"));
	d.Add(wxT("Token_FFixedTimedFiring_Single|[F][i][x][e][d][T][i][m][e][d][F][i][r][i][n][g][_][S][i][n][g][l][e]"));
	d.Add(wxT("Token_FFixedTimedFiring_Periodic|[F][i][x][e][d][T][i][m][e][d][F][i][r][i][n][g][_][P][e][r][i][o][d][i][c]"));
	d.Add(wxT("Token_FTimedFiring|[T][i][m][e][d][F][i][r][i][n][g]"));
	d.Add(wxT("Token_FImmediateFiring|[I][m][m][e][d][i][a][t][e][F][i][r][i][n][g]"));
	d.Add(wxT("Token_F_PrePlaces|[_][P][r][e][P][l][a][c][e][s]"));
	d.Add(wxT("Token_F_SimStart|[_][S][i][m][S][t][a][r][t]"));
	d.Add(wxT("Token_F_SimEnd|[_][S][i][m][E][n][d]"));

	d.Add(wxT("Token_Fsin|[s][i][n]"));
	d.Add(wxT("Token_Fcos|[c][o][s]"));
	d.Add(wxT("Token_Ftan|[t][a][n]"));
	d.Add(wxT("Token_Fasin|[a][s][i][n]"));
	d.Add(wxT("Token_Facos|[a][c][o][s]"));
	d.Add(wxT("Token_Fatan|[a][t][a][n]"));
	d.Add(wxT("Token_Fabs|[a][b][s]"));
	d.Add(wxT("Token_Flog10|[l][o][g][1][0]"));
	d.Add(wxT("Token_Fceil|[c][e][i][l]"));
	d.Add(wxT("Token_Fexp|[e][x][p]"));
	d.Add(wxT("Token_Fsqrt|[s][q][r][t]"));
	d.Add(wxT("Token_Fsqr|[s][q][r]"));
	d.Add(wxT("Token_Ffloor|[f][l][o][o][r]"));
	d.Add(wxT("Token_Flog|[l][o][g]"));
	d.Add(wxT("Token_Fpow|[p][o][w]"));
	d.Add(wxT("Token_Fgt|[g][t]"));
	d.Add(wxT("Token_Feq|[e][q]"));
	d.Add(wxT("Token_Fneq|[n][e][q]"));
	d.Add(wxT("Token_Fand|[a][n][d]"));
	d.Add(wxT("Token_Fgep|[g][e][p]"));
	d.Add(wxT("Token_Fleq|[l][e][q]"));
	d.Add(wxT("Token_For|[o][r]"));
	d.Add(wxT("Token_Flt|[l][t]"));
	d.Add(wxT("Token_Fge|[g][e]"));
	d.Add(wxT("Token_Identifier|[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789]*"));
	return d;
}
wxArrayString
SP_ExportSMART::Get_Function_grammar()
{
	//this is the grammar of the Function in snoopy
	wxArrayString g;
	g.Add(wxT("start|formula"));
	g.Add(wxT("OP_Strich|Token_Plus|Token_Minus"));
	g.Add(wxT("OP_Punkt|Token_Multi|Token_Division|Token_Dach"));
	g.Add(wxT("formula|wert2 OP_Strich formula|wert2"));
	g.Add(wxT("wert2|wert1 OP_Punkt wert2|wert1"));
	g.Add(wxT("wert1|Token_Minus wert|wert"));
	g.Add(wxT("wert|Token_Number|Function|Token_ORBracket formula Token_CRBracket"));
	wxString f=wxT("Function|Token_Identifier|");
	f=f+wxT("Token_FBioMassAction Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_FBioLevelInterpretation Token_ORBracket formula Token_Comma formula Token_CRBracket|");
	f=f+wxT("Token_FMultiplicity Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_FSummation Token_ORBracket Token_F_PrePlaces Token_CRBracket|Token_FSummation Token_ORBracket Token_Identifier Token_CRBracket|");
	f=f+wxT("Token_FProdukt Token_ORBracket Token_F_PrePlaces Token_CRBracket|Token_FProdukt Token_ORBracket Token_Identifier Token_CRBracket|");
	
	f=f+wxT("Token_FBinomialCoefficient Token_ORBracket formula Token_Comma formula Token_CRBracket|");
	f=f+wxT("Token_FLigandBindingPotential Token_ORBracket formula Token_Comma formula Token_CRBracket|");
	f=f+wxT("Token_FFixedTimedFiring_Single Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_FFixedTimedFiring_Periodic Token_ORBracket Token_F_SimStart Token_Comma formula Token_Comma Token_F_SimEnd Token_CRBracket|");
	f=f+wxT("Token_FTimedFiring Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_FImmediateFiring Token_ORBracket Token_CRBracket|");
	f=f+wxT("Token_Fsin Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fcos Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Ftan Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fasin Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Facos Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fatan Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fabs Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Flog10 Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fceil Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fexp Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fsqrt Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fsqr Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Ffloor Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Flog Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fpow Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fgt Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Feq Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fneq Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fand Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fgep Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fleq Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_For Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Flt Token_ORBracket formula Token_CRBracket|");
	f=f+wxT("Token_Fge Token_ORBracket formula Token_CRBracket");
	
	g.Add(f);
	return g;
}

bool
SP_ExportSMART::Transform_TNumber_check(TreeNode * root)
{
	if(root==NULL){
		return false;
	}
	Transform_TNumber_check(root->left);
	Transform_TNumber_check(root->right);
	wxString l_sResultFunction=wxT("");
	if(root->type==wxT("Token_Number"))
	{
			if(!(root->data.First(wxT("."))>0))
				root->data=root->data+wxT(".0");
	}
	return true;
}
bool
SP_ExportSMART::Transform_TIdentifier_Parameter(TreeNode * root)
{
	//this function replace all parameter by value
	if(root==NULL){
		return false;
	}
	Transform_TIdentifier_Parameter(root->left);
	Transform_TIdentifier_Parameter(root->right);
	wxString l_sResultFunction=wxT("");
	if(root->type==wxT("Token_Identifier"))
	{
		if(!root->data.IsEmpty())
		{
			ParameterMap::iterator l_itParam = m_parameterNameMap.find(root->data);
			if(l_itParam != m_parameterNameMap.end())
			{
				l_sResultFunction << l_itParam->second->m_marking;
				root->type=wxT("Token_Number");
				root->data=l_sResultFunction;
			}
			else
			{
				//no parameter
			}
		}
	}
	return true;
}
wxString
SP_ExportSMART::Tokenlist_TIdentifier_Place(TreeNode* root)
{
	//this function check if all Token Identifier places in the graph
	// and make ist to Token_Place
	wxString erg=wxT("");
	if(root==NULL)
		return erg;
	else
	{
		if(root->type==wxT("Token_Identifier"))
		{
			bool find=false;
			wxArrayString placename_list=g_wxAS_place_list;
			for(unsigned long i=0;i<placename_list.Count();i++)
			{
				if(placename_list.Item(i)==root->data)
				{
					find=true;
					break;
				}
			}
			if(!find)
			{
				erg=wxT("Place or Parameter not in Graph'")+root->data+wxT("'");
			}else
			{
				root->type=wxT("Token_Place");
			}
		}
		erg=erg+Tokenlist_TIdentifier_Place(root->left);
		erg=erg+Tokenlist_TIdentifier_Place(root->right);
	}
	return erg;
}
void
SP_ExportSMART::Transform_Token_Place_tk(TreeNode* root)
{
	//this function make tk about all Token_Place
	if(root==NULL)
	{}
	else
	{
		if(root->type==wxT("Token_Place"))
			root->data=wxT("tk(") + root->data + wxT(")");
		Transform_Token_Place_tk(root->left);
		Transform_Token_Place_tk(root->right);
	}
}
bool
SP_ExportSMART::Transform_BiomassAction(TreeNode* root,SP_Transition* t)
{
	//this function transform the snoopy BLI to an equivalentin smart
	if(root==NULL)
		return false;
	else
	{
		Transform_BiomassAction(root->left,t);
		Transform_BiomassAction(root->right,t);
		if(root->type==wxT("Token_FBioMassAction"))
		{
			root->data=wxT("");
			wxString nName = NormalizeString(t->m_name);
			root->type=wxT("FBioMassAction");
			wxString trans=wxT("");
			smartarc* smarc; 
			bool first=true;
			for (m_it_SMArcIt=m_l_SMArcList.begin();m_it_SMArcIt != m_l_SMArcList.end();m_it_SMArcIt++){
				smarc =(*m_it_SMArcIt);
				if(smarc->to_pt==nName)
				{
					if (first==true){
						first=false;
						trans=wxT("bbinom(tk(") + smarc->from_pt + wxT("),") + smarc->arc_cardinality +wxT(")");
					}else{
						trans=trans + wxT("*bbinom(tk(") + smarc->from_pt + wxT("),") + smarc->arc_cardinality +wxT(")");
					}
				}
			}
			if(first==true)
			{
				//error
			}else
			{
				//root->right->right->data=wxT("(") + m_pc_Parser->Tokentree_tostring(root->right->right) + wxT(")*") + trans ;
				//root->right->right->type=wxT("Token_formel");
				root->data=wxT("");
				root->right->right->right->data=wxT(")*")+trans + wxT("");
			}
		}
	}
	return true;
}
bool
SP_ExportSMART::Transform_BioLevelInterpretation(TreeNode* root,SP_Transition* p_transition)
{
	//this function transform the snoopy BLI to an equivalentin smart
	if(root==NULL)
		return false;
	else
	{
		Transform_BioLevelInterpretation(root->left,p_transition);
		Transform_BioLevelInterpretation(root->right,p_transition);
		if (root->type==wxT("Token_FBioLevelInterpretation"))
		{
			wxString nName = NormalizeString(p_transition->m_name);
			smartarc* smarc; 
			wxString trans=wxT("");
			bool first=true;
			unsigned long num=0;
			for (m_it_SMArcIt=m_l_SMArcList.begin();m_it_SMArcIt != m_l_SMArcList.end();m_it_SMArcIt++){
				smarc =(*m_it_SMArcIt);
				if(smarc->to_pt==nName)
				{
					if (first==true){
						first=false;
						trans=wxT("tk(") + smarc->from_pt + wxT(")") ;
						num=1;
					}else{
						trans=trans + wxT("*tk(") + smarc->from_pt + wxT(")");
						num++;
					}
				}
			}
			if(first==false)
			{
				//find Sf
				trans =wxT(")* ")+trans ;//+ m_pc_Parser->Tokentree_tostring(root->right->right)  + wxT(")*") + trans + wxT(")*((");
				TreeNode* t1=root->right->right;
				
				TreeNode* t2=t1->right;
				TreeNode* t3=t2->right;
				root->data=wxT("(");
				t2->type=wxT("Token_BLI1");
				t2->data=trans;
				//t2->left=t3->left;
				t3->type=wxT("Token_BLI2");
				if(num>1)
				{
					
					wxString hsf=wxT("scaling_factor");
					while(num>2)
					{
						hsf=hsf + wxT("*scaling_factor");	
						num--;
					}
					t3->data=wxT(" * ") + hsf+wxT("");
					m_pc_EC->SetConfigurationElement(wxT("scaling_factor"),true, wxT(""));
				}else
				{
					t3->data=wxT("");
				}
				t1->data=trans;
				//root->right->right->data=trans;
				}
		}
	}
	return true;
}
bool
SP_ExportSMART::Transform_Multiplicity(TreeNode* root)
{
	//this function transform Multiplicity
	//respect Transform_Token_Place_tk make Multiplicity to all Places
	if(root==NULL)
		return false;
	else
	{
		Transform_Multiplicity(root->left);
		Transform_Multiplicity(root->right);
		if(root->type==wxT("Token_FMultiplicity"))
		{
			if(root->right->right->type==wxT("Token_Place"))
				root->data=wxT("");
		}
	}
	return true;
}
bool
SP_ExportSMART::Transform_BinominalCoefficient(TreeNode* root)
{
	//this function transform BinominalCoefficient to an equal in smart logic
	if(root==NULL)
		return false;
	else
	{
		Transform_BinominalCoefficient(root->left);
		Transform_BinominalCoefficient(root->right);
		if(root->type==wxT("Token_FBinominalCoefficient"))
		{
			root->data=wxT("bbinom");
		}
	}
	return true;
}
wxString
SP_ExportSMART::Functiontransform(SP_Transition* p_transition)
{
	wxString formel=wxT("");
	if(!p_transition->m_function.IsEmpty())
	{
		//new with m_pc_Parser
		formel=p_transition->m_function;
		TreeNode* function_Tokenlist=m_pc_Parser->Tokenlist_generate(formel,Get_Function_Tokendef());
		if(function_Tokenlist!=NULL)
		{
			if(function_Tokenlist->type!=wxT("Token_error"))
			{
				function_Tokenlist=m_pc_Parser->Tokenlist_RemoveTokentype(function_Tokenlist,wxT("Token_Trenner"));
				TreeNode* function_root=m_pc_Parser->GetnewTreeNode();
				function_root->type=wxT("start");
				long TPos=1;
				bool error=!m_pc_Parser->Parse(function_root,function_root,Get_Function_grammar(),function_Tokenlist,TPos);
				if(!error)
				{
					formel=wxT("");
					//transform
					//replace parameter 
					Transform_TIdentifier_Parameter(function_root);
					//check ident place
					wxString erg=Tokenlist_TIdentifier_Place(function_root);
					if(erg==wxT(""))
					{
						//place tk
						Transform_Token_Place_tk(function_root);

						Transform_BiomassAction(function_root,p_transition);
						Transform_BioLevelInterpretation(function_root,p_transition);
						Transform_BinominalCoefficient(function_root);
						Transform_Multiplicity(function_root);
						Transform_TNumber_check(function_root);
						//not supportet(Summation,Produkt,LigandBindingPotential,Timed..,Fixed..;ImmediateFiring
						formel=m_pc_Parser->Tokentree_tostring(function_root);
					}
				}else
				{
					//parser_error
					formel=wxT("");
				}
			}else
			{
				//unknown Token
				formel=wxT("");
			}
		}
	}
	return formel;
}


////////////////////

///////////////////////
// snoopy net transform
///////////////////////
bool
SP_ExportSMART::Preprocessing()
{
	//this function read all the given input( places,arcs,transitions,...
	g_bool_Check_names_OK=true;
	m_pc_EC->SetConfigurationElement(wxT("scaling_factor"),false,wxT(""));
	//^^variable represent: if all names in graph not smart commands
	//clear all lists
	m_l_SMPlaceList.clear();
	m_l_SMTransitionList.clear();
	m_l_SMArcList.clear();
	m_l_SMInhibitorList.clear();
	m_l_SMGuardList.clear();

	g_long_place_count=0;
	g_long_transitions_count=0;
	g_long_arcs_count=0;
	//bool found;
	// read all places
	
	PlaceIdMap::iterator pIt;
	SP_Place* p;
	g_wxAS_place_list.Clear();
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		p = (*pIt).second;
		g_long_place_count=g_long_place_count+1;
		wxString nName = NormalizeString(p->m_name);
		if (p->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							p->m_name.c_str(), nName.c_str()));
		}
		checkname(nName);
		//place in add to new list
		smartplace* hplace = new smartplace;
		hplace->place_name=nName;
		hplace->place_marking=wxString::Format(wxT("%u"),p->m_marking);
		hplace->id=p->m_id;
		m_l_SMPlaceList.push_back(hplace);
		m_pc_EC->AddtoAllusableElementsforPartition(nName);
		g_wxAS_place_list.Add(nName);
	}
	CTL_SMART->SetPlacenamelist(g_wxAS_place_list);
	IL_SMART->SetPlacenamelist(g_wxAS_place_list);
	// read all arcs
	
	std::list<SP_DiscrArc*>* aList;
	std::list<SP_DiscrArc*>::iterator aIt;
	SP_DiscrArc* a;
	unsigned aNum = 0;
	wxString atype;
	
	smartguard* g;
	bool foundguard;
	wxString pName;
	wxString tName;
	bool foundplace=false;

	TransitionIdMap::iterator tIt;
	SP_Transition* t;

	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;
		aList = m_preTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				g_long_arcs_count=g_long_arcs_count+1;
				atype = wxT("ordinary");
				if ((a->m_edge->GetClassName()) == wxT("Read Edge"))
				{
					atype = wxT("readarc");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Inhibitor Edge"))
				{
					atype = wxT("inhibitor");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Reset Edge"))
				{
					atype = wxT("resetarc");
				}
				else if ((a->m_edge->GetClassName()) == wxT("Equal Edge"))
				{
					atype = wxT("equalarc");
				}
				foundplace=false;
				for (pIt = m_placeIdMap.begin(); (pIt != m_placeIdMap.end())&&(foundplace==false); pIt++) {
					p = (*pIt).second;
					if (p->m_id==a->m_id) 
					{
						foundplace=true;
						pName = NormalizeString(p->m_name);
					}
				}		
				tName =NormalizeString(t->m_name);
				if (atype!=wxT("ordinary")){
					foundguard=false;
					for (m_it_SMGuardIt = m_l_SMGuardList.begin(); m_it_SMGuardIt != m_l_SMGuardList.end(); m_it_SMGuardIt++) {
						g = (*m_it_SMGuardIt);
						if (tName==g->transition) {
							foundguard=true;
							break;
						}
					}
					if (foundguard==false) 
					{
					// add transition to list
						checkname(tName);
						g = new smartguard;
						g->transition=tName;
						m_l_SMGuardList.push_back(g);
					}
					if(atype== wxT("readarc")){
									g->guardinstruction=g->guardinstruction + wxT("&(tk(") + pName + wxT(")>=") + SP_ToWxString(a->m_mult) +wxT(")");
					
					}else if( atype==wxT("equalarc")){
									g->guardinstruction=g->guardinstruction +wxT("&(tk(") + pName + wxT(")==") + SP_ToWxString(a->m_mult) +wxT(")");
					}
				}else if( atype==wxT("resetarc")){	
					//
				}else if( atype==wxT("inhibitorarc")){
					smartinhibitor* hinhibitor = new smartinhibitor;
					hinhibitor->from_p=pName;
					hinhibitor->to_t=tName;
					hinhibitor->arc_cardinality=wxString::Format(wxT("%u"),a->m_mult);
					m_l_SMInhibitorList.push_back(hinhibitor);
				}
				smartarc* harc = new smartarc;
				harc->from_pt=pName;
				harc->to_pt=tName;
				harc->arc_cardinality=wxString::Format(wxT("%u"),a->m_mult);
				m_l_SMArcList.push_back(harc);

				
				/*m_file.Write(wxString::Format(
						wxT("\\arc{A_%u}{\\from{P_%ld} \\to{T_%ld}")
							wxT(" \\weight{%u} \\type{%s}}\n"), aNum, a->m_id,
						t->m_id, a->m_mult, atype.c_str()));*/
				aNum++;
			}
		}
		aList = m_postTrMap[t->m_id];
		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				a = (*aIt);
				g_long_arcs_count=g_long_arcs_count+1;
				foundplace=false;
				for (pIt = m_placeIdMap.begin(); (pIt != m_placeIdMap.end())&&(foundplace==false); pIt++) {
					p = (*pIt).second;
					if (p->m_id==a->m_id) 
					{
						foundplace=true;
						pName = NormalizeString(p->m_name);
					}
				}		
				tName =NormalizeString(t->m_name);
				/*if (t->m_name != tName) {
				  SP_LOGWARNING(
					 wxString::Format("transition name %s was changed to %s",
					  t->m_name.c_str(), tName.c_str()));
				}*/
				smartarc* harc = new smartarc;
				harc->from_pt=tName;// transition_name=nName;
				harc->to_pt=pName;
				harc->arc_cardinality=wxString::Format(wxT("%u"),a->m_mult);
				m_l_SMArcList.push_back(harc);
				aNum++;
			}
		}
	}
	// read all transitions
	//TransitionIdMap::iterator tIt;
	//SP_Transition* t;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		t = (*tIt).second;
		g_long_transitions_count=g_long_transitions_count+1;
		wxString nName = NormalizeString(t->m_name);
		if (t->m_name != nName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("tr name %s was changed to %s"),
							t->m_name.c_str(), nName.c_str()));
		}
		wxString l_sResultFunction=Functiontransform(t);
		
		smarttransition* htransition = new smarttransition;
		htransition->transition_name=nName;
		htransition->firingrule=l_sResultFunction;
		m_l_SMTransitionList.push_back(htransition);
	}
	LoadHierarchy_Net();
	return true;
}

bool
SP_ExportSMART::WritePlaces()
{
	//this function write the places in swart	 
	smartplace* l_pc_smartplace; 
	wxString l_wxS_place_number=wxT("");
	bool l_bool_first=true;
	for (m_it_SMPlaceIt=m_l_SMPlaceList.begin();m_it_SMPlaceIt != m_l_SMPlaceList.end();m_it_SMPlaceIt++){
		l_pc_smartplace =(*m_it_SMPlaceIt);
		if (l_bool_first==true){
			l_bool_first=false;
			m_file.Write(wxT("  place  "));			
			m_file.Write(wxT(" ") + l_pc_smartplace->place_name );
			l_wxS_place_number=wxT(" ") + l_pc_smartplace->place_name + wxT(" net:") + l_pc_smartplace->net;
		}else{
			l_wxS_place_number=l_wxS_place_number +wxT(" ,") + l_pc_smartplace->place_name + wxT(" net:") + l_pc_smartplace->net ;
			l_wxS_place_number=l_wxS_place_number + wxT("id:");
			l_wxS_place_number=l_wxS_place_number	+ wxString::Format(wxT("%d"),l_pc_smartplace->id);
			m_file.Write(wxT(" ,") + l_pc_smartplace->place_name);
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(";\n"));
	}
	//m_file.Write(wxT("// place net:netid || ") + l_wxS_place_number + wxT("\n"));
	//^^ test output
  return true;
}
bool
SP_ExportSMART::WriteFiring()
{
	//this function write the firing rules in smart
	smarttransition* l_pc_smarttransition; 
	bool l_bool_first=true;
	for (m_it_SMTransitionIt=m_l_SMTransitionList.begin();m_it_SMTransitionIt != m_l_SMTransitionList.end();m_it_SMTransitionIt++){
		l_pc_smarttransition =(*m_it_SMTransitionIt);
		if (l_bool_first==true){
			l_bool_first=false;
			m_file.Write(wxT("  firing ("));
			m_file.Write(wxT(" ") + l_pc_smarttransition->transition_name + wxT(":expo(") + l_pc_smarttransition->firingrule + wxT(")"));
		}else{
			m_file.Write(wxT(" ,")+ l_pc_smarttransition->transition_name + wxT(":expo(") + l_pc_smarttransition->firingrule + wxT(")"));
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(");\n"));
	}
	return true;
}
bool
SP_ExportSMART::WriteTransitions()
{
	//this function write the transitions
	smarttransition* l_pc_smarttransition; 
	bool l_bool_first=true;
	for (m_it_SMTransitionIt=m_l_SMTransitionList.begin();m_it_SMTransitionIt != m_l_SMTransitionList.end();m_it_SMTransitionIt++){
		l_pc_smarttransition =(*m_it_SMTransitionIt);
		if (l_bool_first==true){
			l_bool_first=false;
			m_file.Write(wxT("  trans "));
			m_file.Write(wxT(" ") + l_pc_smarttransition->transition_name);
		}else{
			m_file.Write(wxT(" ,") + l_pc_smarttransition->transition_name);
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(";\n"));
	}
  return TRUE;
}
bool
SP_ExportSMART::WritePartitions()
{
	//this function write partition if is it set
	smartplace* l_pc_smartplace; 
	bool l_bool_first=true;
	wxString l_wxS_partopt;
	l_wxS_partopt=m_pc_EC->GetConfigurationElement(wxT("partition"))->value;
	if(l_wxS_partopt==wxT("autopartition"))
	{
		AutoPartition();
		l_wxS_partopt=wxT("userdefinedpartition");
	}
	if (l_wxS_partopt==wxT("none")){
	}else if (l_wxS_partopt==wxT("place=partition")){
		for (m_it_SMPlaceIt=m_l_SMPlaceList.begin();m_it_SMPlaceIt != m_l_SMPlaceList.end();m_it_SMPlaceIt++){
			l_pc_smartplace =(*m_it_SMPlaceIt);
			if (l_bool_first==true){
				l_bool_first=false;
				m_file.Write(wxT("  partition(  "));
				m_file.Write(wxT(" ") + l_pc_smartplace->place_name);
			}else{
				m_file.Write(wxT(" ,") + l_pc_smartplace->place_name);
			}
		}
		if (l_bool_first==false){
			m_file.Write(wxT(");\n"));
		}
	}else if (l_wxS_partopt==wxT("userdefinedpartition")){
		int l_int_part_count=0;
		l_int_part_count=m_pc_EC->GetPartition_count();
		wxString l_wxS_parttxt=wxT("");
		for (int i=1;i<(l_int_part_count+1);i=i+1){
			l_wxS_parttxt=m_pc_EC->GetPartition_aswxString(i);
			l_wxS_parttxt.Replace(wxT(","),wxT(":"),true);
			if (l_bool_first==true){
				l_bool_first=false;
				m_file.Write(wxT("  partition(  "));
				m_file.Write(wxT(" " )+ l_wxS_parttxt);
			}else{
				m_file.Write(wxT(" ,") + l_wxS_parttxt);
			}
		}
		if (l_bool_first==false){
			m_file.Write(wxT(");\n"));
		}
	}else{
		//error
	}
  return TRUE;
}
bool
SP_ExportSMART::WriteInit()
{
	//this function write the initial state and there marks
	smartplace* l_pc_smartplace; 
	bool l_bool_first=true;
	for (m_it_SMPlaceIt=m_l_SMPlaceList.begin();m_it_SMPlaceIt != m_l_SMPlaceList.end();m_it_SMPlaceIt++){
		l_pc_smartplace =(*m_it_SMPlaceIt);
		if(l_pc_smartplace->place_marking!=wxT("0")){
			if (l_bool_first) {
				l_bool_first=false;
				m_file.Write(wxT("  init("));
				m_file.Write(wxString::Format(wxT(" ") + checkname(l_pc_smartplace->place_name) + wxT(":") ));
			}else{
				m_file.Write(wxString::Format(wxT(", ") + checkname(l_pc_smartplace->place_name) + wxT(":") ));
			}
			if(m_pc_EC->GetConfigurationElement(wxT("init_option"))->checked){
				if(m_pc_EC->GetConfigurationElement(wxT("init_option"))->value==wxT("multiply"))
				{
						m_file.Write(wxT("n*")+ l_pc_smartplace->place_marking);
				}
				else
				{
						m_file.Write(wxT("n"));
				}
			}
			else
			{
					m_file.Write( l_pc_smartplace->place_marking);
			}
		}
		
	}
	if (l_bool_first==false){
		m_file.Write(wxT(");\n"));
	}
  return TRUE;
}
bool
SP_ExportSMART::WriteArcs()
{
	//this function write the arcs
	smartarc* l_pc_smarc; 
	bool l_bool_first=true;
	for (m_it_SMArcIt=m_l_SMArcList.begin();m_it_SMArcIt != m_l_SMArcList.end();m_it_SMArcIt++){
		l_pc_smarc =(*m_it_SMArcIt);
		if (l_bool_first) {
			l_bool_first=false;
			m_file.Write(wxT("  arcs("));
			m_file.Write(wxT(" ") + l_pc_smarc->from_pt + wxT(":") + l_pc_smarc->to_pt + wxT(":") + l_pc_smarc->arc_cardinality);
		}else{
			m_file.Write(wxT(", ") + l_pc_smarc->from_pt + wxT(":") + l_pc_smarc->to_pt + wxT(":") + l_pc_smarc->arc_cardinality);
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(");\n"));
	}
  return TRUE;
}
bool
SP_ExportSMART::WriteInhibitor()
{
	//this function write the inhibitor arcs
	smartinhibitor* l_pc_inhibitor; 
	bool l_bool_first=true;
	for (m_it_SMInhibitorIt=m_l_SMInhibitorList.begin();m_it_SMInhibitorIt != m_l_SMInhibitorList.end();m_it_SMInhibitorIt++){
		l_pc_inhibitor =(*m_it_SMInhibitorIt);
		if (l_bool_first) {
			l_bool_first=false;
			m_file.Write(wxT("  inhibitor("));
			m_file.Write(wxT(" ") + l_pc_inhibitor->from_p + wxT(":") + l_pc_inhibitor->to_t + wxT(":") + l_pc_inhibitor->arc_cardinality);
				
		}else{
			m_file.Write(wxT(", ") + l_pc_inhibitor->from_p + wxT(":") + l_pc_inhibitor->to_t + wxT(":") + l_pc_inhibitor->arc_cardinality);
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(");\n"));
	}
	return true;
}
bool
SP_ExportSMART::WriteGuards()
{
	//this function write guards
	smartguard* l_pc_smartguard; 
	bool l_bool_first=true;
	for (m_it_SMGuardIt=m_l_SMGuardList.begin();m_it_SMGuardIt != m_l_SMGuardList.end();m_it_SMGuardIt++){
		l_pc_smartguard =(*m_it_SMGuardIt);
		if (l_bool_first) {
			l_bool_first=false;
			m_file.Write(wxT("  guard("));
			m_file.Write(wxT(" ") + l_pc_smartguard->transition + wxT(":") + l_pc_smartguard->guardinstruction);
				
		}else{
			m_file.Write(wxT(", ") + l_pc_smartguard->transition + wxT(":") + l_pc_smartguard->guardinstruction);
		}
	}
	if (l_bool_first==false){
		m_file.Write(wxT(");\n"));
	}
	return true;
}
wxString
SP_ExportSMART :: checkname( wxString p_name){
	//check the name if it is a SMART language Code
	//name|name|...
	wxString l_all_names=wxT("buffer|partition|place|trans|init|arcs|inhibitor|firing|int|bigint");
	wxString l_name=p_name;
	l_name=l_name.Lower();
	wxArrayString l_split_names=SplitwxString(l_all_names,wxT("|"));
	for(unsigned int i=0;i<l_split_names.Count();i++)
	{
		if(l_name==l_split_names.Item(i))
		{
			g_bool_Check_names_OK=false;
			SP_LOGERROR(
					wxT("name'") + p_name +wxT("' in graph is a command in SMART")
							);
			break;
		}
	}
	return p_name;
}
bool
SP_ExportSMART::WritePre(){
	//write befor SPN Function in Smart file
	if(m_pc_EC->GetConfigurationElement(wxT("Verbose"))->checked){
		m_file.Write(wxT("# Verbose ") + m_pc_EC->GetConfigurationElement(wxT("Verbose"))->value.Lower() + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("Report"))->checked){
		m_file.Write(wxT("# Report ") + m_pc_EC->GetConfigurationElement(wxT("Report"))->value.Lower() + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("GarbageCollection"))->checked){
		m_file.Write(wxT("# GarbageCollection ") + m_pc_EC->GetConfigurationElement(wxT("GarbageCollection"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("ParameterWarnings"))->checked){
		m_file.Write(wxT("# ParameterWarnings ") + m_pc_EC->GetConfigurationElement(wxT("ParameterWarnings"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("StateStorage"))->checked){
		m_file.Write(wxT("# StateStorage ") + m_pc_EC->GetConfigurationElement(wxT("StateStorage"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("MarkovStorage"))->checked){
		m_file.Write(wxT("# MarkovStorage ") + m_pc_EC->GetConfigurationElement(wxT("MarkovStorage"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("RealFormat"))->checked){
		m_file.Write(wxT("# RealFormat ") + m_pc_EC->GetConfigurationElement(wxT("RealFormat"))->value + wxT("\n"));
	}
	///
	if(m_pc_EC->GetConfigurationElement(wxT("IgnoreWeightClasses"))->checked){
		m_file.Write(wxT("# IgnoreWeightClasses ") + m_pc_EC->GetConfigurationElement(wxT("IgnoreWeightClasses"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("InfinityString"))->checked){
		m_file.Write(wxT("# InfinityString \"") + m_pc_EC->GetConfigurationElement(wxT("InfinityString"))->value + wxT("\"\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("BFSTrace"))->checked){
		m_file.Write(wxT("# BFSTrace ") + m_pc_EC->GetConfigurationElement(wxT("BFSTrace"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("WriteDotOutput"))->checked){
		m_file.Write(wxT("# WriteDotOutput ") + m_pc_EC->GetConfigurationElement(wxT("WriteDotOutput"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("DotOutputFile"))->checked){
		m_file.Write(wxT("# DotOutputFile \"") + m_pc_EC->GetConfigurationElement(wxT("DotOutputFile"))->value + wxT("\"\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("DotPrecision"))->checked){
		m_file.Write(wxT("# DotPrecision ") + m_pc_EC->GetConfigurationElement(wxT("DotPrecision"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("DotTitle"))->checked){
		m_file.Write(wxT("# DotTitle \"") + m_pc_EC->GetConfigurationElement(wxT("DotTitle"))->value + wxT("\"\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("EmbedWithElim"))->checked){
		m_file.Write(wxT("# EmbedWithElim ") + m_pc_EC->GetConfigurationElement(wxT("EmbedWithElim"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("EUStrategy"))->checked){
		m_file.Write(wxT("# EUStrategy ") + m_pc_EC->GetConfigurationElement(wxT("EUStrategy"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("GarbageUnit"))->checked){
		m_file.Write(wxT("# GarbageUnit ") + m_pc_EC->GetConfigurationElement(wxT("GarbageUnit"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("GarbageSize"))->checked){
		m_file.Write(wxT("# GarbageSize ") + m_pc_EC->GetConfigurationElement(wxT("GarbageSize"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("MatrixByRows"))->checked){
		m_file.Write(wxT("# MatrixByRows ") + m_pc_EC->GetConfigurationElement(wxT("MatrixByRows"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("MaxConvergeIters"))->checked){
		m_file.Write(wxT("# MaxConvergeIters ") + m_pc_EC->GetConfigurationElement(wxT("MaxConvergeIters"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("MaxNumericalIters"))->checked){
		m_file.Write(wxT("# MaxNumericalIters ") + m_pc_EC->GetConfigurationElement(wxT("MaxNumericalIters"))->value + wxT("\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("MaxPrintedItems"))->checked){
		m_file.Write(wxT("# MaxPrintedItems ") + m_pc_EC->GetConfigurationElement(wxT("MaxPrintedItems"))->value + wxT("\n"));
	}
	

	m_file.Write(wxT("// function for Bionominalformal\n"));
	m_file.Write(wxT("bigint bfact(int n) := cond(n==0,1,bfact(n-1)*n);\n"));
	m_file.Write(wxT("real bbinom(int n, int k):=(bfact(n))/(bfact(n-k));\n"));
	m_file.Write(wxT("\n"));
	return true;
}
bool
SP_ExportSMART::WriteIn(){
	//this function write in the end of the body of smart function
	m_file.Write(wxT("\n"));
	if(m_pc_EC->GetConfigurationElement(wxT("num_states"))->checked){
		m_file.Write(wxT("  bigint numstates := num_states(") + m_pc_EC->GetConfigurationElement(wxT("num_states"))->value.Lower() + wxT(");\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("num_arcs"))->checked){
		m_file.Write(wxT("  bigint numarcs   := num_arcs(") + m_pc_EC->GetConfigurationElement(wxT("num_arcs"))->value.Lower() + wxT(");\n"));
	}
	for(unsigned int i=0;(i)<g_wxAS_CTL_IL_lines_inner.Count();i++)
		m_file.Write(g_wxAS_CTL_IL_lines_inner.Item(i));
	return true;
}
bool
SP_ExportSMART::WritePost(){
	//this function write after a smart body
	wxString parameter=wxT("");
	if(m_pc_EC->GetConfigurationElement(wxT("init_option"))->checked){
			m_file.Write(wxT("int n := read_int(\"n\");\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("scaling_factor"))->checked){
		m_file.Write(wxT("print(\" scaling_factor := sf_mass/sf_n \\n\");\n"));
		//int n := read_int("n");
		m_file.Write(wxT("int sf_n := read_int(\"sf_n\");\n"));
		//real mass := read_real("mass");
		m_file.Write(wxT("real sf_mass := read_real(\"scalingfactor_mass\");\n"));
		//real sf := mass/n;
		m_file.Write(wxT("real scaling_factor := sf_mass/sf_n;\n"));
		//m_file.Write(wxT("int scaling_factor := read_real(\"scalinf factor\");\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("time_prob_at_default"))->checked){
		m_file.Write(wxT("int time_prob_at_default := read_real(\"time_prob_at_default\");\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("num_states"))->checked){
		m_file.Write(wxT("print(\"\\t Number of embedded MC states.. = \",") + m_pc_EC->GetConfigurationElement(wxT("netname"))->value + Get_parameter_string() + wxT(".numstates, \"\\n\");\n"));
	}
	if(m_pc_EC->GetConfigurationElement(wxT("num_arcs"))->checked){
		m_file.Write(wxT("print(\"\\t Number of embedded MC arcs.. = \",") + m_pc_EC->GetConfigurationElement(wxT("netname"))->value + Get_parameter_string() + wxT(".numarcs, \"\\n\");\n"));
	}
	for(unsigned int i=0;/*static_cast<unsigned int>*/(i)<g_wxAS_CTL_IL_lines_outer.Count();i++)
		m_file.Write(g_wxAS_CTL_IL_lines_outer.Item(i));
	m_file.Write(wxT("\n"));
	return true;
}
////////////////////

////////////////////////
//net id check functions
bool
SP_ExportSMART::CheckNetnumberInGraph(SP_DS_Graph* p_pcGraph)
{
	//use wxArrayString,wxString
	wxArrayString nethierarchy;
	nethierarchy.clear();

	SP_ListNodeclass::const_iterator nodeclassIter;
    const SP_ListNodeclass* l_plNC = p_pcGraph->GetNodeclasses();
	//build nethierarchy
	//find Coarse Places and Transistions
	for (nodeclassIter = l_plNC->begin(); nodeclassIter != l_plNC->end(); ++nodeclassIter)
	{
		SP_DS_Nodeclass* nodeclass=(*nodeclassIter);
		if((nodeclass->GetName()==wxT("Coarse Place"))||(nodeclass->GetName()==wxT("Coarse Transition")))
		{
			SP_ListNode::const_iterator nodeIter;
			const SP_ListNode* nodelist = nodeclass->GetElements();
			for (nodeIter = nodelist->begin(); nodeIter != nodelist->end(); ++nodeIter)
			{
				SP_DS_Node* node=(*nodeIter);	
				SP_Data* data=node;
					// build coarse place/transition hierachy
					if (node->GetCoarse())
					{
						wxString netnumber=wxString::Format(wxT("%ld"),data->GetNetnumber());
						wxString pnumber=wxString::Format(wxT("%d"), data->GetCoarse()->GetNetnumber()) ;
						nethierarchy.Add(pnumber);
						nethierarchy.Add(netnumber);
					}
			}
		}
	}
	//check Places and Transitions
	for (nodeclassIter = l_plNC->begin(); nodeclassIter != l_plNC->end(); ++nodeclassIter)
	{
		SP_DS_Nodeclass* nodeclass=(*nodeclassIter);
		if((nodeclass->GetName()==wxT("Place"))||(nodeclass->GetName()==wxT("Transition")))
		{
			SP_ListNode::const_iterator nodeIter;
			const SP_ListNode* nodelist = nodeclass->GetElements();
			for (nodeIter = nodelist->begin(); nodeIter != nodelist->end(); ++nodeIter)
			{
				SP_DS_Node* node=(*nodeIter);
				wxString node_netnumber=wxString::Format(wxT("%d"),node->GetNetnumber());
				//build a list of the net from the graph elements in snoopy net
				wxArrayString graph_netliste;
				graph_netliste.clear();

				SP_DS_Attribute* nattribute= node->GetAttribute(wxT("ID"));
				SP_ListGraphic::const_iterator lG_Iter;
				const SP_ListGraphic* LGraphic=nattribute->GetGraphics();
				for (lG_Iter = LGraphic->begin(); lG_Iter != LGraphic->end();lG_Iter++)
				{
						wxString wxnm=wxString::Format(wxT("%d"),(*lG_Iter)->GetNetnumber());
						graph_netliste.Add(wxnm);
						//add netdepth
						int depth=0;
						bool hasparent=false;
						wxString nowwxnm=wxnm;
						do
						{
							hasparent=false;
							for(unsigned int e1=0;(e1)<(nethierarchy.Count()/2);e1++)
							{
								if(nethierarchy.Item(e1*2)==nowwxnm)
								{
									depth++;
									nowwxnm=nethierarchy.Item((e1*2)+1);
									hasparent=true;
									break;
								}
							}
						}while(hasparent);
						graph_netliste.Add(wxString::Format(wxT("%d"),depth));
				}
				//check if node_netnumber in list
				bool find_in_graph_netliste=false;
				for(unsigned int e=0;(e)<(graph_netliste.Count()/2);e++)
				{
					if(graph_netliste.Item(e*2)==node_netnumber)
					{
						find_in_graph_netliste=true;
						break;
					}
				}
				//if not
				if(!find_in_graph_netliste)
				{
					//find current netnumber
					wxString new_node_netnumber=wxT("");
					if(graph_netliste.Count()!=0)
					{
						long dl=0;
						long dl1=0;
						new_node_netnumber=graph_netliste.Item(0);
						graph_netliste.Item(1).ToLong(&dl);
						for(unsigned int e1=1;(e1)<(graph_netliste.Count()/2);e1++)
						{
							graph_netliste.Item((e1*2)+1).ToLong(&dl1);
							if(dl>dl1){
								dl=dl1;
								new_node_netnumber=graph_netliste.Item(e1*2);
							}
						}
						//set node and attribute netnumber
						new_node_netnumber.ToLong(&dl);
						//int inetnumber=dl;
						// set net number in node
						//node->SetNetnumber(inetnumber,node->GetNetnumber());
						//^^ correct in the net
						//set attribute netnumber
					}
				}
			}
		}
	}
	return true;
}
bool
SP_ExportSMART::LoadHierarchy_Net()
{
	//load the net id hierarchy from the graph information in the snoopy net
	smartplace* l_p_smartplace; 
	long nodeid=0;
	wxString sId;
	//delete old hierarchy
	m_l_SMHierarchy.clear();
	m_l_SMHierarchy.empty();
	//hierarchy read and save
	SP_ListNodeclass::const_iterator nodeclassIter;
    const SP_ListNodeclass* l_plNC = m_graph->GetNodeclasses();
	//find Coarse Places and Transistions
	bool find=false;
	for (nodeclassIter = l_plNC->begin(); nodeclassIter != l_plNC->end(); ++nodeclassIter)
	{
		SP_DS_Nodeclass* nodeclass=(*nodeclassIter);
		SP_ListNode::const_iterator nodeIter;
		const SP_ListNode* nodelist = nodeclass->GetElements();
		for (nodeIter = nodelist->begin(); nodeIter != nodelist->end(); ++nodeIter)
		{
			SP_DS_Node* node=(*nodeIter);	
			SP_Data* data=node;
			if((nodeclass->GetName()==wxT("Coarse Place"))||(nodeclass->GetName()==wxT("Coarse Transition")))
			{
				// bild coarse place/transition hierachy
				if (node->GetCoarse())
				{
					wxString netnumber=wxString::Format(wxT("%ld"),data->GetNetnumber());
					wxString pnumber=wxString::Format(wxT("%d"), data->GetCoarse()->GetNetnumber()) ;
					smarthierarchy* hh = new smarthierarchy;
					hh->net=pnumber;
					hh->parentnet=netnumber;
					m_l_SMHierarchy.push_back(hh);
				}
			}else if(nodeclass->GetName()==wxT("Place"))
			{
				nodeid=-1;
				sId = node->GetAttribute(wxT("ID"))->GetValueString();
				sId.ToLong(&nodeid);
				wxString hnet=wxString::Format(wxT("%d"), node->GetNetnumber());
					
				//put the net to the places
				//if (!(node->GetLogical())){
				find=false;
					for (m_it_SMPlaceIt=m_l_SMPlaceList.begin();m_it_SMPlaceIt != m_l_SMPlaceList.end();m_it_SMPlaceIt++)
					{
						l_p_smartplace=(*m_it_SMPlaceIt);
						if (l_p_smartplace->id==nodeid)
						{					
							wxString hnet1=wxString::Format(wxT("%d"),data->GetNetnumber());
							wxString hnet=wxString::Format(wxT("%d"), node->GetNetnumber());
							if (hnet1!=hnet){
								find=true;
							}
							l_p_smartplace->net=hnet;
							find=true;
							break;
						}
					}
					if (find==false)
					{
						find=false;
					}
				//}
			}
		}
	}
	return true;
}
wxString
SP_ExportSMART::GetRootofHierarchy()
{
	wxString l_wxS_root=wxT("");
	smartplace* l_p_smartplace; 
	
	m_it_SMPlaceIt=m_l_SMPlaceList.begin();
	if(m_it_SMPlaceIt != m_l_SMPlaceList.end())
	{
				l_p_smartplace =(*m_it_SMPlaceIt);
				l_wxS_root=l_p_smartplace->net;
	}
	
	bool find;
	smarthierarchy* l_p_hierarchy;
	
	do
	{
		find=false;
		for (m_it_SMHierarchyIt=m_l_SMHierarchy.begin();m_it_SMHierarchyIt != m_l_SMHierarchy.end();m_it_SMHierarchyIt++)
		{
			l_p_hierarchy =(*m_it_SMHierarchyIt);
			if (l_p_hierarchy->net==l_wxS_root){
				l_wxS_root=l_p_hierarchy->parentnet;
				find=true;
			}
		}
	}while(find==true);
	wxString l_wxS_hierarchy=wxT("// Hierarchy:");
	for (m_it_SMHierarchyIt=m_l_SMHierarchy.begin();m_it_SMHierarchyIt != m_l_SMHierarchy.end();m_it_SMHierarchyIt++)
		{
			l_p_hierarchy =(*m_it_SMHierarchyIt);
			l_wxS_hierarchy=l_wxS_hierarchy +wxT("net:") + l_p_hierarchy->net +wxT("pnet:") +l_p_hierarchy->parentnet;
		}
	//m_file.Write(hierarchy + wxT("\n"));
	//^^ help output in smart to see the hierarchy
	return l_wxS_root;
}
wxArrayString
SP_ExportSMART::GetChildofNet(wxString net,bool childofchilds){
	wxArrayString feld;
	smarthierarchy* h;
	for (m_it_SMHierarchyIt=m_l_SMHierarchy.begin();m_it_SMHierarchyIt != m_l_SMHierarchy.end();m_it_SMHierarchyIt++)
	{
		h =(*m_it_SMHierarchyIt);
		if (h->parentnet==net){
			feld.Add(h->net);
		}
	}
	if(childofchilds){
		wxArrayString childs;
		for(unsigned int i=0;(i)<feld.Count();i++){
			wxString hnet=feld.Item(i);
			wxArrayString hchilds=GetChildofNet(hnet,true);
			for(unsigned int a=0;(a)<hchilds.Count();a++){
				childs.Add(hchilds.Item(a));
			}
		}
		for(unsigned int i=0;(i)<childs.Count();i++)
		{
			feld.Add(childs.Item(i));
		}
	}
	return feld;
}
////////////////////////

///////////////////////
// automatic partition
bool
SP_ExportSMART::AutoPartition(){
	smartplace* p; 
	long depth=0;
	m_pc_EC->GetConfigurationElement(wxT("partition_depth"))->value.ToLong(&depth,10);
	long min_number=0;
	m_pc_EC->GetConfigurationElement(wxT("partition_min_number"))->value.ToLong(&min_number,10);

	wxArrayString startdeptha;
	wxArrayString nextdeptha;
	startdeptha.Clear();
	nextdeptha.clear();
	startdeptha.Add(GetRootofHierarchy());
	if(depth==0)
	{
		nextdeptha.Add(GetRootofHierarchy());
	}else
	{
		for(int i=0;i<depth;i++){
			for(unsigned int a=0;(a)<startdeptha.Count();a++)
			{
				wxArrayString child=GetChildofNet(startdeptha.Item(a),false);
				if (child.Count()==0)
				{
					nextdeptha.Add(startdeptha.Item(a));
				}else
				{
					for(unsigned int b=0;(b)<child.Count();b++)
					{
						nextdeptha.Add(child.Item(b));
					}
				}
			}
			startdeptha.clear();
			for (unsigned int c=0;(c)<nextdeptha.Count();c++)
				startdeptha.Add(nextdeptha.Item(c));
			nextdeptha.clear();
		}
		nextdeptha=startdeptha;
	}
	//delete duplicate in list
	nextdeptha=DeleteDublicate(nextdeptha);
	//delete all actual partitions
	int anz_part=m_pc_EC->GetPartition_count();
	for(int pn=0;pn<anz_part;pn++)
	{
		m_pc_EC->deletePartition(1);
	}
		//write partitions
		//min_number
	int partition=-1;
	for(unsigned int i=0;(i)<nextdeptha.Count();i++)
	{
		wxArrayString child=GetChildofNet(nextdeptha.Item(i),true);
		child.Add(nextdeptha.Item(i));
		child=DeleteDublicate(child);
		long anz_places=0;
		partition=-1;
		m_pc_EC->newPartition();
		partition=m_pc_EC->GetPartition_count();
		wxString netze=wxT("");
		for(unsigned int a=0;(a)<child.Count();a++)
		{
			//new partition
			netze=netze+wxT(",")+child.Item(a);
			for (m_it_SMPlaceIt=m_l_SMPlaceList.begin();m_it_SMPlaceIt != m_l_SMPlaceList.end();m_it_SMPlaceIt++)
			{
				p =(*m_it_SMPlaceIt);
				if (p->net==child.Item(a))
				{
						anz_places++;
						m_pc_EC->AddElementtoPartition(partition,p->place_name);
						m_pc_EC->DeletefromAllusableElementsforPartition(p->place_name);
				}
			}
			
		}
		//m_file.Write(wxT("// partition aus net(") + netze.Right(netze.Len()-1) + wxT(")\n"));
		//^^ test output
		if(anz_places<min_number)
		{
				m_pc_EC->deletePartition(partition);
		}
	}
	//write all other places in one partition
	if(m_pc_EC->GetfromAllusableElementsforPartition_Count()>0)
	{
		m_pc_EC->newPartition();
		partition=m_pc_EC->GetPartition_count();
		while(m_pc_EC->GetfromAllusableElementsforPartition_Count()>0)
		{
			m_pc_EC->AddElementtoPartition(partition,m_pc_EC->GetItemAllusableElementsforPartition(1));
			m_pc_EC->DeletefromAllusableElementsforPartition(m_pc_EC->GetItemAllusableElementsforPartition(1));
		}
	}
	return true;
}

///////////////////////

//////////////////////////
//CTL-IL Transform process
void
SP_ExportSMART::Transform_CTL_IL_toSMART()
{
	g_wxAS_CTL_IL_lines_inner.Clear();
	g_wxAS_CTL_IL_lines_outer.Clear();
	wxString line1=wxT("");
	wxString line2=wxT("");
	unsigned long num=0;
	IL_SMART->Set_netname_parameter_string( m_pc_EC->GetConfigurationElement(wxT("netname"))->value + Get_parameter_string());
	//Steadystate
	wxString ss_data=m_pc_EC->GetConfigurationElement(wxT("SteadyState_data"))->value;
	wxArrayString ss_splitlines= SplitwxString(ss_data,wxT("\n"));
	for(unsigned int i=0;(i)<ss_splitlines.Count();i++)
	{
		if(wxT("")==IL_SMART->IL_Check(ss_splitlines.Item(i)))
		{
			num++;
			wxString num_txt=wxString::Format(wxT("%d"),num);
			g_wxAS_CTL_IL_lines_outer.Add(IL_SMART->Transform_IL_prob_outer(num_txt));
			g_wxAS_CTL_IL_lines_inner.Add(IL_SMART->Transform_IL_prob_inner(num_txt));
		}
	}
	//transiente analyse
	wxString ta_data=m_pc_EC->GetConfigurationElement(wxT("Transiente_data"))->value;
	wxArrayString ta_splitlines=SplitwxString(ta_data,wxT("\n"));
	for(unsigned int i=0;(i)<ta_splitlines.Count();i++)
	{
		wxString line=ta_splitlines.Item(i);
		wxString time=line.SubString(0,line.First(wxT(" at "))-1);
		wxString data=line.SubString(line.First(wxT(" at "))+4,line.Len());
		if(wxT("")==IL_SMART->IL_Check(data))
		{
			num++;
			time=time.Trim(true);
			time=time.Trim(false);
			if(time==wxT(""))
				m_pc_EC->SetConfigurationElement(wxT("time_prob_at_default"),true,wxT(""));
			wxString num_txt=wxString::Format(wxT("%d"),num);
			g_wxAS_CTL_IL_lines_outer.Add(IL_SMART->Transform_IL_probat_outer(num_txt,time));
			g_wxAS_CTL_IL_lines_inner.Add(IL_SMART->Transform_IL_probat_inner(num_txt,time));
		}
	}

	//CTL
	CTL_SMART->Set_netname_parameter_string( m_pc_EC->GetConfigurationElement(wxT("netname"))->value + Get_parameter_string());
	wxString ctl_data=m_pc_EC->GetConfigurationElement(wxT("CTL_data"))->value;
	wxArrayString ctl_data_split=SplitwxString(ctl_data,wxT(";"));
	wxString error;
	num=0;
	for(unsigned int i=0; i<ctl_data_split.Count();i++)
	{
		
		error=wxT("");
		
		error=CTL_SMART->CTL_Check(ctl_data_split.Item(i)+ wxT(";"));
		if(error==wxT(""))
		{	
			num++;
			wxString num_txt=wxString::Format(wxT("%d"),num);
			g_wxAS_CTL_IL_lines_outer.Add(CTL_SMART->Transform_CTL_SMART_outer(num_txt));
			g_wxAS_CTL_IL_lines_inner.Add(CTL_SMART->Transform_CTL_SMART_inner(num_txt));	
		}
	}
}


//////////////////////////
