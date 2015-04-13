// SP_Signer.h: interface for the SP_Signer class.
//
//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/02/19 $
// Short Description: signing (MD5 hash) and verifying *.pn files (graphs)
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_SIGNER_H__)
#define __SP_SIGNER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/string.h>
#include <wx/xml/xml.h>

typedef unsigned char byte;
typedef unsigned int word;

using namespace std;


class SP_Signer {
//TODO:? improve error handling

	class ParseErrorHandler;

	public:

		// The constructor accepts a file name and, if existing, a DOM of that file
		// If the DOM is not given it is constructed automaticly
		//////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readXMLFile()
		/////////////////////////////////////////////////////////////////////////
		SP_Signer(const wxString& FileName);
		virtual ~SP_Signer();

		// Description: Get the name of the current object file
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		wxString			getSPFileName();

		// Description: Set a new object file to work on
		//					The DOM will be reconstructed and member variables will be reset
		// param:		wxString
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readXMLFile()
		/////////////////////////////////////////////////////////////////////////
		void				setSPFileName(const wxString& FileName);

		// Description: Get the signature of the object file (graph)
		//					(if not existing it is created)
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		wxString			getSignatureOfFile();

		// Description: Get the signature of the graphs structure (w/out layout)
		//					(if not existing it is created)
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		wxString			getSignatureOfStructure();

		// Description: Retrieve the file signature of the corresponding signature file
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readSignatureFile()
		/////////////////////////////////////////////////////////////////////////
		wxString			getSignatureOfFileInSgnFile();

		// Description: Retrieve the structure signature of the corresponding signature file
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readSignatureFile()
		/////////////////////////////////////////////////////////////////////////
		wxString			getSignatureOfStructureInSgnFile();

		// Description: Retrieve the time stamp of the corresponding signature file
		// return:	wxString
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readSignatureFile()
		/////////////////////////////////////////////////////////////////////////
		wxString			getTimeStampInSgnFile();

		// Description: Update/Write (if not existing) the signature file
		//					It stores: the object file name, both signatures and a time stamp
		//					(if the signatures don't exist yet they are created)
		/////////////////////////////////////////////////////////////////////////
		// throws (char*) exception in case of any errors
		/////////////////////////////////////////////////////////////////////////
		void				updateSignatureFile();

		// Description: Compares the file signatures of the current object file and
		//					the corresponding signature file
		//					(if the signature of the object file doesn't exist it is created)
		//					(->	<object file name>.sgn file in directory of object file)
		// return:	bool - true if they are equal, false otherwise
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readSignatureFile()
		/////////////////////////////////////////////////////////////////////////
		bool				verifyFile();

		// Description: Compares the structure signatures of the current object file and
		//					the corresponding signature file
		//					(if the signature of the object file doesn't exist it is created)
		// return:	bool - true if they are equal, false otherwise
		/////////////////////////////////////////////////////////////////////////
		// propagates exceptions from readSignatureFile()
		/////////////////////////////////////////////////////////////////////////
		bool				verifyStructure();


	private:
		// DOM representation of the graph
		wxXmlDocument	SPFile;
		// Name of the current object file (which holds the graph)
		wxString			SPFileName;
		// Signature (MD5 hash) of the object file, the graph actually
		wxString			SignatureOfFile;
		// Signature (MD5 hash) of the structure of the graph
		wxString			SignatureOfStructure;
		// From the *.sgn file loaded time stamp
		wxString			ldTimeStamp;
		// From the *.sgn file loaded file signature
		wxString			ldSignatureOfFile;
		// From the *.sgn file loaded structure signature
		wxString			ldSignatureOfStructure;


		// Description: Actually read the object file (*.xml) containing the graph
		//					(-> SPFile, the DOM representation of the graph)
		/////////////////////////////////////////////////////////////////////////
		// throws (char*) exceptions in case of parsing errors
		/////////////////////////////////////////////////////////////////////////
		void				readXMLFile();

		// Description: Hash the graph
		//					(->SignatureOfFile)
		/////////////////////////////////////////////////////////////////////////
		void				createSignatureOfFile();

		// Description: Hash the structure of the graph
		//					(->SignatureOfStructure)
		/////////////////////////////////////////////////////////////////////////
		void				createSignatureOfStructure();

		// Description: Read the *.sgn file
		//					(->ldSignatureOfFile, ldSignatureOfStructure, ldTimeStamp)
		/////////////////////////////////////////////////////////////////////////
		// throws (char*) exception in case of any errors
		/////////////////////////////////////////////////////////////////////////
		void				readSignatureFile();

		// Description: Remove <graphic> nodes from XML
		//
		/////////////////////////////////////////////////////////////////////////
		void				filterGraphicNodes(wxXmlNode* p_pcNode);

		// Description: Serialize the DOM into a wxString (e.g. for hashing)
		//					optionally filtering the layout to retrieve the structure
		// param:		bool - true for filtering layout, false otherwise
		// return:	wxString - the (filtered) serialized DOM
		/////////////////////////////////////////////////////////////////////////
		wxString			serialize(bool filterGraphics);

		// Description: Extract the file name from a given adress
		//					(works for both: slash and backslash)
		// param:		wxString - the adress of the file
		// return:	wxString - the extracted file name
		/////////////////////////////////////////////////////////////////////////
		wxString			getFileNameFromPath(const wxString& path);


		// Description: Reset the signatures (called if a new object file is set)
		/////////////////////////////////////////////////////////////////////////
		inline void		initSignatures() {
			SignatureOfFile.erase();
			SignatureOfStructure.erase();
		}

		// Description: Reset the loaded params (...)
		/////////////////////////////////////////////////////////////////////////
		inline void		initLoadedParams() {
			ldSignatureOfFile.erase();
			ldSignatureOfStructure.erase();
			ldTimeStamp.erase();
		}



	// Description: This subclass does the MD5 hashing
	/////////////////////////////////////////////////////////////////////////
	class MD5Hasher {

		public:

			// The constructor accepts either strings or file names
			// If the specified file cannot be opened the data will be an empty string
			//////////////////////////////////////////////////////////////////////////
			MD5Hasher(const wxString& str, bool isFile = false);
			virtual ~MD5Hasher();

			// Description: actually do the hashing
			// return:	wxString	- the digest
			/////////////////////////////////////////////////////////////////////////
			wxString	hash();

			// Description: set new data
			// param:		wxString	- data string or file name
			// param:		bool		- indicates wether param 1 is string or file name
			void		setData(const wxString& str, bool isFile = false);

			// Description: test for presence of data
			// return:	bool		- true if there is data, false otherwise
			bool		dataEmpty();

			// Description: get digest of current data (if not existing hash() is called)
			// return:	wxString	- the digest
			wxString	getDigest();


		private:

			// The data which is to be hashed
			wxString data;
			// The resulting digest of the hashed data
			wxString digest;
			// Array for storing intermediate results during hashing
			word state[4];
			// Array for storing the data block which is currently processed
			word buffer[16];

	}; //endof MD5Hasher


}; //endof SP_Signer

#endif // !defined(__SP_SIGNER_H__)








