//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#include "sp_ds/extensions/Col_SPN/SP_DS_ColCSPSovler.h"

using Gecode::INT_VAL_MIN;
//using Gecode::ValBranchOptions;

SP_DS_ColCSPSovler::SP_DS_ColCSPSovler( )
{ }

SP_DS_ColCSPSovler::~SP_DS_ColCSPSovler( )
{ }

//initialize the variable array using the number of variables.
void SP_DS_ColCSPSovler::InitializeVarArrary(unsigned int p_nVarNumber)
{
    m_aVariables = IntVarArray(*this, p_nVarNumber);
}

//initialize each variable
void SP_DS_ColCSPSovler::InitializeVariable(wxString p_sVarName, unsigned int p_nArrayPos, unsigned int p_nLow, unsigned int p_nHigh)
{
	m_aVariables[p_nArrayPos] = IntVar(*this, p_nLow, p_nHigh );
	m_mVarName2Pos[p_sVarName] = p_nArrayPos;
}

//add one constraint
void SP_DS_ColCSPSovler::AddConstraint(BoolVar p_bvExpres)
{
	rel(*this, p_bvExpres);
}

//branch each variable
void SP_DS_ColCSPSovler::SetBranch()
{
	for(int i = 0; i < m_aVariables.size(); i++)
	{
		//branch(*this, m_aVariables[i], INT_VAL_MIN, ValBranchOptions::def);
		branch(*this, m_aVariables[i], INT_VAL_MIN());
	}
}


IntVar SP_DS_ColCSPSovler::GetGecodeIntVar( wxString p_sVarName)
{
	int l_nPos = m_mVarName2Pos.find(p_sVarName)->second;
	return m_aVariables[l_nPos];
}

bool SP_DS_ColCSPSovler::GetGecodeIntVar( wxString p_sVarName, IntVar& p_nIntVar)
{
	if(m_mVarName2Pos.find(p_sVarName) == m_mVarName2Pos.end())
		return false;

	int l_nPos = m_mVarName2Pos.find(p_sVarName)->second;
	p_nIntVar =  m_aVariables[l_nPos];
	return true;
}

IntVarArray* SP_DS_ColCSPSovler::GetArrayVariables()
{
	return &m_aVariables;
}

/// Constructor for cloning \a s
SP_DS_ColCSPSovler::SP_DS_ColCSPSovler(bool p_bShare, SP_DS_ColCSPSovler& p_csSolver) : Space(p_bShare,p_csSolver)
{
    m_aVariables.update(*this, p_bShare, p_csSolver.m_aVariables);
}

/// Copy during cloning
Space* SP_DS_ColCSPSovler::copy(bool p_bShare)
{
    return new SP_DS_ColCSPSovler(p_bShare,*this);
}

/// Print solution
void SP_DS_ColCSPSovler::print() const
{
  //std::cout << "x:" << xy[0] << std::endl << "y:" << xy[1] << std::endl ;
  wxString l_sExport = wxT("");
  for(int i=0; i < m_aVariables.size(); i++)
  {
	  l_sExport = l_sExport + wxT("-");
	  int k = m_aVariables[i].val();
	  l_sExport << k;
  }

  SP_LOGMESSAGE(l_sExport + wxT("\n"));
}
