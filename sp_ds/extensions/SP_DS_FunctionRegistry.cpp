/*
 * SP_DS_FunctionRegistry.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: crohr
 */

#include "SP_DS_FunctionRegistry.h"
#include "sp_utilities.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "dssd/functions/functionRegistry.h"
#include "dssd/functions/builder/simFunctionBuilder.h"
#include "dssd/functions/parser/bison_parser.h"

using dssd::functions::FunctionRegistry;
using dssd::functions::SIMFunctionBuilder;
using dssd::functions::BisonFunctionParser;

SP_DS_FunctionRegistryEntry::SP_DS_FunctionRegistryEntry(FunctionRegistryEntry *p_pcHandle):
	m_Handle(p_pcHandle)
{
}
/*
void
SP_DS_FunctionRegistryEntry::print(std::ostream & os) const
{

}
*/
SP_VectorString
SP_DS_FunctionRegistryEntry::getParameter()
{
	SP_VectorString ret;
	if(IsOk())
	{
		for(unsigned i = 0; i < m_Handle->param().size(); ++i)
		{
			ret.push_back(wxString(m_Handle->param()[i].c_str(), wxConvUTF8));
		}
	}
	return ret;
}

SP_FunctionPtr
SP_DS_FunctionRegistryEntry::getFunction()
{
	if(IsOk())
	{
		return SP_FunctionPtr(m_Handle->function().copy());
	}
	else
	{
		return SP_FunctionPtr(NULL);
	}
}

SP_DS_FunctionRegistry::SP_DS_FunctionRegistry(SP_DS_Graph* p_pcGraph):
	m_pcGraph(p_pcGraph),
	m_bIsValid(true),
	m_pcRegistry(FunctionRegistryPtr(new FunctionRegistry())),
	m_pcBuilder(FunctionBuilderPtr(new SIMFunctionBuilder(*m_pcRegistry))),
	m_pcParser(FunctionParserPtr(new BisonFunctionParser(*m_pcBuilder)))
{
	//forbid immediate substitution
	m_pcBuilder->setSubstitutionMode(false);
}

SP_DS_FunctionRegistry::~SP_DS_FunctionRegistry()
{
}

void
SP_DS_FunctionRegistry::LoadFromNet(SP_DS_Graph* p_pcGraph)
{
	if(p_pcGraph->GetNetclass()->GetName().Contains(wxT("Colored")))
	{
		//TODO: not for colored nets
		return;
	}
	SP_DS_Metadataclass* l_pcMC = NULL;
	SP_ListMetadata::const_iterator it,end;

	//load functions
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if(l_pcMC)
	{
		for(SP_DS_Metadata* l_pcMeta : *(l_pcMC->GetElements()))
		{
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sParam = l_pcMeta->GetAttribute(wxT("Parameter"))->GetValueString();
			wxString l_sBody = l_pcMeta->GetAttribute(wxT("Body"))->GetValueString();

			registerFunction(l_sName + wxT("(") + l_sParam + wxT(")"), l_sBody);
		}
	}
	//load constants
	l_pcMC = p_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(l_pcMC)
	{
		for(SP_DS_Metadata* l_pcMeta : *(l_pcMC->GetElements()))
		{
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sValue = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMeta->GetAttribute(wxT("ValueList")))->GetActiveCellValue(1);
			wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMeta->GetAttribute(wxT("Type")))->GetValue();
			if (l_sMetadataType != wxT("TFN"))//Added by G.Assaf
			{
				registerFunction(l_sName, l_sValue);
			}
		}
	}
	m_bIsValid = true;
}

SP_FunctionPtr
SP_DS_FunctionRegistry::parseFunctionString(wxString p_sFunction)
{
	p_sFunction.Trim();
	p_sFunction.Trim(false);
	if(!p_sFunction.IsEmpty())
	{
		try
		{
			m_pcParser->parse(p_sFunction.utf8_str().data());
			SP_FunctionPtr l_pcFunction(m_pcParser->getFunction());
			return l_pcFunction;
		}
		catch(std::exception& e)
		{
			SP_LOGERROR( wxString(e.what(), wxConvUTF8));
		}
		catch(...)
		{
			SP_LOGERROR(wxT("unknown exception, parsing function: ") + p_sFunction );
		}
	}
	return SP_FunctionPtr(NULL);
}

void
SP_DS_FunctionRegistry::registerFunction(wxString p_Signature, wxString p_Definition)
{
    auto l_Signature = parseFunctionString(p_Signature);
	if(!l_Signature)
	{
		SP_LOGERROR(wxT("register function: invalid signature"));
		return;
	}
    auto l_Definition = parseFunctionString(p_Definition);
	if(!l_Definition)
	{
		SP_LOGERROR(wxT("register function: invalid definition"));
		return;
	}
	try
	{
		m_pcBuilder->registerFunctionTemplate(l_Signature->copy(), l_Definition->copy());
	}
	catch(std::exception& e)
	{
		SP_LOGERROR( wxString(e.what(), wxConvUTF8));
	}
	catch(...)
	{
		SP_LOGERROR(wxT("unknown exception, registering function"));
	}
}

void
SP_DS_FunctionRegistry::removeFunction(const wxString& p_sName)
{
	if(!p_sName.IsEmpty())
		m_pcRegistry->removeFunction(p_sName.utf8_str().data());
}

SP_DS_FunctionRegistryEntry
SP_DS_FunctionRegistry::lookUpFunction(const wxString& p_sName)
{
	if(!isValid())
	{
		clear();
		LoadFromNet(m_pcGraph);
	}
	return SP_DS_FunctionRegistryEntry(m_pcRegistry->lookUpFunction(p_sName.utf8_str().data()));
}

void
SP_DS_FunctionRegistry::clear()
{
	m_pcRegistry->clear();
}

unsigned
SP_DS_FunctionRegistry::size() const
{
	return m_pcRegistry->size();
}

SP_FunctionPtr
SP_DS_FunctionRegistry::substituteFunctions(SP_FunctionPtr p_Function)
{
	if(!p_Function)
	{
		SP_LOGERROR(wxT("substitute function: invalid function"));
		return SP_FunctionPtr(NULL);
	}
	SP_FunctionPtr l_pcRet(p_Function->copy());
	if(!isValid())
	{
		clear();
		LoadFromNet(m_pcGraph);
	}
	try
	{
		l_pcRet->substituteFunctions(*m_pcRegistry);
		return l_pcRet;
	}
	catch(std::exception& e)
	{
		SP_LOGERROR( wxString(e.what(), wxConvUTF8));
	}
	catch(...)
	{
		SP_LOGERROR(wxT("unknown exception, substitute function"));
	}
	return p_Function;
}

void
SP_DS_FunctionRegistry::invalidate()
{
	m_bIsValid = false;
}

bool
SP_DS_FunctionRegistry::isValid()
{
	return m_bIsValid;
}
