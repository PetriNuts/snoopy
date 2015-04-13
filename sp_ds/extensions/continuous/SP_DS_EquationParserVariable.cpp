//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/06/15 15:35:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/continuous/SP_DS_EquationParserVariable.h"
#include "sp_ds/attributes/SP_DS_DoubleAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleListAttribute.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_IdAttribute.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "snoopy.h"
#include "sp_ds/netclasses/SP_DS_ContinuousPed.h"




SP_DS_EquationParserVariable::SP_DS_EquationParserVariable(const wxString& p_sName,
                                                           long p_nId):
SP_DS_EquationParserObject(SP_DS_EQUATIONPARSEROBJECT_VARIABLE),
m_sName(p_sName),
m_nId(p_nId),
m_nValue(0.0),
m_bConnected(FALSE),
m_pcNode(NULL),
m_pcEquationParser(NULL)
{
}

SP_DS_EquationParserVariable::~SP_DS_EquationParserVariable()
{
}

SP_DS_EquationParserVariable*
SP_DS_EquationParserVariable::Clone(const wxString& p_sName,
                                    long p_nId)
{
  return new SP_DS_EquationParserVariable(p_sName, p_nId);
}

long
SP_DS_EquationParserVariable::GetID()
{
  return m_nId;
}

const wxString
SP_DS_EquationParserVariable::GetName()
{
  return m_sName;
}

bool
SP_DS_EquationParserVariable::GetConnected()
{
  return m_bConnected;
}
void
SP_DS_EquationParserVariable::SetConnected(bool p_bConnected)
{
  m_bConnected = p_bConnected;
}

bool
SP_DS_EquationParserVariable::GetCalcState()
{
  return m_bCalcState;
}
void
SP_DS_EquationParserVariable::SetCalcState(bool p_bCalcState)
{
  m_bCalcState = p_bCalcState;
}

long double
SP_DS_EquationParserVariable::GetValue()
{
  if (m_pcNode)
  {
    //For Places
	SP_DS_Attribute* l_pcAttr = m_pcNode->GetAttribute(wxT("MarkingList"));
    SP_DS_ColListAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);
         if (l_pcMarkAttr)
         {
           m_nValue = l_pcMarkAttr->GetActiveCellValueDouble(1);
         }
         //for Parameters
         else
         {
        	 SP_DS_Attribute* l_pcAttr = m_pcNode->GetAttribute(wxT("ParameterList"));
        	 SP_DS_ColListAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);
        	 if (l_pcMarkAttr)
        	  {
        	    m_nValue = l_pcMarkAttr->GetActiveCellValueDouble(1);
        	  }
         }
  }
  return m_nValue;
}
void
SP_DS_EquationParserVariable::SetValue(double p_nValue)
{
  m_nValue = p_nValue;
}

SP_DS_EquationParser*
SP_DS_EquationParserVariable::GetEquationParser()
{
  return m_pcEquationParser;
}

void
SP_DS_EquationParserVariable::SetEquationParser(SP_DS_EquationParser* p_pcEquationParser)
{
  if (p_pcEquationParser)
  {
    m_pcEquationParser = p_pcEquationParser;
  }
}

SP_DS_Node*
SP_DS_EquationParserVariable::GetNode()
{
  return m_pcNode;
}

void
SP_DS_EquationParserVariable::SetNode(SP_DS_Node* p_pcNode)
{
  CHECK_POINTER(p_pcNode, return);
  m_pcNode = p_pcNode;
  SP_DS_Attribute* l_pcAttr = m_pcNode->GetAttribute(wxT("MarkingList"));
  SP_DS_ColListAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_ColListAttribute*>(l_pcAttr);
  CHECK_POINTER(l_pcMarkAttr, return);
  m_nValue=l_pcMarkAttr->GetActiveCellValueDouble(1);
  /*if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLE == l_pcAttr->GetAttributeType())
  {
    SP_DS_DoubleAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_DoubleAttribute*>(l_pcAttr);
    CHECK_POINTER(l_pcMarkAttr, return);
    m_nValue = l_pcMarkAttr->GetValue();
  } else if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_LIST == l_pcAttr->GetAttributeType())
  {
    SP_DS_ListAttribute* l_pcListAttr = dynamic_cast<SP_DS_ListAttribute*>(l_pcAttr);
    if (SP_ATTRIBUTELIST_DOUBLE == l_pcListAttr->GetAttributeListType())
    {
      SP_DS_DoubleListAttribute* l_pcMarkAttr = dynamic_cast<SP_DS_DoubleListAttribute*>(l_pcListAttr);
      CHECK_POINTER(l_pcMarkAttr, return);
      m_nValue = l_pcMarkAttr->GetValueDouble();
    }
  }*/
  if (SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID == m_pcNode->GetAttribute(wxT("ID"))->GetAttributeType())
  {
    SP_DS_IdAttribute* l_pcIdAttr = dynamic_cast<SP_DS_IdAttribute*>(m_pcNode->GetAttribute(wxT("ID")));
    CHECK_POINTER(l_pcIdAttr, return);
    m_nId = l_pcIdAttr->GetValue();
  }
}

SP_DS_EQUATIONPARSERVARIABLE_TYPE
SP_DS_EquationParserVariable::GetVarType()
{
  return m_VarType;
}
void
SP_DS_EquationParserVariable::SetVarType(SP_DS_EQUATIONPARSERVARIABLE_TYPE p_VarType)
{
  if (m_VarType != p_VarType)
  {
    m_VarType = p_VarType;
  }
}


const wxString
SP_DS_EquationParserVariable::ToString()
{
  return m_sName;
}

wxString
SP_DS_EquationParserVariable::FormatTexString(const wxString& p_sName, bool p_bConvSpecialChar)
{
	wxString l_sName = p_sName;
	wxString l_sQuoteUnderscore=wxT("");
	if (p_bConvSpecialChar){
		l_sQuoteUnderscore=wxT("\\");

		l_sName.Replace(wxT("$"), wxT("\\$ "));
		l_sName.Replace(wxT("&"), wxT("\\& "));
		l_sName.Replace(wxT("#"), wxT("\\# "));

	}

	 int l_nReplaced = l_sName.Replace(wxT("_"),
		 wxString::Format(wxT("%s_\\mathrm{"),l_sQuoteUnderscore.c_str()));

	  for (int  i = 0; i < l_nReplaced; i++)
	  {
		l_sName.Append(wxT("}"));
	  }







  /* Here all wxT("_" were quoted, only when a number comes after "_")
   then wxT("_") is not quoted

  int l_nUndod=0;
  int i;
  for ( i =0; i<10; i++)
  {
	  l_nUndod += l_sName.Replace(
		  wxString::Format(wxT("\\_\\mathrm{%d"),i).c_str(),
		  wxString::Format(wxT("_%d"),i).c_str());
  }
  for ( i = 0; i < l_nReplaced - l_nUndod; i++)
  {
    l_sName.Append(wxT("}"));
  }

  */
  return l_sName;
}

wxString
SP_DS_EquationParserVariable::ConvertGreek(const wxString& p_sName, bool p_bConvSpecialChar)
{
  wxString result = FormatTexString(p_sName, p_bConvSpecialChar);
  result.Replace(wxT("alpha"), wxT("\\alpha "));
  result.Replace(wxT("gamma"), wxT("\\gamma "));
  result.Replace(wxT("delta"), wxT("\\delta "));
  result.Replace(wxT("epsilon"), wxT("\\epsilon "));
  result.Replace(wxT("var\\epsilon "), wxT("\\varepsilon "));
  result.Replace(wxT("eta"), wxT("\\eta "));
  result.Replace(wxT("b\\eta "), wxT("\\beta "));
  result.Replace(wxT("z\\eta "), wxT("\\zeta "));
  result.Replace(wxT("th\\eta "), wxT("\\theta "));
  result.Replace(wxT("var\\theta "), wxT("\\vartheta "));
  result.Replace(wxT("iota"), wxT("\\iota "));
  result.Replace(wxT("kappa"), wxT("\\kappa "));
  result.Replace(wxT("lambda"), wxT("\\lambda "));
  result.Replace(wxT("mu"), wxT("\\mu "));
  result.Replace(wxT("nu"), wxT("\\nu "));
  result.Replace(wxT("xi"), wxT("\\xi "));
  result.Replace(wxT("pi"), wxT("\\pi "));
  result.Replace(wxT("var\\pi "), wxT("\\varpi "));
  result.Replace(wxT("rho"), wxT("\\rho "));
  result.Replace(wxT("var\\rho "), wxT("\\varrho "));
  result.Replace(wxT("sigma"), wxT("\\sigma "));
  result.Replace(wxT("var\\sigma "), wxT("\\varsigma "));
  result.Replace(wxT("tau"), wxT("\\tau "));
  result.Replace(wxT("upsilon"), wxT("\\upsilon "));
  result.Replace(wxT("phi"), wxT("\\phi "));
  result.Replace(wxT("var\\phi "), wxT("\\varphi "));
  result.Replace(wxT("chi"), wxT("\\chi "));
  result.Replace(wxT("psi"), wxT("\\psi "));
  result.Replace(wxT("omega"), wxT("\\omega "));

  result.Replace(wxT("Alpha"), wxT("\\Alpha "));
  result.Replace(wxT("Gamma"), wxT("\\Gamma "));
  result.Replace(wxT("Delta"), wxT("\\Delta "));
  result.Replace(wxT("Epsilon"), wxT("\\Epsilon "));
  result.Replace(wxT("Var\\epsilon"), wxT("\\Varepsilon "));
  result.Replace(wxT("Eta"), wxT("\\Eta "));
  result.Replace(wxT("B\\eta "), wxT("\\Beta "));
  result.Replace(wxT("Z\\eta "), wxT("\\Zeta "));
  result.Replace(wxT("Th\\eta "), wxT("\\Theta "));
  result.Replace(wxT("var\\theta "), wxT("\\Vartheta "));
  result.Replace(wxT("Iota"), wxT("\\Iota "));
  result.Replace(wxT("Kappa"), wxT("\\Kappa "));
  result.Replace(wxT("Lambda"), wxT("\\Lambda "));
  result.Replace(wxT("Mu"), wxT("\\Mu "));
  result.Replace(wxT("Nu"), wxT("\\Nu "));
  result.Replace(wxT("Xi"), wxT("\\Xi "));
  result.Replace(wxT("Pi"), wxT("\\Pi "));
  result.Replace(wxT("Var\\pi "), wxT("\\Varpi "));
  result.Replace(wxT("Rho"), wxT("\\Rho "));
  result.Replace(wxT("Var\\rho "), wxT("\\Varrho "));
  result.Replace(wxT("Sigma"), wxT("\\Sigma "));
  result.Replace(wxT("Var\\sigma "), wxT("\\Varsigma "));
  result.Replace(wxT("Tau"), wxT("\\Tau "));
  result.Replace(wxT("Upsilon"), wxT("\\Upsilon "));
  result.Replace(wxT("Phi"), wxT("\\Phi "));
  result.Replace(wxT("Var\\phi "), wxT("\\Varphi "));
  result.Replace(wxT("Chi"), wxT("\\Chi "));
  result.Replace(wxT("Psi"), wxT("\\Psi "));
  result.Replace(wxT("Omega"), wxT("\\Omega "));

  result.Replace(wxT(" \\"), wxT("\\"));
  return result;
}

const wxString
SP_DS_EquationParserVariable::ToTexString(bool p_bGreek,
                                          bool p_bParentheses,
										  bool p_bConvSpecialChar)
{
  wxString result = wxT("");

  SP_DS_Nodeclass* l_pcContPlaceNodeClass = SP_Core::Instance()->GetRootDocument()->GetGraph()->GetNodeclass(SP_DS_CONTINUOUS_PLACE);

  const SP_ListNode* l_pcNodeList = l_pcContPlaceNodeClass->GetElements();
  SP_ListNode::const_iterator l_Iter;
  bool l_bisPlace= false;
  for (l_Iter = l_pcNodeList->begin(); l_Iter != l_pcNodeList->end(); ++l_Iter){
	  wxString tt = (*l_Iter)->GetAttribute(wxT("Name"))->GetValueString();
	  if (m_sName.IsSameAs(tt)){
		 l_bisPlace=true;
		}
  }



  if (p_bParentheses &&  l_bisPlace)
  {

    result.Append(wxT("\\left["));
  }
  result.Append(wxT("\\mathrm{"));
  if (p_bGreek)
  {
    result.Append(ConvertGreek(m_sName,  p_bConvSpecialChar));
  } else
  {
    result.Append(FormatTexString(m_sName, p_bConvSpecialChar));
  }
  result.Append(wxT("}"));
  if (p_bParentheses && l_bisPlace )
  {
    result.Append(wxT("\\right]"));
  }
  return result;
}

long double
SP_DS_EquationParserVariable::Calculate()
{
  return GetValue();
}

long double
SP_DS_EquationParserVariable::Calculate(map<wxString, SP_DS_EquationParserObject*>& p_pcVariable)
{
  long double result = 0;
  SP_DS_EquationParserVariable* l_pcVar = dynamic_cast<SP_DS_EquationParserVariable*>(p_pcVariable[m_sName]);

  if (l_pcVar)
  {
    result = l_pcVar->GetValue();
  }
  else
  {
	result = GetValue();
  }
  return result;
}

