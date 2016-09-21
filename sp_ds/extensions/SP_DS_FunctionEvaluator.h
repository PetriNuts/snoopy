/*
 * SP_DS_FunctionEvaluator.h
 *
 *  Created on: May 5, 2014
 *      Author: crohr
 */

#ifndef SP_DS_FUNCTIONEVALUATOR_H_
#define SP_DS_FUNCTIONEVALUATOR_H_

#include "dssz/functions/eval/sim_evaluator.h"
#include "dssz/functions/function_arguments.h"
#include "sp_core/SP_Core.h"
#include "sp_ds/extensions/SP_DS_FunctionRegistry.h"
#include "sp_ds/SP_DS_Node.h"
#include "sp_ds/SP_DS_Edge.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingAttribute.h"
#include "sp_ds/attributes/SP_DS_DoubleMarkingAttribute.h"

template<typename T>
class SP_DS_FunctionEvaluator
{
	using ValueT = T;
	using ArgumentT = std::vector<T>;
	using FunctionT = dsszmc::functions::SimulatorFunction;
	using EvalT = dsszmc::functions::GenericEvaluator<ValueT, ArgumentT, FunctionT>;
	public:
	SP_DS_FunctionEvaluator(SP_DS_FunctionRegistry* p_FunctionRegistry, SP_FunctionPtr p_Function, ValueT p_Default = 0):
		m_FunctionRegistry(p_FunctionRegistry), m_Function(p_Function), m_Default(p_Default)
	{
	}

	T operator()()
	{
		try
		{
			SP_FunctionPtr f = m_FunctionRegistry->substituteFunctions(m_Function);
			return EvalT{static_cast<FunctionT&>(*f)}();
		}
		catch(std::exception& e)
		{
			SP_LOGERROR(wxString(e.what(), wxConvUTF8));
			SP_LOGERROR(wxString(m_Function->toString()));
		}
		return m_Default;
	}

	T operator()(SP_DS_Node* p_pcTrans)
	{
	    if(p_pcTrans)
    	{
			try
			{
				//wxString l_sFunction(m_Function->toString().c_str(), wxConvUTF8);
				//SP_LOGMESSAGE( l_sFunction);
				SP_FunctionPtr f = m_FunctionRegistry->substituteFunctions(m_Function);
				ArgumentT tmp;

				dsszmc::functions::Name2Id l_Places;
				const SP_ListEdge* l_plEdges = p_pcTrans->GetTargetEdges();
				SP_ListEdge::const_iterator it = l_plEdges->begin();
				SP_ListEdge::const_iterator end = l_plEdges->end();
				unsigned int i = 0;
				for(; it != end; ++it)
				{
					SP_DS_Edge* l_pcEdge = *it;
					SP_DS_Node* l_pcPlace = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetSource());
					wxString l_sPlaceName = dynamic_cast<SP_DS_NameAttribute*>(l_pcPlace->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
					l_Places.insert(make_pair(std::string(l_sPlaceName.mb_str()), i));
					ValueT l_nToken = 0;
					SP_DS_Attribute* l_pcAttr = l_pcPlace->GetAttribute(wxT("Marking"));
					if(l_pcAttr)
					{
						bool l_bAnim = !(SP_Core::Instance()->GetAnimMode());

						if(l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)
						{
							l_nToken = static_cast<SP_DS_MarkingAttribute*>(l_pcAttr)->GetValue(l_bAnim);
						}
						else if(l_pcAttr->GetAttributeType() == SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING)
						{
							l_nToken = static_cast<SP_DS_DoubleMarkingAttribute*>(l_pcAttr)->GetValue(l_bAnim);
						}
					}
					tmp.push_back(l_nToken);

					++i;
				}

				//TODO: check this option
				dsszmc::functions::convertOptions co;
				f->setVariableIds(l_Places, co);
				//wxString l_sF(f->toString().c_str(), wxConvUTF8);
				//SP_LOGMESSAGE( l_sF);
				return EvalT{static_cast<FunctionT&>(*f)}(tmp);
			}
			catch(std::exception& e)
			{
				SP_LOGERROR(wxString(e.what(), wxConvUTF8));
				SP_LOGERROR(wxString(m_Function->toString()));
			}
			return m_Default;
    	}
	    else
	    {
	    	return this->operator ()();
	    }
	}

	private:
	SP_DS_FunctionRegistry* m_FunctionRegistry;
	SP_FunctionPtr m_Function;
	ValueT m_Default;
};

typedef SP_DS_FunctionEvaluator<long> SP_DS_FunctionEvaluatorLong;
typedef SP_DS_FunctionEvaluator<double> SP_DS_FunctionEvaluatorDouble;

#endif /* SP_DS_FUNCTIONEVALUATOR_H_ */
