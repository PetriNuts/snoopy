//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2009/02/9 $
// Short Description: export to Prism
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportGCM.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include <wx/tokenzr.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <iostream>
#include <algorithm>

#include "snoopy.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/SP_Core.h"

SP_ExportGCM::SP_ExportGCM()
{
	m_nMode = 0;
	m_PlaceMax.clear();
	m_PlaceName.clear();
	m_Max.clear();
}

SP_ExportGCM::~SP_ExportGCM()
{
}

bool
SP_ExportGCM::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{
	 SP_WDG_NotebookPage* l_pcNotebookPage = p_pcDlg->AddPage(wxT("GCM"));

	wxSizer* l_pcMainTablePropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
											wxT("Boundedness Information") ), wxVERTICAL );
	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("Filename:") ), 0, wxEXPAND | wxALL, 5);

	m_pcFilePickerCtrl = new wxFilePickerCtrl( l_pcNotebookPage, -1, wxT(""), wxT("Browse"), wxT("Text files(*.txt)|*.txt"), wxDefaultPosition,
			wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);
	l_pcRowSizer->Add(m_pcFilePickerCtrl, 1, wxEXPAND | wxALL, 5);
	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcNotebookPage->AddControl(l_pcMainTablePropertiesSizer, 0, wxEXPAND | wxALL, 5);

	l_pcMainTablePropertiesSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
											wxT("Ordering") ), wxVERTICAL );
	l_pcRowSizer = new wxBoxSizer( wxVERTICAL );

	m_pRadioButton1=new wxRadioButton(l_pcNotebookPage, -1, wxT("&Plain"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_pRadioButton2=new wxRadioButton(l_pcNotebookPage, -1, wxT("&Reverse"), wxDefaultPosition, wxDefaultSize);
	m_pRadioButton3=new wxRadioButton(l_pcNotebookPage, -1, wxT("&Random"), wxDefaultPosition, wxDefaultSize);
	m_pRadioButton4=new wxRadioButton(l_pcNotebookPage, -1, wxT("&Noack1"), wxDefaultPosition, wxDefaultSize);
	m_pRadioButton5=new wxRadioButton(l_pcNotebookPage, -1, wxT("&Noack2"), wxDefaultPosition, wxDefaultSize);

	m_pRadioButton5->SetValue(true);

	l_pcRowSizer->Add(m_pRadioButton1, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton2, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton3, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton4, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pRadioButton5, 0, wxEXPAND | wxALL, 5);
	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	m_pRadioButton6=new wxRadioButton(l_pcNotebookPage, -1, wxT("&From file"), wxDefaultPosition, wxDefaultSize);
	m_pcFilePickerCtrlFromFile = new wxFilePickerCtrl( l_pcNotebookPage, -1, wxT(""), wxT("Browse"), wxT("Text files(*.txt)|*.txt"), wxDefaultPosition,
			wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL | wxFLP_CHANGE_DIR);

	l_pcRowSizer->Add(m_pRadioButton6, 0, wxEXPAND | wxALL, 5);
	l_pcRowSizer->Add(m_pcFilePickerCtrlFromFile, 1, wxEXPAND | wxALL, 5);

	l_pcMainTablePropertiesSizer->Add(l_pcRowSizer, 0, wxEXPAND | wxALL, 5);

	l_pcNotebookPage->AddControl(l_pcMainTablePropertiesSizer, 0, wxEXPAND | wxALL, 5);

	 return true;
}

bool
SP_ExportGCM::OnDialogOk()
{
	if(m_pRadioButton1->GetValue())
		m_nOrderMode=SP_PLAIN;
	else if(m_pRadioButton2->GetValue())
		m_nOrderMode=SP_REVERSE;
	else if(m_pRadioButton3->GetValue())
		m_nOrderMode=SP_RANDOM;
	else if(m_pRadioButton4->GetValue())
		m_nOrderMode=SP_NOACK1;
	else if(m_pRadioButton5->GetValue())
		m_nOrderMode=SP_NOACK2;
	else if(m_pRadioButton6->GetValue())
		m_nOrderMode=SP_FROMFILE;
	else
		m_nOrderMode=SP_PLAIN;

	m_FilePath = m_pcFilePickerCtrl->GetPath();
	m_FromFilePath = m_pcFilePickerCtrlFromFile->GetPath();

	if(m_nOrderMode == 5 && m_FromFilePath == wxT(""))
	{
		SP_MESSAGEBOX(wxT("Please select a file"), wxT("Notification"), wxOK | wxICON_INFORMATION );
		return false;
	}

	if(m_FilePath!=wxT(""))
	{
		if((ReadBoundedness())==false)
			return false;
	}

    if(m_nOrderMode == 5 && m_FromFilePath!=wxT(""))
	{
		vector<wxString> l_vOrderedPlace;
		if(FromFile(l_vOrderedPlace)==false)
			return false;
	}

	return true;
}

bool SP_ExportGCM::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_SPN_CLASS);
}

bool SP_ExportGCM::DoWrite()
{
	if (m_FilePath == wxT(""))
	{
		m_nMode = 1;
		OrdinaryWrite();
	}
	else
	{
		m_nMode = 2;
		IntelligentWrite();
	}

	return !m_bError;
}

bool SP_ExportGCM::ReadBoundedness()
{

	////////////////////////////////////
	//Read each line of the file into a vector
	///////////////////////////////////
	wxTextFile l_file;
	m_PlaceMax.clear();
	if (l_file.Open(m_FilePath))
	{
		for (wxString str = l_file.GetFirstLine(); !l_file.Eof(); str = l_file.GetNextLine())
		{
			if (str != wxT(""))
				m_PlaceMax.push_back(str);
		}
	}
	//////////////////////////////////////////////////
	//fill the file to two vectors: place and max vectors
	////////////////////////////////////////////////////
	m_PlaceName.clear();
	m_Max.clear();

	for (unsigned i = 0; i < m_PlaceMax.size(); i++)
	{
		wxStringTokenizer l_tkz(m_PlaceMax[i]);
		if (l_tkz.CountTokens() != 2)
		{
			SP_MESSAGEBOX(wxT("The inputed file is not legal, please reinput."), wxT("Notification"), wxOK | wxICON_INFORMATION );
			return false;
		}
		bool l_bflag = true;
		while (l_tkz.HasMoreTokens())
		{
			wxString l_token = l_tkz.GetNextToken();
			if (l_bflag == true)
			{
				m_PlaceName.push_back(l_token);
				l_bflag = false;
			}
			else
			{
				m_Max.push_back(l_token);
				l_bflag = true;
			}
		}
	}
    ///////////////////////////////
	///////////////////////////////
	long l_nMax;

	for (unsigned i = 0; i < m_Max.size(); i++)
	{
		if (m_Max[i].ToLong(&l_nMax))
		{
			if (l_nMax < 0)
			{
				wxString l_LineNumber;
				if (i == 0)
					l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("st");
				if (i == 1)
					l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("nd");
				if (i == 2)
					l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("rd");
				else
					l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("th");
				SP_MESSAGEBOX(wxT("The maximum value in the ") + l_LineNumber + wxT(" line should not be less than zero. Please modify it"), wxT("Notification"), wxOK | wxICON_INFORMATION );
				return false;
			}
		}
		else
		{
			wxString l_LineNumber;
			if (i == 0)
				l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("st");
			if (i == 1)
				l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("nd");
			if (i == 2)
				l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("rd");
			else
				l_LineNumber = wxString::Format(wxT("%u"), i + 1) + wxT("th");

			SP_MESSAGEBOX(wxT("There are illegal characters in the ") + l_LineNumber + wxT(" line. Please modify it"), wxT("Notification"), wxOK | wxICON_INFORMATION );
			return false;
		}
	}

	////////////////////////////////////////
	//Get the original places from the Graph
	////////////////////////////////////////

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass( wxT("Place") );
	SP_ListNode::const_iterator l_itElem;
	vector<wxString> l_OriginalPlace;
	l_OriginalPlace.clear();

	for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		wxString l_PlaceName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_OriginalPlace.push_back(l_PlaceName);

	}

	if (l_OriginalPlace.size() != m_PlaceName.size())
	{
		SP_MESSAGEBOX(wxT("The places from the file are wrong"), wxT("Notification"), wxOK | wxICON_INFORMATION );
		return false;
	}

	bool l_Flag = false;
	for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
	{
		for (unsigned j = 0; j < m_PlaceName.size(); j++)
		{
			if (l_OriginalPlace[i] == m_PlaceName[j])
			{
				l_Flag = true;
			}
		}
		if (l_Flag == false)
		{
			SP_MESSAGEBOX(wxT("The places from the file are wrong"), wxT("Notification"), wxOK | wxICON_INFORMATION );
			return false;
		}
		l_Flag = false;
	}

	return true;
}

bool SP_ExportGCM::OrdinaryWrite()
{

	m_file.Write(wxT("//created by ") + SP_APP_LONG_NAME + wxT("\n"));
	m_file.Write(wxT("//date: ") + ::wxNow() + wxT("\n\n"));
	WritePlaces();
	WriteParameters();
	WriteTransitions();


	m_file.Write(wxT("\n\n"));

	return !m_bError;

}
bool SP_ExportGCM::IntelligentWrite()
{
	return OrdinaryWrite();
}

bool SP_ExportGCM::WritePlaces()
{
	m_file.Write(wxT("initially\n"));

	PlaceIdMap::iterator pIt;
	SP_Place* m_pSP_Place;

	vector<wxString> l_OriginalPlace;
	vector<wxString> l_OrderPlaces;
	l_OriginalPlace.clear();
	l_OrderPlaces.clear();

	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		m_pSP_Place = (*pIt).second;
		l_OriginalPlace.push_back(m_pSP_Place->m_name);
	}

	switch (m_nOrderMode)
	{
	case 0:
	{
		//Plain ordering
		for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
		{
			l_OrderPlaces.push_back(l_OriginalPlace[i]);
		}
		break;
	}
	case 1:
	{
		Reverse(l_OriginalPlace);
		for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
		{
			l_OrderPlaces.push_back(l_OriginalPlace[i]);
		}
		break;
	}
	case 2:
	{
		Random(l_OriginalPlace);
		for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
		{
			l_OrderPlaces.push_back(l_OriginalPlace[i]);
		}
		break;
	}
	case 3:
	{
		l_OrderPlaces = Noack1();
		break;
	}
	case 4:
	{
		l_OrderPlaces = Noack2();
		break;
	}
	case 5:
	{
		FromFile(l_OrderPlaces);
		break;
	}
	default:
		break;
	}

	for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
	{
		wxString l_Place = l_OrderPlaces[i];
		l_Place << wxT(" = ") << GetMark(l_OrderPlaces[i]);
		m_file.Write(wxT("\t"));
		m_file.Write(l_Place);
		if((i+1) < l_OriginalPlace.size())
		{
			m_file.Write(wxT(","));
		}
		else
		{
			m_file.Write(wxT(";"));
		}
		m_file.Write(wxT("\n"));
	}

	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\n"));

	return TRUE;
}

bool SP_ExportGCM::WriteParameters()
{
	m_file.Write(wxT("constants\n"));

	ParameterMap::iterator l_itParam;
	unsigned int i = 1;
	for(l_itParam = m_parameterNameMap.begin(); l_itParam != m_parameterNameMap.end(); ++l_itParam)
	{
		SP_Parameter *pa = l_itParam->second;
		wxString name = l_itParam->first;

		wxString l_sBuffer = wxT("\t");
		l_sBuffer << name << wxT(" = ") << pa->m_marking;
		if(i < m_parameterNameMap.size())
		{
			l_sBuffer << wxT(",");
		}
		else
		{
			l_sBuffer << wxT(";");
		}
		l_sBuffer << wxT("\n");
		m_file.Write(l_sBuffer);
		++i;
	}
	m_file.Write(wxT("\n\n"));

	return true;
}

bool SP_ExportGCM::WriteTransitions()
{
	m_file.Write(wxT("actions\n"));

	TransitionIdMap::iterator tIt;
	SP_Transition* l_pSP_Transition;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt, aIt2;
	SP_DiscrArc* l_pSP_DiscrArc;

	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		l_pSP_Transition = (*tIt).second;

		//////////////////////
		//write the transition
		//////////////////////

		wxString nName = NormalizeString(l_pSP_Transition->m_name);
		if (l_pSP_Transition->m_name != nName)
		{
			SP_LOGWARNING( wxString::Format(wxT("tr name %s was changed to %s"), l_pSP_Transition->m_name.c_str(), nName.c_str()));
		}
		wxString l_sBuffer;
		l_sBuffer << wxT("//") << nName << wxT("\n");

		///////////////////////
		//Write preconditions
		///////////////////////
		wxString l_sBuffer1;
		wxString l_sBuffer2;
		SP_MapString2Int l_mMultiplicity;
		SP_MapString2Bool l_mReset;

		map<wxString,int> l_mCheckRevserseArcs;

		aList = m_preTrMap[l_pSP_Transition->m_id];
		if (aList)
		{

			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				l_pSP_DiscrArc = (*aIt);
				wxString l_sSeperator;
				if(!l_sBuffer1.IsEmpty())
				{
					l_sSeperator = wxT(" & ");
				}
				wxString nName = GetPlaceName(l_pSP_DiscrArc->m_id);

				//
				wxString l_sTransName = l_pSP_Transition->m_name;
				l_sTransName = l_sTransName + nName;

				if ((l_pSP_DiscrArc->m_edge->GetClassName()) != wxT("Modifier Edge"))
				{
					l_mCheckRevserseArcs[l_sTransName] = l_pSP_Transition->m_id;
				}
				//

				if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Inhibitor Edge"))
				{
					l_sBuffer1 << l_sSeperator << nName << wxT("<") << l_pSP_DiscrArc->m_mult;
				}
				else if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Read Edge"))
				{
					l_sBuffer1 << l_sSeperator << nName << wxT(">") << l_pSP_DiscrArc->m_mult-1;
				}
				else if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Equal Edge"))
				{
					l_sBuffer1 << l_sSeperator << nName << wxT("=") << l_pSP_DiscrArc->m_mult;
				}
				else if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Reset Edge"))
				{
					l_mReset[nName] = true;
				}
				else if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Modifier Edge"))
				{
				}
				else if ((l_pSP_DiscrArc->m_edge->GetClassName()) == wxT("Edge"))
				{
					l_mMultiplicity[nName] -= l_pSP_DiscrArc->m_mult;
				}

			}

			for(SP_MapString2Int::iterator l_it = l_mMultiplicity.begin(); l_it != l_mMultiplicity.end(); l_it++)
			{
				wxString l_sSeperator;
				if(!l_sBuffer1.IsEmpty())
				{
					l_sSeperator = wxT(" & ");
				}

				l_sBuffer1 << l_sSeperator << l_it->first << wxT(">") << ((l_it->second+1) * (-1));

				if(l_mReset.find(l_it->first) != l_mReset.end())
				{
					l_it->second = 0;
				}
			}			
		}

		/////////////////////////
		//write postconditions
		/////////////////////////

		aList = m_postTrMap[l_pSP_Transition->m_id];

		if (aList)
		{
			for (aIt = aList->begin(); aIt != aList->end(); aIt++)
			{
				l_pSP_DiscrArc = (*aIt);

				wxString nName = GetPlaceName(l_pSP_DiscrArc->m_id);

				/*
				wxString l_sTransName = l_pSP_Transition->m_name;
				l_sTransName = l_sTransName + nName;
				if( l_mCheckRevserseArcs.find(l_sTransName) != l_mCheckRevserseArcs.end() )
				{
					l_mMultiplicity[nName] += l_pSP_DiscrArc->m_mult;
					continue;
				}
				*/

				l_mMultiplicity[nName] += l_pSP_DiscrArc->m_mult;
			}
		}

		//begin to write

			for(SP_MapString2Int::iterator l_it = l_mMultiplicity.begin(); l_it != l_mMultiplicity.end(); l_it++)
			{
				wxString l_sSeperator;
				if(!l_sBuffer2.IsEmpty())
				{
					l_sSeperator = wxT(", ");
				}
/*
				if(l_mReset.find(l_it->first) != l_mReset.end())
				{
					l_sBuffer2 << l_sSeperator << l_it->first << wxT("' = ") << l_it->second;
				}
				else
*/
				for(int i = 0; i < abs(l_it->second); ++i)
				{
					if(!l_sBuffer2.IsEmpty())
					{
						l_sSeperator = wxT(", ");
					}
					if(l_it->second < 0)
					{
						l_sBuffer2 << l_sSeperator << l_it->first << wxT("--");
					}
					else
					if(l_it->second > 0)
					{
						l_sBuffer2 << l_sSeperator << l_it->first << wxT("++");
					}
				}
			}

		/////////////////////
		//write function
		////////////////////

		wxString l_sBuffer3;

		if (!l_pSP_Transition->m_function.IsEmpty())
		{
			wxString l_sFunction = l_pSP_Transition->m_function;
			wxString l_sResultFunction;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			////////////////////
			wxString l_BMA = wxT("MassAction");
			if (l_sFunction.Find(l_BMA) != -1)
			{
				//dealing MassAction
				size_t l_lenfun = l_sFunction.length();

				size_t lenBMA = l_BMA.length();
				wxString l_parStr = l_sFunction.SubString(lenBMA + 1, l_lenfun - 2); //such as c9*fs
				wxString l_sResultFunctionPara;

				//////////////////////////////////////////////
				wxString l_sToken;
				for (size_t i = 0; i < l_parStr.Len(); i++)
				{
					wxString l_pchCurrent = l_parStr.Mid(i, 1); //k3*Raf_RasGTP
					if (l_pchCurrent == wxT("(") || l_pchCurrent == wxT(")") || l_pchCurrent == wxT("*") ||
						l_pchCurrent == wxT("+") || l_pchCurrent == wxT(",") || l_pchCurrent == wxT("-") ||
						l_pchCurrent == wxT("/") || l_pchCurrent == wxT("^"))
					{
						if (!l_sToken.IsEmpty())
						{
							l_sResultFunctionPara << l_sToken;
							l_sToken.Clear();
						}
						l_sResultFunctionPara << l_pchCurrent;
					}
					else
					{
						l_sToken << l_pchCurrent;
					}
				}
				if (!l_sToken.IsEmpty())
				{
					l_sResultFunctionPara << l_sToken;
				}
				/////////////////////////////////////////////
				aList = m_preTrMap[l_pSP_Transition->m_id];
				if (aList)
				{
					for (aIt = aList->begin(); aIt != aList->end(); aIt++)
					{
						l_pSP_DiscrArc = (*aIt);
						unsigned l_nmult = l_pSP_DiscrArc->m_mult;
						wxString nName = GetPlaceName(l_pSP_DiscrArc->m_id);
						wxString l_sBinomal;

						//modified on 8 Mar. 2010
						if( l_nmult > 1 )
						{
							for(unsigned j = 1; j <= l_nmult; j++ )
							{
								l_sBinomal += wxT("(") + nName + wxT("+1-")+ wxString::Format(wxT("%d"),j) + wxT(")/") + wxString::Format(wxT("%d"),j);

								if( j != l_nmult )
								{
									l_sBinomal += wxT("*");
								}
							}

							l_sBinomal = wxT("(") + l_sBinomal + wxT(")");
						}
						else
						{
							l_sBinomal = nName;
						}

						//
						l_sResultFunction << wxT("*") << l_sBinomal;
					}
				}
				l_sBuffer3 << wxT("(") << l_sResultFunctionPara << wxT(")") << l_sResultFunction;

			}
			else
				if (l_sFunction.Find(wxT("LevelInterpretation")) != -1)
				{
					//dealing LevelInterpretation
					size_t l_lenfun = l_sFunction.length();
					wxString l_BLI = wxT("LevelInterpretation");
					size_t l_lenBLI = l_BLI.length();
					wxString l_parStr = l_sFunction.SubString(l_lenBLI + 1, l_lenfun - 2);
					wxString l_sResultFunctionPara;
					//////////////////////////////////////////////
				    wxString l_sToken;
					for (size_t i = 0; i < l_parStr.Len(); i++)
					{
						wxString l_pchCurrent = l_parStr.Mid(i, 1); //k3*Raf_RasGTP
						if (l_pchCurrent == wxT("(") || l_pchCurrent == wxT(")") || l_pchCurrent == wxT("*") ||
							l_pchCurrent == wxT("+") || l_pchCurrent == wxT(",") || l_pchCurrent == wxT("-") ||
							l_pchCurrent == wxT("/") || l_pchCurrent == wxT("^"))
						{
							if (!l_sToken.IsEmpty())
							{
								l_sResultFunctionPara << l_sToken;
								l_sToken.Clear();
							}
							l_sResultFunctionPara << l_pchCurrent;
						}
						else
						{
							l_sToken << l_pchCurrent;
						}
					}
					if (!l_sToken.IsEmpty())
					{
						l_sResultFunctionPara << l_sToken;
					}
				/////////////////////////////////////////////
					aList = m_preTrMap[l_pSP_Transition->m_id];
					if (aList)
					{
						for (aIt = aList->begin(); aIt != aList->end(); aIt++)
						{
							l_pSP_DiscrArc = (*aIt);
							wxString nName = GetPlaceName(l_pSP_DiscrArc->m_id);
							l_sResultFunction << wxT("*") << nName;
						}
					}
					int l_numprepla = aList->size() - 1;
					l_sResultFunction << wxT("*func(pow, SF,") << l_numprepla << wxT(")");
					l_sBuffer3 << l_sResultFunctionPara << l_sResultFunction;
				}

				else
				{
					//dealing main
					wxString l_sToken;
					for (size_t i = 0; i < l_sFunction.Len(); i++)
					{
						wxString l_pchCurrent = l_sFunction.Mid(i, 1); //k3*Raf_RasGTP
						if (l_pchCurrent == wxT("(") || l_pchCurrent == wxT(")") || l_pchCurrent == wxT("*") ||
							l_pchCurrent == wxT("+") || l_pchCurrent == wxT(",") || l_pchCurrent == wxT("-") ||
							l_pchCurrent == wxT("/") || l_pchCurrent == wxT("^"))
						{
							if (!l_sToken.IsEmpty())
							{
								l_sResultFunction << l_sToken;
								l_sToken.Clear();
							}
							l_sResultFunction << l_pchCurrent;
						}
						else
						{
							l_sToken << l_pchCurrent;
						}
					}
					if (!l_sToken.IsEmpty())
					{
						l_sResultFunction << l_sToken;
					}
					l_sBuffer3 << l_sResultFunction;
				}
		}

		//write the buffer to file
		wxString l_sBuffer4;
		l_sBuffer4 << l_sBuffer << l_sBuffer1 << wxT(" |- ") << l_sBuffer3 << wxT(" -> ") << l_sBuffer2 << wxT(";");
		l_sBuffer4 << wxT("\n");
		m_file.Write(l_sBuffer4);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

wxString SP_ExportGCM::GetPlaceName(const long PlaceID)
{
	PlaceIdMap::iterator pIt;
	SP_Place* l_pcPlace;
	wxString nName;
	long m_PlaceID = PlaceID;
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		l_pcPlace = (*pIt).second;
		if (l_pcPlace->m_id == m_PlaceID)
			break;
	}
	nName = NormalizeString(l_pcPlace->m_name);
	if (l_pcPlace->m_name != nName)
	{
		SP_LOGWARNING( wxString::Format(wxT("place name %s was changed to %s"), l_pcPlace->m_name.c_str(), nName.c_str()));
	}
	return nName;
}
int SP_ExportGCM::GetMark(const wxString placename)
{
	PlaceIdMap::iterator pIt;
	SP_Place* l_pcPlace;

	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		l_pcPlace = (*pIt).second;
		if (l_pcPlace->m_name == placename)
			return l_pcPlace->m_marking;
	}
	return -1;
}

wxString SP_ExportGCM::GetMax(const wxString placename)
{
	for (unsigned i = 0; i < m_PlaceName.size(); i++)
	{
		if (m_PlaceName[i] == placename)
		{
			return m_Max[i];
		}
	}
	return wxT("");
}

void SP_ExportGCM::Reverse(vector<wxString>& vPlace)
{
	reverse(vPlace.begin(), vPlace.end());
}

void SP_ExportGCM::Random(vector<wxString>& vPlace)
{
	std::random_shuffle(vPlace.begin(), vPlace.end());
}

vector<wxString> SP_ExportGCM::Noack1()
{
	InitNoack1();
	vector<wxString> orderedplaces = OrderNoack1();
	return orderedplaces;
}

vector<wxString> SP_ExportGCM::Noack2()
{
	InitNoack2();
	vector<wxString> orderedplaces = OrderNoack2();
	return orderedplaces;
}

vector<wxString> SP_ExportGCM::OrderNoack1()
{

	vector<SP_Place*> l_vLocalPlaces;
	SP_Place* l_pcPlace;
	PlaceIdMap::iterator pIt;
	vector<wxString> m_vplace;
	m_vplace.clear();
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		l_pcPlace = (*pIt).second;
		l_vLocalPlaces.push_back(l_pcPlace);
	}
	unsigned placeNr_ = l_vLocalPlaces.size();

	vector<bool> localplUsed;//(m_nPlaceNr, false);
	for (unsigned i = 0; i < placeNr_; i++)
	{
		localplUsed.push_back(false);
	}
	vector<float> wv;//(m_nPlaceNr);
	wv.resize(placeNr_);

	//// orderedplcs_ is used to save the ordered places.
	vector<SP_Place*> orderedplcs_;//(m_nPlaceNr);
	orderedplcs_.resize(placeNr_);

	unsigned idx;
	unsigned nr;

	for (unsigned j = 0; j < placeNr_; ++j)
	{
		for (unsigned i = 0; i < placeNr_; ++i)
		{
			if (localplUsed[i])
			{
				wv[i] = 0;
			}
			else
			{
				wv[i] = CalcWeightforNoack1(i);
			}

		}
		idx = FindIdxOfMax(wv, localplUsed);
		nr = placeNr_ - 1 - j;
		orderedplcs_[nr] = l_vLocalPlaces[idx];
		localplUsed[idx] = true;
		AssignedforNoack1(nr, idx);
	}
	for (unsigned i = 0; i < orderedplcs_.size(); i++)
	{
		m_vplace.push_back(GetPlaceName(orderedplcs_[i]->m_id));
	}
	return m_vplace;

}

float SP_ExportGCM::CalcWeightforNoack1(unsigned idx)
{
	float res = 0.0;
	unsigned nrCounted = 0;

	SP_Transition* t;
	unsigned prePNr;
	unsigned postPNr;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	aList = m_prePlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{

		for (aIt = aList->begin(); aIt != aList->end(); ++aIt)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetSource());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			prePNr = m_preTrMap[lFrom] ? m_preTrMap[lFrom]->size() : 0;
			postPNr = m_postTrMap[lFrom] ? m_postTrMap[lFrom]->size() : 0;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					t = m_vTransitions[i];
					break;
				}
			}
			if (prePNr > 0 && postPNr > 0)
			{
				if (m_mTransitionPrePlaceUsed[t] == 0)
				{
					res += 0.1 / prePNr;
				}
				else
				{
					res += m_mTransitionPrePlaceUsed[t] / prePNr;
				}
				res += (2 * m_mTransitionPostPlaceUsed[t]) / postPNr;
				++nrCounted;
			}
		}
	}
	/////////////
	aList = m_postPlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{

		for (aIt = aList->begin(); aIt != aList->end(); ++aIt)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetTarget());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			prePNr = m_preTrMap[lFrom] ? m_preTrMap[lFrom]->size() : 0;
			postPNr = m_postTrMap[lFrom] ? m_postTrMap[lFrom]->size() : 0;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					t = m_vTransitions[i];
					break;
				}
			}
			if (prePNr > 0 && postPNr > 0)
			{
				res += (m_mTransitionPrePlaceUsed[t] + 1) / prePNr;
				if (m_mTransitionPostPlaceUsed[t] == 0)
				{
					res += 0.2 / postPNr;
				}
				else
				{
					res += 2 * m_mTransitionPostPlaceUsed[t] / postPNr;
				}
				++nrCounted;
			}
		}
	}
	res /= nrCounted;
	return res;

}

void SP_ExportGCM::InitNoack1()
{
	SP_Place* m_pSP_Place;
	PlaceIdMap::iterator pIt;
	m_vPlaces.clear();
	m_vTransitions.clear();
	m_vPlacesUsed.clear();
	m_mTransitionPrePlaceUsed.clear();
	m_mTransitionPostPlaceUsed.clear();

	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++) //
	{
		m_pSP_Place = (*pIt).second;
		m_vPlaces.push_back(m_pSP_Place);
	}
	m_nPlaceNr = m_vPlaces.size();

	TransitionIdMap::iterator tIt;
	SP_Transition* l_pSP_Transition;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		l_pSP_Transition = (*tIt).second;
		m_vTransitions.push_back(l_pSP_Transition);
	}
	m_nTransNr = m_vTransitions.size();
	for (unsigned i = 0; i < m_nPlaceNr; i++)
	{
		m_vPlacesUsed.push_back(false);
	}
	for (unsigned i = 0; i < m_nTransNr; ++i)
	{
		m_mTransitionPrePlaceUsed.insert(pair<SP_Transition*, float> ((m_vTransitions)[i], 0));
		m_mTransitionPostPlaceUsed.insert(pair<SP_Transition*, float> ((m_vTransitions)[i], 0));
	}
}

void SP_ExportGCM::AssignedforNoack1(unsigned nr, unsigned idx)
{
	m_vPlacesUsed[idx] = true;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	aList = m_postPlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{
		for (aIt = aList->begin(); aIt != aList->end(); ++aIt)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetTarget());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			SP_Transition* l_pSP_Transition;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{

				if (m_vTransitions[i]->m_id == lFrom)
				{
					l_pSP_Transition = m_vTransitions[i];
					break;
				}
			}
			++m_mTransitionPrePlaceUsed[l_pSP_Transition];
		}
	}

	aList = m_prePlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{

		for (aIt = aList->begin(); aIt != aList->end(); ++aIt)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetSource());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			SP_Transition* l_pSP_Transition;

			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					l_pSP_Transition = m_vTransitions[i];
					break;
				}
			}
			++m_mTransitionPostPlaceUsed[l_pSP_Transition];
		}
	}
}

unsigned SP_ExportGCM::FindIdxOfMax(const vector<float>& v, const vector<bool>& used)
{
	unsigned sz = v.size();
	unsigned res = 0;
	float foundMax = 0;
	for (unsigned i = 0; i < sz; ++i)
	{
		if (v[i] > foundMax)
		{
			res = i;
			foundMax = v[i];
		}
	}
	// all were 0, take first !used
	if (res == 0 && foundMax == 0)
	{
		for (unsigned i = 0; i < sz; ++i)
		{
			if (!used[i])
			{
				res = i;
				break;
			}
		}
	}
	return res;
}

void SP_ExportGCM::InitNoack2()
{
	SP_Place* m_pSP_Place;
	PlaceIdMap::iterator pIt;
	m_vPlaces.clear();
	m_vTransitions.clear();
	m_vPlacesUsed.clear();
	m_mTransitionPrePlaceUsed_2.clear();
	m_mTransitionPostPlaceUsed_2.clear();
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		m_pSP_Place = (*pIt).second;
		m_vPlaces.push_back(m_pSP_Place);
	}
	m_nPlaceNr = m_vPlaces.size();

	TransitionIdMap::iterator tIt;
	SP_Transition* l_pSP_Transition;
	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		l_pSP_Transition = (*tIt).second;
		m_vTransitions.push_back(l_pSP_Transition);
	}
	m_nTransNr = m_vTransitions.size();
	for (unsigned i = 0; i < m_nPlaceNr; i++)
	{
		m_vPlacesUsed.push_back(false);
	}
	for (unsigned i = 0; i < m_nTransNr; ++i)
	{
		m_mTransitionPrePlaceUsed_2.insert(pair<SP_Transition*, float> ((m_vTransitions)[i], 0));
		m_mTransitionPostPlaceUsed_2.insert(pair<SP_Transition*, float> ((m_vTransitions)[i], 0));
	}
}

float SP_ExportGCM::calcWeightforNoack2(unsigned idx)
{
	float res = 0.0;
	unsigned nrCounted = 0;
	SP_Transition* t;
	unsigned prePNr;
	unsigned postPNr;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	aList = m_prePlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{
		for (aIt = aList->begin(); aIt != aList->end(); aIt++)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetSource());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			prePNr = m_preTrMap[lFrom] ? m_preTrMap[lFrom]->size() : 0;
			postPNr = m_postTrMap[lFrom] ? m_postTrMap[lFrom]->size() : 0;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					t = m_vTransitions[i];
					break;
				}
			}
			if (prePNr == 0 && postPNr == 0)
				continue;

			if (prePNr > 0)
			{
				if (m_mTransitionPrePlaceUsed_2[t] == 0)
				{
					res += 0.1 / prePNr;
				}
				else
				{
					res += (double) (m_mTransitionPrePlaceUsed_2[t]) / prePNr;
				}
			}
			if (postPNr > 0)
			{
				if (m_mTransitionPostPlaceUsed_2[t] == 0)
				{
					res += 0.1 / postPNr;
				}
				else
				{
					res += (double) (2 * m_mTransitionPostPlaceUsed_2[t]) / postPNr;
				}
				++nrCounted;

			}
		}
	}

	aList = m_postPlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{

		for (aIt = aList->begin(); aIt != aList->end(); aIt++)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetTarget());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			prePNr = m_preTrMap[lFrom] ? m_preTrMap[lFrom]->size() : 0;
			postPNr = m_postTrMap[lFrom] ? m_postTrMap[lFrom]->size() : 0;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					t = m_vTransitions[i];
					break;
				}
			}
			if (prePNr > 0 && postPNr > 0)
			{
				res += (float) (m_mTransitionPrePlaceUsed_2[t] + 1) / prePNr;
				if (m_mTransitionPostPlaceUsed_2[t] == 0)
				{
					res += 0.2 / postPNr;
				}
				else
				{
					res += (float) (2 * m_mTransitionPostPlaceUsed_2[t]) / postPNr;
				}
				++nrCounted;
			}
		}
	}

	return res /= nrCounted;

}

void SP_ExportGCM::AssignedforNoack2(unsigned nr, unsigned idx)
{
	m_vPlacesUsed[idx] = true;
	list<SP_DiscrArc*>* aList;
	list<SP_DiscrArc*>::iterator aIt;
	aList = m_postPlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{
		for (aIt = aList->begin(); aIt != aList->end(); aIt++)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetTarget());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			SP_Transition* l_pSP_Transition;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					l_pSP_Transition = m_vTransitions[i];
					break;
				}
			}
			++m_mTransitionPrePlaceUsed_2[l_pSP_Transition];
		}
	}

	aList = m_prePlMap[(m_vPlaces[idx])->m_id];
	if (aList)
	{

		for (aIt = aList->begin(); aIt != aList->end(); aIt++)
		{
			SP_DS_Node* nFrom = (SP_DS_Node*) ((*aIt)->m_edge->GetSource());
			wxString sFrom;
			long lFrom;
			sFrom = nFrom->GetAttribute(wxT("ID"))->GetValueString();
			sFrom.ToLong(&lFrom);
			SP_Transition* l_pSP_Transition;
			for (unsigned i = 0; i < m_nTransNr; i++)
			{
				if (m_vTransitions[i]->m_id == lFrom)
				{
					l_pSP_Transition = m_vTransitions[i];
					break;
				}
			}
			++m_mTransitionPostPlaceUsed_2[l_pSP_Transition];
		}
	}

}

vector<wxString> SP_ExportGCM::OrderNoack2()
{
	vector<SP_Place*> l_vLocalPlaces;
	vector<wxString> m_vplace;
	m_vplace.clear();
	SP_Place* l_pPlace;
	PlaceIdMap::iterator pIt;
	for (pIt = m_placeIdMap.begin(); pIt != m_placeIdMap.end(); pIt++)
	{
		l_pPlace = (*pIt).second;
		l_vLocalPlaces.push_back(l_pPlace);
	}
	unsigned placeNr_ = l_vLocalPlaces.size();
	vector<bool> localplUsed;
	for (unsigned i = 0; i < placeNr_; i++)
	{
		localplUsed.push_back(false);
	}
	vector<float> wv;
	vector<SP_Place*> orderedplcs_;
	wv.resize(placeNr_);
	orderedplcs_.resize(placeNr_);
	unsigned idx;
	unsigned nr;
	for (unsigned j = 0; j < placeNr_; ++j)
	{
		for (unsigned i = 0; i < placeNr_; ++i)
		{
			if (localplUsed[i])
			{
				wv[i] = 0;
			}
			else
			{
				wv[i] = calcWeightforNoack2(i);
			}

		}
		idx = FindIdxOfMax(wv, localplUsed);
		nr = placeNr_ - 1 - j;
		orderedplcs_[nr] = l_vLocalPlaces[idx];
		localplUsed[idx] = true;
		AssignedforNoack2(nr, idx);
	}
	for (unsigned i = 0; i < orderedplcs_.size(); i++)
	{
		m_vplace.push_back(GetPlaceName(orderedplcs_[i]->m_id));
	}
	return m_vplace;

}

bool SP_ExportGCM::FromFile(vector<wxString>& vOrderedPlace)
{
	wxTextFile file;
	vOrderedPlace.clear();
	if (file.Open(m_FromFilePath))
	{
		for (wxString str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine())
		{
			if (str != wxT(""))
			{
				wxStringTokenizer l_tkz(str);
				vOrderedPlace.push_back(l_tkz.GetNextToken());
			}
		}
	}


	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass( wxT("Place") );
	SP_ListNode::const_iterator l_itElem;
	vector<wxString> l_OriginalPlace;
	l_OriginalPlace.clear();

	for( l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		wxString l_PlaceName=dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_OriginalPlace.push_back(l_PlaceName);

	}

	if (l_OriginalPlace.size() != vOrderedPlace.size())
	{
		SP_MESSAGEBOX(wxT("The places from the file are wrong"), wxT("Notification"), wxOK | wxICON_INFORMATION );
		return false;
	}

	bool l_Flag = false;

	for (unsigned i = 0; i < l_OriginalPlace.size(); i++)
	{
		for (unsigned j = 0; j < vOrderedPlace.size(); j++)
		{
			if (l_OriginalPlace[i] == vOrderedPlace[j])
			{
				l_Flag = true;
			}
		}
		if (l_Flag == false)
		{
			SP_MESSAGEBOX(wxT("The place of ") + l_OriginalPlace[i] + wxT(" is not found in the file. Please modify it"), wxT("Notification"), wxOK | wxICON_INFORMATION );
            return false;
		}
		l_Flag = false;
	}

	return true;
}

wxString SP_ExportGCM::ReadConstant()
{

	wxString l_Constant;
	TransitionIdMap::iterator tIt;
	SP_Transition* l_pSP_Transition;

	for (tIt = m_trIdMap.begin(); tIt != m_trIdMap.end(); tIt++)
	{
		l_pSP_Transition = (*tIt).second;
		if (!l_pSP_Transition->m_function.IsEmpty())
		{
			wxString l_sFunction = l_pSP_Transition->m_function;
			wxString l_sResultFunction;
			l_sFunction.Replace(wxT(" "), wxT(""));
			l_sFunction.Replace(wxT("\t"), wxT(""));
			l_sFunction.Replace(wxT("\r"), wxT(""));
			l_sFunction.Replace(wxT("\n"), wxT(""));

			if (l_sFunction.Find(wxT("LevelInterpretation")) != -1)
			{
				l_Constant = wxT("const double SF;\n");
				break;
			}

			else
				l_Constant = wxT("");
		}
	}

	return l_Constant;
}

