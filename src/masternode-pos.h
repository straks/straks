

// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef MASTERNODE_POS_H
#define MASTERNODE_POS_H

//#include "bignum.h"
#include "sync.h"
#include "net.h"
#include "key.h"
//#include "core.h"
#include "util.h"
#include "script/script.h"
#include "base58.h"
#include "validation.h"
#include "net_processing.h"

using namespace std;
using namespace boost;

class CMasternodeMessagePOS;
class CMasternodeScanning;
class CMasternodeScanningError;


extern map<uint256, CMasternodeScanningError> mapMasternodeScanningErrors;
extern CMasternodeScanning mnscan;
extern CMasternodeMessagePOS mnMessagePos;

static const int MIN_MASTERNODE_POS_PROTO_VERSION = 70100;

/*
	1% of the network is scanned every 2.5 minutes, making a full
	round of scanning take about 4.16 hours. We're targeting about
	a day of proof-of-service errors for complete removal from the
	masternode system.
*/
static const int MASTERNODE_SCANNING_ERROR_THESHOLD = 6;  //6 Straksdev  to little Masternode for test

#define SCANNING_SUCCESS                       1
#define SCANNING_ERROR_NO_RESPONSE             2
#define SCANNING_ERROR_IX_NO_RESPONSE          3
#define SCANNING_ERROR_MAX                     3

//extern void ProcessMessageMasternodePOS(CNode* pfrom, const string& strCommand, CDataStream& vRecv, CConnman& connman);//

void ProcessMessageMasternodePOS(CNode* pfrom, const string& strCommand, CDataStream& vRecv, CConnman& connman);

class CMasternodeScanning
{
public:
    void DoMasternodePOSChecks();
    void CleanMasternodeScanningErrors();
};

// Returns how many masternodes are allowed to scan each block
int GetCountScanningPerBlock();

class CMasternodeScanningError
{
public:
    CTxIn vinMasternodeA;
    CTxIn vinMasternodeB;
    int nErrorType;
    int nExpiration;
    int nBlockHeight;
    std::vector<unsigned char> vchMasterNodeSignature;

    CMasternodeScanningError ()
    {
        vinMasternodeA = CTxIn();
        vinMasternodeB = CTxIn();
        nErrorType = 0;
        nExpiration = GetTime()+(60*60);
        nBlockHeight = 0;
    }

    CMasternodeScanningError (CTxIn& vinMasternodeAIn, CTxIn& vinMasternodeBIn, int nErrorTypeIn, int nBlockHeightIn)
    {
    	vinMasternodeA = vinMasternodeAIn;
    	vinMasternodeB = vinMasternodeBIn;
    	nErrorType = nErrorTypeIn;
    	nExpiration = GetTime()+(60*60);
    	nBlockHeight = nBlockHeightIn;
    }

    CMasternodeScanningError (CTxIn& vinMasternodeBIn, int nErrorTypeIn, int nBlockHeightIn)
    {
        //just used for IX, MasternodeA is any client
        vinMasternodeA = CTxIn();
        vinMasternodeB = vinMasternodeBIn;
        nErrorType = nErrorTypeIn;
        nExpiration = GetTime()+(60*60);
        nBlockHeight = nBlockHeightIn;
    }

    uint256 GetHash() const {return SerializeHash(*this);}

    bool SignatureValid();
    bool Sign();
    bool IsExpired() {return GetTime() > nExpiration;}
    void Relay(CNode* pnode, CConnman& connman);//
	void RelayProcessBlock();
    bool IsValid() {
    	return (nErrorType > 0 && nErrorType <= SCANNING_ERROR_MAX);
    }

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(vinMasternodeA);
        READWRITE(vinMasternodeB);
        READWRITE(nErrorType);
        READWRITE(nExpiration);
        READWRITE(nBlockHeight);
        READWRITE(vchMasterNodeSignature);
    }
};


#endif
