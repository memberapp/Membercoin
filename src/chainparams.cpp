// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2015-2020 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"
#include "policy/policy.h"
#include "unlimited.h"
#include "versionbits.h" // bip135 added

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

#include <stdio.h>

// Next protocol upgrade will be activated once MTP >= Nov 15 12:00:00 UTC 2020
const uint64_t NOV2020_ACTIVATION_TIME = 1605441600;
uint64_t nMiningForkTime = NOV2020_ACTIVATION_TIME;

CBlock CreateGenesisBlock(CScript prefix,
    const std::string &comment,
    const CScript &genesisOutputScript,
    uint32_t nTime,
    uint32_t nNonce,
    uint32_t nBits,
    int32_t nVersion,
    const CAmount &genesisReward)
{
    const unsigned char *pComment = (const unsigned char *)comment.c_str();
    std::vector<unsigned char> vComment(pComment, pComment + comment.length());

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = prefix << vComment;
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
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
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505,
 * nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase
 * 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime,
    uint32_t nNonce,
    uint32_t nBits,
    int32_t nVersion,
    const CAmount &genesisReward)
{
    const char *pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript()
                                        << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb6"
                                                    "49f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f")
                                        << OP_CHECKSIG;
    return CreateGenesisBlock(CScript() << 486604799 << LegacyCScriptNum(4), pszTimestamp, genesisOutputScript, nTime,
        nNonce, nBits, nVersion, genesisReward);
}

bool CChainParams::RequireStandard() const
{
    // the acceptnonstdtxn flag can only be used to narrow the behavior.
    // A blockchain whose default is to allow nonstandard txns can be configured to disallow them.
    return fRequireStandard || !GetBoolArg("-acceptnonstdtxn", true);
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

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210000;
        // 00000000000000ce80a7e057163a4db1d5ad7b20fb6f598c9597b9665c8fb0d4 - April 1, 2012
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        consensus.BIP65Height = 0; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.BIP66Height = 0; // 00000000000000000379eaa19dce8c9b722d46ae6a57c2f1a988119488b50931
        consensus.BIP68Height = 0; // BIP68, 112, 113 has activated
        consensus.powLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 60 * 60 * 48; // two days
        consensus.nPowTargetSpacing = 78; // 78 seconds
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        // The half life for the ASERT DAA. For every (nASERTHalfLife) seconds behind schedule the blockchain gets,
        // difficulty is cut in half. Doubled if blocks are ahead of schedule.
        // Two days
        consensus.nASERTHalfLife = 60 * 60 * 4; // four hours
        // testing bit
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601LL; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999LL; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].windowsize = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1916; // 95% of 2016

        // Aug, 1 2017 hard fork
        consensus.uahfHeight = 0;
        // Nov, 13 2017 hard fork
        consensus.daaHeight = 0;
        // May, 15 2018 hard fork
        consensus.may2018Height = 0;
        // Nov, 15 2018 hard fork
        consensus.nov2018Height = 0;
        // Noc, 15 2019 hard fork
        consensus.nov2019Height = 0;
        // May, 15 2020 hard fork
        consensus.may2020Height = 0;
        // Nov 15, 2020 12:00:00 UTC protocol upgrade¶
        consensus.nov2020ActivationTime = NOV2020_ACTIVATION_TIME;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xe3;
        pchMessageStart[1] = 0xe1;
        pchMessageStart[2] = 0xf8;
        pchMessageStart[3] = 0xe8;
        pchCashMessageStart[0] = 0xe3;
        pchCashMessageStart[1] = 0xe1;
        pchCashMessageStart[2] = 0xf8;
        pchCashMessageStart[3] = 0xe8;
        nDefaultPort = DEFAULT_MAINNET_PORT;
        nPruneAfterHeight = 100000;
        nDefaultExcessiveBlockSize = DEFAULT_EXCESSIVE_BLOCK_SIZE;
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE;
        nDefaultMaxBlockMiningSize = DEFAULT_BLOCK_MAX_SIZE;

        // genesis = CreateGenesisBlock(1231006505, 2083236893, 0x1d00ffff, 1, 50 * COIN);
        std::vector<unsigned char> rawScript(ParseHex("76a914a123a6fdc265e1bbcf1123458891bd7af1a1b5d988ac"));
        CScript outputScript(rawScript.begin(), rawScript.end());

        uint32_t nBits = UintToArith256(consensus.powLimit).GetCompact();
        arith_uint256 hashTarget = arith_uint256().SetCompact(nBits);

        genesis = CreateGenesisBlock(
            CScript() << 0, "enough already", outputScript, 1654916868, 47566902, nBits, 1, 5 * COIN);
        // genesis.nStartLocation = 64;
        // genesis.nFinalCalculation = 4277402212;
        consensus.hashGenesisBlock = genesis.GetHash();
        if (UintToArith256(consensus.hashGenesisBlock) > hashTarget)
        {
            uint256 thash;
            // char *scratchpad;
            // scratchpad=new char[(1<<30)];
            while (true)
            {
                // int collisions=0;
                // thash = genesis.FindBestPatternHash(collisions,scratchpad,8);
                genesis.nNonce++;
                thash = genesis.GetHash();
                if (UintToArith256(thash) <= hashTarget)
                {
                    printf("nonce %u: hash = %s (target = %s)\n", genesis.nNonce, genesis.GetHash().ToString().c_str(), hashTarget.ToString().c_str());
                    printf("block.nTime = %u \n", genesis.nTime);
                    printf("block.nNonce = %u \n", genesis.nNonce);
                    printf("block.GetHash = %s\n", genesis.GetHash().ToString().c_str());
                    printf("block.nBits = %u \n", genesis.nBits);
                    // printf("block.nStartLocation = %u \n", genesis.nStartLocation);
                    // printf("block.nFinalCalculation = %u \n", genesis.nFinalCalculation);
                    consensus.hashGenesisBlock = genesis.GetHash();
                    break;
                }
                
                if (genesis.nNonce == 0)
                {
                    printf("NONCE WRAPPED, incrementing time\n");
                    ++genesis.nTime;
                }
            }
        }
        // if(noncetry%100000==0){
        //     printf(" %08X\n",noncetry);
        //     printf(" %s\n",genesis.GetHash().ToString().c_str());
        //
        // }
        // noncetry++;


        // assert(consensus.hashGenesisBlock ==
        // uint256S("0x000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"));
        // assert(genesis.hashMerkleRoot ==
        // uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        // List of Member compatible seeders
        vSeeds.push_back(CDNSSeedData("member.cash", "member.cash", true));
        vSeeds.push_back(CDNSSeedData("asia.member.cash", "asia.member.cash", true));
        vSeeds.push_back(CDNSSeedData("europe.member.cash", "europe.member.cash", true));
        vSeeds.push_back(CDNSSeedData("us.member.cash", "us.member.cash", true));

        // vSeeds.push_back(CDNSSeedData("bitcoinunlimited.info", "btccash-seeder.bitcoinunlimited.info", true));
        // vSeeds.push_back(CDNSSeedData("bitcoinforks.org", "seed-bch.bitcoinforks.org", true));
        // vSeeds.push_back(CDNSSeedData("bchd.cash", "seed.bchd.cash", true));
        // vSeeds.push_back(CDNSSeedData("bch.loping.net", "seed.bch.loping.net", true));
        // vSeeds.push_back(CDNSSeedData("electroncash.de", "dnsseed.electroncash.de", true));
        // vSeeds.push_back(CDNSSeedData("flowee.cash", "seed.flowee.cash", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 5);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 128);
        base58Prefixes[EXT_PUBLIC_KEY] =
            boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] =
            boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "member";

        // BITCOINUNLIMITED START
        vFixedSeeds = std::vector<SeedSpec6>();
        // BITCOINUNLIMITED END

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        // clang-format off
        checkpointData = CCheckpointData();
        //MapCheckpoints &checkpoints = checkpointData.mapCheckpoints;
        //checkpoints[ 11111] = uint256S("0x0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d");


        // clang-format on
        // * UNIX timestamp of last checkpoint block
        checkpointData.nTimeLastCheckpoint = 0;
        // * total number of transactions between genesis and last checkpoint
        checkpointData.nTransactionsLastCheckpoint = 0;
        // * estimated number of transactions per day after checkpoint (~3.5 TPS)
        checkpointData.fTransactionsPerDay = 10000.0;
    }
};

static CMainParams mainParams;

class CUnlParams : public CChainParams
{
public:
    CUnlParams()
    {
        strNetworkID = "nol";

        std::vector<unsigned char> rawScript(ParseHex("76a914a123a6fdc265e1bbcf1123458891bd7af1a1b5d988ac"));
        CScript outputScript(rawScript.begin(), rawScript.end());

        genesis = CreateGenesisBlock(CScript() << 0, "Big blocks FTW (for the world)", outputScript, 1496544271,
            2301659837, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = consensus.hashGenesisBlock;
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.BIP68Height = 0;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60 / 10; // two weeks
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        // assert(consensus.hashGenesisBlock ==
        // uint256S("0000000057e31bd2066c939a63b7b8623bd0f10d8c001304bdfc1a7902ae6d35"));

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xce;
        pchMessageStart[2] = 0xc4;
        pchMessageStart[3] = 0xe9;
        nDefaultPort = DEFAULT_NOLNET_PORT;
        nPruneAfterHeight = 100000;
        nDefaultExcessiveBlockSize = std::numeric_limits<uint64_t>::max();
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE_REGTEST;
        nDefaultMaxBlockMiningSize = std::numeric_limits<uint64_t>::max();

        // Aug, 1 2017 hard fork
        consensus.uahfHeight = 0;
        // Nov, 13 hard fork
        consensus.daaHeight = consensus.DifficultyAdjustmentInterval();
        // May, 15 2018 hard fork
        consensus.may2018Height = 0;
        // Nov, 15 2018 hard fork
        consensus.nov2018Height = 0;
        // May, 15 2019 hard fork
        consensus.may2019Height = 0;
        // May 15, 2020 12:00:00 UTC protocol upgrade¶
        consensus.nov2019Height = 0;
        // May, 15 2020 hard fork
        consensus.may2020Height = 0;
        // Nov, 15 2019 12:00:00 UTC fork activation time
        consensus.nov2020ActivationTime = NOV2020_ACTIVATION_TIME;

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("bitcoinunlimited.info", "nolnet-seed.bitcoinunlimited.info", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 25); // P2PKH addresses begin with B
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 68); // P2SH  addresses begin with U
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 35); // WIF   format begins with 2B or 2C
        base58Prefixes[EXT_PUBLIC_KEY] =
            boost::assign::list_of(0x42)(0x69)(0x67)(0x20).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] =
            boost::assign::list_of(0x42)(0x6c)(0x6b)(0x73).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "bchnol";

        vFixedSeeds = std::vector<SeedSpec6>();

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of(0, uint256S("0000000057e31bd2066c939a63b7b8623bd0f10d8c001304bdfc1a7902ae6d35")),
            0, 0, 0};
    }
};
CUnlParams unlParams;


/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams
{
public:
    CTestNetParams()
    {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 514; // 00000000040b4e986385315e14bee30ad876d8b47f748025b26683116d21aa65
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256S("0x0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8");
        consensus.BIP65Height = 581885; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 330776; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.BIP68Height = 770112; // BIP68, 112, 113 has activated
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        // The half life for the ASERT DAA. For every (nASERTHalfLife) seconds behind schedule the blockchain gets,
        // difficulty is cut in half. Doubled if blocks are ahead of schedule.
        // One hour
        consensus.nASERTHalfLife = 60 * 60;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].windowsize = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512; // 75% of 2016

        // Aug, 1 2017 hard fork
        consensus.uahfHeight = 1155876;
        // Nov, 13 hard fork
        consensus.daaHeight = 1188697;
        // May, 15 2018 hard fork
        consensus.may2018Height = 1233070;
        // Nov 15, 2018 hard fork
        consensus.nov2018Height = 1267996;
        // May, 15 2019 hard fork
        consensus.may2019Height = 1303884;
        // Nov, 15 2019 har fork
        consensus.nov2019Height = 1341711;
        // May, 15 2020 hard fork
        consensus.may2020Height = 1378461;
        // Nov 15, 2020 12:00:00 UTC protocol upgrade¶
        consensus.nov2020ActivationTime = NOV2020_ACTIVATION_TIME;


        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        pchCashMessageStart[0] = 0xf4;
        pchCashMessageStart[1] = 0xe5;
        pchCashMessageStart[2] = 0xf3;
        pchCashMessageStart[3] = 0xf4;
        nDefaultPort = DEFAULT_TESTNET_PORT;
        nPruneAfterHeight = 1000;
        nDefaultExcessiveBlockSize = DEFAULT_EXCESSIVE_BLOCK_SIZE;
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE;
        nDefaultMaxBlockMiningSize = DEFAULT_BLOCK_MAX_SIZE;

        genesis = CreateGenesisBlock(1296688602, 414098458, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // assert(consensus.hashGenesisBlock ==
        //        uint256S("0x000000000933ea01ad0ee984209779baaec3ced90fa3f408719526f8d77f4943"));
        // assert(
        //     genesis.hashMerkleRoot ==
        //     uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top

        // bitcoinforks seeders
        vSeeds.emplace_back(CDNSSeedData("bitcoinforks.org", "testnet-seed-bch.bitcoinforks.org", true));
        // BU seeder
        vSeeds.emplace_back(CDNSSeedData("bitcoinunlimited.info", "testnet-seed.bitcoinunlimited.info", true));
        // BCHD
        vSeeds.emplace_back(CDNSSeedData("bchd.cash", "testnet-seed.bchd.cash", true));
        // Loping.net
        vSeeds.emplace_back(CDNSSeedData("loping", "seed.tbch.loping.net", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        cashaddrPrefix = "bchtest";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        // clang-format off
        checkpointData = CCheckpointData();
        MapCheckpoints &checkpoints = checkpointData.mapCheckpoints;
        checkpoints[546]     = uint256S("0x000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70");
        // August 1st 2017 CASH fork (UAHF) activation block
        checkpoints[1155876] = uint256S("0x00000000000e38fef93ed9582a7df43815d5c2ba9fd37ef70c9a0ea4a285b8f5");
        // Nov, 13th 2017. DAA activation block.
        checkpoints[1188697] = uint256S("0x0000000000170ed0918077bde7b4d36cc4c91be69fa09211f748240dabe047fb");
        // May 15th 2018, re-enabling opcodes, max block size 32MB
        checkpoints[1233070] = uint256S("0x0000000000000253c6201a2076663cfe4722e4c75f537552cc4ce989d15f7cd5");
        // Nov 15th 2018, CHECKDATASIG, ctor
        checkpoints[1267996] = uint256S("0x00000000000001fae0095cd4bea16f1ce8ab63f3f660a03c6d8171485f484b24");
        // May 15th 2019, Schnorr + segwit recovery activation block
        checkpoints[1303885] = uint256S("0x00000000000000479138892ef0e4fa478ccc938fb94df862ef5bde7e8dee23d3");
        // Nov 15th 2019 activate Schnorr Multisig, minimal data
        checkpoints[1341712] = uint256S("0x00000000fffc44ea2e202bd905a9fbbb9491ef9e9d5a9eed4039079229afa35b");
        // May 15th 2020 activate op_reverse, SigCheck
        checkpoints[1378461] = uint256S("0x0000000099f5509b5f36b1926bcf82b21d936ebeadee811030dfbbb7fae915d7");
        // Nov 15th 2020 new aserti3-2d DAA
        checkpoints[1421482] = uint256S("0x0000000023e0680a8a062b3cc289a4a341124ce7fcb6340ede207e194d73b60a");

        // clang-format on
        // Data as of block
        checkpointData.nTimeLastCheckpoint = 1573827462;
        // * total number of transactions between genesis and last checkpoint
        checkpointData.nTransactionsLastCheckpoint = 57494631;
        // * estimated number of transactions per day after checkpoint (~1.6 TPS)
        checkpointData.fTransactionsPerDay = 140000;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams
{
public:
    CRegTestParams()
    {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 0; // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 1000; // BIP34 has activated on regtest (Used in rpc activation tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.BIP68Height = 576; // BIP68, 112, 113 has activated
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        // The half life for the ASERT DAA. For every (nASERTHalfLife) seconds behind schedule the blockchain gets,
        // difficulty is cut in half. Doubled if blocks are ahead of schedule.
        // Two days
        consensus.nASERTHalfLife = 2 * 24 * 60 * 60;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999LL;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].windowsize = 144;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 108; // 75% of 144

        // Hard fork is always enabled on regtest.
        consensus.uahfHeight = 0;
        // Nov, 13 hard fork is always on on regtest.
        consensus.daaHeight = 0;
        // May, 15 2018 hard fork is always active on regtest
        consensus.may2018Height = 0;
        // Nov, 15 2018 hard fork is always active on regtest
        consensus.nov2018Height = 0;
        // May, 15 2019 hard fork
        consensus.may2019Height = 0;
        // Nov, 15 2019 hard fork is always active on regtest
        consensus.nov2019Height = 0;
        // May, 15 2020 hard fork
        consensus.may2020Height = 0;
        // Nov 15, 2020 12:00:00 UTC protocol upgrade¶
        consensus.nov2020ActivationTime = NOV2020_ACTIVATION_TIME;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        pchCashMessageStart[0] = 0xda;
        pchCashMessageStart[1] = 0xb5;
        pchCashMessageStart[2] = 0xbf;
        pchCashMessageStart[3] = 0xfa;
        nDefaultPort = DEFAULT_REGTESTNET_PORT;
        nPruneAfterHeight = 1000;
        nDefaultExcessiveBlockSize = DEFAULT_EXCESSIVE_BLOCK_SIZE;
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE_REGTEST;
        nDefaultMaxBlockMiningSize = DEFAULT_BLOCK_MAX_SIZE;

        genesis = CreateGenesisBlock(1296688602, 2, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // assert(consensus.hashGenesisBlock ==
        //        uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
        // assert(
        //     genesis.hashMerkleRoot ==
        //     uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear(); //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of(0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")),
            0, 0, 0};
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        cashaddrPrefix = "bchreg";
    }
};
static CRegTestParams regTestParams;

/**
 * Testnet (v4)
 */
class CTestNet4Params : public CChainParams
{
public:
    CTestNet4Params()
    {
        strNetworkID = "test4";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 1;
        consensus.BIP34Height = 2;
        consensus.BIP34Hash = uint256S("00000000b0c65b1e03baace7d5c093db0d6aac224df01484985ffd5e86a1a20c");
        consensus.BIP65Height = 3;
        consensus.BIP66Height = 4;
        consensus.BIP68Height = 5;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        // The half life for the ASERT DAA. For every (nASERTHalfLife) seconds behind schedule the blockchain gets,
        // difficulty is cut in half. Doubled if blocks are ahead of schedule.
        // One hour
        consensus.nASERTHalfLife = 60 * 60;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].windowsize = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512; // 75% of 2016

        // August 1, 2017 hard fork
        consensus.uahfHeight = 6;

        // November 13, 2017 hard fork
        consensus.daaHeight = 3000;

        // May, 15 2018 hard fork
        consensus.may2018Height = 4000;

        // November 15, 2018 protocol upgrade
        consensus.nov2018Height = 4000;

        // May, 15 2019 hard fork
        consensus.may2019Height = 5000;

        // Nov, 15 2019 hard fork
        consensus.nov2019Height = 5000;

        // May, 15 2020 hard fork
        // NOTE: Due to BCHN having completely removed the historical sig-ops counting code
        //       the May 2020 height must be set to genesis in order to synchronize all blocks
        //       using the post May 2020 hard fork sigchecks code
        consensus.may2020Height = 0;

        // Nov 15, 2020 12:00:00 UTC protocol upgrade
        consensus.nov2020ActivationTime = 1605441600;

        pchMessageStart[0] = 0xcd;
        pchMessageStart[1] = 0x22;
        pchMessageStart[2] = 0xa7;
        pchMessageStart[3] = 0x92;
        pchCashMessageStart[0] = 0xe2;
        pchCashMessageStart[1] = 0xb7;
        pchCashMessageStart[2] = 0xda;
        pchCashMessageStart[3] = 0xaf;
        nDefaultPort = DEFAULT_TESTNET4_PORT;
        nPruneAfterHeight = 1000;
        nDefaultExcessiveBlockSize = DEFAULT_EXCESSIVE_BLOCK_SIZE_TESTNET4;
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE_REGTEST;
        nDefaultMaxBlockMiningSize = DEFAULT_BLOCK_MAX_SIZE_TESTNET4;

        genesis = CreateGenesisBlock(1597811185, 114152193, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // assert(consensus.hashGenesisBlock ==
        //        uint256S("0x000000001dd410c49a788668ce26751718cc797474d3152a5fc073dd44fd9f7b"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back(CDNSSeedData("bitcoinforks.org", "testnet4-seed-bch.bitcoinforks.org", true));
        vSeeds.emplace_back(CDNSSeedData("toom.im", "testnet4-seed-bch.toom.im", true));
        vSeeds.emplace_back(CDNSSeedData("loping.net", "seed.tbch4.loping.net", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        cashaddrPrefix = "bchtest";
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test4, pnSeed6_test4 + ARRAYLEN(pnSeed6_test4));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        // clang-format off
        checkpointData = CCheckpointData();
        MapCheckpoints &checkpoints = checkpointData.mapCheckpoints;
        checkpoints[     0] = uint256S("0x000000001dd410c49a788668ce26751718cc797474d3152a5fc073dd44fd9f7b");
        checkpoints[  5000] = uint256S("0x000000009f092d074574a216faec682040a853c4f079c33dfd2c3ef1fd8108c4");
        // Nov 15th, 2020 new aserti3-2d DAA
        checkpoints[ 16845] = uint256S("0x00000000fb325b8f34fe80c96a5f708a08699a68bbab82dba4474d86bd743077");
        // clang-format on

        // Data as of block
        // 0000000019df558b6686b1a1c3e7aee0535c38052651b711f84eebafc0cc4b5e
        // (height 5677)
        checkpointData.nTimeLastCheckpoint = 1599886634;
        checkpointData.nTransactionsLastCheckpoint = 7432;
        checkpointData.fTransactionsPerDay = 1.3;
    }
};

static CTestNet4Params testNet4Params;

/**
 * Scaling Network
 */
class CScaleNetParams : public CChainParams
{
public:
    CScaleNetParams()
    {
        strNetworkID = "scale";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP16Height = 1;
        consensus.BIP34Height = 2;
        consensus.BIP34Hash = uint256S("00000000c8c35eaac40e0089a83bf5c5d9ecf831601f98c21ed4a7cb511a07d8");
        consensus.BIP65Height = 3;
        consensus.BIP66Height = 4;
        consensus.BIP68Height = 5;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;

        // The half life for the ASERT DAA. For every (nASERTHalfLife) seconds behind schedule the blockchain gets,
        // difficulty is cut in half. Doubled if blocks are ahead of schedule.
        // Two days
        consensus.nASERTHalfLife = 2 * 24 * 60 * 60;
        // REVISIT: Not sure if the following are correct for ScaleNet (copied from TestNet4)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].windowsize = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].threshold = 1512; // 75% of 2016

        // August 1, 2017 hard fork
        consensus.uahfHeight = 6;

        // November 13, 2017 hard fork
        consensus.daaHeight = 3000;

        // May, 15 2018 hard fork
        consensus.may2018Height = 4000;

        // November 15, 2018 protocol upgrade
        consensus.nov2018Height = 4000;

        // May, 15 2019 hard fork
        consensus.may2019Height = 5000;

        // Nov, 15 2019 hard fork
        consensus.nov2019Height = 5000;

        // May, 15 2020 hard fork
        // NOTE: Due to BCHN having completely removed the historical sig-ops counting code
        //       the May 2020 height must be set to genesis in order to synchronize all blocks
        //       using the post May 2020 hard fork sigchecks code
        // NOTE: Specifically in scalenet there are several blocks in the 4000-6000 height range
        //       that fail the historical sig-ops count check but pass the May 2020 sigchecks code
        consensus.may2020Height = 0;

        // Nov 15, 2020 12:00:00 UTC protocol upgrade
        consensus.nov2020ActivationTime = NOV2020_ACTIVATION_TIME;

        pchMessageStart[0] = 0xba;
        pchMessageStart[1] = 0xc2;
        pchMessageStart[2] = 0x2d;
        pchMessageStart[3] = 0xc4;
        pchCashMessageStart[0] = 0xc3;
        pchCashMessageStart[1] = 0xaf;
        pchCashMessageStart[2] = 0xe1;
        pchCashMessageStart[3] = 0xa2;
        nDefaultPort = DEFAULT_SCALENET_PORT;
        nPruneAfterHeight = 10000;
        nDefaultExcessiveBlockSize = DEFAULT_EXCESSIVE_BLOCK_SIZE_SCALENET;
        nMinMaxBlockSize = MIN_EXCESSIVE_BLOCK_SIZE;
        nDefaultMaxBlockMiningSize = DEFAULT_BLOCK_MAX_SIZE_SCALENET;

        genesis = CreateGenesisBlock(1598282438, -1567304284, 0x1d00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // assert(
        //     consensus.hashGenesisBlock ==
        //     uint256S("00000000e6453dc2dfe1ffa19023f86002eb11dbb8e87d0291a4599f0430be52"));
        // assert(genesis.hashMerkleRoot ==
        // uint256S("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back(CDNSSeedData("bitcoinforks.org", "scalenet-seed-bch.bitcoinforks.org", true));
        vSeeds.emplace_back(CDNSSeedData("toom.im", "scalenet-seed-bch.toom.im", true));
        vSeeds.emplace_back(CDNSSeedData("loping.net", "seed.sbch.loping.net", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        cashaddrPrefix = "bchtest";
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_scalenet, pnSeed6_scalenet + ARRAYLEN(pnSeed6_scalenet));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        // clang-format off
        checkpointData = CCheckpointData();
        MapCheckpoints &checkpoints = checkpointData.mapCheckpoints;
        checkpoints[     0] = uint256S("0x00000000e6453dc2dfe1ffa19023f86002eb11dbb8e87d0291a4599f0430be52");
        checkpoints[    45] = uint256S("0x00000000d75a7c9098d02b321e9900b16ecbd552167e65683fe86e5ecf88b320");
        // scalenet periodically reorgs to height 10,000
        checkpoints[ 10000] = uint256S("0x00000000b711dc753130e5083888d106f99b920b1b8a492eb5ac41d40e482905");

        // clang-format on

        // Data as of block
        // REVISIT: Is below data correct? This is based on what BCHN merged
        checkpointData.nTimeLastCheckpoint = 0;
        checkpointData.nTransactionsLastCheckpoint = 0;
        checkpointData.fTransactionsPerDay = 0;
    }
};

static CScaleNetParams scaleNetParams;

CChainParams *pCurrentParams = 0;

const CChainParams &Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(const std::string &chain)
{
    if (chain == CBaseChainParams::MAIN)
        return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
        return testNetParams;
    else if (chain == CBaseChainParams::TESTNET4)
        return testNet4Params;
    else if (chain == CBaseChainParams::SCALENET)
        return scaleNetParams;
    else if (chain == CBaseChainParams::REGTEST)
        return regTestParams;
    else if (chain == CBaseChainParams::UNL)
        return unlParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string &network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

// bip135 begin
/**
 * Return true if a deployment is considered to be configured for the network.
 * Deployments with a zero-length name, or a windowsize or threshold equal to
 * zero are not considered to be configured, and will be reported as 'unknown'
 * if signals are detected for them.
 * Unconfigured deployments can be ignored to save processing time, e.g.
 * in ComputeBlockVersion() when computing the default block version to emit.
 */
bool IsConfiguredDeployment(const Consensus::Params &consensusParams, const int bit)
{
    DbgAssert(bit >= 0 && bit <= (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS, return false);

    const Consensus::ForkDeployment *vdeployments = consensusParams.vDeployments;
    const struct ForkDeploymentInfo &vbinfo = VersionBitsDeploymentInfo[bit];

    if (strlen(vbinfo.name) == 0)
        return false;

    return (vdeployments[bit].windowsize != 0 && vdeployments[bit].threshold != 0);
}

/**
 * Return a string representing CSV-formatted deployments for the network.
 * Only configured deployments satisfying IsConfiguredDeployment() are included.
 */
const std::string NetworkDeploymentInfoCSV(const std::string &network)
{
    const Consensus::Params &consensusParams = Params(network).GetConsensus();
    const Consensus::ForkDeployment *vdeployments = consensusParams.vDeployments;

    std::string networkInfoStr;
    networkInfoStr = "# deployment info for network '" + network + "':\n";

    for (int bit = 0; bit < Consensus::MAX_VERSION_BITS_DEPLOYMENTS; bit++)
    {
        const struct ForkDeploymentInfo &vbinfo = VersionBitsDeploymentInfo[bit];
        if (IsConfiguredDeployment(consensusParams, bit))
        {
            networkInfoStr += network + ",";
            networkInfoStr += std::to_string(bit) + ",";
            networkInfoStr += std::string(vbinfo.name) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].nStartTime) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].nTimeout) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].windowsize) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].threshold) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].minlockedblocks) + ",";
            networkInfoStr += std::to_string(vdeployments[bit].minlockedtime) + ",";
            networkInfoStr += (vbinfo.gbt_force ? "true" : "false");
            networkInfoStr += "\n";
        }
    }
    return networkInfoStr;
}

/**
 * Return a modifiable reference to the chain params, to be updated by the
 * CSV deployment data reading routine.
 */
CChainParams &ModifiableParams()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}
// bip135 end
