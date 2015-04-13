/*
 * SP_DS_CheckNet.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 22.10.2010
 * Short Description:
 */
 //==============================================================================

#ifndef SP_DS_CHECKNET_H_
#define SP_DS_CHECKNET_H_

class SP_DS_Graph;
class SP_DS_CheckTransitions :public SP_Error
{
  private:
	       SP_DS_Graph* m_pcGraph;

  public:

	    SP_DS_CheckTransitions(SP_DS_Graph* p_pcGraph);
        virtual ~SP_DS_CheckTransitions(){};
        virtual bool  CheckRateFunction();
};

#endif /* SP_DS_CHECKNET_H_ */
