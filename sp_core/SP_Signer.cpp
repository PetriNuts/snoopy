// SP_Signer.cpp: implementation of the SP_Signer, the GraphicsFilter
// and the MD5Hasher class.
//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/02/19 $
//////////////////////////////////////////////////////////////////////
#include "sp_core/SP_Signer.h"
#include "sp_defines.h"

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <time.h>

#include "sp_core/MD5HasherDefs.h"


//////////////////////////////////////////////////////////////////////
//SP_Signer construction/destruction
//////////////////////////////////////////////////////////////////////

SP_Signer::SP_Signer(const wxString& FileName) :
SPFileName(FileName)
{
	initSignatures();
	initLoadedParams();
	readXMLFile();
}

SP_Signer::~SP_Signer()
{
}


//////////////////////////////////////////////////////////////////////
//SP_Signer member functions
//////////////////////////////////////////////////////////////////////

wxString SP_Signer::getSPFileName()
{
	return SPFileName;
}


void SP_Signer::setSPFileName(const wxString& FileName)
{
	SPFileName = FileName;
	initSignatures();
	initLoadedParams();
	readXMLFile();
}


void SP_Signer::readXMLFile()
{
	//parsing the file
	SPFile.Load(SPFileName, wxT("UTF-8"), wxXMLDOC_NONE);
}


wxString SP_Signer::getFileNameFromPath(const wxString& path)
{
        wxFileName fn = wxFileName(path);
        return fn.GetFullName();
}

void SP_Signer::updateSignatureFile()
{
	//if either one ot the signatures doesn't exist yet, it is created
	if (SignatureOfFile.empty()) createSignatureOfFile();
	if (SignatureOfStructure.empty()) createSignatureOfStructure();

	//if there was a problem, creating the signatures, the output file is not written
	if (SignatureOfFile.empty() || SignatureOfStructure.empty()) throw wxT("Error while creating signature.");

	//setting binary output file name and opening it
	wxString SgnFileName(SPFileName + wxT(".sgn"));

	wxFFileOutputStream l_cOutput(SgnFileName);
	wxTextOutputStream SgnFile(l_cOutput);
	if (!l_cOutput.IsOk()) throw wxT("Error while opening file for writing the signature.");

	//getting the system time
	time_t systime;
	time(&systime);
	wxString TimeStamp(asctime(localtime(&systime)), wxConvUTF8);
	//asctime() puts a '\n' at the end of the time string which is deleted here:
	TimeStamp.erase(24);

	//writing to *.sgn file
	SgnFile << getFileNameFromPath(SPFileName) << endl;
	SgnFile << TimeStamp << endl;
	SgnFile << SignatureOfFile << endl;
	SgnFile << SignatureOfStructure;
	l_cOutput.Close();
}


void SP_Signer::readSignatureFile()
{
	//setting input file name and opening it
	wxString SgnFileName(SPFileName + wxT(".sgn"));

	wxFFileInputStream SgnFile(SgnFileName);
	wxTextInputStream l_cInput(SgnFile);
	if (!SgnFile.IsOk()) throw wxT("Error while opening signature file.");

	//reading binary input file and assigning signatures to class variables
	wxString ldSPFileName, tmpTS, tmpSOF, tmpSOS;
	ldSPFileName = l_cInput.ReadLine();
	tmpTS = l_cInput.ReadLine();
	ldTimeStamp = tmpTS;
	tmpSOF = l_cInput.ReadLine();
	ldSignatureOfFile = tmpSOF;
	tmpSOS = l_cInput.ReadLine();
	ldSignatureOfStructure = tmpSOS;

	//check loaded params: right file name and 32-char-long signatures
	//because there are exactly 32 chars read, it's enough to check the signature read last
	if ( ldSPFileName != getFileNameFromPath(SPFileName) || ldSignatureOfStructure.length() != 32) {
		initLoadedParams();
		throw wxT("The signature file seems to be corrupted.");
	}
}


wxString SP_Signer::getSignatureOfFile()
{
	if (SignatureOfFile.empty()) createSignatureOfFile();
	return SignatureOfFile;
}


wxString SP_Signer::getSignatureOfStructure()
{
	if (SignatureOfStructure.empty()) createSignatureOfStructure();
	return SignatureOfStructure;
}


wxString SP_Signer::getSignatureOfFileInSgnFile()
{

	if (ldSignatureOfFile.empty()) {
		try {
			readSignatureFile();
		} catch(...) {
			throw;
		}
	}
	return ldSignatureOfFile;
}


wxString SP_Signer::getSignatureOfStructureInSgnFile()
{
	if (ldSignatureOfStructure.empty()) {
		try {
			readSignatureFile();
		} catch(...) {
			throw;
		}
	}
	return ldSignatureOfStructure;
}


wxString SP_Signer::getTimeStampInSgnFile()
{
	if (ldTimeStamp.empty()) {
		try {
			readSignatureFile();
		} catch(...) {
			throw;
		}
	}
	return ldTimeStamp;
}


bool SP_Signer::verifyFile()
{
	try {
		getSignatureOfFileInSgnFile();
	} catch(...) {
		throw;
	}
	//compare DOM signature with loaded signature
	if (getSignatureOfFile() == ldSignatureOfFile) return true;
	else return false;
}


bool SP_Signer::verifyStructure()
{
	try {
		getSignatureOfStructureInSgnFile();
	} catch(...) {
		throw;
	}
	//compare DOM signature with loaded signature
	if (getSignatureOfStructure() == ldSignatureOfStructure) return true;
	else return false;
}

void SP_Signer::createSignatureOfFile()
{
	//serializing with layout filter turned off
	MD5Hasher h(serialize(false));
	SignatureOfFile = h.hash();
}


void SP_Signer::createSignatureOfStructure()
{
	//serializing with layout filter turned on
	MD5Hasher h(serialize(true));
	SignatureOfStructure = h.hash();
}


wxString SP_Signer::serialize(bool filterGraphics)
{
	wxStringOutputStream l_StringOutStream;
	if (filterGraphics)
	{
		filterGraphicNodes(SPFile.GetRoot());
	}

	SPFile.Save(l_StringOutStream);
	wxString tmpString(l_StringOutStream.GetString());

	return tmpString;
}

void SP_Signer::filterGraphicNodes(wxXmlNode* p_pcNode)
{
	wxXmlNode* l_pcChild = p_pcNode->GetChildren();

	while(l_pcChild)
	{
		if(l_pcChild->GetName() == wxT("graphics"))
		{
			wxXmlNode* l_pcDelete = l_pcChild;
			l_pcChild = l_pcChild->GetNext();
			p_pcNode->RemoveChild(l_pcDelete);
			wxDELETE(l_pcDelete);
		}
		else
		{
			filterGraphicNodes(l_pcChild);
			l_pcChild = l_pcChild->GetNext();
		}
	}
}


//////////////////////////////////////////////////////////////////////
//MD5Hasher: construction/destruction
//////////////////////////////////////////////////////////////////////

SP_Signer::MD5Hasher::MD5Hasher(const wxString& str, bool isFile) {

	setData(str, isFile);

}

SP_Signer::MD5Hasher::~MD5Hasher() {}


//////////////////////////////////////////////////////////////////////
// MD5Hasher: member functions
//////////////////////////////////////////////////////////////////////

void SP_Signer::MD5Hasher::setData(const wxString& str, bool isFile) {

	// Reading and storing the data from the specified file
	// NOTE:	This only works for text files because in other file formats
	//				it's likely to have a NULL-character somewhere in the file
	//				content which would terminate the data string before the
	//				actual end.
	//				But for our purpose it will do.
	if (isFile) {
		wxFFile file(str);
		if (!file.IsOpened())
		{
			data = wxT("");
		} else {
			//determining the file size
			word fileSize = file.Seek(0,wxFromEnd);
			file.Seek(0, wxFromStart);

			//char array to hold the file content
			wxChar* tmp_data = new wxChar[fileSize+1];

			//reading the file into tmp_data and assigning it to the class variable data
			file.Read(tmp_data, fileSize);

			tmp_data[fileSize] = '\0';
			data = tmp_data;

			wxDELETEA(tmp_data);
		}
	// Storing the given string
	} else data = str;

	digest.erase();

}


bool SP_Signer::MD5Hasher::dataEmpty() {

	return data.empty();

}


wxString SP_Signer::MD5Hasher::hash() {

	bool finishedReading = false;
	bool finishedAll = false;
	unsigned long index = 0;			// index for reading data
	unsigned int bytesRead = 0;		// number of bytes read

	// initializing with constants
	state[0] = MD5_INIT1;
	state[1] = MD5_INIT2;
	state[2] = MD5_INIT3;
	state[3] = MD5_INIT4;
	// used for doing the transformations during the hash process
	word A, B, C, D;

	wxString tmpS;
	while (!finishedAll) {
		if (!finishedReading) {
			//reading data of 64-byte (16-word) -blocks into buffer
		  	// bytesRead = data.copy((char*)buffer, 64, index);

	        tmpS = data.Mid(index, 64);
			bytesRead = tmpS.length();
			memcpy((char*) buffer, tmpS.ToAscii(), bytesRead);

			//if the end of the data has been reached, the data has to be padded...
			if (bytesRead < 64) {
				//...which means there is a '1' added to the end and the rest is filled with '0'
				memcpy((byte*)buffer + bytesRead, padding, 64 - bytesRead);
				finishedReading = true;
			}
		}	else {
			//we get here if the last data block was more than 55 bytes long
			//that means we need an additional 64 byte block...
			//... in order to be able to store the data size properly which takes 8 bytes
			bytesRead = 0;
			//since there is no more data the buffer is just filled with '0'
			memcpy((byte*)buffer, padding + 1, 56);
		}

		//storing the data size in the last 8 bytes of the final block (lowest first)
		if (bytesRead < 56) {
			buffer[14] = data.size() << 3;
			buffer[15] = data.size() >> 29;

			finishedAll = true;
		}

		// setting index to the next block of data
		index +=64;

		// setting transformation variables to current state
		A = state[0];
		B = state[1];
		C = state[2];
		D = state[3];

		//doing the transformations...
		//... round 1
		OP(F, A, B, C, D, buffer[0], MD5_S11, MD5_T01);
		OP(F, D, A, B, C, buffer[1], MD5_S12, MD5_T02);
		OP(F, C, D, A, B, buffer[2], MD5_S13, MD5_T03);
		OP(F, B, C, D, A, buffer[3], MD5_S14, MD5_T04);

		OP(F, A, B, C, D, buffer[4], MD5_S11, MD5_T05);
		OP(F, D, A, B, C, buffer[5], MD5_S12, MD5_T06);
		OP(F, C, D, A, B, buffer[6], MD5_S13, MD5_T07);
		OP(F, B, C, D, A, buffer[7], MD5_S14, MD5_T08);

		OP(F, A, B, C, D, buffer[8], MD5_S11, MD5_T09);
		OP(F, D, A, B, C, buffer[9], MD5_S12, MD5_T10);
		OP(F, C, D, A, B, buffer[10], MD5_S13, MD5_T11);
		OP(F, B, C, D, A, buffer[11], MD5_S14, MD5_T12);

		OP(F, A, B, C, D, buffer[12], MD5_S11, MD5_T13);
		OP(F, D, A, B, C, buffer[13], MD5_S12, MD5_T14);
		OP(F, C, D, A, B, buffer[14], MD5_S13, MD5_T15);
		OP(F, B, C, D, A, buffer[15], MD5_S14, MD5_T16);

		//... round 2
		OP(G, A, B, C, D, buffer[1], MD5_S21, MD5_T17);
		OP(G, D, A, B, C, buffer[6], MD5_S22, MD5_T18);
		OP(G, C, D, A, B, buffer[11], MD5_S23, MD5_T19);
		OP(G, B, C, D, A, buffer[0], MD5_S24, MD5_T20);

		OP(G, A, B, C, D, buffer[5], MD5_S21, MD5_T21);
		OP(G, D, A, B, C, buffer[10], MD5_S22, MD5_T22);
		OP(G, C, D, A, B, buffer[15], MD5_S23, MD5_T23);
		OP(G, B, C, D, A, buffer[4], MD5_S24, MD5_T24);

		OP(G, A, B, C, D, buffer[9], MD5_S21, MD5_T25);
		OP(G, D, A, B, C, buffer[14], MD5_S22, MD5_T26);
		OP(G, C, D, A, B, buffer[3], MD5_S23, MD5_T27);
		OP(G, B, C, D, A, buffer[8], MD5_S24, MD5_T28);

		OP(G, A, B, C, D, buffer[13], MD5_S21, MD5_T29);
		OP(G, D, A, B, C, buffer[2], MD5_S22, MD5_T30);
		OP(G, C, D, A, B, buffer[7], MD5_S23, MD5_T31);
		OP(G, B, C, D, A, buffer[12], MD5_S24, MD5_T32);

		//... round 3
		OP(H, A, B, C, D, buffer[5], MD5_S31, MD5_T33);
		OP(H, D, A, B, C, buffer[8], MD5_S32, MD5_T34);
		OP(H, C, D, A, B, buffer[11], MD5_S33, MD5_T35);
		OP(H, B, C, D, A, buffer[14], MD5_S34, MD5_T36);

		OP(H, A, B, C, D, buffer[1], MD5_S31, MD5_T37);
		OP(H, D, A, B, C, buffer[4], MD5_S32, MD5_T38);
		OP(H, C, D, A, B, buffer[7], MD5_S33, MD5_T39);
		OP(H, B, C, D, A, buffer[10], MD5_S34, MD5_T40);

		OP(H, A, B, C, D, buffer[13], MD5_S31, MD5_T41);
		OP(H, D, A, B, C, buffer[0], MD5_S32, MD5_T42);
		OP(H, C, D, A, B, buffer[3], MD5_S33, MD5_T43);
		OP(H, B, C, D, A, buffer[6], MD5_S34, MD5_T44);

		OP(H, A, B, C, D, buffer[9], MD5_S31, MD5_T45);
		OP(H, D, A, B, C, buffer[12], MD5_S32, MD5_T46);
		OP(H, C, D, A, B, buffer[15], MD5_S33, MD5_T47);
		OP(H, B, C, D, A, buffer[2], MD5_S34, MD5_T48);

		//... round 4
		OP(I, A, B, C, D, buffer[0], MD5_S41, MD5_T49);
		OP(I, D, A, B, C, buffer[7], MD5_S42, MD5_T50);
		OP(I, C, D, A, B, buffer[14], MD5_S43, MD5_T51);
		OP(I, B, C, D, A, buffer[5], MD5_S44, MD5_T52);

		OP(I, A, B, C, D, buffer[12], MD5_S41, MD5_T53);
		OP(I, D, A, B, C, buffer[3], MD5_S42, MD5_T54);
		OP(I, C, D, A, B, buffer[10], MD5_S43, MD5_T55);
		OP(I, B, C, D, A, buffer[1], MD5_S44, MD5_T56);

		OP(I, A, B, C, D, buffer[8], MD5_S41, MD5_T57);
		OP(I, D, A, B, C, buffer[15], MD5_S42, MD5_T58);
		OP(I, C, D, A, B, buffer[6], MD5_S43, MD5_T59);
		OP(I, B, C, D, A, buffer[13], MD5_S44, MD5_T60);

		OP(I, A, B, C, D, buffer[4], MD5_S41, MD5_T61);
		OP(I, D, A, B, C, buffer[11], MD5_S42, MD5_T62);
		OP(I, C, D, A, B, buffer[2], MD5_S43, MD5_T63);
		OP(I, B, C, D, A, buffer[9], MD5_S44, MD5_T64);

		// saving the new state after transformation
		state[0] += A;
		state[1] += B;
		state[2] += C;
		state[3] += D;
	}

	//reversing the result states
	state[0] = ((state[0] << 24) & 0xFF000000) | ((state[0] << 8) & 0x00FF0000)
					 | ((state[0] >> 24) & 0x000000FF) | ((state[0] >> 8) & 0x0000FF00);
	state[1] = ((state[1] << 24) & 0xFF000000) | ((state[1] << 8) & 0x00FF0000)
					 | ((state[1] >> 24) & 0x000000FF) | ((state[1] >> 8) & 0x0000FF00);
	state[2] = ((state[2] << 24) & 0xFF000000) | ((state[2] << 8) & 0x00FF0000)
					 | ((state[2] >> 24) & 0x000000FF) | ((state[2] >> 8) & 0x0000FF00);
	state[3] = ((state[3] << 24) & 0xFF000000) | ((state[3] << 8) & 0x00FF0000)
					 | ((state[3] >> 24) & 0x000000FF) | ((state[3] >> 8) & 0x0000FF00);

	//converting the result to a string
	//TODO: there's probably a nicer way for doing this
	wxChar dig[33] = {0};

	wxSprintf(dig,wxT("%08X%08X%08X%08X"), (unsigned int)state[0], (unsigned int)state[1], (unsigned int)state[2], (unsigned int)state[3]);

	return digest = dig;

}


wxString SP_Signer::MD5Hasher::getDigest() {

	if (!digest.empty()) return digest;
	else return hash();

}
