/*
 * SP_SimpleNetBuilder.h
 *
 *  Created on: Mar 18, 2015
 *      Author: crohr
 */

#ifndef SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_
#define SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_

#include "dssd/misc/net.h"
#include "sp_ds/extensions/Col_PN/SyntaxChecking/SP_CPN_SyntaxChecking.h"
class SP_DS_Graph;

namespace dssd
{
namespace andl
{
	class simple_net_builder;
}
}

class SP_AbstractNetBuilder
{
public:
	virtual ~SP_AbstractNetBuilder() = default;

	virtual bool operator()(SP_DS_Graph* p_pcGraph) = 0;

	dssd::andl::Net_ptr GetNet() { return m_Net; }
	SP_DS_Graph* GetGraph() { return m_pcGraph; }

protected:

	SP_DS_Graph* m_pcGraph;
	dssd::andl::Net_ptr m_Net;
};

class SP_SimpleNetBuilder final : public SP_AbstractNetBuilder
{
public:
	virtual ~SP_SimpleNetBuilder() = default;

	bool operator()(SP_DS_Graph* p_pcGraph);

private:
	bool CreatePlaces(dssd::andl::simple_net_builder& b);
	bool CreateTransitions(dssd::andl::simple_net_builder& b);
	bool CreateConstants(dssd::andl::simple_net_builder& b);
	bool CreateFunctions(dssd::andl::simple_net_builder& b);
	bool CreateObservers(dssd::andl::simple_net_builder& b);

};

class SP_ColoredNetBuilder final : public SP_AbstractNetBuilder
{
public:
	SP_ColoredNetBuilder(bool p_bIsExport = false) { m_bIsExport = p_bIsExport; }
	virtual ~SP_ColoredNetBuilder() = default;

	bool operator()(SP_DS_Graph* p_pcGraph);

private:
	  bool CreateVariables(dssd::andl::simple_net_builder& b);
	  bool CreateColorsets(dssd::andl::simple_net_builder& b);
	  bool CreateColorFunctions(dssd::andl::simple_net_builder& b);
	  bool CreatePlaces(dssd::andl::simple_net_builder& b);
	  bool CreateTransitions(dssd::andl::simple_net_builder& b);
	  bool CreateConstants(dssd::andl::simple_net_builder& b);
	  bool CreateFunctions(dssd::andl::simple_net_builder& b);
	  bool CreateObservers(dssd::andl::simple_net_builder& b);
	  void PrePareMarkingString(wxString& p_sMarkingExpression,wxString& p_sOutput);
private:
	bool m_bIsExport;

};

#endif /* SP_CORE_TOOLS_SP_SIMPLENETBUILDER_H_ */
