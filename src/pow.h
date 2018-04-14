// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers 
// Copyright (c) 2015-2017 The Dash developers 
// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_POW_H
#define STRAKS_POW_H

#include "consensus/params.h"

#include <stdint.h>

class CBlockHeader;
class CBlockIndex;
class uint256;

/**TODO-- change it to bitsent style */
// Define difficulty retarget algorithms
enum DiffMode {
    DIFF_DEFAULT = 0, // Default to invalid 0
    DIFF_DKGW3     = 1, // Retarget every x blocks
    DIFF_KGW     = 2, // Retarget using Kimoto Gravity Well
    DIFF_DGW     = 3, // Retarget using Dark Gravity Wave v3
    DIFF_DS     = 4, // Retarget using Digishield
    DIFF_D106     = 5, // Retarget using D106
};

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params&);
uint32_t GetNextWorkRequiredD106(const CBlockIndex *pindexPrev, const CBlockHeader *pblock, const Consensus::Params &params);
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params&);

/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params&);
double ConvertBitsToDouble(unsigned int nBits);

#endif // STRAKS_POW_H
