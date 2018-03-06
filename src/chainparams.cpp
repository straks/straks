// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017 The Dash developers
// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"
#include "base58.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);

    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "WSJ 16/Nov/2017: You Do Not Understand Bitcoin: Readers Bite Back";
    const CScript genesisOutputScript = CScript() << ParseHex("04b36e3c4a54589d96c47dbdec6672477b6c4a01434a5f42257976d9053bcf8f809b857900ac246162b60c13ac25351952bea1e128d60a4e110ec9390fd51dfe95");
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("00000b6321951f2ed170bbc9b7a360995176f2df418b0e275149bfce2fde3d6c");
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.BIP102HeightDelta = 0;

        consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");

        consensus.nPowAveragingWindow = 8;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 31; // Zawy adjustment: (1 - 1/0.765)x100 (0.765 = v1b)
        consensus.nPowMaxAdjustUp = 23.7; // Zawy adjustment: 1- 1/(1+.31)x100 = 23.7

        consensus.nPowTargetTimespan = 30 * 60; //30 minutes
        consensus.nPowTargetSpacing = 60; //1 minute

        consensus.fPowD106SwitchHeight = consensus.DifficultyAdjustmentInterval();

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        consensus.nRuleChangeActivationThreshold = 9576; 
        consensus.nMinerConfirmationWindow = 10080;  

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1496275200;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit2x
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nStartTime = 1496275200; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nTimeout = 1514764800; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideRuleChangeActivationThreshold = 1; 

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0000000000000000000000000000000000000000000000000fdcb80725085fc5");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        //5 weeks from launch, fair chance to buy/accumulate. 50400 blocks
        consensus.MasternodePaymentStartHeight = 50400;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xb0;
        pchMessageStart[1] = 0xd5;
        pchMessageStart[2] = 0xf0;
        pchMessageStart[3] = 0x2c;
        nDefaultPort = 7575;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1510790624, 1034946, 0x1e0ffff0, 1, 10 * COIN);//Time, Nonce,nBits, version, reward
        consensus.hashGenesisBlock = genesis.GetHash();

/*
CreateGenesisBlock(1510790624, 388287, 0x1e0ffff0, 1, 10 * COIN)
NEW **mainnet** genesis nonce: 1034946
NEW **mainnet** genesis hash: 00000df14d859c4b3219d93978bcf02afc123d2344a2ed39033e1208948aa7c0
NEW **mainnet** merkle root: 15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4
*/

        assert(consensus.hashGenesisBlock == uint256S("00000df14d859c4b3219d93978bcf02afc123d2344a2ed39033e1208948aa7c0"));
        assert(genesis.hashMerkleRoot == uint256S("15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4"));

        vSeeds.push_back(CDNSSeedData("sm001.alphaqub.com", "sm001.alphaqub.com"));
        vSeeds.push_back(CDNSSeedData("sm002.alphaqub.com", "sm002.alphaqub.com"));
        vSeeds.push_back(CDNSSeedData("sm003.alphaqub.com", "sm003.alphaqub.com"));
        vSeeds.push_back(CDNSSeedData("sm004.alphaqub.com", "sm004.alphaqub.com"));
        vSeeds.push_back(CDNSSeedData("sm005.alphaqub.com", "sm005.alphaqub.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,63);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,204);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            {
                {     0, uint256S("00000df14d859c4b3219d93978bcf02afc123d2344a2ed39033e1208948aa7c0")},
                { 19760, uint256S("000000000008cdaee7a7cc3e8e5336ce2426eb7079f9a43a4dbf15ac8c08cbec")},
                { 34000, uint256S("00000000008fa092d7379523824d65470e1f62afdd9e4efe225b2a80461c16a1")},
                { 50400, uint256S("00000000003c5729841f6d3f44d267e6be7e6b9a7cd4c9b2480a396f22e7561e")},
                { 94858, uint256S("000000000003b195d823ee34f2ffa36933e4696fa845508e0fa9238e0dbb9718")},
                { 136201, uint256S("00000000000def1328cef45871f0cd91dabd28dc5ba117535cbcee19eebdf79c")}
            }
        };

        // treasury payment adresses 
        vTreasuryRewardAddress = {
            "3K3bPrW5h7DYEMp2RcXawTCXajcm4ZU9Zh",
            "33Ssxmn3ehVMgyxgegXhpLGSBpubPjLZQ6",
            "3HFPNAjesiBY5sSVUmuBFnMEGut69R49ca",
            "37jLjjfUXQU4bdqVzvpUXyzAqPQSmxyByi"
        };

        // [1] UNIX timestamp of last known number of transactions
        // [2] total number of transactions between genesis and that timestamp
        //     (the tx=... number in the SetBestChain debug.log lines)
        // [3] estimated number of transactions per second after that timestamp
        chainTxData = ChainTxData{
            1516904966,
            139917,
            1
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";

        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("0000013dcc24cb29b041d5c89763f3aa44340faf556101783818a6ca8eb59e59");
        consensus.BIP65Height = 0; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 0; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.BIP102HeightDelta = 0;

        consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");

        consensus.nPowAveragingWindow = 8;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 31; // Zawy adjustment: (1 - 1/0.765)x100 (0.765 = v1b)
        consensus.nPowMaxAdjustUp = 23.7; // Zawy adjustment: 1- 1/(1+.31)x100 = 23.7

        consensus.nPowTargetTimespan = 1 * 60; //1 minutes
        consensus.nPowTargetSpacing = 10; //seconds 

        consensus.fPowD106SwitchHeight = 30;

        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        consensus.nRuleChangeActivationThreshold = 6480; 
        consensus.nMinerConfirmationWindow = 8640; //1 day

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1496275200;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit2x
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nStartTime = 1496275200; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nTimeout = 1514764800; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideRuleChangeActivationThreshold = 1; 

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("00000000000000000000000000000000000000000000000000000055196d0002");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        consensus.MasternodePaymentStartHeight = 1440;

        pchMessageStart[0] = 0x2a;
        pchMessageStart[1] = 0x1e;
        pchMessageStart[2] = 0xd5;
        pchMessageStart[3] = 0xd1;
        nDefaultPort = 7565;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1510791350, 1559747, 0x1e0ffff0, 1, 10 * COIN);//Time, Nonce,nBits, version, reward
        consensus.hashGenesisBlock = genesis.GetHash();

/*
CreateGenesisBlock(1510791350, 1093629, 0x1e0ffff0, 1, 10 * COIN)
NEW **testnet** genesis nonce: 1559747
NEW **testnet** genesis hash: 000000cd747bd0b653e1fe417b60c1d9e990600cf2ff193404ea12c3ecb348b4
NEW **testnet** merkle root: 15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4
*/

        assert(consensus.hashGenesisBlock == uint256S("000000cd747bd0b653e1fe417b60c1d9e990600cf2ff193404ea12c3ecb348b4"));
        assert(genesis.hashMerkleRoot == uint256S("15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4"));

        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("st001.radixpi.com", "st001.radixpi.com"));
        vSeeds.push_back(CDNSSeedData("st002.radixpi.com", "st002.radixpi.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0xA2)(0xAE)(0xC9)(0xA6).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x46)(0x00)(0x2A)(0x10).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            {
                {     0, uint256S("000000cd747bd0b653e1fe417b60c1d9e990600cf2ff193404ea12c3ecb348b4")},
                {119692, uint256S("000004d943ed36053076a62b20cc7bec6a8a4c66ed1f6a6e88deeca015ff555a")},
                {195182, uint256S("0000001cbc01c2d26da194f44fd63e35eb87e65642c8611c93e9c14a2f2c0a0d")},
                {232597, uint256S("0000023d86e2181d63c6db4cb82e5889737b824efd71904f03fe76898a941211")},
            }
        };

        vTreasuryRewardAddress = {
            "93ASJtDuVYVdKXemH9BrtSMscznvsp9stD"
        };

        chainTxData = ChainTxData{
            1512085561,
            119905,
            6
        };
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("000002db0642636c786f756062dd7a92f35a2be1665816f8bfa33111ae902b37");
        consensus.BIP65Height = 0; 
        consensus.BIP66Height = 0; 
        consensus.powLimit = uint256S("00000fffff000000000000000000000000000000000000000000000000000000");
        consensus.BIP102HeightDelta = 0;

        consensus.nPowAveragingWindow = 8;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 31; // Zawy adjustment: (1 - 1/0.765)x100 (0.765 = v1b)
        consensus.nPowMaxAdjustUp = 23.7; // Zawy adjustment: 1- 1/(1+.31)x100 = 23.7

        consensus.nPowTargetTimespan = 60; //1 minute
        consensus.nPowTargetSpacing = 1; //1 second

        consensus.fPowD106SwitchHeight = 30;

        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;

        consensus.nRuleChangeActivationThreshold = 5400; 
        consensus.nMinerConfirmationWindow = 7200; 

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1496275200;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1514764800;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nOverrideRuleChangeActivationThreshold = 1; 

        // Deployment of SegWit2x
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].bit = 4;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nStartTime = 1496275200; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nTimeout = 1514764800; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideMinerConfirmationWindow = 1; 
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT2X].nOverrideRuleChangeActivationThreshold = 1; 

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0");

        consensus.MasternodePaymentStartHeight = 1440;

        pchMessageStart[0] = 0x6e;
        pchMessageStart[1] = 0x5c;
        pchMessageStart[2] = 0x2c;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 7505;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1510791654, 1079442, 0x1e0ffff0, 1, 10 * COIN);//Time, Nonce,nBits, version, reward
        consensus.hashGenesisBlock = genesis.GetHash();

/*
CreateGenesisBlock(1510272549, 1547336, 0x1e0ffff0, 1, 10 * COIN)
NEW **regtest** genesis nonce: 1079442
NEW **regtest** genesis hash: 000002db0642636c786f756062dd7a92f35a2be1665816f8bfa33111ae902b37
NEW **regtest** merkle root: 15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4
 */

        assert(consensus.hashGenesisBlock == uint256S("000002db0642636c786f756062dd7a92f35a2be1665816f8bfa33111ae902b37"));
        assert(genesis.hashMerkleRoot == uint256S("15343d9e3cfff44854ec63cc588d5a1ed6ea971085c2be97acb0ea22c0e94fc4"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  0, uint256S("000002db0642636c786f756062dd7a92f35a2be1665816f8bfa33111ae902b37") )
        };

        vTreasuryRewardAddress = {
            "2N6TJncxtMLSqEurFaEGtpLxdvq5wiVcR6S"
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,122);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0xA2)(0xAE)(0xC9)(0xA6).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x46)(0x00)(0x2A)(0x10).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
        return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
        return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}


std::string CChainParams::GetTreasuryRewardAddressAtHeight(int nHeight) const {
    size_t i = nHeight % vTreasuryRewardAddress.size();
    return vTreasuryRewardAddress[i];
}

CScript CChainParams::GetTreasuryRewardScriptAtHeight(int nHeight) const {
    CStraksAddress address(GetTreasuryRewardAddressAtHeight(nHeight).c_str());
    assert(address.IsValid());
    assert(address.IsScript());
    CScriptID scriptID = boost::get<CScriptID>(address.Get()); // Get() returns a boost variant
    CScript script = CScript() << OP_HASH160 << ToByteVector(scriptID) << OP_EQUAL;
    return script; 
}

std::string CChainParams::GetTreasuryRewardAddressAtIndex(int i) const {
    assert(i >= 0 && i < vTreasuryRewardAddress.size());
    return vTreasuryRewardAddress[i];
}

