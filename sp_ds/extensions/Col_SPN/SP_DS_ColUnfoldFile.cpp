//////////////////////////////////////////////////////////////////////
// $Author: fei liu $
// $Version: 0.1 $
// $Date: 2011/09/06 $
// Short Description: unfold a net from a file
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_SPN/SP_DS_ColUnfoldFile.h"
#include "sp_utilities.h"

#include "sp_core/SP_Core.h"




SP_DS_ColUnfoldFile::SP_DS_ColUnfoldFile(SP_DS_ColPN_Unfolding* p_pcColPN_Unfolding):
m_pcColPN_Unfolding(p_pcColPN_Unfolding)
{
}

SP_DS_ColUnfoldFile::~SP_DS_ColUnfoldFile()
{
}


bool SP_DS_ColUnfoldFile::DoWrite(const wxString& p_sFile)
{
	//open a file	
	m_fFile.Open(p_sFile.c_str(),wxT("wt"));
	if( !m_fFile.IsOpened())
	{
		return false;
	}		
	
	m_fFile.Write(wxT("\\beginnet{") + GetNetName(p_sFile) + wxT("}\n\n"));
	
	WritePlaceClass();
	WriteTransClass();	

	
	m_fFile.Write(wxT("\n"));
	m_fFile.Write(wxT("\\endnet\n"));


	//close the file
	m_fFile.Close();
	if( m_fFile.IsOpened())
	{
		return false;
	}

	return true;
}

bool SP_DS_ColUnfoldFile::WritePlaceClass()
{
	WritePlaces(m_pcColPN_Unfolding->GetUnfoldedDiscPlaces());
	WritePlaces(m_pcColPN_Unfolding->GetUnfoldedContPlaces());	

	return true;
}

bool SP_DS_ColUnfoldFile::WritePlaces(SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces)
{
	wxString l_sPlaceType;
	SP_CPN_UnfoldedPlaces::iterator itMap1;
	for(itMap1 = p_pmmUnfoldedPlaces.begin(); itMap1 != p_pmmUnfoldedPlaces.end(); itMap1++)
	{
		wxString l_sColPlName = itMap1->first;			
		SP_CPN_UnfoldedPlace::iterator itMap2; 
		wxString l_sBuffer;
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{						
			wxString l_sColor = itMap2->first;								
			wxString l_sNodeType = itMap2->second.m_sNodeType;
			l_sPlaceType = l_sNodeType;
			wxString l_sColorSetName = itMap2->second.m_sColorSet;	
			bool l_bIsolated = itMap2->second.m_bIsolated;

			l_sBuffer << wxT("\\color{") << l_sColor << wxT("}");
			l_sBuffer << wxT("\\nodetype{") << l_sNodeType << wxT("}");
			l_sBuffer << wxT("\\colorset{") << l_sColorSetName << wxT("}");
			l_sBuffer << wxT("\\isolated{") << l_bIsolated << wxT("}");
				
				if(l_sNodeType == SP_DS_DISCRETE_PLACE)
				{
					for(unsigned int i = 0; i < itMap2->second.m_anNetMarkings.size(); i++)
					{
						long l_nMarking = itMap2->second.m_anNetMarkings[i];
						l_sBuffer << wxString::Format(wxT("\\init{%ld}"),l_nMarking);
					}
				}
				if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
				{
					for(unsigned int i = 0; i < itMap2->second.m_adNetMarkings.size(); i++)
					{
						double l_dMarking = itMap2->second.m_adNetMarkings[i];
						l_sBuffer << wxString::Format(wxT("\\init{%f}"),l_dMarking);
					}
				}
				l_sBuffer << wxT("\n");
						
		}
		m_fFile.Write( wxT("\\place \n") );

		m_fFile.Write( wxT("\\name{") + l_sColPlName +wxT("} \n") );
		m_fFile.Write( l_sBuffer );
		m_fFile.Write(wxT("\n"));
	}

	return true;
}

bool SP_DS_ColUnfoldFile::WriteTransClass()
{
	WriteTransitions(m_pcColPN_Unfolding->GetUnfoldedStochTransions());
	WriteTransitions(m_pcColPN_Unfolding->GetUnfoldedContTransions());
	WriteTransitions(m_pcColPN_Unfolding->GetUnfoldedImmTransions());
	WriteTransitions(m_pcColPN_Unfolding->GetUnfoldedDetTransions());
	WriteTransitions(m_pcColPN_Unfolding->GetUnfoldedSchedTransions());

	return true;
}

bool SP_DS_ColUnfoldFile::WriteTransitions(SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions)
{
	wxString l_sTransType;
	SP_CPN_UnfoldedTransitions::iterator itMap1;
	for(itMap1 = p_mmUnfoldedTransions.begin(); itMap1 != p_mmUnfoldedTransions.end(); itMap1++)
	{
		wxString l_sColTRName = itMap1->first;
		wxString l_sBuffer;		
		SP_CPN_UnfoldedTransition::iterator itMap2; 

		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{					
				
				wxString l_sBinding = itMap2->first;				
				wxString l_NodeType = itMap2->second.m_sType;
				l_sTransType = l_NodeType;
				wxString l_sAnimName = itMap2->second.m_sAnimTransInstName;


				l_sBuffer << wxT("\\binding{") << l_sBinding << wxT("}");
				l_sBuffer << wxT("\\nodetype{") << l_NodeType << wxT("}");
				l_sBuffer << wxT("\\animationname{") << l_sAnimName << wxT("}");
				
				for(unsigned int i = 0; i < itMap2->second.m_anNetFunctions.size(); i++)
				{
					wxString l_sfunction = itMap2->second.m_anNetFunctions[i];
					l_sBuffer << wxT("\\function{") + l_sfunction +wxT("}");							
				}	

				//write arcs
				l_sBuffer << wxT("\n");
				WriteArcs(itMap2->second.m_vInputArcs,l_sBuffer,true);
				WriteArcs(itMap2->second.m_vOutputArcs,l_sBuffer,false);
		}

		m_fFile.Write( wxT("\\transition \n") );

		m_fFile.Write( wxT("\\name{") + l_sColTRName +wxT("} \n") );
		m_fFile.Write( l_sBuffer );
		m_fFile.Write(wxT("\n"));
	}


	return TRUE;
}


bool SP_DS_ColUnfoldFile::WriteArcs(vector<SP_CPN_UnfoldedArcInfo>& p_vArcs, wxString& p_sBuffer, bool p_bInput)
{
	for(unsigned i = 0; i < p_vArcs.size(); i++)
	{
		SP_CPN_UnfoldedArcInfo& l_ArcInfo = p_vArcs[i];
		wxString l_sColoredPlName = l_ArcInfo.m_sColPlaceName;
		wxString l_sColor = l_ArcInfo.m_sColor;		
		wxString l_sDiscContType = l_ArcInfo.m_sDiscContType;
		wxString l_sArcType = l_ArcInfo.m_sArcType ;

		if(p_bInput)
			p_sBuffer << wxT("\\inputarc \n");
		else
			p_sBuffer << wxT("\\outputarc \n");

		p_sBuffer << wxT("\\coloredplacename{") << l_sColoredPlName << wxT("}");
		p_sBuffer << wxT("\\placecolor{") << l_sColor << wxT("}");
		p_sBuffer << wxT("\\arctypedc{") << l_sDiscContType << wxT("}");
		p_sBuffer << wxT("\\arctypespecial{") << l_sArcType << wxT("}");

		if(l_sDiscContType == wxT("Place"))
		{
			long l_nMultiplicity = l_ArcInfo.m_nMultiplicity;
			p_sBuffer << wxString::Format(wxT("\\multiplicity{%ld}"),l_nMultiplicity);
		}
		else
		{
			double l_dMultiplicity = l_ArcInfo.m_nMultiplicity;
			p_sBuffer << wxString::Format(wxT("\\multiplicity{%f}"),l_dMultiplicity);
		}
		p_sBuffer << wxT("\n");
	}

	return TRUE;
}




wxString SP_DS_ColUnfoldFile::GetNetName(const wxString& p_fName)
{
	wxFileName fn = wxFileName(p_fName);

	wxString l_sFilename = fn.GetName();
	if (l_sFilename.IsEmpty())
		return l_sFilename;

	wxString tmp = l_sFilename;
	for (size_t i = 0; i < l_sFilename.length(); i++)
	{
		if (!(tmp.GetChar(i).IsAscii()) || ! wxIsalnum(tmp.GetChar(i)))
		{
			tmp.SetChar(i, '_');
		}
	}
	return tmp;
}







bool SP_DS_ColUnfoldFile::DoRead(const wxString& p_sFile)
{
	//open a file	
	m_fFile.Open(p_sFile.c_str());
	if( !m_fFile.IsOpened())
	{
		return false;
	}
	
	wxString l_sInput;
	m_fFile.ReadAll(&l_sInput);
	wxStringTokenizer l_Tokenizer(l_sInput,wxT(" {}\n\r\t"),wxTOKEN_STRTOK);
	m_pTokenizer = &l_Tokenizer;
	
	if( !Parse() )
	{
		return false;
	}


	//close the file
	m_fFile.Close();
	if( m_fFile.IsOpened())
	{
		return false;
	}

	return true;
}


bool SP_DS_ColUnfoldFile::Parse()
{
	if(m_pTokenizer->HasMoreTokens())
	{
		m_sCurrentToken = m_pTokenizer->GetNextToken();
	}
	else
	{
		return false;
	}

	if(m_sCurrentToken.Cmp(wxT("\\beginnet"))==0)
	{
		if(m_pTokenizer->HasMoreTokens())
		{
			wxString l_sNetName = m_pTokenizer->GetNextToken();				
		}
		else
		{
			return FALSE;			
		}
	}
	
	m_sCurrentToken = m_pTokenizer-> GetNextToken();

	while(m_sCurrentToken.Cmp(wxT("")))
	{			
		if(m_sCurrentToken.Cmp(wxT("\\place"))==0)
		{				
			if(!parsePlace())
				return false;
		}		
		else if(m_sCurrentToken.Cmp(wxT("\\transition"))==0)
		{
			if(!parseTransition())
				return false;
		}
		else if(m_sCurrentToken.Cmp(wxT("\\endnet"))==0)
		{
				return true;
		}
		else
		{
			return false;			
		}
		
	}//while		
	
	return true;
}

bool SP_DS_ColUnfoldFile::parsePlace()
{
	if(m_pTokenizer->HasMoreTokens())
	{
		//wxString l_sColoredName = m_pTokenizer->GetNextToken();
	}
	else
	{
		return false;
	}

	wxString l_sColoredName;
	SP_CPN_UnfoldedPlace l_mUnfoldedPlaceInfo;
	bool l_bNext = true;

	while(m_pTokenizer->HasMoreTokens())
	{
		if(l_bNext)
			m_sCurrentToken = m_pTokenizer->GetNextToken();

		if(m_sCurrentToken.Cmp(wxT("\\name"))==0 )
		{
			if(CheckValue())
			{
				l_sColoredName = m_sCurrentToken;				
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\color"))==0 )
		{
			if(!parseColor(l_mUnfoldedPlaceInfo))
				return false;
			l_bNext = false;
		}	
		else
		{
			if(l_mUnfoldedPlaceInfo.size()>0 && l_mUnfoldedPlaceInfo.begin()->second.m_sNodeType == wxT("Place"))
			{
				m_pcColPN_Unfolding->GetUnfoldedDiscPlaces()[l_sColoredName] = l_mUnfoldedPlaceInfo;
			}
			else
			{
				m_pcColPN_Unfolding->GetUnfoldedContPlaces()[l_sColoredName] = l_mUnfoldedPlaceInfo;
			}
			return TRUE;
		}
	}

	return true;
}

bool SP_DS_ColUnfoldFile::parseColor(SP_CPN_UnfoldedPlace& p_mUnfoldedPlaceInfo)
{
	SP_CPN_UnfoldedPlaceInfo l_UnPlInfo;
	wxString l_sColoredName;
	vector<long> l_vnMarking;
	vector<double> l_vdMarking;

	if(m_pTokenizer->HasMoreTokens())
	{
		if(CheckValue())
		{
			l_sColoredName = m_sCurrentToken;				
		}
		else
		{				
			return FALSE;
		}
	}
	else
	{
		return false;
	}
	while(m_pTokenizer->HasMoreTokens())
	{
		m_sCurrentToken = m_pTokenizer->GetNextToken();
		if(m_sCurrentToken.Cmp(wxT("\\nodetype"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sNodetype = m_sCurrentToken;	
				l_UnPlInfo.m_sNodeType = l_sNodetype;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\colorset"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sColorset = m_sCurrentToken;		
				l_UnPlInfo.m_sColorSet = l_sColorset;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\isolated"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sIsolated = m_sCurrentToken;	
				long l_nIsolated;
				if(!l_sIsolated.ToLong(&l_nIsolated))
					return false;
				l_UnPlInfo.m_bIsolated = l_nIsolated;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\init"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sInit = m_sCurrentToken;	
				if( l_UnPlInfo.m_sNodeType == wxT("Place") )
				{
					long l_nMark;
					if(!l_sInit.ToLong(&l_nMark) )
						return false;
					l_vnMarking.push_back(l_nMark);
				}
				else
				{
					double l_dMark;
					if(!l_sInit.ToDouble(&l_dMark) )
						return false;
					l_vdMarking.push_back(l_dMark);
				}
			}
			else
			{					
				return FALSE;
			}
		}		
		else
		{
			if( l_UnPlInfo.m_sNodeType == wxT("Place") )
				l_UnPlInfo.m_anNetMarkings  = l_vnMarking;
			else
				l_UnPlInfo.m_adNetMarkings  = l_vdMarking;

			p_mUnfoldedPlaceInfo[l_sColoredName] = l_UnPlInfo;

			
			
			return TRUE;
		}

	}

	return true;
}



bool SP_DS_ColUnfoldFile::parseTransition()
{
	if(m_pTokenizer->HasMoreTokens())
	{
		//wxString l_sColoredName = m_pTokenizer->GetNextToken();
	}
	else
	{
		return false;
	}

	wxString l_sColoredName;
	SP_CPN_UnfoldedTransition l_mUnfoldedTransInfo;

	bool l_bNext = true;

	while(m_pTokenizer->HasMoreTokens())
	{
		if(l_bNext)
			m_sCurrentToken = m_pTokenizer->GetNextToken();

		if(m_sCurrentToken.Cmp(wxT("\\name"))==0 )
		{
			if(CheckValue())
			{
				l_sColoredName = m_sCurrentToken;				
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\binding"))==0 )
		{
			if(!Parsebinding(l_mUnfoldedTransInfo))
				return false;
			l_bNext = false;
		}	
		else
		{				
			return TRUE;
		}
	}
	if(m_sCurrentToken.Cmp(wxT("\\endnet"))==0)
	{
			if(l_mUnfoldedTransInfo.size()>0 && l_mUnfoldedTransInfo.begin()->second.m_sType == SP_DS_STOCHASTIC_TRANS )
			{
				m_pcColPN_Unfolding->GetUnfoldedStochTransions()[l_sColoredName] = l_mUnfoldedTransInfo;
			}
			else if(l_mUnfoldedTransInfo.size()>0 && l_mUnfoldedTransInfo.begin()->second.m_sType == SP_DS_CONTINUOUS_TRANS )
			{
				m_pcColPN_Unfolding->GetUnfoldedContTransions()[l_sColoredName] = l_mUnfoldedTransInfo;
			}
			else if(l_mUnfoldedTransInfo.size()>0 && l_mUnfoldedTransInfo.begin()->second.m_sType == SP_DS_IMMEDIATE_TRANS )
			{
				m_pcColPN_Unfolding->GetUnfoldedImmTransions()[l_sColoredName] = l_mUnfoldedTransInfo;
			}
			else if(l_mUnfoldedTransInfo.size()>0 && l_mUnfoldedTransInfo.begin()->second.m_sType == SP_DS_DETERMINISTIC_TRANS )
			{
				m_pcColPN_Unfolding->GetUnfoldedDetTransions()[l_sColoredName] = l_mUnfoldedTransInfo;
			}
			else if(l_mUnfoldedTransInfo.size()>0 && l_mUnfoldedTransInfo.begin()->second.m_sType == SP_DS_SCHEDULED_TRANS )
			{
				m_pcColPN_Unfolding->GetUnfoldedSchedTransions()[l_sColoredName] = l_mUnfoldedTransInfo;
			}
	}


	return true;
}

bool SP_DS_ColUnfoldFile::Parsebinding(SP_CPN_UnfoldedTransition& p_nUnfoldedTransInfo)
{
	SP_CPN_UnfoldedTransInfo l_UnTrInfo;
	wxString l_sBinding;
	vector<wxString> l_vFunctions;
	vector<SP_CPN_UnfoldedArcInfo> l_vInputArcs;
	vector<SP_CPN_UnfoldedArcInfo> l_vOutputArcs;

	if(m_pTokenizer->HasMoreTokens())
	{
		if(CheckValue())
		{
			l_sBinding = m_sCurrentToken;				
		}
		else
		{				
			return FALSE;
		}
	}
	else
	{
		return false;
	}

	bool l_bNext = true; 
	while(m_pTokenizer->HasMoreTokens())
	{
		if(l_bNext)
			m_sCurrentToken = m_pTokenizer->GetNextToken();

		if(m_sCurrentToken.Cmp(wxT("\\nodetype"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sNodetype = m_sCurrentToken;	
				l_UnTrInfo.m_sType = l_sNodetype;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\animationname"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sAnimName = m_sCurrentToken;		
				l_UnTrInfo.m_sAnimTransInstName = l_sAnimName;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\function"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sInit = m_sCurrentToken;	
				l_vFunctions.push_back(l_sInit);
			}
			else
			{					
				return FALSE;
			}
		}		
		else if(m_sCurrentToken.Cmp(wxT("\\inputarc"))==0 )
		{
			//
			if(!ParseArc(l_vInputArcs))
				return false;
			l_bNext = false;
		}
		else if(m_sCurrentToken.Cmp(wxT("\\outputarc"))==0 )
		{
			//
			if(!ParseArc(l_vOutputArcs))
				return false;
			l_bNext = false;
		}
		
		else
		{
			l_UnTrInfo.m_anNetFunctions = l_vFunctions;
			l_UnTrInfo.m_vInputArcs  = l_vInputArcs;
			l_UnTrInfo.m_vOutputArcs = l_vOutputArcs;
			p_nUnfoldedTransInfo[l_sBinding] = l_UnTrInfo;
			
			return TRUE;
		}
	}

	if(m_sCurrentToken.Cmp(wxT("\\endnet"))==0)
	{
		l_UnTrInfo.m_anNetFunctions = l_vFunctions;
		l_UnTrInfo.m_vInputArcs  = l_vInputArcs;
		l_UnTrInfo.m_vOutputArcs = l_vOutputArcs;
		p_nUnfoldedTransInfo[l_sBinding] = l_UnTrInfo;
	}

	return true;
}

bool SP_DS_ColUnfoldFile::ParseArc(vector<SP_CPN_UnfoldedArcInfo>& p_vInputArcs)
{
	SP_CPN_UnfoldedArcInfo l_UnArcInfo;

	if(m_pTokenizer->HasMoreTokens())
	{		
	}
	else
	{
		return false;
	}
	while(m_pTokenizer->HasMoreTokens())
	{
		m_sCurrentToken = m_pTokenizer->GetNextToken();
		if(m_sCurrentToken.Cmp(wxT("\\coloredplacename"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sColPlName = m_sCurrentToken;	
				l_UnArcInfo.m_sColPlaceName = l_sColPlName;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\placecolor"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sColor = m_sCurrentToken;		
				l_UnArcInfo.m_sColor = l_sColor;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\arctypedc"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sArc = m_sCurrentToken;		
				l_UnArcInfo.m_sDiscContType = l_sArc;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\arctypespecial"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sArc = m_sCurrentToken;		
				l_UnArcInfo.m_sArcType = l_sArc;
			}
			else
			{				
				return FALSE;
			}
		}
		else if(m_sCurrentToken.Cmp(wxT("\\multiplicity"))==0 )
		{
			if(CheckValue())
			{
				wxString l_sMult = m_sCurrentToken;	
				if(l_UnArcInfo.m_sDiscContType == wxT("Place") )
				{
					long l_nMark;
					if(!l_sMult.ToLong(&l_nMark) )
						return false;
					l_UnArcInfo.m_nMultiplicity  = l_nMark;
					
				}
				else
				{
					double l_dMark;
					if(!l_sMult.ToDouble(&l_dMark) )
						return false;
					l_UnArcInfo.m_dMultiplicity  = l_dMark;
				}
				
			}
			else
			{					
				return FALSE;
			}
		}		
		else
		{			
			p_vInputArcs.push_back(l_UnArcInfo);			
			return TRUE;
		}

	}

	return true;
}

bool SP_DS_ColUnfoldFile::CheckValue()
{
	if(!m_pTokenizer->HasMoreTokens())
		return false;

	m_sCurrentToken = m_pTokenizer->GetNextToken();
	if(m_sCurrentToken.IsSameAs(wxT("\\name")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\color")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\init")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\nodetype")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\colorset")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\isolated")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\binding")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\animationname")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\inputarc")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\coloredplacename")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\placecolor")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\arctypedc")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\arctypespecial")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\multiplicity")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\Place")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\Transition")))return FALSE;
	if(m_sCurrentToken.IsSameAs(wxT("\\endnet")))return FALSE;
	return TRUE;
}
