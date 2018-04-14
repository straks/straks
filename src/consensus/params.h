// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers 
// Copyright (c) 2017 The Dash developers 
// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_CONSENSUS_PARAMS_H
#define STRAKS_CONSENSUS_PARAMS_H

#include "uint256.h"
#include <map>
#include <string>

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_CSV, // Deployment of BIP68, BIP112, and BIP113.
    DEPLOYMENT_SEGWIT, // Deployment of BIP141, BIP143, and BIP147.
    DEPLOYMENT_SEGWIT2X, // Deployment of SEGWIT2X
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;
	
	int64_t nHeight; //maybe "int"

    /** Overriding the default threshold if not zero. */
    uint32_t nOverrideRuleChangeActivationThreshold = 0;

    /** Overriding the default confirmation window if not zero . */
    uint32_t nOverrideMinerConfirmationWindow = 0;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height delta at which BIP102 becomes active */
    int BIP102HeightDelta;

    /** When to start masternode payments? */
    int64_t MasternodePaymentStartHeight;

    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;

    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];

    /** Proof of work parameters */
    uint256 powLimit;

    int64_t fPowD106SwitchHeight;

    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;

    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t DifficultyAdjustmentInterval() const { return nPowTargetTimespan / nPowTargetSpacing; }

    bool fDigishieldDifficultyCalculation;
    bool fPowAllowDigishieldMinDifficultyBlocks; // Allow minimum difficulty blocks where a retarget would normally occur

    int64_t nPowAveragingWindow;
    int64_t nPowMaxAdjustDown;
    int64_t nPowMaxAdjustUp;
    int64_t AveragingWindowTimespan() const { return nPowAveragingWindow * nPowTargetSpacing; }
    int64_t MinActualTimespan() const { return (AveragingWindowTimespan() * (100 - nPowMaxAdjustUp  )) / 100; }
    int64_t MaxActualTimespan() const { return (AveragingWindowTimespan() * (100 + nPowMaxAdjustDown)) / 100; }

    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;
};
} // namespace Consensus

#endif // STRAKS_CONSENSUS_PARAMS_H
