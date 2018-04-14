// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef ACTIVEMASTERNODE_H
#define ACTIVEMASTERNODE_H

//#include "bignum.h"
#include "sync.h"
#include "net.h"
#include "netbase.h"
#include "key.h"
//#include "core.h"
#include "init.h"
#include "wallet/wallet.h"
#include "net_processing.h"
#include <vector> //
#include "masternode.h"

//[squbs] calibrate masternode collateral
static const int MASTERNODEAMOUNT = 15000;

// [squbs] Reactive Equilibria V1 - Payment Equilbrium Factor (PEF)
// Value sensitivity is reasonably high. Make sure you understand
// implications if you modify.
static const double MASTERNODE_PEF = 0.55;

// Responsible for activating the Masternode and pinging the network
class CActiveMasternode
{
public:
	// Initialized by init.cpp
	// Keys for the main Masternode
	CPubKey pubKeyMasternode;

	// Initialized while registering Masternode
	CTxIn vin;
    CService service;

    int status;
    std::string notCapableReason;

    CActiveMasternode()
    {        
        status = MASTERNODE_NOT_PROCESSED;
    }

    /// Manage status of main Masternode
    void ManageStatus(); 

    /// Ping for main Masternode
    bool Dseep(std::string& errorMessage); 
    /// Ping for any Masternode
    bool Dseep(CTxIn vin, CService service, CKey key, CPubKey pubKey, std::string &retErrorMessage, bool stop); 

    /// Stop main Masternode
    bool StopMasterNode(std::string& errorMessage); 
    /// Stop remote Masternode
    bool StopMasterNode(std::string strService, std::string strKeyMasternode, std::string& errorMessage); 
    /// Stop any Masternode
    bool StopMasterNode(CTxIn vin, CService service, CKey key, CPubKey pubKey, std::string& errorMessage); 

    /// Register remote Masternode
    bool Register(std::string strService, std::string strKey, std::string txHash, std::string strOutputIndex, std::string strDonationAddress, std::string strDonationPercentage, std::string& errorMessage); 
    /// Register any Masternode
    bool Register(CTxIn vin, CService service, CKey key, CPubKey pubKey, CKey keyMasternode, CPubKey pubKeyMasternode, CScript donationAddress, int donationPercentage, std::string &retErrorMessage); 

    /// Get 5000DRK input that can be used for the Masternode
    bool GetMasterNodeVin(CTxIn& vin, CPubKey& pubkey, CKey& secretKey);
    bool GetMasterNodeVin(CTxIn& vin, CPubKey& pubkey, CKey& secretKey, std::string strTxHash, std::string strOutputIndex);
    //vector<COutput> SelectCoinsMasternode();
    bool GetVinFromOutput(COutput out, CTxIn& vin, CPubKey& pubkey, CKey& secretKey);
	std::vector<COutput> SelectCoinsMasternode();//todo++
    /// Enable hot wallet mode (run a Masternode with no funds)
    bool EnableHotColdMasterNode(CTxIn& vin, CService& addr);
};

extern CActiveMasternode activeMasternode;

#endif
