/*
 * SP_FunctionRegistry.h
 *
 *  Created on: Apr 23, 2014
 *      Author: crohr
 */

#ifndef SP_DS_FUNCTIONREGISTRY_H_
#define SP_DS_FUNCTIONREGISTRY_H_

#include "sp_core/base/SP_Error.h"
#include "sp_core/SP_Vector.h"
#include "sp_ds/extensions/SP_DS_Function.h"

namespace dsszmc
{
	namespace functions
	{
		class FunctionRegistry;
		class FunctionRegistryEntry;
		class AbstractFunctionBuilder;
		class FunctionParser;
	}
}

class SP_DS_FunctionRegistryEntry
{
private:
	typedef dsszmc::functions::FunctionRegistryEntry FunctionRegistryEntry;
	FunctionRegistryEntry* m_Handle;

public:

	SP_DS_FunctionRegistryEntry(FunctionRegistryEntry* p_pcHandle);
	SP_FunctionPtr getFunction();
	SP_VectorString getParameter();

	bool IsOk()
	{
		return (m_Handle != NULL);
	}
	//void print(std::ostream &os) const;

};

class SP_DS_Graph;

class SP_DS_FunctionRegistry: public SP_Error
{
public:
	SP_DS_FunctionRegistry(SP_DS_Graph* p_pcGraph);
	virtual ~SP_DS_FunctionRegistry();

	void LoadFromNet(SP_DS_Graph* p_pcGraph);

	SP_FunctionPtr parseFunctionString(wxString p_sFunction);

	void registerFunction(wxString p_Signature, wxString p_Definition);

	void removeFunction(const wxString& p_sName);

	void clear();

	SP_DS_FunctionRegistryEntry lookUpFunction(const wxString& p_sName);

	unsigned size() const;

	SP_FunctionPtr substituteFunctions(SP_FunctionPtr p_Function);

	void invalidate();
	bool isValid();

private:
	SP_DS_Graph* m_pcGraph;
	bool m_bIsValid;

	typedef std::shared_ptr<dsszmc::functions::FunctionRegistry> FunctionRegistryPtr;
	FunctionRegistryPtr m_pcRegistry;

	typedef std::shared_ptr<dsszmc::functions::AbstractFunctionBuilder> FunctionBuilderPtr;
	FunctionBuilderPtr m_pcBuilder;

	typedef std::shared_ptr<dsszmc::functions::FunctionParser> FunctionParserPtr;
	FunctionParserPtr m_pcParser;
};

#endif /* SP_DS_FUNCTIONREGISTRY_H_ */
