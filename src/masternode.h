// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef MASTERNODE_H
#define MASTERNODE_H
#include "utilstrencodings.h"//todo
#include "arith_uint256.h"//todo
#include "netmessagemaker.h"//todo
#include "sync.h"
#include "net.h"
#include "key.h"
//#include "core.h"
#include "util.h"
#include "script/script.h"
#include "base58.h"
#include "validation.h"
#include "net_processing.h"
#include "masternode-pos.h"

#define MASTERNODE_NOT_PROCESSED               0 // initial state
#define MASTERNODE_IS_CAPABLE                  1
#define MASTERNODE_NOT_CAPABLE                 2
#define MASTERNODE_STOPPED                     3
#define MASTERNODE_INPUT_TOO_NEW               4
#define MASTERNODE_PORT_NOT_OPEN               6
#define MASTERNODE_PORT_OPEN                   7
#define MASTERNODE_SYNC_IN_PROCESS             8
#define MASTERNODE_REMOTELY_ENABLED            9

#define MASTERNODE_MIN_CONFIRMATIONS           15
#define MASTERNODE_MIN_DSEEP_SECONDS           (30*60)
#define MASTERNODE_MIN_DSEE_SECONDS            (10*60)
#define MASTERNODE_PING_SECONDS                (5*60)
#define MASTERNODE_EXPIRATION_SECONDS          (65*60)
#define MASTERNODE_REMOVAL_SECONDS             (70*60)

using namespace std;

class CMasternode;
class CMasternodePayments;
class CMasternodePaymentWinner;
class CMasternodePaymentsMessage;

class CMasternodeScanning;

extern CMasternodePayments masternodePayments;
extern CMasternodePaymentsMessage mnPaymentMessage;
extern map<uint256, CMasternodePaymentWinner> mapSeenMasternodeVotes;
extern map<int64_t, uint256> mapCacheBlockHashes;

//void ProcessMessageMasternodePayments(CNode* pfrom, const string& strCommand, CDataStream& vRecv, CConnman& connman);
bool GetBlockHash(uint256& hash, int nBlockHeight);

void ProcessMessageMasternodePayments(CNode* pfrom, const string& strCommand, CDataStream& vRecv, CConnman& connman);

//
// The Masternode Class. For managing the Darksend process. It contains the input of the 1000DRK, signature to prove
// it's the one who own that ip address and code for calculating the payment election.
//
class CMasternode
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:
    enum state {
        MASTERNODE_ENABLED = 1,
        MASTERNODE_EXPIRED = 2,
        MASTERNODE_VIN_SPENT = 3,
        MASTERNODE_REMOVE = 4,
        MASTERNODE_POS_ERROR = 5
    };

    CTxIn vin;
    CService addr;
    CPubKey pubkey;
    CPubKey pubkey2;
    std::vector<unsigned char> sig;
    int activeState;
    int64_t sigTime; //dsee message times
    int64_t lastDseep;
    int64_t lastTimeSeen;
    int cacheInputAge;
    int cacheInputAgeBlock;
    bool unitTest;
    bool allowFreeTx;
    int protocolVersion;
    int64_t nLastDsq; //the dsq count from the last dsq broadcast of this node
    CScript donationAddress;
    int donationPercentage;
    int nVote;
    int64_t lastVote;
    int nScanningErrorCount;
    int nLastScanningErrorBlockHeight;

    CMasternode();
    CMasternode(const CMasternode& other);
    CMasternode(CService newAddr, CTxIn newVin, CPubKey newPubkey, std::vector<unsigned char> newSig, int64_t newSigTime, CPubKey newPubkey2, int protocolVersionIn, CScript donationAddress, int donationPercentage);

    void swap(CMasternode& first, CMasternode& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two classes,
        // the two classes are effectively swapped
        swap(first.vin, second.vin);
        swap(first.addr, second.addr);
        swap(first.pubkey, second.pubkey);
        swap(first.pubkey2, second.pubkey2);
        swap(first.sig, second.sig);
        swap(first.activeState, second.activeState);
        swap(first.sigTime, second.sigTime);
        swap(first.lastDseep, second.lastDseep);
        swap(first.lastTimeSeen, second.lastTimeSeen);
        swap(first.cacheInputAge, second.cacheInputAge);
        swap(first.cacheInputAgeBlock, second.cacheInputAgeBlock);
        swap(first.unitTest, second.unitTest);
        swap(first.allowFreeTx, second.allowFreeTx);
        swap(first.protocolVersion, second.protocolVersion);
        swap(first.nLastDsq, second.nLastDsq);
        swap(first.donationAddress, second.donationAddress);
        swap(first.donationPercentage, second.donationPercentage);
        swap(first.nVote, second.nVote);
        swap(first.lastVote, second.lastVote);
        swap(first.nScanningErrorCount, second.nScanningErrorCount);
        swap(first.nLastScanningErrorBlockHeight, second.nLastScanningErrorBlockHeight);
    }

    CMasternode& operator=(CMasternode from)
    {
        swap(*this, from);
        return *this;
    }
    friend bool operator==(const CMasternode& a, const CMasternode& b)
    {
        return a.vin == b.vin;
    }
    friend bool operator!=(const CMasternode& a, const CMasternode& b)
    {
        return !(a.vin == b.vin);
    }

    uint256 CalculateScore(int mod=1, int64_t nBlockHeight=0);

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        // serialized format:
        // * version byte (currently 0)
        // * all fields (?)
        {
            LOCK(cs);
            unsigned char nVersion = 0;
            READWRITE(nVersion);
            READWRITE(vin);
            READWRITE(addr);
            READWRITE(pubkey);
            READWRITE(pubkey2);
            READWRITE(sig);
            READWRITE(activeState);
            READWRITE(sigTime);
            READWRITE(lastDseep);
            READWRITE(lastTimeSeen);
            READWRITE(cacheInputAge);
            READWRITE(cacheInputAgeBlock);
            READWRITE(unitTest);
            READWRITE(allowFreeTx);
            READWRITE(protocolVersion);
            READWRITE(nLastDsq);
            READWRITE(*(CScriptBase*)(&donationAddress));
            READWRITE(donationPercentage);
            READWRITE(nVote);
            READWRITE(lastVote);
            READWRITE(nScanningErrorCount);
            READWRITE(nLastScanningErrorBlockHeight);
        }
    }


    void UpdateLastSeen(int64_t override=0)
    {
        if(override == 0) {
            lastTimeSeen = GetAdjustedTime();
        } else {
            lastTimeSeen = override;
        }
    }

    inline uint64_t SliceHash(uint256& hash, int slice)
    {
        uint64_t n = 0;
        memcpy(&n, &hash+slice*64, 64);
        return n;
    }

    void Check();

    bool UpdatedWithin(int seconds)
    {
        // LogPrintf("UpdatedWithin %d, %d --  %d \n", GetAdjustedTime() , lastTimeSeen, (GetAdjustedTime() - lastTimeSeen) < seconds);

        return (GetAdjustedTime() - lastTimeSeen) < seconds;
    }

    void Disable()
    {
        lastTimeSeen = 0;
    }

    bool IsEnabled()
    {
        return activeState == MASTERNODE_ENABLED;
    }

    int GetMasternodeInputAge()
    {
        if(chainActive.Tip() == NULL) return 0;

        if(cacheInputAge == 0) {
            cacheInputAge = GetInputAge(vin);
            cacheInputAgeBlock = chainActive.Tip()->nHeight;
        }

        return cacheInputAge+(chainActive.Tip()->nHeight-cacheInputAgeBlock);
    }

    void ApplyScanningError(CMasternodeScanningError& mnse)
    {
        if(!mnse.IsValid()) return;

        if(mnse.nBlockHeight == nLastScanningErrorBlockHeight) return;
        nLastScanningErrorBlockHeight = mnse.nBlockHeight;

        if(mnse.nErrorType == SCANNING_SUCCESS) {
            nScanningErrorCount--;
            if(nScanningErrorCount < 0) nScanningErrorCount = 0;
        } else { //all other codes are equally as bad
            nScanningErrorCount++;

            if(nScanningErrorCount >= 4)
            {
                nScanningErrorCount = 0;
                LogPrintf("S-Reset Bad Masternodescore \n");
            }

            if(nScanningErrorCount > MASTERNODE_SCANNING_ERROR_THESHOLD*2) nScanningErrorCount = MASTERNODE_SCANNING_ERROR_THESHOLD*2;
        }
    }

    std::string Status() {
        std::string strStatus = "ACTIVE";

        if(activeState == CMasternode::MASTERNODE_ENABLED) strStatus   = "ENABLED";
        if(activeState == CMasternode::MASTERNODE_EXPIRED) strStatus   = "EXPIRED";
        if(activeState == CMasternode::MASTERNODE_VIN_SPENT) strStatus = "VIN_SPENT";
        if(activeState == CMasternode::MASTERNODE_REMOVE) strStatus    = "REMOVE";
        if(activeState == CMasternode::MASTERNODE_POS_ERROR) strStatus = "POS_ERROR";

        return strStatus;
    }

};

// for storing the winning payments



class CMasternodePaymentWinner
{
public:
    int nBlockHeight;
    CTxIn vin;
    CScript payee;
    std::vector<unsigned char> vchSig;
    uint64_t score;


    CMasternodePaymentWinner() {
        nBlockHeight = 0;
        score = 0;
        vin = CTxIn();
        payee = CScript();
    }

    uint256 GetHash()
    {
        arith_uint256 n2, n3;

        n2 = UintToArith256(lyra2re2_hash(BEGIN(nBlockHeight), END(nBlockHeight)));
        n3 = UintToArith256(vin.prevout.hash) > n2 ? UintToArith256(vin.prevout.hash) - n2 : n2 - UintToArith256(vin.prevout.hash);

        return ArithToUint256(n3);
    }

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(nBlockHeight);
        READWRITE(*(CScriptBase*)(&payee));//--todo test
        READWRITE(vin);
        READWRITE(score);
        READWRITE(vchSig);
    }
};

//
// Masternode Payments Class
// Keeps track of who should get paid for which blocks
//

class CMasternodePayments
{
private:
    std::vector<CMasternodePaymentWinner> vWinning;
    int nSyncedFromPeer;
    std::string strMasterPrivKey;
    std::string strTestPubKey;
    std::string strMainPubKey;
    bool enabled;
    int nLastBlockHeight;

public:

    CMasternodePayments() {
        strMainPubKey = "03a0d8182aa1594353051219402c79598109d80ec3ae0dd4fb7df46e8d962c7439"; 
        strTestPubKey = "02f17b0c3e27065fcc400e5781d2995a8c4d9054d6aebd66149e3009c0c3f72242"; 
        enabled = false;
    }

    bool SetPrivKey(std::string strPrivKey);
    bool CheckSignature(CMasternodePaymentWinner& winner);
    bool Sign(CMasternodePaymentWinner& winner);

    // Deterministically calculate a given "score" for a masternode depending on how close it's hash is
    // to the blockHeight. The further away they are the better, the furthest will win the election
    // and get paid this block
    //

    uint64_t CalculateScore(uint256 blockHash, CTxIn& vin);
    bool GetWinningMasternode(int nBlockHeight, CTxIn& vinOut);
    bool AddWinningMasternode(CMasternodePaymentWinner& winner);
    bool ProcessBlock(int nBlockHeight);
    void Relay(CMasternodePaymentWinner& winner);
    void Sync(CNode* node, CConnman& connman);
    void CleanPaymentList();
    int LastPayment(CMasternode& mn);
    bool IsEnabled() { return enabled; };

    //slow
    bool GetBlockPayee(int nBlockHeight, CScript& payee);
};


#endif
