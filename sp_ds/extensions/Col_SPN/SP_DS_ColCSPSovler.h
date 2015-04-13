//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////


#ifndef __SP_DS_ColCSPSovler__
#define __SP_DS_ColCSPSovler__

#include <stdlib.h> 
#include "sp_core/SP_Core.h"

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using Gecode::Space;
using Gecode::BoolVar;
using Gecode::IntVar;
using Gecode::IntVarArray;

class SP_DS_ColCSPSovler: public Space 
{
private:

  IntVarArray m_aVariables;   

  std::map<wxString, int> m_mVarName2Pos;

public:
  SP_DS_ColCSPSovler( );
  ~SP_DS_ColCSPSovler( );


public:
	//initialize the variable array using the number of variables.
	void InitializeVarArrary(unsigned int p_nVarNumber);
	//initialize each variable
	void InitializeVariable(wxString p_sVarName, unsigned int p_nArrayPos, unsigned int p_nLow, unsigned int p_nHigh);

	//add one constraint
	void AddConstraint(BoolVar p_bvExpres);

	//branch each variable
	void SetBranch();

	IntVar GetGecodeIntVar( wxString p_sVarName);

	bool GetGecodeIntVar( wxString p_sVarName, IntVar& p_nIntVar);

	IntVarArray* GetArrayVariables();

  /// Constructor for cloning \a s
  SP_DS_ColCSPSovler(bool p_bShare, SP_DS_ColCSPSovler& p_csSolver);
  
  /// Copy during cloning
  virtual Space*  copy(bool p_bShare);

  /// Print solution
  virtual void  print() const;

};



/*
class SP_DS_ColCSPSovler: public Space 
{
private:

  IntVarArray xy;   

public:
  /// Post constraints
  SP_DS_ColCSPSovler( )
  {
   // rel(*this, y != x);
	//rel(*this, y, IRT_LE, 3);
	
	 xy = IntVarArray(*this,2);
	 xy[0] = IntVar(*this, 1, 3);
	 xy[1] = IntVar(*this, 1, 4);

	std::vector<int> l_vInt;
	l_vInt.push_back(1);
	l_vInt.push_back(2);

	for(unsigned i=0; i <l_vInt.size(); i++)
	{
		IntVar Temp0 = expr(*this,1+l_vInt[i]);
		if(i==0)
			rel(*this, Temp0 == xy[0]);
		else
		rel(*this, Temp0 == xy[1]);
	}
	

	branch(*this, xy[0], INT_VAL_MIN, ValBranchOptions::def);
	branch(*this, xy[1], INT_VAL_MIN, ValBranchOptions::def);
	


  }

  /// Constructor for cloning \a s
  SP_DS_ColCSPSovler(bool share, SP_DS_ColCSPSovler& s) : Space(share,s)
  {
		xy.update(*this, share, s.xy);
		
		
  }

  //BoolExpr nBoolExpr(

  /// Copy during cloning
  virtual Space*
  copy(bool share)
  {
	 return new SP_DS_ColCSPSovler(share,*this);
  }
  /// Print solution
  virtual void  print() const 
  {
	  std::cout << "x:" << xy[0] << std::endl << "y:" << xy[1] << std::endl ;
  }
};
*/



#endif // __SP_DS_ColCSPSovler__

