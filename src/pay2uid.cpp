#include "pay2uid.h"
#include "net.h"
#include "netmessagemaker.h"
#include "clientversion.h"
#include "primitives/transaction.h"


#include <boost/filesystem.hpp>


CCriticalSection cs_process_usermessage;

CUserManager usermanager;
CUser users;

CUserDB::CUserDB()
{
    uidPath = GetDataDir() / "uid.dat";
    strMagicMessage = "UserID";
}

bool CUserDB::Write_uid(const CUserManager& userFileToWrite)
{
    int64_t nStart = GetTimeMillis();

    // serialize, checksum data up to that point, then append checksum
    CDataStream ssUsers(SER_DISK, CLIENT_VERSION);
    ssUsers << strMagicMessage; 
    ssUsers << userFileToWrite;
    uint256 hash = Hash(ssUsers.begin(), ssUsers.end());
    ssUsers << hash;

    // open output file, and associate with CAutoFile
    FILE *file = fopen(uidPath.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, uidPath.string());

    // Write and commit header, data
    try {
        fileout << ssUsers;
    }
    catch (std::exception &e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    //FileCommit(fileout);
    fileout.fclose();

    LogPrintf("Written uids to   %dms\n", GetTimeMillis() - nStart);

    return true;
}

CUserDB::uidFileReadResult CUserDB::Read_uid(CUserManager& userFileToLoad)
{
    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE *file = fopen(uidPath.string().c_str(), "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull())
    {
        error("%s : Failed to open file %s", __func__, uidPath.string());
        return FileError;
    }

    
    int fileSize = boost::filesystem::file_size(uidPath);
    int dataSize = fileSize - sizeof(uint256);

    if (dataSize < 0)
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char *)&vchData[0], dataSize);
        filein >> hashIn;
    }
    catch (std::exception &e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    CDataStream ssUsers(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssUsers.begin(), ssUsers.end());
    if (hashIn != hashTmp)
    {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }
	
    std::string strMagicMessageTmp;
    try {

        ssUsers >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp)
        {
            error("%s : Invalid uid magic message", __func__);
            return IncorrectMagicMessage;
        }

        ssUsers >> userFileToLoad;
    }
    catch (std::exception &e) {
        //userFileToLoad.Clear();
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }
    LogPrintf("Loaded users from uid.dat  %dms\n", GetTimeMillis() - nStart);

    return Ok;
}




CUser::CUser(){
	LOCK(cs);
	user_id = "";
	address = "";
}
CUser::CUser(const CUser& anotherUser)
{
	LOCK(cs);
	user_id = anotherUser.user_id;
	address = anotherUser.address;
}
CUser::CUser(std::string newuser_id, std::string newaddress){
	LOCK(cs);
	user_id = newuser_id;
	address = newaddress;
}
CUserManager::CUserManager()
{
	LOCK(cs);
	samplenum = 0;
}




bool CUserManager::AddUser(CUser &user)
{
	LOCK(cs);
	
	if(!IsUserAlreadyExists(user.user_id)){
		vUsers.push_back(user);
        return true;
	}
	
	return false;
}
bool CUserManager::IsUserAlreadyExists(const std::string user_id)
{
	LOCK(cs);
	
	CUser* puser = search_uid(user_id);
	
	if(puser != NULL){
		return true;
	}
	return false;
	
}

CUser *CUserManager::search_uid(const std::string &uid)
{
	LOCK(cs);
	LogPrintf("start search for user  %s, ", uid);
	BOOST_FOREACH(CUser& user, vUsers)
    {LogPrintf("vUsers size %s ", vUsers.size());
        if(user.user_id == uid){
			LogPrintf("user found %s ", uid);
            return &user;
		}
    }
    return NULL;
}

bool RegisterUsers(const std::string user_id, const std::string user_address)
{
	CUserDB userdb;
	CUserManager tempUsermanager;
	
	//Verify 
    CUserDB::uidFileReadResult readResult = userdb.Read_uid(tempUsermanager);
    // there was an error and it was not an error on file openning => do not proceed
    if (readResult == CUserDB::FileError){
        LogPrintf("Missing file uid.dat, will try to recreate\n");
	}
    else if (readResult != CUserDB::Ok)
    {
        LogPrintf("Error reading : ");
        if(readResult == CUserDB::IncorrectFormat)
            LogPrintf("magic is ok but data has invalid format, will try to recreate\n");
        else
        {
            LogPrintf("file format is unknown or invalid, please fix it manually\n");
            return false;
        }
    }
	CUser u(user_id, user_address);
    if(tempUsermanager.AddUser(u)){
		if(userdb.Write_uid(tempUsermanager))
			LogPrintf("User added succesfull , usedID = %s, address = %s", u.user_id, u.address);
	} else {
		return false;
	}
	return true;
    
}

void GetAddress(const std::string userID, std::string &outAddress){
	CUserDB userdb;
	CUserManager Tempusermanager;
	
	//Verify 
    CUserDB::uidFileReadResult readResult = userdb.Read_uid(Tempusermanager);
    // there was an error and it was not an error on file openning => do not proceed
    if (readResult == CUserDB::FileError){
        LogPrintf("Missing file uid.dat, will try to recreate\n");
	}
    else if (readResult != CUserDB::Ok)
    {
        LogPrintf("Error reading : ");
        if(readResult == CUserDB::IncorrectFormat)
            LogPrintf("magic is ok but data has invalid format, will try to recreate\n");
        else
        {
            LogPrintf("file format is unknown or invalid, please fix it manually\n");
            return;
        }
    }
	CUser* puser = Tempusermanager.search_uid(userID);
	if(puser != NULL)
		outAddress = puser->address;
	else 
		LogPrintf("failed to out address\n");
}

void CUserManager::ProcessUserMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman)
{
	LOCK(cs_process_usermessage);
	
	if (strCommand == "uid"){
		std::string user_id_temp;
		std::string address_temp;
		
		vRecv >> user_id_temp >> address_temp;
		
		if(!RegisterUsers(user_id_temp, address_temp)){
			LogPrintf("RegisterUsers: failed to register");
		}
		else{
			RelayUserMessage(user_id_temp, address_temp);
		}
	}
}

void CUserManager::RelayUserMessage(const std::string userID, const std::string address)
{
	g_connman->ForEachNode([&userID, &address](CNode* pnode)
    {
		if(pnode->nVersion >= 70083){
        g_connman->PushMessage(pnode, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "uid", userID, address));}
    });
}