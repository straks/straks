#ifndef STRAKS_PAY2UID_H
#define STRAKS_PAY2UID_H


#include "sync.h"
#include "net.h"
#include "key.h"
#include "util.h"
#include "version.h"



using namespace std;

class CUserManager;
class CUser;

extern CUserManager usermanager;
extern CUser users;

class CUser
{
private :
	mutable CCriticalSection cs;
public :
	std::string user_id;
	std::string address;
	
	CUser();
	CUser(const CUser& anotherUser);
	CUser(std::string newuser_id, std::string newaddress);
	
	ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
	{
        {
            LOCK(cs);
                
            READWRITE(user_id);
            READWRITE(address);
        }
    }
	
	
};

class CUserManager
{
private:
	mutable CCriticalSection cs;
	
	std::vector<CUser> vUsers;
	
public:
    int samplenum;
	ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
	{
        {
            LOCK(cs);
				
            READWRITE(vUsers);
        }
    }
	CUserManager();
    CUserManager(CUserManager& other);
	
    bool AddUser(CUser &user);
	bool IsUserAlreadyExists(const std::string string);
	void GetAddress(const std::string userID, std::string &outAddress);
	
	CUser* search_uid(const std::string &uid);
	
	void ProcessUserMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman);
	
	void RelayUserMessage(const std::string userID, const std::string address);
	
};

class CUserDB
{
private:
    boost::filesystem::path uidPath;
    std::string strMagicMessage;
public:
    enum uidFileReadResult {
        Ok,
        FileError,
        HashReadError,
        IncorrectHash,
        IncorrectMagicMessage,
        IncorrectFormat
    };

    CUserDB();
    bool Write_uid(const CUserManager &userFileToWrite);
    uidFileReadResult Read_uid(CUserManager& userFileToLoad);
};

bool RegisterUsers(const std::string user_id, const std::string user_address);
void GetAddress(const std::string userID, std::string &outAddress);

#endif