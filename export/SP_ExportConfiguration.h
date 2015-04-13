//////////////////////////////////////////////////////////////////////
// $Author: Steffen Liese $
// $Version: 0.1 $
// $Date: 2009/01/25 $
// Short Description: export configuration 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_EXPORTCONFIGURATION__
#define __SP_EXPORTCONFIGURATION__

#include <wx/string.h>
#include <wx/object.h>
#include <wx/wx.h>
#include "sp_core/SP_List.h"

struct ConfigElement
{
	wxString name;
	bool checked;
	wxString value;
};
struct Partition
{
	wxString name;
	std::list<ConfigElement*> ConfigList;
	std::list<ConfigElement*>::iterator ConfigListIt;
};

  


class SP_ExportConfiguration
{

 private:
  
  //bool Preprocessing();
  

  std::list<ConfigElement*> ConfigList;
  std::list<ConfigElement*>::iterator ConfigListIt;
  std::list<Partition*> PartitionList;
  std::list<Partition*>::iterator PartitionListIt;
  std::list<ConfigElement*> AllElementsforPartitionList;
  std::list<ConfigElement*>::iterator AllElementsforPartitionListIt;

 public:
  SP_ExportConfiguration();
  virtual ~SP_ExportConfiguration();



	bool AddConfigurationElement(wxString Element_name,bool Element_checked,wxString Element_value);
    bool SetConfigurationElement(wxString Element_name,bool Element_checked,wxString Element_value);
	ConfigElement* GetConfigurationElement(wxString Element_name);


	bool AddtoAllusableElementsforPartition(wxString Element_name);
	bool DeletefromAllusableElementsforPartition(wxString Element_name);
	int GetfromAllusableElementsforPartition_Count();
	wxString GetItemAllusableElementsforPartition(int i);

	bool newPartition();
	bool deletePartition(int Partition_number);
	bool AddElementtoPartition(int Partition_number,wxString Element_name);
	bool DeleteElementfromPartition(int Partition_number,wxString Element_name);
	Partition* GetPartition(int Partition_number);
	int GetPartition_count();
	wxArrayString GetPartitions_aswxArrayString();
	wxArrayString GetALLusableElementsforPartition_aswxArrayString();
	wxArrayString GetPartition_aswxArrayString(int Partition_number);
	wxString GetPartition_aswxString(int Partition_number);

};

#endif // __SP_EXPORTCONFIGURATION__

