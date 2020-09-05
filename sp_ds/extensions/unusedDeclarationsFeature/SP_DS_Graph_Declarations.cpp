//////////////////////////////////////////////////////////////////////
// $Author: george assaf $
// $Version: 0.1 $
// $Date: 2020/06/14 $
//description: compuation of graph/ANDL/CANDL declarations dependency, and unused declaration of an input graph
////////////////////////////////////////////////////////////////////
#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_Graph_Declarations.h"
SP_DS_Graph_Declarations::SP_DS_Graph_Declarations(SP_DS_Graph* graph,bool p_bisAndl) :m_pcGraph1(graph),m_bIsANDL(p_bisAndl)
{
	m_mColorSet2Definition = {};
	m_mVar2ColorSet = {};
	m_Function2Definition = {};
	m_mObserver2Body = {};
}

SP_DS_Graph_Declarations::SP_DS_Graph_Declarations(dssd::andl::Net_ptr p_pcNet, bool p_bIsANDL) :m_pcANDLFile(p_pcNet), m_bIsANDL(p_bIsANDL)
{
	m_mColorSet2Definition = {};
	m_mVar2ColorSet = {};
	m_Function2Definition = {};
	m_mObserver2Body = {};
}
void SP_DS_Graph_Declarations::operator()()
{
	if (!m_pcGraph1 && !m_bIsANDL) return ;


	if (!m_bIsANDL && m_pcGraph1->GetNetclass()->GetName().Contains("Colored"))
	{
		//collect color sets
		SP_DS_Metadataclass* l_pcMetadataclass1 = NULL;
		SP_DS_Metadata* l_pcNewMetadata1 = NULL;

		l_pcMetadataclass1 = m_pcGraph1->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);

		l_pcNewMetadata1 = l_pcMetadataclass1->GetElements()->front();

		SP_DS_ColListAttribute * l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata1->GetAttribute(wxT("ColorsetList")));

	 
		for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList1->GetCell(i, 0).c_str();
			wxString l_sColors = l_pcColList1->GetCell(i, 2).c_str();
			m_mColorSet2Definition[l_sName] = l_sColors;
		}


		l_pcMetadataclass1 = m_pcGraph1->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
		l_pcNewMetadata1 = l_pcMetadataclass1->GetElements()->front();
		l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata1->GetAttribute(wxT("StructuredColorsetList")));
		for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList1->GetCell(i, 0).c_str();
			wxString l_sColors = l_pcColList1->GetCell(i, 2).c_str();
			wxString l_sType = l_pcColList1->GetCell(i, 1).c_str();
			 
			if (l_sType != wxT("product") && l_sType != wxT("union"))
			{
				wxString l_SColors;
				l_SColors << l_sType << wxT("[") << l_sColors << wxT("]");
				m_mColorSet2Definition[l_sName] = l_SColors;
				continue;
			}
			m_mColorSet2Definition[l_sName] = l_sColors;
	    }


		l_pcMetadataclass1 = m_pcGraph1->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);


		l_pcNewMetadata1 = l_pcMetadataclass1->GetElements()->front();

		l_pcColList1 = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata1->GetAttribute(wxT("AliasColorsetList")));


		for (unsigned int i = 0; i < l_pcColList1->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList1->GetCell(i, 0).c_str();
			wxString l_sColors = l_pcColList1->GetCell(i, 1).c_str();
			m_mColorSet2Definition[l_sName] = l_sColors;

		}
		//collect variables
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

		SP_DS_Metadata* l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

		SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));


		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
			wxString l_sColorset = l_pcColList->GetCell(i, 1).c_str();
			m_mVar2ColorSet[l_sName] = l_sColorset;
		}
		//collect color functions
		l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);


		l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

		 l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));


		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			set<wxString> l_setCS;
			wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
			wxString l_sBody= l_pcColList->GetCell(i, 3).c_str();
			wxString l_sParam = l_pcColList->GetCell(i, 2).c_str();
			FuncStruct fun;
			fun.body = l_sBody;
			set<wxString> l_sParamTypes;

			wxString l_sparams;
			wxString l_str = l_sParam;
			while (l_sParam.Contains(","))
			{
				wxString l_scopy = l_sParam;
				l_sparams = l_scopy.AfterFirst(',');
				wxString par = l_str.BeforeFirst(',');
				l_str = l_sparams;
				l_sParam = l_sparams;
				l_sParamTypes.insert(par);
			}
			if (!l_sParam.IsEmpty() && !l_sParam.Contains(','))
			{
				l_sParamTypes.insert(l_sParam);
			}
		 
			fun.paramTypes = l_sParamTypes;

			m_Function2Definition[l_sName] = fun;
		}

		//collect constants
		wxString l_sTempClassName = m_pcGraph1->GetNetclass()->GetName();
		wxString l_sClass;
		if (l_sTempClassName.Contains("Fuzzy"))
		{
			l_sClass = SP_DS_META_CONSTANT;
		}
		else
		{
			l_sClass = SP_DS_CPN_CONSTANT_HARMONIZING;
		}
		SP_DS_Metadataclass* mc = m_pcGraph1->GetMetadataclass(l_sClass);
		SP_ListMetadata::const_iterator it;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcMetadata = *it;
			
			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));
			vector<wxString> l_vVluesets;
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				wxString value=l_pcColList->GetCell(i, 1);
				l_vVluesets.push_back(value);
			}

			m_mConstant2Values[l_sName] = l_vVluesets;
		}

		//collect observers
		SP_DS_Metadataclass* l_pcMetadataclassObservers = m_pcGraph1->GetMetadataclass(SP_DS_META_OBSERVER);
		if (l_pcMetadataclassObservers)
		  {
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = l_pcMetadataclassObservers->GetElements()->begin();
				l_itElem != l_pcMetadataclassObservers->GetElements()->end(); ++l_itElem)
				{
				  SP_DS_Metadata* l_pcMetadata = *l_itElem;
				  wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
				  wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
                  m_mObserver2Body[l_sName] = l_sBody;
				}
			}
	}
	else if(!m_bIsANDL && m_pcGraph1)
	{//plain PN
		//collect constants
		SP_DS_Metadataclass* mc = m_pcGraph1->GetMetadataclass(SP_DS_META_CONSTANT);
		SP_ListMetadata::const_iterator it;

		for (it = mc->GetElements()->begin(); it != mc->GetElements()->end(); ++it)
		{
			SP_DS_Metadata* l_pcMetadata = *it;

			wxString l_sName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));
			vector<wxString> l_vVluesets;
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				wxString value = l_pcColList->GetCell(i, 1);
				l_vVluesets.push_back(value);
			}

			m_mConstant2Values[l_sName] = l_vVluesets;
		}
		//collect functions
		for (SP_DS_Metadataclass* mc : *(m_pcGraph1->GetMetadataclasses()))
		{
			if (mc->GetShowInDeclarationTreeOther()
				|| mc->GetShowInDeclarationTreeColorSet())
			{
				if (mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && mc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{
					for (SP_DS_Metadata* meta : *(mc->GetElements()))
					{
						wxString l_sMetaclass = mc->GetDisplayName();
						wxString l_sName = meta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						/**********/
						if (l_sMetaclass == wxT("Functions"))
						{
							wxString l_sName = meta->GetAttribute(wxT("Name"))->GetValueString();
							wxString l_sParamList = meta->GetAttribute(wxT("Parameter"))->GetValueString();
							wxString l_sBody = meta->GetAttribute(wxT("Body"))->GetValueString();
							FuncStruct fun;
							fun.body = l_sBody;
							std::set<wxString> l_setParam;
							wxString l_sparams;
							wxString l_str = l_sParamList;
							while (l_sParamList.Contains(","))
							{
								wxString l_scopy = l_sParamList;
								l_sparams = l_scopy.AfterFirst(',');
								wxString par = l_str.BeforeFirst(',');
								l_str = l_sparams;
								l_sParamList = l_sparams;
								l_setParam.insert(par);
							 }
							if (!l_sParamList.IsEmpty()&& !l_sParamList.Contains(','))
							{
								l_setParam.insert(l_sParamList);
							}
							fun.paramTypes = l_setParam;
							m_Function2Definition[l_sName] = fun;
						}
					}
				}
			}
		}
      //collect observers
		SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_META_OBSERVER);
		if (l_pcMetadataclass)
		{
			SP_ListMetadata::const_iterator l_itElem;
			for (l_itElem = l_pcMetadataclass->GetElements()->begin();
				l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Metadata* l_pcMetadata = *l_itElem;
				wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
				wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();

				m_mObserver2Body[l_sName] = l_sBody;
			}
		}

	}
	else
	{
		if (m_bIsANDL && m_pcANDLFile)
		{
			for (auto constant : *m_pcANDLFile->constants_)
			{//collect constants
				if (!constant) continue;

				wxString name = constant->name_;
				vector<wxString> l_vVluesets;
				for (auto& vset : constant->values_)
				{
					l_vVluesets.push_back(vset);
				}
				m_mConstant2Values[name] = l_vVluesets;
			}
			for (auto fun : *m_pcANDLFile->functions_)
			{
				wxString funName = fun->name_;
				wxString funParam = fun->param_;
				wxString funBody = fun->body_;
				FuncStruct structFun;
				structFun.body = funBody;
				std::set<wxString> l_setParam;
				wxString l_sparams;
				wxString l_str = funParam;
				while (funParam.Contains(","))
				{
					wxString l_scopy = funParam;
					l_sparams = l_scopy.AfterFirst(',');
					wxString par = l_str.BeforeFirst(',');
					l_str = l_sparams;
					funParam = l_sparams;
					l_setParam.insert(par);
				}
				if (!funParam.IsEmpty() && !funParam.Contains(','))
				{
					l_setParam.insert(funParam);
				}
				structFun.paramTypes = l_setParam;
				m_Function2Definition[funName] = structFun;
			}

			//observers
			for (auto& obs : *m_pcANDLFile->observers_)
			{
				if (!obs) continue;

				wxString name = obs->name_;
				wxString type = wxT("Place");
				if (obs->type_ == dssd::andl::ObserverType::TRANS_T) {
					type = wxT("Transition");
				}
				wxString body = obs->function_;
				m_mObserver2Body[name] = body;
			}
			//////////////CANDL part if exists/////////////////
			for (auto& cs : *m_pcANDLFile->colorsets_)
			{
				wxString l_sCsName(cs->name_.c_str(), wxConvUTF8);
				wxString l_sCsdef(cs->definition_.c_str(), wxConvUTF8);
				m_mColorSet2Definition[l_sCsName] = l_sCsdef;
			 }

			for (auto& var: *m_pcANDLFile->variables_)
			{
				 wxString l_sVarName(var->name_.c_str(), wxConvUTF8);
				 wxString l_sCs(var->colorset_.c_str(), wxConvUTF8);
				 m_mVar2ColorSet[l_sVarName] = l_sCs;
			}

			for (auto& fun : *m_pcANDLFile->color_functions_)
			{
				 wxString l_sFunName(fun->name_.c_str(), wxConvUTF8);
				 FuncStruct fun1;
				 fun1.body = fun->body_.c_str();
				 std::set<wxString> l_setParam;
				 for (auto par : fun->params_)
				 {
					l_setParam.insert(par.first);
				 }
				  fun1.paramTypes = l_setParam;
				  m_Function2Definition[l_sFunName] = fun1;
			}
		}
	}
	BuildDeclarationDependencies();//build dependency trees of the declarations
	
	if (!m_bIsANDL)//the graph has to be not null
	{
		ComputeUsedDeclarations();//check if declarations are ued in the NET and assigen the result to dependency trees
	}
}

bool SP_DS_Graph_Declarations::IsToken(const wxString& p_sExp, const wxString& p_sToken)
{

	wxStringTokenizer tokenizer(p_sExp, ":+-/*%(),={.}[]&|>< ");
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		if (token == p_sToken) {
			return true;
		}

	}
	return false;
}

void SP_DS_Graph_Declarations::BuildConstantDependencyTree(const wxString& p_sConstant, sp_node* tree)
{

	for (auto itConstMap = m_mConstant2Values.begin(); itConstMap != m_mConstant2Values.end(); ++itConstMap)
	{
		if (p_sConstant != itConstMap->first) continue;

		 
		for (auto itColorsetMap = m_mColorSet2Definition.begin(); itColorsetMap != m_mColorSet2Definition.end(); ++itColorsetMap)
		{
			if (IsToken(itColorsetMap->second, itConstMap->first))
			{

				(tree->child).push_back(newNode(itColorsetMap->first, NODE_TYPE::COLOR_SET));
				BuildColorSetDependencyTree(itColorsetMap->first, tree->child[tree->child.size() - 1]);

				for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
				{
					for (auto itSetParam = itFunMap->second.paramTypes.begin(); itSetParam != itFunMap->second.paramTypes.end(); ++itSetParam)
					{
						if (IsToken(*itSetParam, itColorsetMap->first))
						{
							(tree->child).push_back(newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR));
							BuildFunctionDependencyTree(itFunMap->first, tree->child[tree->child.size() - 1]);
							break;
						}
					}
				}
			}
		}

		// check colorfunctions
		for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
		{
			wxString l_sFunBody = itFunMap->second.body;
			if (IsToken(l_sFunBody, itConstMap->first))
			{
				(tree->child).push_back(newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR));
				BuildFunctionDependencyTree(itFunMap->first, tree->child[tree->child.size() - 1]);
			}
		}
		//observers
		for (auto itObserverMap = m_mObserver2Body.begin(); itObserverMap != m_mObserver2Body.end(); ++itObserverMap)
		{
			wxString l_sObserverBody = itObserverMap->second;
			if (IsToken(l_sObserverBody, itConstMap->first))
			{
				(tree->child).push_back(newNode(itObserverMap->first, NODE_TYPE::OBSERVER));
				BuildObserverDependencyTree(itObserverMap->first, tree->child[tree->child.size() - 1]);
				
			}
		}
		//check other constants
		for (auto itConst = m_mConstant2Values.begin(); itConst != m_mConstant2Values.end(); ++itConst)
		{
			if (itConst->first != itConstMap->first)
			{
				wxString l_sVal = itConst->second[0];
				if (IsToken(l_sVal, itConstMap->first))
				{    //recursion step
					(tree->child).push_back(newNode(itConst->first, NODE_TYPE::CONSTANT));
					BuildConstantDependencyTree(itConst->first, tree->child[tree->child.size() - 1]);
				}
			}
		}
	  
	}
 
}

void SP_DS_Graph_Declarations::BuildConstantsDependencyTrees()
{
	for (auto itConstMap = m_mConstant2Values.begin(); itConstMap != m_mConstant2Values.end(); ++itConstMap)
	{
		sp_node* l_pcConstTreeRoot = newNode(itConstMap->first, NODE_TYPE::CONSTANT);

		for (auto itColorsetMap = m_mColorSet2Definition.begin(); itColorsetMap != m_mColorSet2Definition.end(); ++itColorsetMap)
		{
			if (IsToken(itColorsetMap->second, itConstMap->first))
			{
				 
				(l_pcConstTreeRoot->child).push_back(newNode(itColorsetMap->first, NODE_TYPE::COLOR_SET));
				BuildColorSetDependencyTree(itColorsetMap->first, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size() - 1]);
			
				for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
				{
					for (auto itSetParam = itFunMap->second.paramTypes.begin(); itSetParam != itFunMap->second.paramTypes.end(); ++itSetParam)
					{
						if (IsToken(*itSetParam, itColorsetMap->first))
						{
							(l_pcConstTreeRoot->child).push_back(newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR));
							BuildFunctionDependencyTree(itFunMap->first, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size() - 1]);
							break;
						}
					}
				}
			}
		}

		for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
		{
			if (IsToken(itFunMap->second.body, itConstMap->first))
			{
				(l_pcConstTreeRoot->child).push_back(newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR));
				BuildFunctionDependencyTree(itFunMap->first, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size() - 1]);
			}
		}
		for (auto itObserverMap = m_mObserver2Body.begin(); itObserverMap != m_mObserver2Body.end(); ++itObserverMap)
		{
			if (IsToken(itObserverMap->second, itConstMap->first))
			{
				(l_pcConstTreeRoot->child).push_back(newNode(itObserverMap->first, NODE_TYPE::OBSERVER));
				//BuildObserverDependencyTree(itObserverMap->first, l_pcConstTreeRoot->child[l_pcConstTreeRoot->child.size() - 1]);
			}
		}
		//constants
		BuildConstantDependencyTree(itConstMap->first, l_pcConstTreeRoot);
		m_vConstantsDepTrees.push_back(l_pcConstTreeRoot);
	}
	 
}

void SP_DS_Graph_Declarations::BuildVariableDependencies()
{
	for (auto itVarMap = m_mVar2ColorSet.begin(); itVarMap != m_mVar2ColorSet.end(); ++itVarMap)
	{
		sp_node* l_pcConstTreeRoot = newNode(itVarMap->first, NODE_TYPE::VAR);
		BuildVariableDependencyTree(itVarMap->first, l_pcConstTreeRoot);
		m_vVarsDepTrees.push_back(l_pcConstTreeRoot);
	}
}

void SP_DS_Graph_Declarations::BuildVariableDependencyTree(const wxString& p_Var, sp_node* tree)
{
	for (auto itVarMap = m_mVar2ColorSet.begin(); itVarMap != m_mVar2ColorSet.end(); ++itVarMap)
	{
		if (itVarMap->first == p_Var)
		{
			for (auto itMapcCS = m_mColorSet2Definition.begin(); itMapcCS != m_mColorSet2Definition.end(); ++itMapcCS)
			{
				if (IsToken(itMapcCS->second, itVarMap->first))
				{
					tree->child.push_back(newNode(itMapcCS->first, NODE_TYPE::COLOR_SET));
					BuildColorSetDependencyTree(itMapcCS->first, tree->child[tree->child.size() - 1]);
				}
			}
		}
	}
}

void SP_DS_Graph_Declarations:: BuildFunctionsDependencyTrees()
{
	for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
	{
		sp_node* l_pcfunTreeRoot = newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR);
		BuildFunctionDependencyTree(itFunMap->first, l_pcfunTreeRoot);
		m_vFunctionsDepTrees.push_back(l_pcfunTreeRoot);
	}
}


void SP_DS_Graph_Declarations::BuildObserverssDependencyTrees()
{
	for (auto itMap = m_mObserver2Body.begin(); itMap != m_mObserver2Body.end(); ++itMap)
	{
		sp_node* l_pcObserverTree = newNode(itMap->first, NODE_TYPE::OBSERVER);
		BuildObserverDependencyTree(itMap->first, l_pcObserverTree);
		m_vObserverDepTrees.push_back(l_pcObserverTree);
	}
}

void SP_DS_Graph_Declarations::BuildObserverDependencyTree(const wxString& p_sObservName, sp_node* tree)
{
	for (auto itObserverMap = m_mObserver2Body.begin(); itObserverMap != m_mObserver2Body.end(); ++itObserverMap)
	{
		if (itObserverMap->first != p_sObservName)
		{
			wxString l_sBody = itObserverMap->second;
			if (IsToken(l_sBody, p_sObservName))
			{
				(tree->child).push_back(newNode(itObserverMap->first, NODE_TYPE::OBSERVER));
				BuildFunctionDependencyTree(itObserverMap->first, tree->child[tree->child.size() - 1]);
			}
		}
	}
}

void SP_DS_Graph_Declarations::BuildFunctionDependencyTree(const wxString& p_sFunName, sp_node* tree)
{
	for (auto itFunMap = m_Function2Definition.begin(); itFunMap != m_Function2Definition.end(); ++itFunMap)
	{
		if (itFunMap->first != p_sFunName)
		{
			wxString l_sBody = itFunMap->second.body;
			if (IsToken(l_sBody, p_sFunName))
			{//case where function is used by another function
				(tree->child).push_back(newNode(itFunMap->first, NODE_TYPE::FUN_COLOOR));
				BuildFunctionDependencyTree(itFunMap->first, tree->child[tree->child.size() - 1]);
			}

		}
	}

	//observers may use functions
	for (auto itObserverMap = m_mObserver2Body.begin(); itObserverMap != m_mObserver2Body.end(); ++itObserverMap)
	{
		wxString l_sBody = itObserverMap->second;
		if (IsToken(l_sBody, p_sFunName))
		{
			(tree->child).push_back(newNode(itObserverMap->first, NODE_TYPE::OBSERVER));
			 
		}
	}
}

void  SP_DS_Graph_Declarations::BuildColorSetDependencyTrees()
{
	for (auto itColorsetMap = m_mColorSet2Definition.begin(); itColorsetMap != m_mColorSet2Definition.end(); ++itColorsetMap)
	{
		sp_node* l_pcColorSetTree = newNode(itColorsetMap->first, NODE_TYPE::COLOR_SET);
		BuildColorSetDependencyTree(itColorsetMap->first, l_pcColorSetTree);
	    
		m_vColorsetDepTrees.push_back(l_pcColorSetTree);
	}
	
}


void  SP_DS_Graph_Declarations::BuildColorSetDependencyTree(const wxString& p_sCS, sp_node* tree)
{
	for (auto itColorsetMap = m_mColorSet2Definition.begin(); itColorsetMap != m_mColorSet2Definition.end(); ++itColorsetMap)
	{
		if (p_sCS != itColorsetMap->first) continue;

		// build variable tree
		for (auto itVarMap = m_mVar2ColorSet.begin(); itVarMap != m_mVar2ColorSet.end(); ++itVarMap)
		{
			if (itVarMap->second == itColorsetMap->first || itVarMap->second == p_sCS)
			{
				(tree->child).push_back(newNode(itVarMap->first, NODE_TYPE::VAR));
			}

		}

		//build Function tree
		for (auto itMapfun = m_Function2Definition.begin(); itMapfun != m_Function2Definition.end(); ++itMapfun)
		{
			for (auto itParamSet = itMapfun->second.paramTypes.begin(); itParamSet != itMapfun->second.paramTypes.end(); ++itParamSet)
			{
				if (IsToken(*itParamSet, itColorsetMap->first))
					(tree->child).push_back(newNode(itMapfun->first, NODE_TYPE::FUN_COLOOR));
			}
			//TODO check colour set if it is used in fun body e.g, elemOf
		}

		for (auto itColorset = m_mColorSet2Definition.begin(); itColorset != m_mColorSet2Definition.end(); ++itColorset)
		{
			if (IsToken(itColorset->second, itColorsetMap->first))
			{
				(tree->child).push_back(newNode(itColorset->first, NODE_TYPE::COLOR_SET));
				BuildColorSetDependencyTree(itColorset->first, tree->child[tree->child.size() - 1]);
			}
		}
	}
	 
}


void  SP_DS_Graph_Declarations::LevelOrderTraversal(sp_node * root, std::map<NODE_TYPE, std::set<wxString>>& l_mapResultKey)
{
	if (root == NULL)
		return;

	// Standard level order traversal code 
	// using queue 
	queue<sp_node *> q;  // Create a queue 
	q.push(root); // Enqueue root  
	while (!q.empty())
	{
		int n = q.size();

		// If this node has children 
		while (n > 0)
		{
			// Dequeue an item from queue and print it 
			sp_node * p = q.front();
			q.pop();
			l_mapResultKey[p->type].insert(p->key);
			// Enqueue all children of the dequeued item 
			if(p)//added later
			for (int i = 0; i<p->child.size(); i++)
				q.push(p->child[i]);
			n--;
		}
	}
}
void SP_DS_Graph_Declarations::BuildDeclarationDependencies()
{
	BuildConstantsDependencyTrees();
	BuildColorSetDependencyTrees();
	BuildVariableDependencies();
	BuildFunctionsDependencyTrees();
	BuildObserverssDependencyTrees();
}

void SP_DS_Graph_Declarations::ClearAll()
{
	m_mColorSet2Definition.clear();
	m_mConstant2Values.clear();
	m_mVar2ColorSet.clear();
	m_Function2Definition.clear();
	m_mObserver2Body.clear();
	m_vColorsetDepTrees.clear();
	m_vConstantsDepTrees.clear();
	m_vFunctionsDepTrees.clear();
	m_vVarsDepTrees.clear();
	m_vObserverDepTrees.clear();

}

/*check if constants or variables, function(for colored/uncolored pn) are used in net marking*/
bool SP_DS_Graph_Declarations::IsDeclarationExistInNetMarking(const wxString& p_sDec)
{
	if (!m_pcGraph1) return false;

	wxString l_netType = m_pcGraph1->GetNetclass()->GetDisplayName();
	if (!l_netType.Contains(wxT("Colored")))
	{
		for (SP_DS_Nodeclass* nc : *(m_pcGraph1->GetNodeclasses()))
		{
			if (nc->GetShowInElementTree())
			{
				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{

					if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
					{
						for (SP_DS_Node* node1 : *(nc->GetElements()))
						{
							wxString l_sNodeclass = nc->GetDisplayName();
							wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
							wxString l_smarking;
							if (l_sNodeclass == wxT("Continuous Place"))
							{
								//continuous place
								l_smarking = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING)->GetValueString();
							}
							else
							{
								l_smarking = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING)->GetValueString();

							}

							if (IsToken(l_smarking, p_sDec))
							{
								return true;
							}
						}
					 
					}
				}
			}
		}
	}
	else {
		//coloured net
		for (SP_DS_Nodeclass* nc : *(m_pcGraph1->GetNodeclasses()))
		{
			if (nc->GetShowInElementTree())
			{
				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
				{

					if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
					{
						for (SP_DS_Node* node1 : *(nc->GetElements()))
						{

							wxString l_sNodeclass = nc->GetDisplayName();
							wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
							wxString l_smarking;
							SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(node1->GetAttribute(SP_DS_CPN_MARKINGLIST));
							if (!l_pcColList) { break; }
							int count = l_pcColList->GetColCount();
							 
							for (unsigned int j = 0; j < l_pcColList->GetColCount(); j++)
							{
							  
								for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
								{
									wxString l_sColorExpr = l_pcColList->GetCell(i, j);
									wxString l_sTokenNum = l_pcColList->GetCell(i, j + 1);
							         
									if(IsToken(l_sColorExpr,p_sDec)||IsToken(l_sTokenNum,p_sDec))
									{
										return true;
									}
									 
								}
								j++;
							}
						}
					
					}
				}
			}
		}
	}
	return false;
}


bool SP_DS_Graph_Declarations::IsExistonArcMultiplicity(const wxString& p_sDec) 
{
	if (!m_pcGraph1) return false;

	if ( m_pcGraph1->GetNetclass()->GetName().Contains("Colored"))
	{
		return false;
	}

	for (SP_DS_Nodeclass* nc : *(m_pcGraph1->GetNodeclasses()))
	{

		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{

				for (SP_DS_Node* node1 : *(nc->GetElements()))
				{

					std::string l_sName = dynamic_cast<SP_DS_NameAttribute*>(node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					for (SP_DS_Edge* l_pcEdge : *(node1->GetSourceEdges()))
					{
						SP_DS_Node* l_pcTarget = dynamic_cast<SP_DS_Node*>(l_pcEdge->GetTarget());
						std::string l_sTarget = dynamic_cast<SP_DS_NameAttribute*>(l_pcTarget->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

						SP_DS_Attribute* l_pcAtt = l_pcEdge->GetAttribute(wxT("Multiplicity"));

						if (l_pcAtt == NULL) //e.g., modifier arc has no Multiplicity
						{
							continue;
						}
						wxString l_sMult = l_pcAtt->GetValueString();
						if (IsToken(l_sMult, p_sDec))
						{
							return true;
						}
					}

				}

			}
		}
	}
	return false;
}


 
bool SP_DS_Graph_Declarations::CheckDeclarationInColouredPredicateofTrans(const wxString& p_sDec)
{
	if (!m_pcGraph1) return false;

	if (!m_pcGraph1->GetNetclass()->GetName().Contains("Colored")) return false;

	SP_ListNode::const_iterator l_itElem;
	SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
	for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph1->GetNodeclass(*it_vTrns);

		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcTransNode = (*l_itElem);


				wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
				 
				wxString l_sGuard;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));

				l_sGuard = l_pcColList->GetCell(0, 1);
				wxString l_sExpressionUnique = l_sGuard;


				if (l_sExpressionUnique.Contains("("))
				{
					l_sExpressionUnique = l_sExpressionUnique.BeforeFirst('(');
					 
				}
				else
				{
					l_sExpressionUnique = l_sExpressionUnique.BeforeFirst(' ');
					l_sExpressionUnique.Replace(wxT(" "), wxT(""));
				}
 
				if(IsToken(l_sExpressionUnique,p_sDec))
				{
					return true;
				}
			}
		}

	}

	SP_ListNode::const_iterator l_itElem1;
	wxString l_sNetClass = m_pcGraph1->GetNetclass()->GetName();

	if ((l_sNetClass == SP_DS_COLSPN_CLASS) ||
		(l_sNetClass == SP_DS_COLCPN_CLASS) ||
		(l_sNetClass == SP_DS_COLHPN_CLASS) ||
		(l_sNetClass == SP_DS_FUZZY_ColCPN_CLASS) ||
		(l_sNetClass == SP_DS_FUZZY_ColSPN_CLASS) ||
		(l_sNetClass == SP_DS_FUZZY_ColHPN_CLASS))
	{

		SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
		for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
		{
			SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph1->GetNodeclass(*it_vTrns);

			if (l_pcNodeclass)
			{
				for (l_itElem1 = l_pcNodeclass->GetElements()->begin(); l_itElem1 != l_pcNodeclass->GetElements()->end(); ++l_itElem1)
				{
					SP_DS_Node* l_pcTransNode = (*l_itElem1);

					wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

					SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(wxT("FunctionList")));

					for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
					{
						wxString l_sPredicate = wxString(l_pcColList->GetCell(i, 0).c_str());
						if (IsToken(l_sPredicate, p_sDec))
						{
							return true;
						}
					}
				}
			}
		}
	}
 
	return false;
}


bool SP_DS_Graph_Declarations::CheckDeclarationInTransitionFunction(const wxString& p_sDec)
{
	if (!m_pcGraph1) return false;

	if (m_pcGraph1->GetNetclass()->GetName().Contains(wxT("Colored")))
		return false;

	for (SP_DS_Nodeclass* nc : *(m_pcGraph1->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{


				for (SP_DS_Node* node1 : *(nc->GetElements()))
				{
					wxString l_sNodeclass = nc->GetDisplayName();
					wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
					if (l_sNodeclass.Contains(wxT("Transition")))
					{
						SP_DS_ColListAttribute* pColListAttr = dynamic_cast<SP_DS_ColListAttribute*>(node1->GetAttribute("FunctionList"));

						if (!pColListAttr) return false;
						wxString l_sFunction = pColListAttr->GetCell(0, 1);
						if (IsToken(l_sFunction, p_sDec))
						{
							return true;
						}
  
					}

				}
			}
		}
	}
	return false;

}

bool   SP_DS_Graph_Declarations::CheckDeclarationOverArcsAndGuards(const wxString& p_sDec)
{
	if (!m_pcGraph1) return false;

	if (!m_pcGraph1->GetNetclass()->GetName().Contains(wxT("Colored")))
		return false;

	SP_ListNode::const_iterator l_itElem;
	SP_ListEdge::const_iterator l_itEdges;
	SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };
	for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph1->GetNodeclass(*it_vTrns);

		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcTransNode = (*l_itElem);

				//get arc expressions
				for (l_itEdges = l_pcTransNode->GetTargetEdges()->begin(); l_itEdges != l_pcTransNode->GetTargetEdges()->end(); ++l_itEdges)
				{
					if ((*l_itEdges)->GetSource())
					{
						SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetSource());
						SP_DS_Edge* l_pcEdge = *l_itEdges;

						if (!l_pcPlaceNode || !l_pcEdge) {
							continue;
						}

						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						if (!l_pcColList)
							continue;

						wxString l_sExp = l_pcColList->GetCell(0, 1);

						if (IsToken(l_sExp, p_sDec))
						{
							return true;
						}
					}
				}

				//get arc expressions
				for (l_itEdges = l_pcTransNode->GetSourceEdges()->begin(); l_itEdges != l_pcTransNode->GetSourceEdges()->end(); ++l_itEdges)
				{
					if ((*l_itEdges)->GetTarget())
					{
						SP_DS_Node* l_pcPlaceNode = dynamic_cast<SP_DS_Node*>((*l_itEdges)->GetTarget());
						SP_DS_Edge* l_pcEdge = *l_itEdges;

						if (!l_pcPlaceNode || !l_pcEdge) {
							continue;
						}

						SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcEdge->GetAttribute(SP_DS_CPN_INSCRIPTION));
						if (!l_pcColList)
							continue;

						wxString l_sExp = l_pcColList->GetCell(0, 1);

						if (IsToken(l_sExp, p_sDec))
						{
							return true;
					    }
					}
				}
 
				// To get the name of the transition
				wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

				wxString l_sGuard;
				SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcTransNode->GetAttribute(SP_DS_CPN_GUARDLIST));

				l_sGuard = l_pcColList->GetCell(0, 1);
				wxString l_s = l_pcColList->GetCell(0, 0);

				 if(IsToken(l_sGuard,p_sDec))
				 {
					 return true;
				 }
			}
		}

	}
 
	return false;
}

bool SP_DS_Graph_Declarations::IsDeclarationExistInRateFunctions(const wxString& p_sDec)
{
	wxString m_sNetClass = m_pcGraph1->GetNetclass()->GetName();

	if (!(m_sNetClass == SP_DS_COLSPN_CLASS ||
		m_sNetClass == SP_DS_COLCPN_CLASS ||
		m_sNetClass == SP_DS_COLHPN_CLASS ||
		m_sNetClass == SP_DS_FUZZY_ColCPN_CLASS ||
		m_sNetClass == SP_DS_FUZZY_ColSPN_CLASS ||
		m_sNetClass == SP_DS_FUZZY_ColHPN_CLASS ||
		m_sNetClass == SP_DS_SPN_CLASS ||
		m_sNetClass == SP_DS_CONTINUOUSPED_CLASS ||
		m_sNetClass == SP_DS_HYBRIDPN_CLASS ||
		m_sNetClass == SP_DS_FUZZYCPN_CLASS ||
		m_sNetClass == SP_DS_FUZZYSPN_CLASS ||
		m_sNetClass == SP_DS_FUZZYHPN_CLASS
		))
		return false;

	SP_ListNode::const_iterator l_itElem;
	SP_VectorString l_vTransNC = { SP_DS_CONTINUOUS_TRANS ,SP_DS_STOCHASTIC_TRANS ,wxT("Immediate Transition") ,wxT("Deterministic Transition") ,wxT("Scheduled Transition") };

	for (auto it_vTrns = l_vTransNC.begin(); it_vTrns != l_vTransNC.end(); ++it_vTrns)
	{
		SP_DS_Nodeclass* l_pcNodeclass = m_pcGraph1->GetNodeclass(*it_vTrns);

		if (l_pcNodeclass)
		{
			for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
			{
				SP_DS_Node* l_pcTransNode = (*l_itElem);

				wxString l_sTransName = dynamic_cast<SP_DS_NameAttribute*>(l_pcTransNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();


				SP_DS_ColListAttribute* l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcTransNode->GetAttribute(wxT("FunctionList")));

				for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
				{
					wxString l_sRateFun = wxString(l_pcColList->GetCell(i, 1).c_str());
					if (IsToken(l_sRateFun, p_sDec))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool  SP_DS_Graph_Declarations::IsColorsetUsedByColoredPlace(const wxString& p_SColorSet)
{
	if (!m_pcGraph1) return false;

	wxString m_sNetClass = m_pcGraph1->GetNetclass()->GetName();
	if (!m_sNetClass.Contains(wxT("Colored"))) return false;

	for (SP_DS_Nodeclass* nc : *(m_pcGraph1->GetNodeclasses()))
	{
		if (nc->GetShowInElementTree())
		{
			if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_ID) && nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))
			{

				if (nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_MARKING) | nc->HasAttributeType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_DOUBLEMARKING))
				{
					for (SP_DS_Node* node1 : *(nc->GetElements()))
					{
						//cs assigned by places
						wxString l_sNodeclass = nc->GetDisplayName();
						wxString l_sName = node1->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
						SP_DS_Attribute* l_pcAttr = node1->GetAttribute(SP_DS_CPN_COLORSETNAME);
						std::string l_sCSName = l_pcAttr->GetValueString();
						
						if (IsToken(l_sCSName, p_SColorSet))
						{
							return true;
						}

					}
				}
			}
		}
	}
	return false;
}

bool SP_DS_Graph_Declarations::CheckConstantInNet(const wxString& p_sConst)
{
	if (!IsDeclarationExistInNetMarking(p_sConst))
	{
		wxString m_sNetClass = m_pcGraph1->GetNetclass()->GetName();

		if (!m_sNetClass.Contains(wxT("Colored")))
		{
			if (!IsExistonArcMultiplicity(p_sConst))
			{
				if (!IsDeclarationExistInRateFunctions(p_sConst))
				{
					return false;
				}
			}
			
		}
		else
		{
			if (!CheckDeclarationOverArcsAndGuards(p_sConst))
			{
				if (!CheckDeclarationInColouredPredicateofTrans(p_sConst))
				{
					if (!IsDeclarationExistInRateFunctions(p_sConst))
					{
						return false;
					}
				}
			}
		}
		
	}
	return true;
}

bool SP_DS_Graph_Declarations::CheckColorSetInNet(const wxString& p_sColorSet)
{
	if (!IsColorsetUsedByColoredPlace(p_sColorSet))
	{
		if (!CheckDeclarationOverArcsAndGuards(p_sColorSet))
		{
			if (!CheckDeclarationInColouredPredicateofTrans(p_sColorSet))
			{

				return false;
			}
		}
	}
	
	return true;
}

bool SP_DS_Graph_Declarations::CheckFunctionInNet(const wxString& p_sFun)
{
	if (!CheckDeclarationOverArcsAndGuards(p_sFun))
	{
		if (!IsDeclarationExistInNetMarking(p_sFun))
		{
			if (!CheckDeclarationInColouredPredicateofTrans(p_sFun))
			{
				if (!IsExistonArcMultiplicity(p_sFun))//uncolored net
				{
					if (!CheckDeclarationInTransitionFunction(p_sFun))

					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool SP_DS_Graph_Declarations::CheckVariableInNet(const wxString& p_sVar)
{
	if (!IsDeclarationExistInNetMarking(p_sVar))
	{
		if (!CheckDeclarationOverArcsAndGuards(p_sVar))
		{
			if (!CheckDeclarationInColouredPredicateofTrans(p_sVar))
			{
				return false;
			}
		}
	}
	return true;
}

void  SP_DS_Graph_Declarations::ComputeUsedColorSets()
{
	for (unsigned int i = 0; i < m_vColorsetDepTrees.size(); i++)
	{
		if (m_vColorsetDepTrees[i]->isUsed) { continue; }

		if (CheckColorSetInNet(m_vColorsetDepTrees[i]->key))
		{
			m_vColorsetDepTrees[i]->isUsed = true;
		}
		else
		{
			std::map<NODE_TYPE, set<wxString>> l_mNode2Childers;
			LevelOrderTraversal(m_vColorsetDepTrees[i], l_mNode2Childers);
			//LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNode2Childers);
			for (auto itMapChildern = l_mNode2Childers.begin(); itMapChildern != l_mNode2Childers.end(); ++itMapChildern)
			{
				for (auto iTSet = itMapChildern->second.begin(); iTSet != itMapChildern->second.end(); ++iTSet)
				{
					 
					if (itMapChildern->first == NODE_TYPE::COLOR_SET)
					{
						if (CheckColorSetInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vColorsetDepTrees.size(); j++)
							{
								if (m_vColorsetDepTrees[j]->key == *iTSet)
								{
									m_vColorsetDepTrees[j]->isUsed = true;
									m_vColorsetDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
					if (itMapChildern->first == NODE_TYPE::VAR)
					{
						if (CheckVariableInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vVarsDepTrees.size(); j++)
							{
								if (m_vVarsDepTrees[j]->key == *iTSet)
								{
									m_vVarsDepTrees[j]->isUsed = true;
									m_vColorsetDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}

					if (itMapChildern->first == NODE_TYPE::FUN_COLOOR)
					{
						if (CheckFunctionInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vFunctionsDepTrees.size(); j++)
							{
								if (m_vFunctionsDepTrees[j]->key == *iTSet)
								{
									m_vFunctionsDepTrees[j]->isUsed = true;
									m_vColorsetDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

void SP_DS_Graph_Declarations::ComputeUsedFunctions()
{
	for (unsigned int i = 0; i < m_vFunctionsDepTrees.size(); i++)
	{
		if (m_vFunctionsDepTrees[i]->isUsed) { continue; }

		if (CheckFunctionInNet(m_vFunctionsDepTrees[i]->key))
		{
			m_vFunctionsDepTrees[i]->isUsed = true;

		}
		else
		{
			std::map<NODE_TYPE, set<wxString>> l_mNode2Childers;
			//LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNode2Childers);
			LevelOrderTraversal(m_vFunctionsDepTrees[i], l_mNode2Childers);
			for (auto itMapChildern = l_mNode2Childers.begin(); itMapChildern != l_mNode2Childers.end(); ++itMapChildern)
			{
				for (auto iTSet = itMapChildern->second.begin(); iTSet != itMapChildern->second.end(); ++iTSet)
				{
					if (itMapChildern->first == NODE_TYPE::FUN_COLOOR)
					{
						if (CheckFunctionInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vFunctionsDepTrees.size(); j++)
							{
								if (m_vFunctionsDepTrees[j]->key == *iTSet)
								{
									m_vFunctionsDepTrees[j]->isUsed = true;
									m_vFunctionsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
					if (itMapChildern->first == NODE_TYPE::OBSERVER)
					{
						if (CheckObserverInViewe(*iTSet))
						{
							for (unsigned int j = 0; j < m_vObserverDepTrees.size(); j++)
							{
								if (m_vObserverDepTrees[j]->key == *iTSet)
								{
									m_vObserverDepTrees[j]->isUsed = true;
									m_vFunctionsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

}

void  SP_DS_Graph_Declarations::ComputeUsedVariables()
{
	for (unsigned int i = 0; i < m_vVarsDepTrees.size(); i++)
	{
		if (m_vVarsDepTrees[i]->isUsed) { continue; }

		if (CheckVariableInNet(m_vVarsDepTrees[i]->key))
		{
			m_vVarsDepTrees[i]->isUsed = true;
		}
		else
		{
			std::map<NODE_TYPE, set<wxString>> l_mNode2Childers;
			LevelOrderTraversal(m_vVarsDepTrees[i], l_mNode2Childers);

			for (auto itMapChildern = l_mNode2Childers.begin(); itMapChildern != l_mNode2Childers.end(); ++itMapChildern)
			{
				for (auto iTSet = itMapChildern->second.begin(); iTSet != itMapChildern->second.end(); ++iTSet)
				{
					if (itMapChildern->first == NODE_TYPE::VAR)
					{
						if (CheckVariableInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vVarsDepTrees.size(); j++)
							{
								if (m_vVarsDepTrees[j]->key == *iTSet)
								{
									m_vVarsDepTrees[j]->isUsed = true;
									m_vVarsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}

					if (itMapChildern->first == NODE_TYPE::COLOR_SET)
					{
						if (CheckColorSetInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vColorsetDepTrees.size(); j++)
							{
								if (m_vColorsetDepTrees[j]->key == *iTSet)
								{
									m_vColorsetDepTrees[j]->isUsed = true;
									m_vVarsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}


void  SP_DS_Graph_Declarations::ComputeUsedConstants()
{
	for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
	{
		if (m_vConstantsDepTrees[i]->isUsed) { continue; }

		if (CheckConstantInNet(m_vConstantsDepTrees[i]->key))
		{//if the constant is used directly in the net marking
			m_vConstantsDepTrees[i]->isUsed = true;
		}
		else
		{
			std::map<NODE_TYPE, set<wxString>> l_mNode2Childers;
			LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNode2Childers);

			for (auto itMapChildern = l_mNode2Childers.begin(); itMapChildern != l_mNode2Childers.end(); ++itMapChildern)
			{
				for (auto iTSet = itMapChildern->second.begin(); iTSet != itMapChildern->second.end(); ++iTSet)
				{
				 
					if (itMapChildern->first == NODE_TYPE::CONSTANT)
					{
						if (CheckConstantInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vConstantsDepTrees.size(); j++)
							{
								if (m_vConstantsDepTrees[j]->key == *iTSet)
								{
									m_vConstantsDepTrees[j]->isUsed = true;
									m_vConstantsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
					if (itMapChildern->first == NODE_TYPE::COLOR_SET)
					{
						if (CheckColorSetInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vColorsetDepTrees.size(); j++)
							{
								if (m_vColorsetDepTrees[j]->key == *iTSet)
								{
									m_vColorsetDepTrees[j]->isUsed = true;
									m_vConstantsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}

					if (itMapChildern->first == NODE_TYPE::FUN_COLOOR)
					{
						if (CheckFunctionInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vFunctionsDepTrees.size(); j++)
							{
								if (m_vFunctionsDepTrees[j]->key == *iTSet)
								{
									m_vFunctionsDepTrees[j]->isUsed = true;
									m_vConstantsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}

					if (itMapChildern->first == NODE_TYPE::VAR)
					{
						if (CheckVariableInNet(*iTSet))
						{
							for (unsigned int j = 0; j < m_vVarsDepTrees.size(); j++)
							{
								if (m_vVarsDepTrees[j]->key == *iTSet)
								{
									m_vVarsDepTrees[j]->isUsed = true;
									m_vConstantsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
					 
					if (itMapChildern->first == NODE_TYPE::OBSERVER)
					{
						if (CheckObserverInViewe(*iTSet))
						{
							for (unsigned int j = 0; j < m_vObserverDepTrees.size(); j++)
							{
								if (m_vObserverDepTrees[j]->key == *iTSet)
								{
									m_vObserverDepTrees[j]->isUsed = true;
									m_vConstantsDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
					
				}
			}

		}
	}
}

void SP_DS_Graph_Declarations::ComputeUsedObservers()
{
	for (unsigned int i = 0; i < m_vObserverDepTrees.size(); i++)
	{
		if (CheckObserverInViewe(m_vObserverDepTrees[i]->key))
		{
			m_vObserverDepTrees[i]->isUsed = true;
		}
		else
		{
			std::map<NODE_TYPE, set<wxString>> l_mNode2Childers;
			LevelOrderTraversal(m_vObserverDepTrees[i], l_mNode2Childers);
			for (auto itMapChildern = l_mNode2Childers.begin(); itMapChildern != l_mNode2Childers.end(); ++itMapChildern)
			{
				for (auto iTSet = itMapChildern->second.begin(); iTSet != itMapChildern->second.end(); ++iTSet)
				{
					if (itMapChildern->first == NODE_TYPE::OBSERVER)
					{
						if (CheckObserverInViewe(*iTSet))
						{
							for (unsigned int j = 0; j < m_vObserverDepTrees.size(); j++)
							{
								if (m_vObserverDepTrees[j]->key == *iTSet)
								{
									m_vObserverDepTrees[j]->isUsed = true;
									m_vObserverDepTrees[i]->isUsed = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

bool SP_DS_Graph_Declarations::CheckObserverInViewe(const wxString& p_sObserve)
{
	wxString l_netType = m_pcGraph1->GetNetclass()->GetDisplayName();
	if (l_netType == SP_DS_SIMPLEGRAPH_CLASS | l_netType == SP_DS_BIPARTGRAPH_CLASS | l_netType == SP_DS_PN_CLASS | l_netType == SP_DS_EXTPN_CLASS)
	{
		return false;
	}
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph1->GetMetadataclass(wxT("Plot"));
	SP_DS_Metadata* m_pcCurrentTablePlot;
	const SP_ListMetadata* l_pcViewList = l_pcMetadataclass->GetElements();
	if (!l_pcViewList->empty())
		
		if (l_pcViewList) {

			for (SP_ListMetadata::const_iterator l_itElem = l_pcViewList->begin(); l_itElem != l_pcViewList->end(); ++l_itElem)
			{
				wxString l_sName = (*l_itElem)->GetAttribute(wxT("Name"))->GetValueString();
				m_pcCurrentTablePlot = (*l_itElem);
				SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
				for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
				{
					l_sName = l_pcPlaceIdList->GetCell(i, 6);
					if (IsToken(l_sName, p_sObserve))
					{
						return true;
					}

				}
			}
		}

	/**********************check all views for used observers***********************************/
	wxString l_sName;

	//get plot list. We ignore tables in old implementation
	SP_DS_Metadataclass* l_pcMetadataclass1 = m_pcGraph1->GetMetadataclass(wxT("Plot"));


	const SP_ListMetadata* l_pcViewList1 = l_pcMetadataclass1->GetElements();

	//read all views
	for (SP_DS_Metadata* l_pcMeta : *l_pcViewList1)
	{
		l_sName = l_pcMeta->GetAttribute(wxT("Name"))->GetValueString();

		SP_DS_ColListAttribute* l_pcPlaceIdList = dynamic_cast<SP_DS_ColListAttribute*>(m_pcCurrentTablePlot->GetAttribute(wxT("CurveInfo")));
		for (unsigned long i = 0; i < l_pcPlaceIdList->GetRowCount(); i++)
		{

			l_sName = l_pcPlaceIdList->GetCell(i, 6);
			if (IsToken(l_sName, p_sObserve))
			{
				return true;
			}

		}

	}
	return false;
}

void SP_DS_Graph_Declarations::ComputeUsedDeclarations()
{
	  ComputeUsedConstants();
	  ComputeUsedColorSets();
	  ComputeUsedFunctions();
	  ComputeUsedVariables();
	  ComputeUsedObservers();
}

void SP_DS_Graph_Declarations::ComputeBackwardDependency( wxString& p_sdec, NODE_TYPE p_T, set<wxString>& p_sSet)
{
	wxString l_sDeclaration = p_sdec;
	if (l_sDeclaration.Contains(":"))
	{
		l_sDeclaration = l_sDeclaration.BeforeFirst(':');
		l_sDeclaration.Trim();
		l_sDeclaration.Trim(false);
	}
	if (l_sDeclaration.Contains("("))
	{
		l_sDeclaration = l_sDeclaration.BeforeFirst('(');
		l_sDeclaration.Trim();
		l_sDeclaration.Trim(false);
	}
	if (l_sDeclaration.Contains("="))
	{
		l_sDeclaration = l_sDeclaration.BeforeFirst('=');
		l_sDeclaration.Trim();
		l_sDeclaration.Trim(false);
	}
	p_sdec = l_sDeclaration;
	if (p_T == NODE_TYPE::CONSTANT)
	{
		for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
		{
			if (m_vConstantsDepTrees[i]->key != p_sdec)
			{
				std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
				LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNodeType2Children);

				for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
				{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
					for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
					{
						if (*itSet == p_sdec)
						{
							p_sSet.insert(m_vConstantsDepTrees[i]->key);
							break;
						}
					}
				}
			}
		}
	}

	if (p_T == NODE_TYPE::COLOR_SET)
	{
		for (unsigned int i = 0; i < m_vColorsetDepTrees.size(); i++)
		{
			if (m_vColorsetDepTrees[i]->key != p_sdec)
			{
				std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
				LevelOrderTraversal(m_vColorsetDepTrees[i], l_mNodeType2Children);

				for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end();++ itMap)
				{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
					for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
					{
						if (*itSet == p_sdec)
						{
							p_sSet.insert(m_vColorsetDepTrees[i]->key);
							break;
						}
					}
				}
			}
		}
	 
		//check constants
		for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
		{
				std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
				LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNodeType2Children);

				for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
				{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
					for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
					{
						if (*itSet == p_sdec)
						{
							p_sSet.insert(m_vConstantsDepTrees[i]->key);
							break;
						}
					}
				}
			}
		/*color set may also depend on variable, e.g, SubCS=Grid2D[x>3&y<2];*/
		for (unsigned int i = 0; i < m_vVarsDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vVarsDepTrees[i], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
			{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vVarsDepTrees[i]->key);
						break;
					}
				}
			}
		}
          }

	if (p_T == NODE_TYPE::FUN_COLOOR)
	{
		for (unsigned int i = 0; i < m_vFunctionsDepTrees.size(); i++)
		{
			if (m_vFunctionsDepTrees[i]->key != p_sdec)
			{
				std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
				LevelOrderTraversal(m_vFunctionsDepTrees[i], l_mNodeType2Children);

				for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end();++ itMap)
				{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
					for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
					{
						if (*itSet == p_sdec)
						{
							p_sSet.insert(m_vFunctionsDepTrees[i]->key);
							break;
						}
					}
				}
			}
			 
		}

		/*check constants**/
		for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
			{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vConstantsDepTrees[i]->key);
						break;
					}
				}
			}
		}

		/**check color set as well if it the function is one of childern**/
		for (unsigned int i = 0; i < m_vColorsetDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vColorsetDepTrees[i], l_mNodeType2Children);

				for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end();++ itMap)
				{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
					for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
					{
						if (*itSet == p_sdec)
						{
							p_sSet.insert(m_vColorsetDepTrees[i]->key);
							break;
						}
					}
				}
		}

	}

	if (p_T == NODE_TYPE::VAR)
	{
		for (unsigned int i = 0; i < m_vColorsetDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vColorsetDepTrees[i], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end();++ itMap)
			{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vColorsetDepTrees[i]->key);
						break;
					}
				}
			}
		}


		/**search constants trees*/
		for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end();++ itMap)
			{    //go through all children, check if the declaration, e.g, constant is exist, then add the father node as backward dependency
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vConstantsDepTrees[i]->key);
						break;
					}
				}
			}
		}
		
	}

	if (p_T == NODE_TYPE::OBSERVER)
	{
		for (unsigned int i = 0; i < m_vConstantsDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vConstantsDepTrees[i], l_mNodeType2Children);
			
			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vConstantsDepTrees[i]->key);
						break;
					}
				}
			}
		}
		
		for (unsigned int j = 0; j < m_vFunctionsDepTrees.size(); j++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vFunctionsDepTrees[j], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vFunctionsDepTrees[j]->key);
						break;
					}
				}
			}
		}

		for (unsigned int i = 0; i < m_vObserverDepTrees.size(); i++)
		{
			std::map<NODE_TYPE, std::set<wxString>> l_mNodeType2Children;
			LevelOrderTraversal(m_vObserverDepTrees[i], l_mNodeType2Children);

			for (auto itMap = l_mNodeType2Children.begin(); itMap != l_mNodeType2Children.end(); ++itMap)
			{
				for (auto itSet = itMap->second.begin(); itSet != itMap->second.end(); ++itSet)
				{
					if (*itSet == p_sdec)
					{
						p_sSet.insert(m_vObserverDepTrees[i]->key);
						break;
					}
				}
			}
		}
	}

	p_sSet.insert(p_sdec);
}

bool SP_DS_Graph_Declarations::DeleteConstant(const wxString& p_sConstName)
{
	if (!m_pcGraph1) return false;

	if (m_pcGraph1->GetNetclass()->GetDisplayName().Contains(wxT("Colored"))) {

		wxString l_sTempClassName = m_pcGraph1->GetNetclass()->GetName();
		wxString l_sClass;
		if (l_sTempClassName.Contains("Fuzzy"))
		{
			l_sClass = SP_DS_META_CONSTANT;
		}
		else
		{
			l_sClass = SP_DS_CPN_CONSTANT_HARMONIZING;
		}

		SP_DS_Metadataclass* l_pcConstants = m_pcGraph1->GetMetadataclass(l_sClass);

		for (SP_DS_Metadata* l_pcMeta : *(l_pcConstants)->GetElements())
		{
			wxString l_sNameCon = l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
			if (l_sNameCon == p_sConstName)
			{
				return (l_pcConstants->RemoveElement(l_pcMeta));
			}
		}
	}
	else {

		SP_DS_Metadataclass* l_pcConstants = m_pcGraph1->GetMetadataclass(SP_DS_META_CONSTANT);
		for (SP_DS_Metadata* l_pcMeta : *(l_pcConstants)->GetElements())
		{
			wxString l_sNameCon = l_pcMeta->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME)->GetValueString();
			if (l_sNameCon == p_sConstName)
			{
				return (l_pcConstants->RemoveElement(l_pcMeta));
			}
		}
	}
	return false;
}

bool SP_DS_Graph_Declarations::DeleteVariable(const wxString& p_svar)
{
	if (!m_pcGraph1) return false;
	SP_DS_Metadataclass* l_pcMetadataclass;
	l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);

	SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());//l_pcMetadataclass->NewElement(1);
	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sVar = l_pcColList->GetCell(i, 0);
		if (p_svar == l_sVar)
		{
			return (l_pcColList->RemoveRow(i));
		}
	}
	return false;
}

bool SP_DS_Graph_Declarations::DeleteColorSet(const wxString& p_sCsName)
{
	if (!m_pcGraph1) return false;

	if (!m_pcGraph1->GetNetclass()->GetDisplayName().Contains(wxT("Colored")))  return false;

	SP_DS_Metadataclass* l_pcMetadataclass;
	SP_VectorString l_vCSType = { SP_DS_CPN_BASICCOLORSETCLASS,SP_DS_CPN_STRUCTUREDCOLORSETCLASS,SP_DS_CPN_ALIASCOLORSETCLASS };
	SP_VectorString l_vCSAtt = { wxT("ColorsetList"),wxT("StructuredColorsetList"),wxT("AliasColorsetList") };
	for (int cs = 0; cs < l_vCSType.size(); cs++)
	{
		l_pcMetadataclass = m_pcGraph1->GetMetadataclass(l_vCSType[cs]);

		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(l_vCSAtt[cs]));

		for (int i = 0; i < l_pcColList->GetRowCount(); i++)
		{
			wxString l_sCSName = l_pcColList->GetCell(i, 0);
			wxString l_sCSComp = l_pcColList->GetCell(i, 2);
			if (l_sCSName == p_sCsName)
			{
			  return (l_pcColList->RemoveRow(i));
		    }
		}
	}
	return false;
}

bool SP_DS_Graph_Declarations::DeleteFunction(const wxString& p_sfun)
{
	if (!m_pcGraph1) return false;
	SP_DS_Metadataclass* l_pcMetadataclass;
	bool l_bIsColored = false;
	wxString l_sNet = m_pcGraph1->GetNetclass()->GetDisplayName();
	if (l_sNet.Contains(wxT("Colored")))
	{
		l_bIsColored = true;
	}
	if (l_bIsColored)
	{
		l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_META_FUNCTION);
		SP_DS_Metadata* l_pcNewMetadata = *(l_pcMetadataclass->GetElements()->begin());
		SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
		for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
		{

			wxString l_sName = l_pcColList->GetCell(i, 1);
			if (p_sfun == l_sName)
			{
				return (l_pcColList->RemoveRow(i));	 
			}
		}

	}
	else {
		l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_META_FUNCTION);
	}

	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			if (p_sfun == l_sName)
			{
				return (l_pcMetadataclass->RemoveElement(l_pcMetadata));
			 
			}
		}
	}
	return false;
}

bool SP_DS_Graph_Declarations::DeleteObserver(const wxString& p_sobserver)
{
	if (!m_pcGraph1) return false;
	SP_DS_Metadataclass* l_pcMetadataclass = m_pcGraph1->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			if (p_sobserver == l_sName)
			{
				return (l_pcMetadataclass->RemoveElement(l_pcMetadata));
				//SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
			}
		}
	}
	return false;
}
