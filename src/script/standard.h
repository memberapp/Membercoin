// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2015-2020 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_STANDARD_H
#define BITCOIN_SCRIPT_STANDARD_H

#include "script/interpreter.h"
#include "uint256.h"

#include <boost/variant.hpp>

#include <stdint.h>

static const bool DEFAULT_ACCEPT_DATACARRIER = true;

class CKeyID;
class CScript;

/** A reference to a CScript: the Hash160 of its serialization (see script.h) */
class CScriptID : public uint160
{
public:
    CScriptID() : uint160() {}
    CScriptID(const CScript &in);
    CScriptID(const uint160 &in) : uint160(in) {}
};

static const unsigned int MAX_OP_RETURN_RELAY = 5120; //! bytes (+1 for OP_RETURN, +2 for the pushdata opcodes)
extern bool fAcceptDatacarrier;
extern unsigned nMaxDatacarrierBytes;

/**
 * Mandatory script verification flags that all new blocks must comply with for
 * them to be valid. (but old blocks may not comply with) Currently just P2SH,
 * but in the future other flags may be added, such as a soft-fork to enforce
 * strict DER encoding.
 *
 * Failing one of these tests may trigger a DoS ban - see CheckInputs() for
 * details.
 */
/* clang-format off */
static const uint32_t MANDATORY_SCRIPT_VERIFY_FLAGS = SCRIPT_VERIFY_P2SH |
                                                      SCRIPT_VERIFY_STRICTENC |
                                                      SCRIPT_ENABLE_SIGHASH_FORKID |
                                                      SCRIPT_VERIFY_LOW_S |
                                                      SCRIPT_VERIFY_NULLFAIL |
                                                      SCRIPT_VERIFY_MINIMALDATA |
                                                      SCRIPT_ENABLE_SCHNORR_MULTISIG;
/* clang-format on */

enum txnouttype
{
    TX_NONSTANDARD,
    // 'standard' transaction types:
    TX_PUBKEY,
    TX_PUBKEYHASH,
    TX_SCRIPTHASH,
    TX_MULTISIG,
    TX_CLTV,
    TX_LABELPUBLIC,
    TX_NULL_DATA,
};

class CNoDestination
{
public:
    friend bool operator==(const CNoDestination &a, const CNoDestination &b) { return true; }
    friend bool operator<(const CNoDestination &a, const CNoDestination &b) { return true; }
};

/**
 * A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a member address
 */
typedef boost::variant<CNoDestination, CKeyID, CScriptID> CTxDestination;

const char *GetTxnOutputType(txnouttype t);

bool Solver(const CScript &scriptPubKey, txnouttype &typeRet, std::vector<std::vector<unsigned char> > &vSolutionsRet);
bool ExtractDestination(const CScript &scriptPubKey, CTxDestination &addressRet);
bool ExtractDestinations(const CScript &scriptPubKey,
    txnouttype &typeRet,
    std::vector<CTxDestination> &addressRet,
    int &nRequiredRet);

const char *GetTxnOutputType(txnouttype t);
bool IsValidDestination(const CTxDestination &dest);

CScript GetScriptForDestination(const CTxDestination &dest);
CScript GetScriptForRawPubKey(const CPubKey &pubkey);
CScript GetScriptForMultisig(int nRequired, const std::vector<CPubKey> &keys);
CScript GetScriptForFreeze(CScriptNum nLockTime, const CPubKey &pubKey);
CScript GetScriptLabelPublic(const std::string &labelPublic);


#endif // BITCOIN_SCRIPT_STANDARD_H
