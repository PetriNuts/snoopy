/*
 * SPNetUnfolder.h
 *
 *  Created on: Mar 20, 2015
 *      Author: crohr
 */

#ifndef SP_CORE_TOOLS_SP_NETUNFOLDER_H_
#define SP_CORE_TOOLS_SP_NETUNFOLDER_H_

#include "sp_core/tools/SP_SimpleNetBuilder.h"

class SP_DS_ColPN_Unfolding;

namespace dssd
{
namespace unfolding
{
	class idd_guard_representation;
	struct gecode_guard_representation;
}
}

template<typename Repr>
class SP_AbstractNetUnfolder
{
public:

	bool operator()(SP_DS_Graph* p_pcGraph, bool evalTokens = false, bool evalArcInscriptions = false,bool p_bShowLog=true);

	SP_DS_Graph* GetGraph() { return m_pcGraph; }
	dssd::andl::Net_ptr GetColoredNet() { return m_ColoredNet; }
	dssd::andl::Net_ptr GetUnfoldedNet() { return m_UnfoldedNet; }

	bool FillInResults(SP_DS_ColPN_Unfolding* p_pcResults);

protected:
	SP_DS_Graph* m_pcGraph;
	dssd::andl::Net_ptr m_ColoredNet;
	dssd::andl::Net_ptr m_UnfoldedNet;
};

using SP_IddNetUnfolder = SP_AbstractNetUnfolder<dssd::unfolding::idd_guard_representation>;
using SP_GecodeNetUnfolder = SP_AbstractNetUnfolder<dssd::unfolding::gecode_guard_representation>;

#endif /* SP_CORE_TOOLS_SP_NETUNFOLDER_H_ */
