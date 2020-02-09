// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017 PM-Tech
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

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
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */

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

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 840000;     // <-- Testcoin: unused
        consensus.nMajorityEnforceBlockUpgrade = 1875;  // <-- Testcoin: 1000 blocks LTC = 2500 blocks TST
        consensus.nMajorityRejectBlockOutdated = 2375;  // <-- Testcoin: 1000 blocks LTC = 2500 blocks TST
        consensus.nMajorityWindow = 2500;               // <-- Testcoin: 1000 blocks LTC = 2500 blocks TST
        consensus.BIP34Height = 1;                      // <-- Testcoin: forgot to update initial code, update after V3 or higher active!!!
        consensus.BIP34Hash = uint256S("0xd1c175570320d4d6388a4525385b8f20460d340f621cfeebb9824712b9e593c5"); // <-- Testcoin: used for BIP30 enforcement, set to corresponding BIP34 Height after V3 activation!
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.CharityPubKey = "1cec44c9f9b769ae08ebf9d694c7611a16edf615"; // EKnqTC9XEuucZEhD3miDGnbJxBptcxhByA
        consensus.nPowTargetTimespan = 60;  // <-- Testcoin: OK
        consensus.nPowTargetSpacing = 60;   // <-- Testcoin: OK
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% of 20160
        consensus.nMinerConfirmationWindow = 20160; // <-- Testcoin: approx. 2 weeks
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1485561600; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517356801; // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1485561600; // January 28, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1517356801; // January 31st, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("00000000000000000000000000000000000000000000000000c77bc63bfadbd1"); // <-- Testcoin: OK

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xee;  // <--Testcoin: OK
        pchMessageStart[1] = 0xcb;  // <--Testcoin: OK
        pchMessageStart[2] = 0x4c;  // <--Testcoin: OK
        pchMessageStart[3] = 0xca;  // <--Testcoin: OK
        nDefaultPort = 41826;       // <--Testcoin: OK
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock("NY Times 19/Feb/2014 North Korea Arrests Christian Missionary From Australia", CScript() << OP_DUP << OP_HASH160 << ParseHex("1cec44c9f9b769ae08ebf9d694c7611a16edf615") << OP_EQUALVERIFY << OP_CHECKSIG, 1392841423, 3236648, 0x1e0ffff0, 1, 50 * COIN); // <--Testcoin: OK
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x4e56204bb7b8ac06f860ff1c845f03f984303b5b97eb7b42868f714611aed94b")); // <--Testcoin: OK
        assert(genesis.hashMerkleRoot == uint256S("0xb3e47e8776012ee4352acf603e6b9df005445dcba85c606697f422be3cc26f9b")); // <--Testcoin: OK

        // Note that of those with the service bits flag, most only support a subset of possible options
//        vSeeds.push_back(CDNSSeedData("tst.foundation", "dnsseeder01.tst.foundation")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("tst.foundation", "dnsseeder02.tst.foundation")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed01.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed02.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed03.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed04.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed05.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed06.chainworksindustries.com")); // <--Testcoin: OK
//        vSeeds.push_back(CDNSSeedData("chainworksindustries.com", "tst-seed07.chainworksindustries.com")); // <--Testcoin: OK

        // Testcoin: Address encoding P2SH BTC = 5 / LTC NEW = 50 / TST OLD = 5

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,55);                                                                         // <-- Testcoin: different than LTC and BTC!
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,176);                                                                         // <-- Testcoin: OK
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));  // <-- Testcoin: Seeder array to be extended

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

    checkpointData = (CCheckpointData) {
        boost::assign::map_list_of
            (        0, uint256S("0x4e56204bb7b8ac06f860ff1c845f03f984303b5b97eb7b42868f714611aed94b"))  // <--Testcoin: OK
            (    14871, uint256S("0x5dedc3dd860f008c717d69b8b00f0476de8bc6bdac8d543fb58c946f32f982fa"))  // <--Testcoin: OK
            (    36032, uint256S("0xff37468190b2801f2e72eb1762ca4e53cda6c075af48343f28a32b649512e9a8"))  // <--Testcoin: OK
            (    51365, uint256S("0x702b407c68091f3c97a587a8d92684666bb622f6821944424b850964b366e42c"))  // <--Testcoin: OK
            (   621000, uint256S("0xe2bf6d219cff9d6d7661b7964a05bfea3128265275c3673616ae71fed7072981"))  // <--Testcoin: OK
            (  1410100, uint256S("0xf6736ff2a7743014ab1902e442328f5c9928ce7f4edb2b4fd0130010cb4cebc4")), // <--Testcoin: OK
            1494147472, // * UNIX timestamp of last checkpoint block                                     // <--Testcoin: OK
            2147811,    // * total number of transactions between genesis and last checkpoint            // <--Testcoin: OK
                      //   (the tx=... number in the SetBestChain debug.log lines)
            2000     // * estimated number of transactions per day after checkpoint                      // <--Testcoin: OK
            };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v4)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 840000;     // <-- Testcoin: unused
        consensus.nMajorityEnforceBlockUpgrade = 180;   // <-- Testcoin: quicker on Testnet
        consensus.nMajorityRejectBlockOutdated = 228;   // <-- Testcoin: quicker on Testnet
        consensus.nMajorityWindow = 240;                // <-- Testcoin: quicker on Testnet
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256S("");
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.CharityPubKey = "02bde17927d1340113fa6f795cac7ffee4c9364ba55f7e7b97413b48e69817baa6"; // mjY2nBQ78Ctmoq2AvzM3GcNuJUxRoRziiW
        consensus.nPowTargetTimespan = 60;  // <-- Testcoin: OK
        consensus.nPowTargetSpacing = 60;   // <-- Testcoin: OK
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517356801; // January 31st, 2018

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1483228800; // January 1, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1517356801; // January 31st, 2018

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00"); // <-- Testcoin: chaeck after wallet update

        pchMessageStart[0] = 0xaf;  // <--Testcoin: OK
        pchMessageStart[1] = 0x2a;  // <--Testcoin: OK
        pchMessageStart[2] = 0x0f;  // <--Testcoin: OK
        pchMessageStart[3] = 0x1c;  // <--Testcoin: OK
        nDefaultPort = 31826;       // <--Testcoin: OK
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock("NY Times 19/Feb/2014 North Korea Arrests Christian Missionary From Australia", CScript() << OP_DUP << OP_HASH160 << ParseHex("1cec44c9f9b769ae08ebf9d694c7611a16edf615") << OP_EQUALVERIFY << OP_CHECKSIG, 1494757042, 2231829, 0x1e0ffff0, 1, 50 * COIN); // <--Testcoin: OK
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xa4271888b5e60092c3e7183a76d454741e9a7a55f2b4afbe574615829e406bee"));   // <--Testcoin: OK
        assert(genesis.hashMerkleRoot == uint256S("0xb3e47e8776012ee4352acf603e6b9df005445dcba85c606697f422be3cc26f9b"));   // <--Testcoin: OK

        vFixedSeeds.clear();
//        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
//        vSeeds.push_back(CDNSSeedData("test.tst.foundation", "dnsseedertest01.tst.foundation")); //<-- Testcoin: OK

        // Testcoin: OK

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,58);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;


        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
                ( 0, uint256S("0xa4271888b5e60092c3e7183a76d454741e9a7a55f2b4afbe574615829e406bee"))  // <--Testcoin: OK
                ( 6, uint256S("0x8618a815ad94f918a7d3d4df7ebc4df5f14da1ea25d0eb156b6a32f9621c2ce4")), // <--Testcoin: OK
            1494590578,                                                                               // <--Testcoin: OK
            7,                                                                                        // <--Testcoin: OK
            1500                                                                                      // <--Testcoin: OK
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test <-- PM-Tech: copy litecoin data here for useable automated testing
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.CharityPubKey = "0377ba3117d776b40b49a910e869cd32adee4d33578f7bf52e1879ea739c9796ca"; // mtFhq4FbmFkoFmMECc7NXzrYiaxEii56su
        consensus.nPowTargetTimespan = 3.5 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0xaf;
        pchMessageStart[1] = 0xfb;
        pchMessageStart[2] = 0x5b;
        pchMessageStart[3] = 0xad;
        nDefaultPort = 31826;
        nPruneAfterHeight = 1000;


        /**
        * For RegTest we simulate the Litecoin Genesis so we can re-use all validation data
        */

        genesis = CreateGenesisBlock("NY Times 05/Oct/2011 Steve Jobs, Apple’s Visionary, Dies at 56", CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG, 1296688602, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x530827f38f93b43ed12af0b3ad25a288dc02ed74d6d7857862df51fc56c416f9"));
        assert(genesis.hashMerkleRoot == uint256S("0x97ddfbbae6be97fd6cdf3e7ca13232a3afff2353e29badfab7f73011edd4ced9"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
//        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
                ( 0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")),
                0,
                0,
                0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SCRIPT_ADDRESS2] = std::vector<unsigned char>(1,58);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
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

