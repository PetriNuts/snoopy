//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.00 $
// $Date: 2010/08/01 10:20:00 $
// Short Description: generate twin nets class
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_PN/Folding/SP_CPN_GenMasterNet.h"
#include <algorithm>
#include "sp_core/SP_Core.h"

#include <wx/tokenzr.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <iostream>


#include "sp_ds/SP_DS_Metadataclass.h"
#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ChooseMasterNet.h"



SP_CPN_GenMasterNet::SP_CPN_GenMasterNet(SP_DS_Graph* p_pcGraph)
{
	m_pcGraph = p_pcGraph;
	m_sColorSetName = wxT("CS");
	m_sVariableName= wxT("x");

	m_sNetClass = m_pcGraph->GetNetclass()->GetName();	
	if( m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		m_sPlaceNodeClass = SP_DS_CONTINUOUS_PLACE;
		m_sTransNodeClass = SP_DS_CONTINUOUS_TRANS;
		m_sStandardEdgeClass = SP_DS_EDGE;
	}
	else
	{
		m_sPlaceNodeClass = SP_DS_DISCRETE_PLACE;
		m_sTransNodeClass = SP_DS_STOCHASTIC_TRANS;
		m_sStandardEdgeClass = SP_DS_EDGE;
	}	
}

SP_CPN_GenMasterNet::~SP_CPN_GenMasterNet()
{	
}


bool SP_CPN_GenMasterNet::GenerateMasterNet()
{

	if(m_pcGraph->GetNodeclass(m_sPlaceNodeClass)->GetElements()->size() ==  0 && m_pcGraph->GetNodeclass(m_sTransNodeClass)->GetElements()->size() ==  0)
	{
		SP_MESSAGEBOX(wxT("Please open a Petri Net first"), wxT("Generate a master net"), wxOK | wxICON_ERROR);
		return false;
	}	

	SP_DLG_ChooseMasterNet l_cDlg(NULL);
	if (l_cDlg.ShowModal() == wxID_OK)
	{
		m_sFilePath = 	l_cDlg.GetFilePath();
		m_sColorSetName = l_cDlg.GetColorSetName();
		m_sVariableName = l_cDlg.GetVariableName();
		l_cDlg.Destroy();
	}
	else
	{
		l_cDlg.Destroy();
		return true;
	}	
	
	m_mvTransitionToColors.clear();

	wxTextFile l_file;	
	if ( !l_file.Open(m_sFilePath) )
	{
		SP_MESSAGEBOX(wxT("Failed to open a file specifying subnets"), wxT("Generate a master net"), wxOK | wxICON_ERROR);
		compute();
		return false;
	}

	//get tokens
	vector<wxString> l_vLine;
	vector<wxString> l_vIDName;
	vector<wxString> l_vColor_ID_Name;
	
	for (wxString str = l_file.GetFirstLine(); !l_file.Eof(); str = l_file.GetNextLine())
	{
		if (str != wxT(""))
			l_vLine.push_back(str);
	}

	if(l_vLine.size() == 0)
	{
		SP_MESSAGEBOX(wxT("Illegal file. Please reinput."), wxT("Generate a master net"), wxOK | wxICON_ERROR );
		return false;
	}

	if(l_vLine.size() < 2)
	{
		SP_MESSAGEBOX(wxT("The net is not covered by any T-invariant, or the file contains no T-invariant."), wxT("Generate a master net"), wxOK | wxICON_INFORMATION );
		return true;
	}
	
	wxString l_sColor;
	for (unsigned i = 1; i < l_vLine.size(); i++)
	{
		wxStringTokenizer l_tkz( l_vLine[i] );
		if (l_tkz.CountTokens() < 3)
		{
			SP_MESSAGEBOX(wxT("Illegal T-invariant file. Please reinput."), wxT("Generate a master net"), wxOK | wxICON_ERROR );
			return false;
		}

		wxString l_token = l_tkz.GetNextToken();		
		if( l_token != wxT("|") )	
		{
			l_sColor = l_token;
			l_token = l_tkz.GetNextToken();		
			l_token = l_tkz.GetNextToken();	

			m_mvTransitionToColors[l_token].push_back(l_sColor);			
		}
		else
		{
			l_token = l_tkz.GetNextToken();	
			m_mvTransitionToColors[l_token].push_back(l_sColor);
		}		
	}

	m_sColorMax = l_sColor;

	//compare ID/Name with those in the net m_pcGraph
	unsigned int l_nCount = 0;
	SP_ListNode::const_iterator l_Iter;	
	const SP_ListNode* l_plElements = m_pcGraph->GetNodeclass( m_sTransNodeClass )->GetElements();
	for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
	{
		SP_DS_Node* l_pcNode = *l_Iter;
		long l_nID = dynamic_cast<SP_DS_IdAttribute*> (l_pcNode->GetAttribute(wxT("ID")))->GetValue();
		wxString l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		wxString l_sIDDotName = wxString::Format(wxT("%d"),l_nID)+wxT(".") + l_sNodeName;
		if( m_mvTransitionToColors.find(l_sIDDotName) != m_mvTransitionToColors.end() )
		{
			l_nCount++;
		}
		else
		{
			//highlight it
			wxColour l_cRed;
			l_cRed.Set(255,0,0);

			SP_ListGraphic* l_grList;
			SP_ListGraphic::const_iterator l_grIt;
			l_grList = NULL;
			l_grList = l_pcNode->GetGraphics();
			if (l_grList->size() != 0)
			{
				l_grIt = l_grList->begin();
				while (l_grIt != l_grList->end())
				{	
					(*l_grIt)->SetBrush(wxTheBrushList->FindOrCreateBrush(l_cRed, (*l_grIt)->GetBrush()->GetStyle()));
					(*l_grIt)->Update(TRUE);
					l_grIt++;
				}
			}		
			
		}
	}

	if( l_nCount != m_mvTransitionToColors.size() )
	{
		SP_MESSAGEBOX(wxT("Illegal T-invariant file. Please reinput."), wxT("Generate a master net"), wxOK | wxICON_ERROR );
		return false;
	}


	//Generate a master net
	if( !Generate() )
		return false;

	return true;
}

bool SP_CPN_GenMasterNet::Generate()
{

	if( ! GenerateDeclaration() )
		return false;

	if(! ProcessTransition() )
		return false;

	if(! ProcessPlace() )
		return false;

	ProcessArc(m_sStandardEdgeClass);
	ProcessArc( SP_DS_READ_EDGE);
	ProcessArc( SP_DS_RESET_EDGE );
	ProcessArc( SP_DS_EQUAL_EDGE );
	ProcessArc( SP_DS_INHIBITOR_EDGE );
	ProcessArc( SP_DS_MODIFIER_EDGE );
	return true;
}

bool SP_CPN_GenMasterNet::GenerateDeclaration()
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	
	//Generate a color set
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;	
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;

	l_pcColList->Clear();
	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(0,0,m_sColorSetName );
	l_pcColList->SetCell(0,1,wxT("int"));
	l_pcColList->SetCell(0,2,wxT("1-") + m_sColorMax);

	//Declare variables
	l_pcMetadataclass = m_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;

	l_pcColList->AppendEmptyRow();
	l_pcColList->SetCell(0,0,m_sVariableName);
	l_pcColList->SetCell(0,1,m_sColorSetName);

	//update color set definition
	m_pcGraph->CreateDeclarationTree()->UpdateColorSetTree();

	return true;
}

bool SP_CPN_GenMasterNet::ProcessTransition()
{
	SP_DS_ColListAttribute * l_pcColList;
	SP_ListNode::const_iterator l_Iter;	
	const SP_ListNode* l_plElements;
	
	l_plElements = m_pcGraph->GetNodeclass( m_sTransNodeClass )->GetElements();

	for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
	{
		SP_DS_Node* l_pcNode = *l_Iter;
		long l_nID = dynamic_cast<SP_DS_IdAttribute*> (l_pcNode->GetAttribute(wxT("ID")))->GetValue();
		wxString l_sNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST) );
		//write guards
		vector<wxString> l_vColors = m_mvTransitionToColors[ wxString::Format(wxT("%d"),l_nID)+wxT(".") + l_sNodeName];
		wxString l_sGuard = wxT("");
		for(unsigned i = 0; i < l_vColors.size(); i++ )
		{
			if(i == 0)
				l_sGuard = m_sVariableName + wxT("=") + l_vColors[i];
			else
				l_sGuard = l_sGuard + wxT("|") + m_sVariableName + wxT("=") + l_vColors[i];
		}
		l_pcColList->SetCell(0, 1, l_sGuard);	

		//reposition the guard
		SP_ListGraphic::const_iterator l_itGr; 
		for( l_itGr = l_pcColList->GetGraphics()->begin(); l_itGr != l_pcColList->GetGraphics()->end(); l_itGr++)
		{
			(*l_itGr)->SetOffsetX(-45);
			(*l_itGr)->SetOffsetY(-25);  
			(*l_itGr)->SetShow( true );
			(*l_itGr)->Update(true);
		}

		GenerateInitialMarking(l_pcNode,&l_vColors);

		l_pcNode->Update(TRUE);
	}
	return true;
}


bool SP_CPN_GenMasterNet::ProcessPlace()
{
	SP_DS_ColListAttribute * l_pcColList;
	SP_ListNode::const_iterator l_Iter;	
	const SP_ListNode* l_plElements;

	l_plElements = m_pcGraph->GetNodeclass( m_sPlaceNodeClass )->GetElements();
	for ( l_Iter = l_plElements->begin(); l_Iter != l_plElements->end(); ++l_Iter )
	{
		SP_DS_Node* p_pcNode = *l_Iter;

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
		l_pcColList->Clear();

		//
		long l_nColorMax;
		if( ! m_sColorMax.ToLong(&l_nColorMax) )
		{
			l_nColorMax = 0; 
		}
		for( long i = 1; i <= l_nColorMax; i++)
		{
			l_pcColList->AppendEmptyColum();
			l_pcColList->SetColLabel(i+1, wxString::Format(wxT("%d"), i ) + wxT(": Marking"));
		}
		
		wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(p_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		map< wxString, map<wxString, long> >::iterator itMap;
		for( itMap = m_mmColor2PlaceName2Token.begin(); itMap != m_mmColor2PlaceName2Token.end(); itMap++)
		{
			wxString l_sColor = itMap->first;
			map<wxString, long> l_mPlaceName2Token = itMap->second;
			long l_nToken = 0;
			if(l_mPlaceName2Token.find(l_sPlaceName) != l_mPlaceName2Token.end())
			{
				l_nToken = l_mPlaceName2Token[l_sPlaceName];
			}
		
			long l_nColor;
			if( !l_sColor.ToLong(&l_nColor))
			{
				l_nColor = 0;
			}

			if( l_nToken != 0)
			{
				unsigned l_nRow = l_pcColList->AppendEmptyRow();
				l_pcColList->SetCell(l_nRow,0,l_sColor);
				l_pcColList->SetCell(l_nRow,l_nColor+1,wxString::Format(wxT("%d"),l_nToken));
			}			
		}


		SP_DS_TextAttribute* l_pcColorSetAttibute = dynamic_cast< SP_DS_TextAttribute* >(p_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME) );
		l_pcColorSetAttibute->SetValue( m_sColorSetName );
		//do we need to set initial marking?

		SP_DS_Attribute* l_pcMarkingAttr = p_pcNode->GetAttribute(wxT("Marking"));
		if( l_pcMarkingAttr )
		{
			l_pcMarkingAttr->SetValueString( wxString::Format(wxT("%d"),0));
		}

		p_pcNode->Update(TRUE);
	}

	return true;
}

bool SP_CPN_GenMasterNet::ProcessArc(wxString p_sEdgeClassName)
{
	SP_DS_Edgeclass* l_pcNodeclass;
	l_pcNodeclass = m_pcGraph->GetEdgeclass( p_sEdgeClassName );
	if( ! l_pcNodeclass )
		return true;

	SP_ListEdge::const_iterator l_IterEdge;	
	const SP_ListEdge* l_plEdgeElements = m_pcGraph->GetEdgeclass( p_sEdgeClassName )->GetElements();

	for ( l_IterEdge = l_plEdgeElements->begin(); l_IterEdge != l_plEdgeElements->end(); ++l_IterEdge )
	{
		SP_DS_Edge* p_pcEdge = *l_IterEdge;
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(p_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );

		if( ! l_pcColList )
			return false;

		wxString l_sExpression = l_pcColList->GetCell(0,1);
		l_sExpression = l_sExpression.Before(wxT('`'));
		long l_nMultiplicity;
		if(! l_sExpression.ToLong(&l_nMultiplicity))
			l_nMultiplicity = 1;
		if( l_nMultiplicity == 1)
			l_pcColList->SetCell(0, 1, m_sVariableName);	
		else
			l_pcColList->SetCell(0, 1, wxString::Format(wxT("%d`"),l_nMultiplicity) + m_sVariableName);

		ShowArcExpression(l_pcColList);

		p_pcEdge->Update(TRUE);
	}

	return true;
}

void SP_CPN_GenMasterNet::ShowArcExpression(SP_DS_ColListAttribute * p_pcColList)
{
	SP_ListGraphic::const_iterator l_itGr; 
	for( l_itGr = p_pcColList->GetGraphics()->begin(); l_itGr != p_pcColList->GetGraphics()->end(); l_itGr++)
	{
		(*l_itGr)->SetShow( true );
		(*l_itGr)->Update(true);
	}
}


void SP_CPN_GenMasterNet::GenerateInitialMarking(SP_DS_Node* p_pcNode, vector<wxString>* p_pvColors)
{
	SP_ListEdge::const_iterator l_IterEdge;	

	for( l_IterEdge = p_pcNode->GetTargetEdges()->begin(); l_IterEdge != p_pcNode->GetTargetEdges()->end(); l_IterEdge++)
	{
		SP_DS_Edge* l_pcEdge = *l_IterEdge;
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION) );

		wxString l_sExpression = l_pcColList->GetCell(0,1);
		l_sExpression = l_sExpression.Before(wxT('`'));
		long l_nMultiplicity;
		if(! l_sExpression.ToLong(&l_nMultiplicity))
		{
			l_nMultiplicity = 1;
		}

		SP_DS_Node* l_pcPlaceNode = dynamic_cast< SP_DS_Node* >(l_pcEdge->GetSource() );

		//judge if a place contains tokens
		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_MARKINGLIST) );
		if(l_pcColList->GetRowCount()== 0 || 
			(l_pcColList->GetRowCount()== 1 && l_pcColList->GetColCount()==2 && l_pcColList->GetCell(0,1)==wxT(""))||
			(l_pcColList->GetRowCount()== 1 && l_pcColList->GetColCount()==2 && l_pcColList->GetCell(0,1)==wxT("0"))||
			(l_pcColList->GetRowCount()== 1 && l_pcColList->GetColCount()==2 && l_pcColList->GetCell(0,1)==wxT("0.0")))
			continue;

		wxString l_sPlaceNodeName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlaceNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		for(unsigned i = 0; i < p_pvColors->size(); i++)
		{
			wxString l_sColor = (*p_pvColors)[i];
			m_mmColor2PlaceName2Token[l_sColor][l_sPlaceNodeName] = l_nMultiplicity;
		}
	}

}










////////////////////
vector<vector<wxString> > SP_CPN_GenMasterNet::CompMSum( vector<vector<wxString> > A, vector<vector<wxString> > B, wxString c)
{
	vector<vector<wxString> > R;
	R.resize(A.size() );
	for(unsigned k = 0; k < R.size(); k++)
		R[k].resize(R.size());

	

	for(unsigned i = 0; i < A.size(); i++ )
	{
		for(unsigned j = 0; j < A[i].size(); j++ )
		{
			if( A[i][j] == wxT("0") &&  B[i][j] == wxT("0"))
			{	
				R[i][j] = wxT("0");
			}
			else if( A[i][j] == wxT("0") &&  B[i][j] != wxT("0"))
			{
				if(c == wxT("1"))
					R[i][j] = wxT("(") + B[i][j] + wxT(")");
				else
					R[i][j] = wxT("(") + c + wxT("*")+ B[i][j] +wxT(")");
			}
			else if( A[i][j] != wxT("0") &&  B[i][j] == wxT("0"))
			{				
					R[i][j] = wxT("(") + A[i][j] + wxT(")");
				
			}
			else
			{
				if(c == wxT("1"))
					R[i][j] = wxT("(") + A[i][j] +wxT("+")+ B[i][j] + wxT(")");
				else
					R[i][j] = wxT("(") + A[i][j] +wxT("+")+ c + wxT("*") + B[i][j] +wxT(")");
			}
		}
	}

	return R;
}

vector<vector<wxString> > SP_CPN_GenMasterNet::CompMMlt( vector<vector<wxString> > A, vector<vector<wxString> > B)
{
	vector<vector<wxString> > R;
	R.resize(A.size() );
	for(unsigned k = 0; k < R.size(); k++)
		R[k].resize(R.size());

	for(unsigned i = 0; i < A.size(); i++ )
	{
		for(unsigned j = 0; j < A.size(); j++ )
		{			
			wxString res = wxT("0");
			for(unsigned k = 0; k < A.size(); k++ )
			{
				if( !(A[i][k] == wxT("0") ||  B[i][k] == wxT("0")))
					res = res + wxT("+") + A[i][k]+ wxT("*") + B[k][j];				
			}

			if(res == wxT("0") || res == wxT("00"))
				R[i][j] = wxT("0");
			else
			{
				res = res.AfterFirst(wxT('+'));
				R[i][j] = wxT("(") + res + wxT(")") ;
			}
		}
	}

	return R;
}


vector<vector<wxString> > SP_CPN_GenMasterNet::CompKMlt( vector<vector<wxString> > A, vector<vector<wxString> > B)
{
	vector<vector<wxString> > R;
	unsigned l_nsize = A.size()*A.size();
	R.resize( l_nsize );
	for(unsigned k = 0; k < R.size(); k++)
		R[k].resize( l_nsize);

	unsigned int l_nBsize = B.size();

	for(unsigned i = 0; i < A.size(); i++ )
	{
		for(unsigned j = 0; j < A.size(); j++ )
		{			
			for(unsigned m = 0; m < B.size(); m++ )
			{
				for(unsigned n = 0; n < B.size(); n++ )
				{
					if( A[i][j] == wxT("0") ||  B[m][n] == wxT("0"))
					{
						R[i* l_nBsize +m][j*l_nBsize + n] = wxT("0");
					}
					else

						R[i* l_nBsize +m][j*l_nBsize + n] = wxT("(") + A[i][j] + wxT("*") + B[m][n] + wxT(")") ;
				}				
			}					
		}
	}

	return R;
}

void SP_CPN_GenMasterNet::compute()
{
	//six states
	vector<vector<wxString> > Km;	
	Km.resize( 6 );
	for(unsigned k = 0; k < Km.size(); k++)
		Km[k].resize( 6);
	Km[0][0] = wxT("0") ;
	Km[0][1] = wxT("0") ;
	Km[0][2] = wxT("0") ;
	Km[0][3] = wxT("0") ;
	Km[0][4] = wxT("0") ;
	Km[0][5] = wxT("0") ;

	Km[1][0] = wxT("(ka-)") ;
	Km[1][1] = wxT("(-ka-)") ;
	Km[1][2] = wxT("0") ;
	Km[1][3] = wxT("0") ;
	Km[1][4] = wxT("0") ;
	Km[1][5] = wxT("0") ;


	Km[2][0] = wxT("0") ;
	Km[2][1] = wxT("(kb-)") ;
	Km[2][2] = wxT("(-kb-)") ;
	Km[2][3] = wxT("0") ;
	Km[2][4] = wxT("0") ;
	Km[2][5] = wxT("0") ;

	Km[3][0] = wxT("0") ;
	Km[3][1] = wxT("0") ;
	Km[3][2] = wxT("(kc-)") ;
	Km[3][3] = wxT("((-kc-)+(-kd-))") ;
	Km[3][4] = wxT("0") ;
	Km[3][5] = wxT("(kd-)") ;

	Km[4][0] = wxT("0") ;
	Km[4][1] = wxT("0") ;
	Km[4][2] = wxT("0") ;
	Km[4][3] = wxT("(kf-)") ;
	Km[4][4] = wxT("(-kf-)") ;
	Km[4][5] = wxT("0") ;

	Km[5][0] = wxT("0") ;
	Km[5][1] = wxT("(ke-)") ;
	Km[5][2] = wxT("0") ;
	Km[5][3] = wxT("0") ;
	Km[5][4] = wxT("0") ;
	Km[5][5] = wxT("(-ke-)") ;








	vector<vector<wxString> > Kp;	
	Kp.resize( 6 );
	for(unsigned k = 0; k < Kp.size(); k++)
		Kp[k].resize( 6);

	Kp[0][0] = wxT("(-ka+)") ;
	Kp[0][1] = wxT("(ka+)") ;
	Kp[0][2] = wxT("0") ;
	Kp[0][3] = wxT("0") ;
	Kp[0][4] = wxT("0") ;
	Kp[0][5] = wxT("0") ;

	Kp[1][0] = wxT("0") ;
	Kp[1][1] = wxT("((-kb+)+(-ke+))") ;
	Kp[1][2] = wxT("(kb+)") ;
	Kp[1][3] = wxT("0") ;
	Kp[1][4] = wxT("0") ;
	Kp[1][5] = wxT("(ke+)") ;


	Kp[2][0] = wxT("0") ;
	Kp[2][1] = wxT("0") ;
	Kp[2][2] = wxT("(-kc+)") ;
	Kp[2][3] = wxT("(kc+)") ;
	Kp[2][4] = wxT("0") ;
	Kp[2][5] = wxT("0") ;

	Kp[3][0] = wxT("0") ;
	Kp[3][1] = wxT("0") ;
	Kp[3][2] = wxT("0") ;
	Kp[3][3] = wxT("(-kf+)") ;
	Kp[3][4] = wxT("(kf+)") ;
	Kp[3][5] = wxT("0") ;

	Kp[4][0] = wxT("0") ;
	Kp[4][1] = wxT("0") ;
	Kp[4][2] = wxT("0") ;
	Kp[4][3] = wxT("0") ;
	Kp[4][4] = wxT("0") ;
	Kp[4][5] = wxT("0") ;

	Kp[5][0] = wxT("0") ;
	Kp[5][1] = wxT("0") ;
	Kp[5][2] = wxT("0") ;
	Kp[5][3] = wxT("(kd+)") ;
	Kp[5][4] = wxT("0") ;
	Kp[5][5] = wxT("(-kd+)") ;



	vector<vector<wxString> > I;	
	I.resize( 6 );
	for(unsigned k = 0; k < I.size(); k++)
		I[k].resize( 6);
	I[0][0] = wxT("1") ;
	I[0][1] = wxT("0") ;
	I[0][2] = wxT("0") ;
	I[0][3] = wxT("0") ;
	I[0][4] = wxT("0") ;
	I[0][5] = wxT("0") ;

	I[1][0] = wxT("0") ;
	I[1][1] = wxT("1") ;
	I[1][2] = wxT("0") ;
	I[1][3] = wxT("0") ;
	I[1][4] = wxT("0") ;
	I[1][5] = wxT("0") ;

	I[2][0] = wxT("0") ;
	I[2][1] = wxT("0") ;
	I[2][2] = wxT("1") ;
	I[2][3] = wxT("0") ;
	I[2][4] = wxT("0") ;
	I[2][5] = wxT("0") ;

	I[3][0] = wxT("0") ;
	I[3][1] = wxT("0") ;
	I[3][2] = wxT("0") ;
	I[3][3] = wxT("1") ;
	I[3][4] = wxT("0") ;
	I[3][5] = wxT("0") ;

	I[4][0] = wxT("0") ;
	I[4][1] = wxT("0") ;
	I[4][2] = wxT("0") ;
	I[4][3] = wxT("0") ;
	I[4][4] = wxT("1") ;
	I[4][5] = wxT("0") ;

	I[5][0] = wxT("0") ;
	I[5][1] = wxT("0") ;
	I[5][2] = wxT("0") ;
	I[5][3] = wxT("0") ;
	I[5][4] = wxT("0") ;
	I[5][5] = wxT("1") ;

	

	vector<vector<wxString> > Io;	
	Io.resize( 6 );
	for(unsigned k = 0; k < Io.size(); k++)
		Io[k].resize( 6);

	Io[0][0] = wxT("0") ;
	Io[0][1] = wxT("0") ;
	Io[0][2] = wxT("0") ;
	Io[0][3] = wxT("0") ;
	Io[0][4] = wxT("0") ;
	Io[0][5] = wxT("0") ;

	Io[1][0] = wxT("0") ;
	Io[1][1] = wxT("0") ;
	Io[1][2] = wxT("0") ;
	Io[1][3] = wxT("0") ;
	Io[1][4] = wxT("0") ;
	Io[1][5] = wxT("0") ;

	Io[2][0] = wxT("0") ;
	Io[2][1] = wxT("0") ;
	Io[2][2] = wxT("0") ;
	Io[2][3] = wxT("0") ;
	Io[2][4] = wxT("0") ;
	Io[2][5] = wxT("0") ;

	Io[3][0] = wxT("0") ;
	Io[3][1] = wxT("0") ;
	Io[3][2] = wxT("0") ;
	Io[3][3] = wxT("0") ;
	Io[3][4] = wxT("0") ;
	Io[3][5] = wxT("0") ;

	Io[4][0] = wxT("0") ;
	Io[4][1] = wxT("0") ;
	Io[4][2] = wxT("0") ;
	Io[4][3] = wxT("0") ;
	Io[4][4] = wxT("0") ;
	Io[4][5] = wxT("0") ;

	Io[5][0] = wxT("0") ;
	Io[5][1] = wxT("0") ;
	Io[5][2] = wxT("0") ;
	Io[5][3] = wxT("0") ;
	Io[5][4] = wxT("0") ;
	Io[5][5] = wxT("1") ;


	vector<vector<wxString> > R1;
	R1 = CompMSum(Km,Kp,wxT("c_i") );

	vector<vector<wxString> > R21;
	R21 = CompKMlt(R1,I);	

	vector<vector<wxString> > R22;
	R22 = CompKMlt(I,R1);

	vector<vector<wxString> > R2;
	R2 = CompMSum(R21,R22,wxT("1"));
	
	vector<vector<wxString> > R3;
	R3=CompKMlt(CompMMlt(Io,Kp),I);

	vector<vector<wxString> > R4;
	R4=CompKMlt(Io,Kp);	

	vector<vector<wxString> > R5;
	R5=CompKMlt(Kp,Io);	

	vector<vector<wxString> > R6;
	R6=CompKMlt(I,CompMMlt(Io,Kp));

	vector<vector<wxString> > R7;
	R7 = CompMSum(R2,R3,wxT("c_d"));

	vector<vector<wxString> > R8;
	R8 = CompMSum(R7,R4,wxT("c_12"));

	vector<vector<wxString> > R9;
	R9 = CompMSum(R8,R5,wxT("c_21"));

	vector<vector<wxString> > R10;
	R10 = CompMSum(R9,R6,wxT("c_d"));


	SP_LOGERROR(wxT(" R10 \n"));
	for(unsigned i = 0; i < R10.size(); i++ )
	{
		wxString l_srow ;
		l_srow << i;
		SP_LOGERROR( l_srow + wxT(" New row \n"));

		for(unsigned j = 0; j < R10.size(); j++ )
		{	
			SP_LOGERROR(R10[i][j]);
		}
		
	}



	/* three states
	vector<vector<wxString> > Km;	
	Km.resize( 3 );
	for(unsigned k = 0; k < Km.size(); k++)
		Km[k].resize( 3);
	Km[0][0] = wxT("0") ;
	Km[0][1] = wxT("0") ;
	Km[0][2] = wxT("0") ;
	Km[1][0] = wxT("(ka-)") ;
	Km[1][1] = wxT("(-ka-)") ;
	Km[1][2] = wxT("0") ;
	Km[2][0] = wxT("0") ;
	Km[2][1] = wxT("(kb-)") ;
	Km[2][2] = wxT("(-kb-)") ;


	vector<vector<wxString> > Kp;	
	Kp.resize( 3 );
	for(unsigned k = 0; k < Kp.size(); k++)
		Kp[k].resize( 3);
	Kp[0][0] = wxT("(-ka+)") ;
	Kp[0][1] = wxT("(ka+)") ;
	Kp[0][2] = wxT("0") ;
	Kp[1][0] = wxT("0") ;
	Kp[1][1] = wxT("(-kb+)") ;
	Kp[1][2] = wxT("(kb+)") ;
	Kp[2][0] = wxT("0") ;
	Kp[2][1] = wxT("0") ;
	Kp[2][2] = wxT("0") ;


	vector<vector<wxString> > I;	
	I.resize( 3 );
	for(unsigned k = 0; k < I.size(); k++)
		I[k].resize( 3);
	I[0][0] = wxT("1") ;
	I[0][1] = wxT("0") ;
	I[0][2] = wxT("0") ;
	I[1][0] = wxT("0") ;
	I[1][1] = wxT("1") ;
	I[1][2] = wxT("0") ;
	I[2][0] = wxT("0") ;
	I[2][1] = wxT("0") ;
	I[2][2] = wxT("1") ;

	vector<vector<wxString> > Io;	
	Io.resize( 3 );
	for(unsigned k = 0; k < Io.size(); k++)
		Io[k].resize( 3);
	Io[0][0] = wxT("0") ;
	Io[0][1] = wxT("0") ;
	Io[0][2] = wxT("0") ;
	Io[1][0] = wxT("0") ;
	Io[1][1] = wxT("0") ;
	Io[1][2] = wxT("0") ;
	Io[2][0] = wxT("0") ;
	Io[2][1] = wxT("0") ;
	Io[2][2] = wxT("1") ;


	vector<vector<wxString> > R1;
	R1 = CompMSum(Km,Kp,wxT("c_i") );

	vector<vector<wxString> > R21;
	R21 = CompKMlt(R1,I);	

	vector<vector<wxString> > R22;
	R22 = CompKMlt(I,R1);

	vector<vector<wxString> > R2;
	R2 = CompMSum(R21,R22,wxT("1"));
	
	vector<vector<wxString> > R3;
	R3=CompKMlt(CompMMlt(Io,Kp),I);

	vector<vector<wxString> > R4;
	R4=CompKMlt(Io,Kp);	

	vector<vector<wxString> > R5;
	R5=CompKMlt(Kp,Io);	

	vector<vector<wxString> > R6;
	R6=CompKMlt(I,CompMMlt(Io,Kp));

	vector<vector<wxString> > R7;
	R7 = CompMSum(R2,R3,wxT("c_d"));

	vector<vector<wxString> > R8;
	R8 = CompMSum(R7,R4,wxT("c_12"));

	vector<vector<wxString> > R9;
	R9 = CompMSum(R8,R5,wxT("c_21"));

	vector<vector<wxString> > R10;
	R10 = CompMSum(R9,R6,wxT("c_d"));


	SP_LOGERROR(wxT(" R10 \n"));
	for(unsigned i = 0; i < R10.size(); i++ )
	{
		for(unsigned j = 0; j < R10.size(); j++ )
		{	
			SP_LOGERROR(R10[i][j]);
		}
		SP_LOGERROR(wxT(" New row \n"));
	}
	*/

}
