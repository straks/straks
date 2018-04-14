// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternode.h"
#include "masternodeman.h"
#include "signhelper_mn.h"//todo++
//#include "core.h"
#include "util.h"
#include "sync.h"
#include "addrman.h"
#include "net.h"
#include "net_processing.h"
#include "consensus/validation.h"
#include <boost/lexical_cast.hpp>

CCriticalSection cs_masternodepayments;


/** Object for who's going to get paid on which blocks */
CMasternodePayments masternodePayments;
// keep track of Masternode votes I've seen
map<uint256, CMasternodePaymentWinner> mapSeenMasternodeVotes;
// keep track of the scanning errors I've seen
map<uint256, int> mapSeenMasternodeScanningErrors;
// cache block hashes as we calculate them
std::map<int64_t, uint256> mapCacheBlockHashes;

static void RelayMNpayments(CMasternodePaymentWinner& winner, CNode* pnode, CConnman &connman)
{
    CInv inv(MSG_MASTERNODE_WINNER, winner.GetHash());

    vector<CInv> vInv;
    vInv.push_back(inv);
    /*LOCK(cs_vNodes);
    BOOST_FOREACH(CNode* pnode, vNodes){
        pnode->PushMessage("inv", vInv);
    }*/
    connman.ForEachNode([&vInv, &connman](CNode* pnode)
    {
        connman.PushMessage(pnode, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "inv", vInv));
    });
}

void ProcessMessageMasternodePayments(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman)
{
    if(IsInitialBlockDownload()) return;

    if (strCommand == "mnget") { //Masternode Payments Request Sync
        if(fProUserModeDarksendInstantX2) return; //disable all Darksend/Masternode related functionality

        if(pfrom->HasFulfilledRequest("mnget")) {
            LogPrintf("mnget - peer already asked me for the list\n");
            Misbehaving(pfrom->GetId(), 20);
            return;
        }

        pfrom->FulfilledRequest("mnget");
        masternodePayments.Sync(pfrom, connman);
        LogPrintf("mnget - Sent Masternode winners to %s\n", pfrom->addr.ToString().c_str());
    }
    else if (strCommand == "mnw") { //Masternode Payments Declare Winner

        LOCK(cs_masternodepayments);

        //this is required in litemode
        CMasternodePaymentWinner winner;
        vRecv >> winner;

        if(chainActive.Tip() == NULL) return;

        CTxDestination address1;
        ExtractDestination(winner.payee, address1);
        CStraksAddress address2(address1);

        arith_uint256 hash = UintToArith256(winner.GetHash());
        if(mapSeenMasternodeVotes.count(ArithToUint256(hash))) {
            if(fDebug) LogPrintf("mnw - seen vote %s Addr %s Height %d bestHeight %d\n", hash.ToString().c_str(), address2.ToString().c_str(), winner.nBlockHeight, chainActive.Tip()->nHeight);
            return;
        }

        if(winner.nBlockHeight < chainActive.Tip()->nHeight - 10 || winner.nBlockHeight > chainActive.Tip()->nHeight+20) {
            LogPrintf("mnw - winner out of range %s Addr %s Height %d bestHeight %d\n", winner.vin.ToString().c_str(), address2.ToString().c_str(), winner.nBlockHeight, chainActive.Tip()->nHeight);
            return;
        }

        if(winner.vin.nSequence != std::numeric_limits<unsigned int>::max()) {
            LogPrintf("mnw - invalid nSequence\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        LogPrintf("mnw - winning vote - Vin %s Addr %s Height %d bestHeight %d\n", winner.vin.ToString().c_str(), address2.ToString().c_str(), winner.nBlockHeight, chainActive.Tip()->nHeight);

        if( winner.nBlockHeight > (chainActive.Tip()->nHeight+1)
            && !masternodePayments.CheckSignature(winner)) {
            LogPrintf("mnw - invalid signature\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        mapSeenMasternodeVotes.insert(make_pair(ArithToUint256(hash), winner));

        if(masternodePayments.AddWinningMasternode(winner)) {
            LogPrintf("mnw - \n");
            RelayMNpayments(winner, pfrom, connman);
        }
    }
}

struct CompareValueOnly
{
    bool operator()(const pair<int64_t, CTxIn>& t1,
                    const pair<int64_t, CTxIn>& t2) const
    {
        return t1.first < t2.first;
    }
};

//Get the last hash that matches the modulus given. Processed in reverse order
bool GetBlockHash(uint256& hash, int nBlockHeight)
{
    if (chainActive.Tip() == NULL) return false;

    if(nBlockHeight == 0)
        nBlockHeight = chainActive.Tip()->nHeight;

    if(mapCacheBlockHashes.count(nBlockHeight)) {
        hash = mapCacheBlockHashes[nBlockHeight];
        return true;
    }

    const CBlockIndex *BlockLastSolved = chainActive.Tip();
    const CBlockIndex *BlockReading = chainActive.Tip();

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || chainActive.Tip()->nHeight+1 < nBlockHeight) return false;

    int nBlocksAgo = 0;
    if(nBlockHeight > 0) nBlocksAgo = (chainActive.Tip()->nHeight+1)-nBlockHeight;
    assert(nBlocksAgo >= 0);

    int n = 0;
    for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
        if(n >= nBlocksAgo) {
            hash = BlockReading->GetBlockHash();
            mapCacheBlockHashes[nBlockHeight] = hash;
            return true;
        }
        n++;

        if (BlockReading->pprev == NULL) {
            assert(BlockReading);
            break;
        }
        BlockReading = BlockReading->pprev;
    }

    return false;
}

CMasternode::CMasternode()
{
    LOCK(cs);
    vin = CTxIn();
    addr = CService();
    pubkey = CPubKey();
    pubkey2 = CPubKey();
    sig = std::vector<unsigned char>();
    activeState = MASTERNODE_ENABLED;
    sigTime = GetAdjustedTime();
    lastDseep = 0;
    lastTimeSeen = 0;
    cacheInputAge = 0;
    cacheInputAgeBlock = 0;
    unitTest = false;
    allowFreeTx = true;
    protocolVersion = MIN_PEER_PROTO_VERSION;
    nLastDsq = 0;
    donationAddress = CScript();
    donationPercentage = 0;
    nVote = 0;
    lastVote = 0;
    nScanningErrorCount = 0;
    nLastScanningErrorBlockHeight = 0;
}

CMasternode::CMasternode(const CMasternode& other)
{
    LOCK(cs);
    vin = other.vin;
    addr = other.addr;
    pubkey = other.pubkey;
    pubkey2 = other.pubkey2;
    sig = other.sig;
    activeState = other.activeState;
    sigTime = other.sigTime;
    lastDseep = other.lastDseep;
    lastTimeSeen = other.lastTimeSeen;
    cacheInputAge = other.cacheInputAge;
    cacheInputAgeBlock = other.cacheInputAgeBlock;
    unitTest = other.unitTest;
    allowFreeTx = other.allowFreeTx;
    protocolVersion = other.protocolVersion;
    nLastDsq = other.nLastDsq;
    donationAddress = other.donationAddress;
    donationPercentage = other.donationPercentage;
    nVote = other.nVote;
    lastVote = other.lastVote;
    nScanningErrorCount = other.nScanningErrorCount;
    nLastScanningErrorBlockHeight = other.nLastScanningErrorBlockHeight;
}

CMasternode::CMasternode(CService newAddr, CTxIn newVin, CPubKey newPubkey, std::vector<unsigned char> newSig, int64_t newSigTime, CPubKey newPubkey2, int protocolVersionIn, CScript newDonationAddress, int newDonationPercentage)
{
    LOCK(cs);
    vin = newVin;
    addr = newAddr;
    pubkey = newPubkey;
    pubkey2 = newPubkey2;
    sig = newSig;
    activeState = MASTERNODE_ENABLED;
    sigTime = newSigTime;
    lastDseep = 0;
    lastTimeSeen = 0;
    cacheInputAge = 0;
    cacheInputAgeBlock = 0;
    unitTest = false;
    allowFreeTx = true;
    protocolVersion = protocolVersionIn;
    nLastDsq = 0;
    donationAddress = newDonationAddress;
    donationPercentage = newDonationPercentage;
    nVote = 0;
    lastVote = 0;
    nScanningErrorCount = 0;
    nLastScanningErrorBlockHeight = 0;
}

//
// Deterministically calculate a given "score" for a Masternode depending on how close it's hash is to
// the proof of work for that block. The further away they are the better, the furthest will win the election
// and get paid this block
//
uint256 CMasternode::CalculateScore(int mod, int64_t nBlockHeight)
{
    if(chainActive.Tip() == NULL) return ArithToUint256(0);

    uint256 hash = ArithToUint256(0);
    uint256 aux;
    aux = ArithToUint256(UintToArith256(vin.prevout.hash) + vin.prevout.n);

    if(!GetBlockHash(hash, nBlockHeight)) return ArithToUint256(0);

    uint256 hash2 = Hash(BEGIN(hash), END(hash));
    uint256 hash3 = Hash(BEGIN(hash), END(hash), BEGIN(aux), END(aux));

    arith_uint256 r;
    r = (UintToArith256(hash3) > UintToArith256(hash2) ? UintToArith256(hash3) - UintToArith256(hash2) : UintToArith256(hash2) - UintToArith256(hash3));

    return ArithToUint256(r);
}

void CMasternode::Check()
{
    //TODO: Random segfault with this line removed
    TRY_LOCK(cs_main, lockRecv);
    if(!lockRecv) return;

    if(nScanningErrorCount >= MASTERNODE_SCANNING_ERROR_THESHOLD)
    {
        activeState = MASTERNODE_POS_ERROR; // BBBBB
        return;
    }

    //once spent, stop doing the checks
    if(activeState == MASTERNODE_VIN_SPENT) return;


    if(!UpdatedWithin(MASTERNODE_REMOVAL_SECONDS)) {
        activeState = MASTERNODE_REMOVE;
        return;
    }

    if(!UpdatedWithin(MASTERNODE_EXPIRATION_SECONDS)) {
        activeState = MASTERNODE_EXPIRED;
        return;
    }

    if(!unitTest) {
        CValidationState state;
        CMutableTransaction mtx;
        CTxOut vout = CTxOut(MASTERNODEAMOUNT * COIN, darkSendSigner.collateralPubKey);
        mtx.vin.push_back(vin);
        mtx.vout.push_back(vout);

        if(!AcceptableInputs(mempool, state, MakeTransactionRef(mtx))) {
            activeState = MASTERNODE_VIN_SPENT;
            LogPrintf("tx failed to get accepted on mempool");
            return;
        }

    }
    return;

    activeState = MASTERNODE_ENABLED; // OK
}

bool CMasternodePayments::CheckSignature(CMasternodePaymentWinner& winner)
{
    //note: need to investigate why this is failing
    std::string strMessage = winner.vin.ToString().c_str() + boost::lexical_cast<std::string>(winner.nBlockHeight) + winner.payee.ToString();

    //LogPrintf("Masternode CheckSignature Network: %s\n", Params().NetworkIDString().c_str());

    std::string strPubKey = (Params().NetworkIDString() == "main") ? strMainPubKey : strTestPubKey;
    CPubKey pubkey(ParseHex(strPubKey));

    std::string errorMessage = "";
    if(!darkSendSigner.VerifyMessage(pubkey, winner.vchSig, strMessage, errorMessage)) {
        return false;
    }

    return true;
}

bool CMasternodePayments::Sign(CMasternodePaymentWinner& winner)
{
    std::string strMessage = winner.vin.ToString().c_str() + boost::lexical_cast<std::string>(winner.nBlockHeight) + winner.payee.ToString();

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if(!darkSendSigner.SetKey(strMasterPrivKey, errorMessage, key2, pubkey2))
    {
        LogPrintf("CMasternodePayments::Sign - ERROR: Invalid Masternodeprivkey: '%s'\n", errorMessage.c_str());
        return false;
    }

    if(!darkSendSigner.SignMessage(strMessage, errorMessage, winner.vchSig, key2)) {
        LogPrintf("CMasternodePayments::Sign - Sign message failed");
        return false;
    }

    if(!darkSendSigner.VerifyMessage(pubkey2, winner.vchSig, strMessage, errorMessage)) {
        LogPrintf("CMasternodePayments::Sign - Verify message failed");
        return false;
    }

    return true;
}


uint64_t CMasternodePayments::CalculateScore(uint256 blockHash, CTxIn& vin)
{
    uint256 n1 = blockHash;
    uint256 n2, n3;
    arith_uint256 n4;

    n2 = lyra2re2_hash(BEGIN(n1), END(n1));
    n3 = lyra2re2_hash(BEGIN(vin.prevout.hash), END(vin.prevout.hash));
    n4 = UintToArith256(n3) > UintToArith256(n2) ? (UintToArith256(n3) - UintToArith256(n2)) : (UintToArith256(n2) - UintToArith256(n3));

    return n4.Get64();
}

bool CMasternodePayments::GetBlockPayee(int nBlockHeight, CScript& payee)
{
    BOOST_FOREACH(CMasternodePaymentWinner& winner, vWinning) {
        if(winner.nBlockHeight == nBlockHeight) {
            payee = winner.payee;
            return true;
        }
    }

    return false;
}

bool CMasternodePayments::GetWinningMasternode(int nBlockHeight, CTxIn& vinOut)
{
    BOOST_FOREACH(CMasternodePaymentWinner& winner, vWinning) {
        if(winner.nBlockHeight == nBlockHeight) {
            vinOut = winner.vin;
            return true;
        }
    }

    return false;
}

bool CMasternodePayments::AddWinningMasternode(CMasternodePaymentWinner& winnerIn)
{
    uint256 blockHash = uint256();

    //if(!GetBlockHash(blockHash, winnerIn.nBlockHeight-15)) {
   //     return false;
   // }

    winnerIn.score = CalculateScore(blockHash, winnerIn.vin);

    bool foundBlock = false;
    BOOST_FOREACH(CMasternodePaymentWinner& winner, vWinning) {
        if(winner.nBlockHeight == winnerIn.nBlockHeight) {
            foundBlock = true;
            if(winner.score < winnerIn.score) {
                winner.score = winnerIn.score;
                winner.vin = winnerIn.vin;
                winner.payee = winnerIn.payee;
                winner.vchSig = winnerIn.vchSig;

                mapSeenMasternodeVotes.insert(make_pair(winnerIn.GetHash(), winnerIn));

                return true;
            }
        }
    }

    // if it's not in the vector
    if(!foundBlock) {
        vWinning.push_back(winnerIn);
        mapSeenMasternodeVotes.insert(make_pair(winnerIn.GetHash(), winnerIn));

        return true;
    }

    return false;
}

void CMasternodePayments::CleanPaymentList()
{
    LOCK(cs_masternodepayments);

    if(chainActive.Tip() == NULL) return;

    int nLimit = std::max(((int)mnodeman.size())*2, 1000);

    vector<CMasternodePaymentWinner>::iterator it;
    for(it=vWinning.begin(); it<vWinning.end(); it++) {
        if(chainActive.Tip()->nHeight - (*it).nBlockHeight > nLimit) {
            if(fDebug) LogPrintf("CMasternodePayments::CleanPaymentList - Removing old Masternode payment - block %d\n", (*it).nBlockHeight);
            vWinning.erase(it);
            break;
        }
    }
}

bool CMasternodePayments::ProcessBlock(int nBlockHeight)
{
    LOCK(cs_masternodepayments);

    if(!enabled) {
        /*
        LogPrintf("Masternodelist entry sync status: %d\n", mnodeman.AwaitingEntrySync());

        if(mnodeman.AwaitingEntrySync() == 0) {
            CMasternode* pmn = mnodeman.GetCurrentMasterNode(1);

            if(pmn) {
                CMasternodePaymentWinner newWinner;
                newWinner.score = 0;
                newWinner.nBlockHeight = (nBlockHeight-10) + 1;
                newWinner.vin = pmn->vin;
                newWinner.payee=GetScriptForDestination(pmn->pubkey.GetID());

                if(masternodePayments.AddWinningMasternode(newWinner)) {
                    //LogPrintf(" masternodePayments.AddWinningMasternode failed!\n");
                    nLastBlockHeight = newWinner.nBlockHeight;
                    return true;
                }
            }
        } */
        return false;
    }

    if(nBlockHeight <= nLastBlockHeight) {
        LogPrintf(" error::nBlockHeight <= nLastBlockHeight %d. \n", nBlockHeight);
        return false;
    }

    CMasternodePaymentWinner newWinner;
    int nMinimumAge = mnodeman.CountEnabled();
    CScript payeeSource;

    uint256 hash;
    if(!GetBlockHash(hash, nBlockHeight-10)) {
        LogPrintf(" error::GetBlockHash(hash, nBlockHeight-10)");
        return false;
    }
    unsigned int nHash;
    memcpy(&nHash, &hash, 2);

    //LogPrintf(" ProcessBlock Start nHeight %d. \n", nBlockHeight);

    std::vector<CTxIn> vecLastPayments;
    BOOST_REVERSE_FOREACH(CMasternodePaymentWinner& winner, vWinning)
    {
        //if we already have the same vin - we have one full payment cycle, break
        if(vecLastPayments.size() > nMinimumAge) break;
        vecLastPayments.push_back(winner.vin);
    }

    // pay to the oldest MN that still had no payment but its input is old enough and it was active long enough
    CMasternode *pmn = mnodeman.FindOldestNotInVec(vecLastPayments, nMinimumAge, 0);
    if(pmn != NULL)
    {
        LogPrintf(" Found by FindOldestNotInVec \n");

        newWinner.score = 0;
        newWinner.nBlockHeight = nBlockHeight;
        newWinner.vin = pmn->vin;

        if(pmn->donationPercentage > 0 && (nHash % 100) <= (unsigned int)pmn->donationPercentage) {
            newWinner.payee = pmn->donationAddress;
        } else {
            newWinner.payee=GetScriptForDestination(pmn->pubkey.GetID());
        }

        payeeSource=GetScriptForDestination(pmn->pubkey.GetID());
    }

    //if we can't find new MN to get paid, pick first active MN counting back from the end of vecLastPayments list
    if(newWinner.nBlockHeight == 0 && nMinimumAge > 0)
    {
        LogPrintf(" Find by reverse \n");
        BOOST_REVERSE_FOREACH(CTxIn& vinLP, vecLastPayments)
        {
            CMasternode* pmn = mnodeman.Find(vinLP);
            if(pmn != NULL)
            {
                pmn->Check();
                if(!pmn->IsEnabled()) continue;

                newWinner.score = 0;
                newWinner.nBlockHeight = nBlockHeight;
                newWinner.vin = pmn->vin;

                if(pmn->donationPercentage > 0 && (nHash % 100) <= (unsigned int)pmn->donationPercentage) {
                    newWinner.payee = pmn->donationAddress;
                } else {
                    newWinner.payee=GetScriptForDestination(pmn->pubkey.GetID());
                }
                payeeSource=GetScriptForDestination(pmn->pubkey.GetID());

                break; // we found active MN
            }
        }
    }

    if(newWinner.nBlockHeight == 0) return false;

    CTxDestination address1;
    ExtractDestination(newWinner.payee, address1);
    CStraksAddress address2(address1);

    CTxDestination address3;

    ExtractDestination(payeeSource, address3);
    CStraksAddress address4(address3);

    if(fDebug) LogPrintf("Set Winner payee %s nHeight %d vin source %s. \n", address2.ToString().c_str(), newWinner.nBlockHeight, address4.ToString().c_str());

    if(Sign(newWinner))
    {
        if(AddWinningMasternode(newWinner))
        {
            Relay(newWinner);
            nLastBlockHeight = nBlockHeight;
            return true;
        }
    }

    return false;
}

void CMasternodePayments::Relay(CMasternodePaymentWinner& winner)
{
    CInv inv(MSG_MASTERNODE_WINNER, winner.GetHash());

    vector<CInv> vInv;
    vInv.push_back(inv);
    /*LOCK(cs_vNodes);
    BOOST_FOREACH(CNode* pnode, vNodes){
        pnode->PushMessage("inv", vInv);
    }*/
    g_connman->ForEachNode([&vInv](CNode* pnode)
    {
        g_connman->PushMessage(pnode, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "inv", vInv));
    });
}

void CMasternodePayments::Sync(CNode* node, CConnman& connman)
{
    LOCK(cs_masternodepayments);

    BOOST_FOREACH(CMasternodePaymentWinner& winner, vWinning)
    if(winner.nBlockHeight >= chainActive.Tip()->nHeight-10 && winner.nBlockHeight <= chainActive.Tip()->nHeight + 20)
        connman.PushMessage(node, CNetMsgMaker(PROTOCOL_VERSION).Make(SERIALIZE_TRANSACTION_NO_WITNESS, "mnw", winner));//node->PushMessage("mnw", winner);

}


bool CMasternodePayments::SetPrivKey(std::string strPrivKey)
{
    CMasternodePaymentWinner winner;

    strMasterPrivKey = strPrivKey;

    //LogPrintf("CMasternodePayments::SetPrivKey() - %s\n", strPrivKey.c_str());
    Sign(winner);

    if(CheckSignature(winner)) {
        LogPrintf("CMasternodePayments::SetPrivKey - Successfully initialized as Masternode payments master\n");
        enabled = true;
        return true;
    } else {
        return false;
    }
}
