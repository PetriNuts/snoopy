/*
 * SP_DS_CheckNet.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.10.2010
 * Short Description:
 */
 //==============================================================================
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/extensions/ext_SPN/SP_DS_StParser.h"
#include "sp_ds/extensions/HybridPN/SP_DS_CheckTransitions.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"


SP_DS_CheckTransitions::SP_DS_CheckTransitions(SP_DS_Graph* p_pcGraph):m_pcGraph(p_pcGraph)
{

}

bool SP_DS_CheckTransitions :: CheckRateFunction( )
{

	SP_DS_StParser l_cParser;
	bool l_bError=false;

	//if there is no graph the return
	CHECK_POINTER(m_pcGraph,return false);

	const SP_ListNodeclass* l_pcAllNode =m_pcGraph->GetNodeclasses();
	CHECK_POINTER(l_pcAllNode,return false);

	wxString l_sAttributeName;
	wxString l_sTransitionName;

	SP_DS_ColListAttribute* l_pcColList;

	SP_ListNodeclass::const_iterator l_itNodeClass=l_pcAllNode->begin();

	for(;l_itNodeClass!=l_pcAllNode->end();l_itNodeClass++)
	{
		wxString l_sNodeClassName=(*l_itNodeClass)->GetName();

		//Get all node
		const SP_ListNode* l_pcAllNodes=(*l_itNodeClass)->GetElements();
		CHECK_POINTER(l_pcAllNodes,return false);


		//Stochastic || continuous || immediate transition
		if( l_sNodeClassName ==SP_DS_STOCHASTIC_TRANS || l_sNodeClassName==SP_DS_CONTINUOUS_TRANS ||
				                                                          l_sNodeClassName==SP_DS_IMMEDIATE_TRANS)
		{
			l_sAttributeName=wxT("FunctionList");
		}
		else if(l_sNodeClassName==SP_DS_DETERMINISTIC_TRANS)
		{
			l_sAttributeName=wxT("DelayList");
		}
		else if(l_sNodeClassName==SP_DS_SCHEDULED_TRANS)
		{
			l_sAttributeName=wxT("PeriodicList");
		}

		//not transition? so go on
		else continue;

		SP_ListNode::const_iterator l_itNode=l_pcAllNodes->begin();

		for(;l_itNode!=l_pcAllNodes->end();l_itNode++)
		{
			//Get the attribute list
			l_pcColList=dynamic_cast< SP_DS_ColListAttribute* >((*l_itNode)->GetAttribute(l_sAttributeName));
			CHECK_POINTER(l_pcColList,return false);

			wxString l_sFormula = wxT("");

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				if( l_sNodeClassName == SP_DS_STOCHASTIC_TRANS||l_sNodeClassName == SP_DS_CONTINUOUS_TRANS )
				{
					l_sFormula = l_pcColList->GetCell(i, 1);
				}
				if( l_sNodeClassName == SP_DS_IMMEDIATE_TRANS )
				{
					l_sFormula = wxT("ImmediateFiring(") + l_pcColList->GetCell(i, 1) + wxT(")");
				}
				if( l_sNodeClassName == SP_DS_DETERMINISTIC_TRANS  )
				{
					l_sFormula = wxT("TimedFiring(")+ l_pcColList->GetCell(i, 1) +wxT(")");

				}
				if( l_sNodeClassName == SP_DS_SCHEDULED_TRANS)
				{
					wxString l_Begin=l_pcColList->GetCell(i, 1);
					wxString l_Repetition=l_pcColList->GetCell(i, 2);
					wxString l_End=l_pcColList->GetCell(i, 3);

					l_sFormula = wxT("FixedTimedFiring_Periodic(")+ l_Begin +wxT(",")+ l_Repetition+wxT(",")+l_End+wxT(")");
				}

				wxArrayString* l_plMissingVariableList = 0;
				if (!(l_cParser.CheckFormulaFunction(l_sFormula, *l_itNode)))
				{
					l_plMissingVariableList = l_cParser.GetMissingVariableList();

					wxString l_sVariables = wxT("The formula \"");
					l_sVariables += l_sFormula;
					l_sVariables += wxT("\" is incorrect. \n\nOccured problems:\n\n");

					for (unsigned int i = 0; i < l_plMissingVariableList->Count(); i++)
					{
						l_sVariables += (*l_plMissingVariableList)[i];
						l_sVariables += wxT("\n");
					}
					if (l_plMissingVariableList->size()>0)
					{
					   l_sVariables+=wxT("Is not a pre-place or a parameter");
					}

					l_sVariables = l_sTransitionName + wxT(": ") + l_sVariables;
					//SP_MESSAGEBOX(l_sVariables, wxT("Error"), wxOK | wxICON_ERROR );
					SP_LOGERROR(l_sVariables);
					l_bError = true;
				}
			}
		}
	}

	if(l_bError)
		return false;

	return true;
}


