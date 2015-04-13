//////////////////////////////////////////////////////////////////////
// $Author: Steffen Liese $
// $Version: 0.1 $
// $Date: 2009/01/25 $
// Short Description: export configuration 
//////////////////////////////////////////////////////////////////////
#include "export/SP_ExportConfiguration.h"

SP_ExportConfiguration::SP_ExportConfiguration()
{
} 

SP_ExportConfiguration::~SP_ExportConfiguration()
{
} 

bool 
SP_ExportConfiguration::AddConfigurationElement(wxString Element_name,bool Element_checked,wxString Element_value)
{
	if (GetConfigurationElement(Element_name)== NULL )
	{
		ConfigElement* CF = new ConfigElement;
		CF->name=Element_name;
		CF->checked=Element_checked;
		CF->value=Element_value;
		ConfigList.push_back(CF);
		return true;
	}
	return false;
}
bool 
SP_ExportConfiguration::SetConfigurationElement(wxString Element_name,bool Element_checked,wxString Element_value)
{
	ConfigElement* CF;
	CF=GetConfigurationElement(Element_name);
	if (CF == NULL)
	{
		//add
		AddConfigurationElement(Element_name,Element_checked,Element_value);
	}else{
		//set
		CF->name=Element_name;
		CF->checked=Element_checked;
		CF->value=Element_value;
	}
	return true;
}
ConfigElement* 
SP_ExportConfiguration::GetConfigurationElement(wxString Element_name)
{
	ConfigElement* CF; 	
	for (ConfigListIt=ConfigList.begin();ConfigListIt != ConfigList.end();ConfigListIt++){
		CF =(*ConfigListIt);
		if (Element_name==CF->name){
			return CF;
		}
	}
	return NULL;
}


bool 
SP_ExportConfiguration::AddtoAllusableElementsforPartition(wxString Element_name)
{   
	//no duplikate
	ConfigElement* CF1; 	
	for (AllElementsforPartitionListIt=AllElementsforPartitionList.begin();AllElementsforPartitionListIt != AllElementsforPartitionList.end();AllElementsforPartitionListIt++){
		CF1 =(*AllElementsforPartitionListIt);
		if (CF1->name==Element_name)
		{
			return false;
		}
	}
	//AllelementsforPartition=List;
	ConfigElement* CF = new ConfigElement;
	CF->name=Element_name;
	CF->checked=false;
	CF->value=wxT("place");
	AllElementsforPartitionList.push_back(CF);
	return true;
}
bool 
SP_ExportConfiguration::DeletefromAllusableElementsforPartition(wxString Element_name)
{   
	ConfigElement* CF; 	
	for (AllElementsforPartitionListIt=AllElementsforPartitionList.begin();AllElementsforPartitionListIt != AllElementsforPartitionList.end();AllElementsforPartitionListIt++){
		CF =(*AllElementsforPartitionListIt);
		if (CF->name==Element_name)
		{
			AllElementsforPartitionList.remove(CF);
			delete CF;
			return true;
		}
	}
	return false;
}
bool 
SP_ExportConfiguration::newPartition()
{
		Partition* P = new Partition;
		P->name=wxT("partition");
		PartitionList.push_back(P);
	return true;
}

bool 
SP_ExportConfiguration::deletePartition(int Partition_number)
{
	wxString txt;
	Partition* P; 	

	P=GetPartition(Partition_number);
	if (P!=NULL)
	{
		ConfigElement* CF; 
		//while(P->ConfigList.size() >0){
		//while(P->ConfigListIt != P->ConfigList.end()){
		for (P->ConfigListIt=P->ConfigList.begin();P->ConfigListIt != P->ConfigList.end();P->ConfigListIt++){
			CF =(*P->ConfigListIt);
			AddtoAllusableElementsforPartition(CF->name);
			//P->ConfigListIt=P->ConfigList.begin();
			//P->ConfigList.remove(CF);
			//delete CF;
		//}
		}
		PartitionList.remove(P);
		delete P;
	}else
	{
		return false;
	}
	return true;
}
bool 
SP_ExportConfiguration::AddElementtoPartition(int Partition_number,wxString Element_name)
{
	Partition* P; 	
	P=GetPartition(Partition_number);
	if (P!=NULL)
	{
		ConfigElement* CF = new ConfigElement;
		CF->name=Element_name;
		CF->checked=false;
		CF->value=wxT("place");
		P->ConfigList.push_back(CF);
		return true;
	}
	return false;
}
bool 
SP_ExportConfiguration::DeleteElementfromPartition(int Partition_number,wxString Element_name)
{
	Partition* P; 	
	P=GetPartition(Partition_number);
	if (P!=NULL)
	{
		ConfigElement* CF; 
		for (P->ConfigListIt=P->ConfigList.begin();P->ConfigListIt != P->ConfigList.end();P->ConfigListIt++){
			CF =(*P->ConfigListIt);
			if(CF->name==Element_name)
			{
				AddtoAllusableElementsforPartition(CF->name);
				P->ConfigList.remove(CF);
				delete CF;
				return true;
			}
		}
	}
	return false;
}
Partition* 
SP_ExportConfiguration::GetPartition(int Partition_number)
{
	Partition* P; 	
	int i=0;
	for (PartitionListIt=PartitionList.begin();PartitionListIt != PartitionList.end();PartitionListIt++){
		P =(*PartitionListIt);
		i=i+1;
		if (i==Partition_number){
			return P;
		}
	}
	return NULL;
}
int
SP_ExportConfiguration::GetPartition_count()
{
	int i=0;
	for (PartitionListIt=PartitionList.begin();PartitionListIt != PartitionList.end();PartitionListIt++){
		i=i+1;
	}
	return i;
}
wxArrayString
SP_ExportConfiguration::GetPartitions_aswxArrayString()
{
	wxArrayString feld;
	int p_anz=GetPartition_count();
	for (int i=1;(i<(p_anz+1));i++){
		wxString txt=GetPartition_aswxString(i);
		if (txt==wxT(""))
		{
			txt=wxT("...");
		}
		feld.push_back(wxT("{") + txt + wxT("}"));
	}
	return feld;
}
wxArrayString
SP_ExportConfiguration::GetALLusableElementsforPartition_aswxArrayString()
{
	wxArrayString feld;

	ConfigElement* CF; 	
	for (AllElementsforPartitionListIt=AllElementsforPartitionList.begin();AllElementsforPartitionListIt != AllElementsforPartitionList.end();AllElementsforPartitionListIt++){
		CF =(*AllElementsforPartitionListIt);
		feld.push_back(CF->name);
	}
	return feld;
}
wxArrayString
SP_ExportConfiguration::GetPartition_aswxArrayString(int Partition_number)
{
	wxArrayString feld;
	Partition* P;
	wxString Partitionstring=wxT("");
	P=GetPartition(Partition_number);
	if (P!=NULL)
	{
		ConfigElement* CF; 	
		for (P->ConfigListIt=P->ConfigList.begin();P->ConfigListIt != P->ConfigList.end();P->ConfigListIt++){
			CF =(*P->ConfigListIt);
			feld.push_back(CF->name);
		}
	}
	return feld;
}
wxString
SP_ExportConfiguration::GetPartition_aswxString(int Partition_number)
{
	Partition* P;
	wxString Partitionstring=wxT("");
	P=GetPartition(Partition_number);
	bool first=true;
	if (P!=NULL)
	{
		ConfigElement* CF; 
		for (P->ConfigListIt=P->ConfigList.begin();P->ConfigListIt != P->ConfigList.end();P->ConfigListIt++){
			CF =(*P->ConfigListIt);
			if (first){
				Partitionstring= CF->name;
				first=false;
			}else{
				Partitionstring=Partitionstring + wxT(",") + CF->name;
			}
		}
	}
	return Partitionstring;
}

int 
SP_ExportConfiguration::GetfromAllusableElementsforPartition_Count()
{
	int anz=0;
	ConfigElement* CF; 	
	for (AllElementsforPartitionListIt=AllElementsforPartitionList.begin();AllElementsforPartitionListIt != AllElementsforPartitionList.end();AllElementsforPartitionListIt++){
		CF =(*AllElementsforPartitionListIt);
		anz++;
	}
	return anz;
}
wxString 
SP_ExportConfiguration::GetItemAllusableElementsforPartition(int i)
{
	int anz=0;
	wxString elem=wxT("");
	ConfigElement* CF; 	
	for (AllElementsforPartitionListIt=AllElementsforPartitionList.begin();AllElementsforPartitionListIt != AllElementsforPartitionList.end();AllElementsforPartitionListIt++){
		CF =(*AllElementsforPartitionListIt);
		anz++;
		if(anz==i)
		{
			elem=CF->name;
			break;
		}
	}
	return elem;
}
