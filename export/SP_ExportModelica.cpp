/*
 * SP_ExportModelica.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 11.8.2010
 * Short Description:
 */
 //==============================================================================
#include "export/SP_ExportModelica.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/extensions/continuous/SP_DS_ContinuousPlaceFormula.h"
#include "sp_gui/dialogs/SP_DLG_ExportProperties.h"

SP_ExportModelica::SP_ExportModelica(): SP_ExportPT()
{
	m_sConnections=wxT("");
	l_mNodeInConnection.clear();
	l_mNodeOutConnection.clear();
}

SP_ExportModelica::~SP_ExportModelica()
{

}

bool SP_ExportModelica::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_HYBRIDPN_CLASS || cName == SP_DS_SPN_CLASS  || cName == SP_DS_CONTINUOUSPED_CLASS );
}


bool SP_ExportModelica::DoWrite()
{
	m_file.Write(wxT("import \"PetriNetsDym.mo\";\n")) ;

	wxString l_sModelName=m_file.GetName().AfterLast(wxT('/'));
	l_sModelName=l_sModelName.Before(wxT('.'));

	m_file.Write(wxT("model ")+l_sModelName);

	if (TransformParameters() && TransformPlaces() && TransformTransitions() && TransformEdges() &&  WriteSimulationFile(l_sModelName))
	{
		m_file.Write(wxT("end ")+l_sModelName + wxT(";"));
		return true;
	}

	else
		return false;

}
bool SP_ExportModelica::TransformPlaces()
{
 const SP_ListNode* l_pcPlaces;
 SP_ListNode::const_iterator l_itPlace;

	//Discrete places
	 CHECK_POINTER(m_graph,return false);
	 if(m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)!=NULL)
	 {
		 l_pcPlaces=m_graph->GetNodeclass(SP_DS_DISCRETE_PLACE)->GetElements();
		 CHECK_POINTER(l_pcPlaces,return false);

		  m_file.Write(wxT("\n"));
		  for(l_itPlace=l_pcPlaces->begin();l_itPlace!= l_pcPlaces->end(); l_itPlace++)
		  {

			  m_file.Write(MakeModelicaPlace(wxT("PD"),*l_itPlace));
			  m_file.Write(wxT("\n"));
		  }
	 }

	  //continuous places
	if(m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)!=NULL)
	{
	  l_pcPlaces=m_graph->GetNodeclass(SP_DS_CONTINUOUS_PLACE)->GetElements();
	  CHECK_POINTER(l_pcPlaces,return false);

	  for(l_itPlace=l_pcPlaces->begin();l_itPlace!= l_pcPlaces->end(); l_itPlace++)
	  {

		m_file.Write(MakeModelicaPlace(wxT("PC"),*l_itPlace));
		m_file.Write(wxT("\n"));
	  }
	}

	return true;
}

wxString SP_ExportModelica::MakeModelicaPlace(const wxString & p_sPlaceType,SP_DS_Node* p_pcPlace)
{
  //Target Edges
  const SP_ListEdge* l_pcTargetEdges=(p_pcPlace)->GetTargetEdges();
  CHECK_POINTER(l_pcTargetEdges,return wxT("Error during places transformation"));

	wxString l_sName = GetNodeName(p_pcPlace);
	wxString l_sMarking;

	//write The places
	wxString l_ModelicaPlace=wxT("");

	if(p_sPlaceType==wxT("PD"))
	{
		l_ModelicaPlace=wxT("  PetriNets.Discrete.") + p_sPlaceType;
		l_sMarking = wxString::Format(wxT("%g"),
					dynamic_cast< SP_DS_DoubleMarkingAttribute* >(( p_pcPlace )->GetAttribute(wxT("Marking")) )->GetValue(true));
	}
	else
	{
		l_ModelicaPlace=wxT("  PetriNets.Continuous.") + p_sPlaceType;
		l_sMarking = wxString::Format(wxT("%ld"),
					dynamic_cast< SP_DS_MarkingAttribute* >(( p_pcPlace )->GetAttribute(wxT("Marking")) )->GetValue(true));
	}

	l_ModelicaPlace+=wxString::Format(wxT("%d"),l_pcTargetEdges->size())+ wxT("_")+wxString::Format(wxT("%d"),GetNodeSourceEdgesCount(p_pcPlace))+
	wxT(" ")+l_sName +wxT("(")+ wxT("num_tokens_start=") + l_sMarking+wxT(");");

	return l_ModelicaPlace;
}

bool SP_ExportModelica::TransformTransitions()
{
	const SP_ListNode* l_pcTrans;
	 SP_ListNode::const_iterator l_itTrans;

	//Continuous Transitions
	 CHECK_POINTER(m_graph,return false);
	 if(m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)!=NULL)
	 {
		 l_pcTrans=m_graph->GetNodeclass(SP_DS_CONTINUOUS_TRANS)->GetElements();
		 CHECK_POINTER(l_pcTrans,return false);
		 //Perform transformation
		 for(l_itTrans=l_pcTrans->begin();l_itTrans!=l_pcTrans->end();l_itTrans++)
		 {
			m_file.Write(MakeModelicaContinuousTransition(*l_itTrans));
			m_file.Write(wxT("\n"));
		 }
	 }

	 //Stochastic Transition
	 if(m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)!=NULL)
	 {
		 l_pcTrans=m_graph->GetNodeclass(SP_DS_STOCHASTIC_TRANS)->GetElements();
		 CHECK_POINTER(l_pcTrans,return false);
		 //Perform transformation
		 for(l_itTrans=l_pcTrans->begin();l_itTrans!=l_pcTrans->end();l_itTrans++)
		 {
			m_file.Write(MakeModelicaStochasticTransition(*l_itTrans));
			m_file.Write(wxT("\n"));
		 }
	 }

	 //Deterministic Transition
	 if(m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)!=NULL)
	 {
		 l_pcTrans=m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements();
		 CHECK_POINTER(l_pcTrans,return false);
		 //Perform transformation
		 for(l_itTrans=l_pcTrans->begin();l_itTrans!=l_pcTrans->end();l_itTrans++)
		 {
			m_file.Write(MakeModelicaDiscreteTransition(*l_itTrans));
			m_file.Write(wxT("\n"));
		 }
	 }

	 //Immediate Transition
	 if(m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)!=NULL)
	 {
		 l_pcTrans=m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements();
		 CHECK_POINTER(l_pcTrans,return false);
		 //Perform transformation
		 for(l_itTrans=l_pcTrans->begin();l_itTrans!=l_pcTrans->end();l_itTrans++)
		 {
			m_file.Write(MakeModelicaDiscreteTransition(*l_itTrans));
			m_file.Write(wxT("\n"));
		 }
	 }


  return true;
}
void SP_ExportModelica::AddConnection(SP_DS_Edge* p_pcEdge)
{
	long l_nOutIndex=++l_mNodeOutConnection[p_pcEdge->GetSource()->GetId()];
	long l_nInIndex=++l_mNodeInConnection[p_pcEdge->GetTarget()->GetId()];
	m_sConnections+=MakeConnector(p_pcEdge,l_nOutIndex,l_nInIndex)+wxT("\n");
}
bool SP_ExportModelica::TransformEdges()
{

	  m_file.Write(wxT("equation\n"));
	  m_file.Write(m_sConnections);

	 return true;
}

wxString SP_ExportModelica:: MakeConnector(SP_DS_Edge* p_pcEdge,long p_nOutIndex,long p_nInIndex)
{
 wxString l_sConnectionStr=wxT("  connect(");
 wxString l_sOutNodeType,l_sInNodeType;

	SP_DS_Node* l_pcOutNode=dynamic_cast<SP_DS_Node*>(p_pcEdge->GetSource());
	SP_DS_Node* l_pcInNode=dynamic_cast<SP_DS_Node*>(p_pcEdge->GetTarget());

	wxString l_sOutNodeName=GetNodeName(l_pcOutNode);
   if(l_pcOutNode->GetClassName().Contains(wxT("Place")))
	   l_sOutNodeType=wxT("Transition");
   else
	   l_sOutNodeType=wxT("Places");

   //In Node wxString
   wxString l_sInNodeName= GetNodeName(l_pcInNode);
   if(l_pcInNode->GetClassName().Contains(wxT("Place")))
	   l_sInNodeType=wxT("Transition");
   else
	   l_sInNodeType=wxT("Places");

 //Write The Connection
   l_sConnectionStr+=l_sOutNodeName+wxT(".out")+l_sOutNodeType+ wxString::Format(wxT("%d"),p_nOutIndex )+
					 wxT(",")+l_sInNodeName+wxT(".in")+l_sInNodeType+wxString::Format(wxT("%d"),p_nInIndex )+wxT(");");
  return l_sConnectionStr;
}
wxString SP_ExportModelica::MakeModelicaContinuousTransition(SP_DS_Node* p_pcTrans)
{

	const SP_ListEdge* l_pcSourceEdges=(p_pcTrans)->GetSourceEdges();
	CHECK_POINTER(l_pcSourceEdges,return wxT("Error during places transformation"));
	wxString l_ModelicaTransition=wxT("  PetriNets.Continuous.TC");

	wxString l_sName = GetNodeName(p_pcTrans);

	l_ModelicaTransition+=wxString::Format(wxT("%d"),GetNodeTargetEdgesCount(p_pcTrans))+wxT("_")+wxString::Format(wxT("%d"),l_pcSourceEdges->size())
			             +wxT(" ")+l_sName+
			                               wxT("(")
			                               +GetNodeArcsWeights(p_pcTrans)+

			                               wxT(");");


	return l_ModelicaTransition;
}
wxString SP_ExportModelica::MakeModelicaStochasticTransition(SP_DS_Node* p_pcTrans)
{
	const SP_ListEdge* l_pcSourceEdges=(p_pcTrans)->GetSourceEdges();
    CHECK_POINTER(l_pcSourceEdges,return wxT("Error during places transformation"));
	wxString l_ModelicaTransition=wxT("  PetriNets.Stochastic.TS");

	wxString l_sName =GetNodeName(p_pcTrans);

	SP_DS_ColListAttribute *l_pcEquationAttr=dynamic_cast<SP_DS_ColListAttribute*>(p_pcTrans->GetAttribute(wxT("FunctionList")));
	CHECK_POINTER(l_pcEquationAttr,return wxT("Error"));

	SP_DS_ContinuousPlaceFormula l_Equation(l_pcEquationAttr->GetActiveCellValue(1));
	l_ModelicaTransition+=wxString::Format(wxT("%d"),GetNodeTargetEdgesCount(p_pcTrans))+wxT("_")+wxString::Format(wxT("%d"),l_pcSourceEdges->size())
				             +wxT(" ")+l_sName+
				                               wxT("(")
				                               +wxT("dis=\"exp\",lambda=")+l_Equation.ReplacePatternInTransitionFormula(p_pcTrans,true)+wxT(",")
				                               +GetNodeArcsWeights(p_pcTrans)+

				                               wxT(");");

	return l_ModelicaTransition;
}
wxString SP_ExportModelica::MakeModelicaDiscreteTransition(SP_DS_Node* p_pcTrans)
{


	const SP_ListEdge* l_pcSourceEdges=(p_pcTrans)->GetSourceEdges();
	CHECK_POINTER(l_pcSourceEdges,return wxT("Error during places transformation"));
	wxString l_ModelicaTransition=wxT("  PetriNets.Discrete.TD");

	wxString l_sName = GetNodeName(p_pcTrans);
	wxString l_sDelay;

	if(p_pcTrans->GetClassName()!=SP_DS_IMMEDIATE_TRANS)
	{
	   SP_DS_ColListAttribute *l_pcDelayAttr=dynamic_cast<SP_DS_ColListAttribute*>(p_pcTrans->GetAttribute(wxT("DelayList")));
	   CHECK_POINTER(l_pcDelayAttr,return wxT("Error"));
	   l_sDelay=l_pcDelayAttr->GetActiveCellValue(1);
	}
	else
		l_sDelay=wxT("0");

	l_ModelicaTransition+=wxString::Format(wxT("%d"),GetNodeTargetEdgesCount(p_pcTrans))+wxT("_")+wxString::Format(wxT("%d"),l_pcSourceEdges->size())
							 +wxT(" ")+l_sName+
											   wxT("(")
											   +wxT("delay=")+l_sDelay+wxT(",")
											   +GetNodeArcsWeights(p_pcTrans)+

											   wxT(");");

	return l_ModelicaTransition;
}

long SP_ExportModelica::GetNodeSourceEdgesCount(SP_DS_Node* p_pcNode)
{
	long l_nEdgesCount=0;
    SP_ListEdge::const_iterator l_itEdge;
	const SP_ListEdge* l_pcSourceEdges=p_pcNode->GetSourceEdges();
	CHECK_POINTER(l_pcSourceEdges,return -1);

	for(l_itEdge=l_pcSourceEdges->begin();l_itEdge!=l_pcSourceEdges->end();l_itEdge++)
		{
		  if((*l_itEdge)->GetClassName()!=SP_DS_EDGE)
			{
				if (!CheckForExistingEdge(p_pcNode,dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetTarget())))
				{
					l_nEdgesCount++;
				}
			}
			else
			l_nEdgesCount++;
		}
	return l_nEdgesCount;
}
long SP_ExportModelica::GetNodeTargetEdgesCount(SP_DS_Node* p_pcNode)
{
	long l_nEdgesCount=0;
	SP_ListEdge::const_iterator l_itEdge;
    const SP_ListEdge* l_pcTargetEdges=p_pcNode->GetTargetEdges();
		CHECK_POINTER(l_pcTargetEdges,return -1);

	for(l_itEdge=l_pcTargetEdges->begin();l_itEdge!=l_pcTargetEdges->end();l_itEdge++)
		{
		   if((*l_itEdge)->GetClassName()!=SP_DS_EDGE)
			{
				if (!CheckForExistingEdge(dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource()),p_pcNode))
				{
					l_nEdgesCount++;
				}
			}
			else
			 l_nEdgesCount++;
		}

   return l_nEdgesCount;
}

bool SP_ExportModelica::CheckForExistingEdge(SP_DS_Node* p_pSourceNodes, SP_DS_Node* p_pcDestNode)
{

	const SP_ListEdge* l_pcSourceEdges=p_pSourceNodes->GetSourceEdges();
    SP_ListEdge::const_iterator l_itEdge;
	CHECK_POINTER(l_pcSourceEdges,return -1);

	for(l_itEdge=l_pcSourceEdges->begin();l_itEdge!=l_pcSourceEdges->end();l_itEdge++)
	{
		if((*l_itEdge)->GetClassName()==SP_DS_EDGE)
		{
			if((*l_itEdge)->GetTarget()==p_pcDestNode)
			return true;
		}
	}

	return false;
}

wxString SP_ExportModelica::GetArcWeight(SP_DS_Edge* p_pcEdge,const wxString& p_sArcAddedWeight)
{
  SP_DS_Attribute* l_pcAtt=p_pcEdge->GetAttribute(wxT("Multiplicity"));

   CHECK_POINTER(l_pcAtt, return wxT("Error: No Equation Attribute"));

   return p_sArcAddedWeight==wxT("")? l_pcAtt->GetValueString():

									l_pcAtt->GetValueString()==wxT("1")? p_sArcAddedWeight :

									l_pcAtt->GetValueString()+wxT("*")+p_sArcAddedWeight;
}

wxString SP_ExportModelica::GetNodeArcsWeights(SP_DS_Node* p_pcNode)
{
	SP_ListEdge::const_iterator l_itEdge;

	wxString l_sResult=wxT("");
	long l_nArcsCount=1;
	wxString l_sArcType;
	const SP_ListEdge* l_pcTargetEdges=p_pcNode->GetTargetEdges();
	const SP_ListEdge* l_pcSourceEdges=p_pcNode->GetSourceEdges();
	CHECK_POINTER(l_pcTargetEdges,return wxT("Error"));
	CHECK_POINTER(l_pcSourceEdges,return wxT("Error"));
    wxString l_sArcAddedWeight(wxT(""));

	if(p_pcNode->GetClassName()==SP_DS_CONTINUOUS_TRANS)
	{
	  SP_DS_ColListAttribute* l_pcAttr=dynamic_cast<SP_DS_ColListAttribute*>(p_pcNode->GetAttribute(wxT("FunctionList")));
	  CHECK_POINTER(l_pcAttr,return wxT("Error"));
	  l_sArcAddedWeight=l_pcAttr->GetActiveCellValue(1);
	  SP_DS_ContinuousPlaceFormula l_equation(l_sArcAddedWeight);
	  l_sArcAddedWeight=l_equation.ReplacePatternInTransitionFormula(p_pcNode,true);

	}
	//Construct the Source Weights
	for(l_itEdge=l_pcSourceEdges->begin();l_itEdge!=l_pcSourceEdges->end();l_itEdge++)
	{
		if((*l_itEdge)->GetClassName()==SP_DS_EDGE)
		{
			l_sResult+= wxT("add")+ wxString::Format(wxT("%d"),l_nArcsCount)+wxT("=")+GetArcWeight(*l_itEdge,l_sArcAddedWeight)+wxT(",");
			AddConnection((*l_itEdge));
			l_nArcsCount++;
		}
	}

	//Construct the Target Weights
	l_nArcsCount=1;
    for(l_itEdge=l_pcTargetEdges->begin();l_itEdge!=l_pcTargetEdges->end();l_itEdge++)
	{
    	l_sArcType=(*l_itEdge)->GetClassName();
    	if((*l_itEdge)->GetClassName()==SP_DS_EDGE)
    	{
    	  l_sResult+= wxT("sub")+wxString::Format(wxT("%d"),l_nArcsCount)+wxT("=")+GetArcWeight(*l_itEdge,l_sArcAddedWeight)+wxT(",")+GetExtendedArcsMultiplicity(l_nArcsCount,dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource()),p_pcNode);
	      l_nArcsCount++;
	      AddConnection((*l_itEdge));
    	}
    	else
    	if(!CheckForExistingEdge(dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource()),p_pcNode) &&
    			(l_sArcType == SP_DS_INHIBITOR_EDGE || l_sArcType == SP_DS_READ_EDGE))
    	 {
    		l_sResult+= wxT("sub")+wxString::Format(wxT("%d"),l_nArcsCount)+wxT("=0")+wxT(",")+GetExtendedArcsMultiplicity(l_nArcsCount,dynamic_cast<SP_DS_Node*>((*l_itEdge)->GetSource()),p_pcNode);
    	    l_nArcsCount++;
    	    AddConnection((*l_itEdge));
    	 }
	}

    l_sResult=l_sResult.BeforeLast(',');
  return l_sResult;
}

wxString SP_ExportModelica::GetExtendedArcsMultiplicity(long p_nArcPosition,SP_DS_Node* p_pSourceNodes,SP_DS_Node* p_pcDestNode)
{
	wxString l_sResult=wxT("");

	const SP_ListEdge* l_pcSourceEdges=p_pSourceNodes->GetSourceEdges();
	SP_ListEdge::const_iterator l_itEdge;
	CHECK_POINTER(l_pcSourceEdges,return wxT("Error: No Source Edges"));

	for(l_itEdge=l_pcSourceEdges->begin();l_itEdge!=l_pcSourceEdges->end();l_itEdge++)
	{

	   if((*l_itEdge)->GetTarget()==p_pcDestNode)
	   {
		  wxString l_sArcType=(*l_itEdge)->GetClassName();

		  //Modelica's inhibition
		  if(l_sArcType==SP_DS_INHIBITOR_EDGE)
			  l_sResult+=wxT("inhibition")+wxString::Format(wxT("%d"),p_nArcPosition)+wxT("=")+GetArcWeight((*l_itEdge),wxT(""))+wxT(",");
		  else
			if(l_sArcType==SP_DS_READ_EDGE)
			  	l_sResult+=wxT("threshold")+wxString::Format(wxT("%d"),p_nArcPosition)+wxT("=")+GetArcWeight((*l_itEdge),wxT(""))+wxT(",");
		  else
			  if(l_sArcType!=SP_DS_EDGE)
				  SP_LOGWARNING(wxT("This arc type is not supported by Modelica:")+l_sArcType+wxT("\n"));

	   }
	}



	return l_sResult;
}

bool SP_ExportModelica::AddToDialog(SP_DLG_ExportProperties* p_pcDlg, SP_MDI_Doc* p_pcDoc)
{

	SP_WDG_NotebookPage* l_pcNotebookPage = p_pcDlg->AddPage(wxT("Simulation"));

    wxSizer* l_pcMainSizer =	new wxStaticBoxSizer( new wxStaticBox( l_pcNotebookPage, -1,
												wxT("") ), wxVERTICAL );

	m_GenerateSimulationFile=new wxCheckBox(l_pcNotebookPage,-1,wxT("Generate Simulation File"));
	m_GenerateSimulationFile->SetValue(true);
	m_pcStartSimulationTimeCtrl = new wxTextCtrl( l_pcNotebookPage, -1,wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pcEndSimulationTimeCtrl=new wxTextCtrl( l_pcNotebookPage, -1, wxT("100"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pcSimulationInterval=new wxTextCtrl( l_pcNotebookPage, -1,wxT("100"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pcTolerance=new wxTextCtrl( l_pcNotebookPage, -1,wxT("1e-4"), wxDefaultPosition, wxDefaultSize, 0 );

	//Start Time
	wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(m_GenerateSimulationFile, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("Start Time:") ), 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(m_pcStartSimulationTimeCtrl, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

    l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("End Time:") ), 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(m_pcEndSimulationTimeCtrl, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("Simulation Interval:") ), 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(m_pcSimulationInterval, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcRowSizer->Add(new wxStaticText( l_pcNotebookPage, -1, wxT("Tolerance:") ), 1, wxALL | wxEXPAND, 5);
	l_pcRowSizer->Add(m_pcTolerance, 0, wxALL, 5);
	l_pcMainSizer->Add(l_pcRowSizer, 1, wxEXPAND | wxALL, 5);

	l_pcNotebookPage->AddControl(l_pcMainSizer, 0, wxEXPAND | wxALL, 5);
	return true;
}

bool SP_ExportModelica::WriteSimulationFile(const wxString& p_sFileName)
{
	wxFFile l_file;
	wxString l_sFilename;
	l_sFilename=m_fileName.BeforeLast(wxT('/'))+wxT("/")+p_sFileName+wxT(".mos");

  if(m_GenerateSimulationFile->GetValue()==true)
  {
	  if(!ValidateSimulationValues())
		 return false;

	  if (!l_file.Open(l_sFilename.c_str(), wxT("wt")) )
		{
			return false;
		}

	l_file.Write(wxT("cd(\" \");	//select the path where the Model and the library are \n"));
	l_file.Write(wxT("loadFile(\"PetriNets.mo\");\n"));
	l_file.Write(wxT("loadFile(\"") +p_sFileName+wxT(".mo\");\n"));
	l_file.Write(wxT("simulate(")+p_sFileName+wxT(", startTime=")+m_pcStartSimulationTimeCtrl->GetValue()+
			    wxT(", stopTime=")+ m_pcEndSimulationTimeCtrl->GetValue()+ wxT(", numberOfIntervals=")+
			    m_pcSimulationInterval->GetValue()+wxT(", tolerance=")+ m_pcTolerance->GetValue()+
			    wxT(", method=\"dassl\"")+wxT(");"));


	// close file handle and check if this was ok
	CHECK_BOOL((l_file.Close()), return false);
  }
	return true;

}
bool SP_ExportModelica::ValidateSimulationValues()
{
  return true;
}

bool SP_ExportModelica::TransformParameters()
{
	 const SP_ListNode* l_pcParams;
	 SP_ListNode::const_iterator l_itParam;

	//Discrete places
	 CHECK_POINTER(m_graph,return false);
	 if(m_graph->GetNodeclass(wxT("Parameter"))!=NULL)
	 {
		 l_pcParams=m_graph->GetNodeclass(wxT("Parameter"))->GetElements();
		 CHECK_POINTER(l_pcParams,return false);

		  m_file.Write(wxT("\n"));
		  for(l_itParam=l_pcParams->begin();l_itParam!= l_pcParams->end(); l_itParam++)
		  {
			  wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>((*l_itParam)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			  wxString l_sMarking=dynamic_cast< SP_DS_ColListAttribute* >(( *l_itParam )->GetAttribute(wxT("ParameterList")) )->GetActiveCellValue(1);
			  m_file.Write(wxT("  parameter Real ")+l_sName+wxT("=")+l_sMarking+wxT(";"));
			  m_file.Write(wxT("\n"));
		  }
	 }

	return true;
}

wxString SP_ExportModelica::GetNodeName(SP_DS_Node* p_pcNode)
{

	wxString l_sName;

	l_sName=p_pcNode->GetAttribute(wxT("Name"))->GetValueString();

	if(l_sName.IsEmpty())
		SP_LOGWARNING(wxT("Modelica does not support empty names"));

	l_sName = dynamic_cast<SP_DS_NameAttribute*>((p_pcNode)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

 return l_sName;
}
