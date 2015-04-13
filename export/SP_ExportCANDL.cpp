//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2012/01/06 $
// Short Description: export to CANDL
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportCANDL.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"

#include <wx/tokenzr.h>


SP_ExportCANDL::SP_ExportCANDL()
{
}

SP_ExportCANDL::~SP_ExportCANDL()
{
}

bool SP_ExportCANDL::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS ||
			cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS ||
			cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportCANDL::DoWrite()
{
	wxString l_sType = wxT("colpn");
	if(m_doc->GetNetclassName() == SP_DS_COLEPN_CLASS)
	{
		l_sType = wxT("colxpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLSPN_CLASS)
	{
		l_sType = wxT("colspn");
		if(!m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("colgspn");
		}
		if(!m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
			|| !m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("colxspn");
		}
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLCPN_CLASS)
	{
		l_sType = wxT("colcpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLHPN_CLASS)
	{
		l_sType = wxT("colhpn");
	}
	m_file.Write(l_sType + wxT(" [") + GetNetName(m_fileName) + wxT("]\n") + wxT("{\n"));
	WriteConstants();
	WriteColorsets();
	WriteVariables();
	WriteFunctions();
	WritePlaces();
	WriteObservers();
	WriteTransitions();



	m_file.Write(wxT("}\n"));
	return !m_bError;
}

bool SP_ExportCANDL::WritePlaces()
{
	m_file.Write(wxT("places:\n"));

	SP_ListNodeclass::const_iterator itNC = m_graph->GetNodeclasses()->begin();
	for(; itNC != m_graph->GetNodeclasses()->end(); ++itNC)
	{
		wxString l_sNCName = (*itNC)->GetDisplayName();
		if(l_sNCName.Find(wxT("Place")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !(*itNC)->GetElements()->empty())
			{
				m_file.Write(wxT("  ") + l_sNCName.BeforeFirst(' ').Lower() + wxT(":\n"));
			}
			SP_ListNode::const_iterator itNode = (*itNC)->GetElements()->begin();
			for(; itNode != (*itNC)->GetElements()->end(); ++itNode)
			{
				SP_DS_Node* l_pcNode = (*itNode);
				wxString l_sBuffer = wxT("    ");
				l_sBuffer << dynamic_cast<SP_DS_TextAttribute*>(l_pcNode->GetAttribute(SP_DS_CPN_COLORSETNAME))->GetValue();
				l_sBuffer << wxT(" ");
				l_sBuffer << dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_sBuffer << wxT(" = { ");
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_MARKINGLIST));
				for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
				{
					wxString l_sColors = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn());
					if(l_sColors.IsEmpty())
					{
						l_sColors = wxT("all()");
					}
					wxString l_sMarking = l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1);
					if(l_sMarking.IsEmpty())
					{
						l_sMarking = wxT("0");
					}
					if(i>0)
					{
						l_sBuffer << wxT(",\n        ");
					}
					l_sBuffer << wxT("{") << l_sMarking
							<< wxT("`") << l_sColors
							<< wxT("}");
				}
				l_sBuffer << wxT(" };\n");
				m_file.Write(l_sBuffer);
			}
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCANDL::WriteTransitions()
{
	m_file.Write(wxT("transitions:\n"));
	wxString l_Sep = wxT(" & ");

	SP_ListNodeclass::const_iterator itNC = m_graph->GetNodeclasses()->begin();
	for(; itNC != m_graph->GetNodeclasses()->end(); ++itNC)
	{
		wxString l_sNCName = (*itNC)->GetDisplayName();
		if(l_sNCName.Find(wxT("Transition")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			if(l_sNCName.Find(' ') != wxNOT_FOUND && !(*itNC)->GetElements()->empty())
			{
				m_file.Write(wxT("  ") + l_sNCName.BeforeFirst(' ').Lower() + wxT(":\n"));
			}
			SP_ListNode::const_iterator itNode = (*itNC)->GetElements()->begin();
			for(; itNode != (*itNC)->GetElements()->end(); ++itNode)
			{
				SP_DS_Node* l_pcNode = (*itNode);
				wxString l_sBuffer = wxT("    ");
				l_sBuffer << dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				//guard
				wxString l_sGuard;
				if(l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(SP_DS_CPN_GUARDLIST));
					l_sGuard << l_pcColList->GetActiveCellValue(1);
					if(!l_sGuard.IsEmpty())
					{
						l_sGuard.Prepend(wxT(" ["));
						l_sGuard.Append(wxT("] "));
					}
				}


				// conditions && updates
				wxString l_sConditions;
				wxString l_sUpdates;

				SP_ListEdge::const_iterator itEdge = l_pcNode->GetSourceEdges()->begin();
				for(; itEdge != l_pcNode->GetSourceEdges()->end(); ++itEdge)
				{
					SP_DS_Edge* l_pcEdge = (*itEdge);
					SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
					wxString l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if(!l_sUpdates.IsEmpty())
					{
						l_sUpdates << l_Sep;
					}
					l_sUpdates << wxT("[");
					l_sUpdates << l_sTarget;
					l_sUpdates << wxT(" + {");
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
					l_sUpdates << l_pcColList->GetActiveCellValue(1);
					l_sUpdates << wxT("}]");
				}

				itEdge = l_pcNode->GetTargetEdges()->begin();
				for(; itEdge != l_pcNode->GetTargetEdges()->end(); ++itEdge)
				{
					SP_DS_Edge* l_pcEdge = (*itEdge);
					SP_DS_Node* l_pcSource = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					wxString l_sSource = dynamic_cast<SP_DS_NameAttribute*>(l_pcSource->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					if(l_pcEdge->GetClassName() == SP_DS_EDGE)
					{
						if(!l_sUpdates.IsEmpty())
						{
							l_sUpdates << l_Sep;
						}
						l_sUpdates << wxT("[");
						l_sUpdates << l_sSource;
						l_sUpdates << wxT(" - {");
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						l_sUpdates << l_pcColList->GetActiveCellValue(1);
						l_sUpdates << wxT("}]");
					}
					else if(l_pcEdge->GetClassName() == SP_DS_READ_EDGE)
					{
						if(!l_sConditions.IsEmpty())
						{
							l_sConditions << l_Sep;
						}
						l_sConditions << wxT("[");
						l_sConditions << l_sSource;
						l_sConditions << wxT(" >= {");
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						l_sConditions << l_pcColList->GetActiveCellValue(1);
						l_sConditions << wxT("}]");
					}
					else if(l_pcEdge->GetClassName() == SP_DS_INHIBITOR_EDGE)
					{
						if(!l_sConditions.IsEmpty())
						{
							l_sConditions << l_Sep;
						}
						l_sConditions << wxT("[");
						l_sConditions << l_sSource;
						l_sConditions << wxT(" < {");
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						l_sConditions << l_pcColList->GetActiveCellValue(1);
						l_sConditions << wxT("}]");
					}
					else if(l_pcEdge->GetClassName() == SP_DS_EQUAL_EDGE)
					{
						if(!l_sConditions.IsEmpty())
						{
							l_sConditions << l_Sep;
						}
						l_sConditions << wxT("[");
						l_sConditions << l_sSource;
						l_sConditions << wxT(" = {");
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						l_sConditions << l_pcColList->GetActiveCellValue(1);
						l_sConditions << wxT("}]");
					}
					else if(l_pcEdge->GetClassName() == SP_DS_RESET_EDGE)
					{
						if(!l_sUpdates.IsEmpty())
						{
							l_sUpdates << l_Sep;
						}
						if(l_sUpdates.Replace(l_sSource + wxT(" + "), l_sSource + wxT(" = ")) == 0)
						{
							l_sUpdates << wxT("[");
							l_sUpdates << l_sSource;
							l_sUpdates << wxT(" = {");
							SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
							l_sUpdates << l_pcColList->GetActiveCellValue(1);
							l_sUpdates << wxT("}]");
						}
					}
					else if(l_pcEdge->GetClassName() == SP_DS_MODIFIER_EDGE)
					{
						if(!l_sConditions.IsEmpty())
						{
							l_sConditions << l_Sep;
						}
						l_sConditions << wxT("[");
						l_sConditions << l_sSource;
						l_sConditions << wxT(" {");
						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						l_sConditions << l_pcColList->GetActiveCellValue(1);
						l_sConditions << wxT("}]");
					}

				}

				// function
				wxString l_sFunction;
				if(l_pcNode->GetAttribute(wxT("FunctionList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("FunctionList")));
					l_sFunction << wxT("\n        : {");
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						if(i>0)
						{
							l_sFunction << wxT(",\n          ");
						}
						l_sFunction << wxT("{") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn())
								<< wxT(" : ") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1)
								<< wxT("}");
					}
					l_sFunction << wxT("}");
				}
				else if(l_pcNode->GetAttribute(wxT("DelayList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("DelayList")));
					l_sFunction << wxT("\n        : {");
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						if(i>0)
						{
							l_sFunction << wxT(",\n          ");
						}
						l_sFunction << wxT("{") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn())
								<< wxT(" : ") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1)
								<< wxT("}");
					}
					l_sFunction << wxT("}");
				}
				else if(l_pcNode->GetAttribute(wxT("PeriodicList")))
				{
					SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("PeriodicList")));
					l_sFunction << wxT("\n        : {");
					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						if(i>0)
						{
							l_sFunction << wxT(",\n          ");
						}
						l_sFunction << wxT("{") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn())
								<< wxT(" : ") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+1)
								<< wxT(",") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+2)
								<< wxT(",") << l_pcColList->GetCell(i,l_pcColList->GetActiveColumn()+3)
								<< wxT("}");
					}
					l_sFunction << wxT("}");
				}

				l_sBuffer << l_sGuard << wxT("\n        : ") << l_sConditions << wxT("\n        : ") << l_sUpdates << l_sFunction << wxT(";\n");
				m_file.Write(l_sBuffer);
			}
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCANDL::WriteConstants()
{
	m_file.Write(wxT("constants:\n"));

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << l_pcColList->GetCell(i,1) << wxT(" ") << l_pcColList->GetCell(i,0) <<  wxT(" = ") << l_pcColList->GetCell(i,2) << wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	SP_ListNodeclass::const_iterator itNC = m_graph->GetNodeclasses()->begin();
	for(; itNC != m_graph->GetNodeclasses()->end(); ++itNC)
	{
		wxString l_sNCName = (*itNC)->GetDisplayName();
		if(l_sNCName.Find(wxT("Parameter")) != wxNOT_FOUND
			&& l_sNCName.Find(wxT("Coarse")) == wxNOT_FOUND)
		{
			SP_ListNode::const_iterator itNode = (*itNC)->GetElements()->begin();
			for(; itNode != (*itNC)->GetElements()->end(); ++itNode)
			{
				SP_DS_Node* l_pcNode = (*itNode);
				wxString l_sBuffer = wxT("    ");
				l_sBuffer << wxT("double ");
				l_sBuffer << dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				l_sBuffer << wxT(" = ");
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
				l_sBuffer << l_pcColList->GetActiveCellValue(1);
				l_sBuffer << wxT(";\n");
				m_file.Write(l_sBuffer);
			}
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportCANDL::WriteColorsets()
{
	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return false;


	m_file.Write(wxT("simplecolorsets:\n"));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << l_pcColList->GetCell(i,1) << wxT(" ") << l_pcColList->GetCell(i,0) <<  wxT(" = ") << l_pcColList->GetCell(i,2) << wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return false;
	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;
	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return false;

	m_file.Write(wxT("compoundcolorsets:\n"));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << l_pcColList->GetCell(i,1) << wxT(" ") << l_pcColList->GetCell(i,0) <<  wxT(" = ") << l_pcColList->GetCell(i,2) << wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));
	return true;
}

bool SP_ExportCANDL::WriteVariables()
{
	m_file.Write(wxT("variables:\n"));

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << l_pcColList->GetCell(i,1) << wxT(" ") << l_pcColList->GetCell(i,0) << wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));
	return true;
}

bool SP_ExportCANDL::WriteFunctions()
{
	m_file.Write(wxT("functions:\n"));

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_DS_Metadata* l_pcNewMetadata;
	SP_DS_ColListAttribute * l_pcColList;

	if(!m_graph)
		return false;

	l_pcMetadataclass = m_graph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return false;

	l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
	if(!l_pcNewMetadata)
		return false;

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return false;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << l_pcColList->GetCell(i,0) << wxT(" ") << l_pcColList->GetCell(i,1)
				<< wxT("( ") << l_pcColList->GetCell(i,2) << wxT(" )\n")
				<< wxT("        { ") << l_pcColList->GetCell(i,3) << wxT(" } ")
				<< wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));
	return true;
}

bool SP_ExportCANDL::WriteObservers()
{
	//m_file.Write(wxT("observers:\n"));

	//TODO

	//m_file.Write(wxT("\n"));
	return true;
}
