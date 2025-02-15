// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2015-2020 The Bitcoin Unlimited developers
// Copyright (C) 2020 Tom Zander <tomz@freedommail.ch>
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef __cplusplus
#error This header can only be compiled as C++.
#endif

#ifndef BITCOIN_PROTOCOL_H
#define BITCOIN_PROTOCOL_H

#include "netbase.h"
#include "serialize.h"
#include "uint256.h"
#include "version.h"

#include <stdint.h>
#include <string>

#define MESSAGE_START_SIZE 4

/** Message header.
 * (4) message start.
 * (12) command.
 * (4) size.
 * (4) checksum.
 */
class CMessageHeader
{
public:
    typedef unsigned char MessageStartChars[MESSAGE_START_SIZE];

    CMessageHeader(const MessageStartChars &pchMessageStartIn);
    CMessageHeader(const MessageStartChars &pchMessageStartIn, const char *pszCommand, unsigned int nMessageSizeIn);

    std::string GetCommand() const;
    bool IsValid(const MessageStartChars &messageStart) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        READWRITE(FLATDATA(pchMessageStart));
        READWRITE(FLATDATA(pchCommand));
        READWRITE(nMessageSize);
        READWRITE(nChecksum);
    }

    // TODO: make private (improves encapsulation)
public:
    enum
    {
        COMMAND_SIZE = 12,
        MESSAGE_SIZE_SIZE = sizeof(int),
        CHECKSUM_SIZE = sizeof(int),

        MESSAGE_SIZE_OFFSET = MESSAGE_START_SIZE + COMMAND_SIZE,
        CHECKSUM_OFFSET = MESSAGE_SIZE_OFFSET + MESSAGE_SIZE_SIZE,
        HEADER_SIZE = MESSAGE_START_SIZE + COMMAND_SIZE + MESSAGE_SIZE_SIZE + CHECKSUM_SIZE
    };
    char pchMessageStart[MESSAGE_START_SIZE];
    char pchCommand[COMMAND_SIZE];
    unsigned int nMessageSize;
    unsigned int nChecksum;
};

/**
 * Member protocol message types. When adding new message types, don't forget
 * to update allNetMessageTypes in protocol.cpp.
 */
namespace NetMsgType
{
/**
 * The version message provides information about the transmitting node to the
 * receiving node at the beginning of a connection.
 * @see https://bitcoin.org/en/developer-reference#version
 */
extern const char *VERSION;
/**
 * The verack message acknowledges a previously-received version message,
 * informing the connecting node that it can begin to send other messages.
 * @see https://bitcoin.org/en/developer-reference#verack
 */
extern const char *VERACK;
/**
 * The addr (IP address) message relays connection information for peers on the
 * network.
 * @see https://bitcoin.org/en/developer-reference#addr
 */
extern const char *ADDR;
/**
 * The inv message (inventory message) transmits one or more inventories of
 * objects known to the transmitting peer.
 * @see https://bitcoin.org/en/developer-reference#inv
 */
extern const char *INV;
/**
 * The getdata message requests one or more data objects from another node.
 * @see https://bitcoin.org/en/developer-reference#getdata
 */
extern const char *GETDATA;
/**
 * The merkleblock message is a reply to a getdata message which requested a
 * block using the inventory type MSG_MERKLEBLOCK.
 * @since protocol version 70001 as described by BIP37.
 * @see https://bitcoin.org/en/developer-reference#merkleblock
 */
extern const char *MERKLEBLOCK;
/**
 * The getblocks message requests an inv message that provides block header
 * hashes starting from a particular point in the block chain.
 * @see https://bitcoin.org/en/developer-reference#getblocks
 */
extern const char *GETBLOCKS;
/**
 * The getheaders message requests a headers message that provides block
 * headers starting from a particular point in the block chain.
 * @since protocol version 31800.
 * @see https://bitcoin.org/en/developer-reference#getheaders
 */
extern const char *GETHEADERS;
/**
 * The tx message transmits a single transaction.
 * @see https://bitcoin.org/en/developer-reference#tx
 */
extern const char *TX;
/**
 * The headers message sends one or more block headers to a node which
 * previously requested certain headers with a getheaders message.
 * @since protocol version 31800.
 * @see https://bitcoin.org/en/developer-reference#headers
 */
extern const char *HEADERS;
/**
 * The block message transmits a single serialized block.
 * @see https://bitcoin.org/en/developer-reference#block
 */
extern const char *BLOCK;
/**
 * BUIP010 Xtreme Thinblocks: The thinblock message transmits a single serialized thinblock.
 */
extern const char *THINBLOCK;
/**
 * BUIP010 Xtreme Thinblocks: The xthinblock message transmits a single serializexd xthinblock.
 */
extern const char *XTHINBLOCK;
/**
 * BUIP010 Xtreme Thinblocks: The xblocktx message transmits a single serialized xblocktx.
 */
extern const char *XBLOCKTX;
/**
 * BUIP010 Xtreme Thinblocks: The get_xblocktx message transmits a single serialized get_xblocktx.
 */
extern const char *GET_XBLOCKTX;
/**
 * BUIP010 Xtreme Thinblocks: The get_xthin message transmits a single serialized get_xthin.
 */
extern const char *GET_XTHIN;
/**
 * The get_thin message is a request for a thinblock with the full 256 bit tx hashes.
 */
extern const char *GET_THIN;
/**
 * The grapheneblock message transmits a single serialized graphene block.
 */
extern const char *GRAPHENEBLOCK;
/**
 * The graphenetx message transmits a single serialized grblktx.
 */
extern const char *GRAPHENETX;
/**
 * The get_graphenetx message transmits a single serialized get_grblktx.
 */
extern const char *GET_GRAPHENETX;
/**
 * The get_graphene message transmits a single serialized get_grblk.
 */
extern const char *GET_GRAPHENE;
/**
 * The get_graphene_recovery message transmits a single serialized
 * RequestGrapheneReceiverRecover object.
 */
extern const char *GET_GRAPHENE_RECOVERY;
/**
 * The graphene_recovery message transmits a single serialized
 * CGrapheneReceiverRecover object.
 */
extern const char *GRAPHENE_RECOVERY;
/**
 * The mempoolsync message transmits a single serialized get_memsync.
 */
extern const char *MEMPOOLSYNC;
/**
 * The mempoolsynctx message transmits a single serialized get_memsynctx.
 */
extern const char *MEMPOOLSYNCTX;
/**
 * The get_mempoolsync message transmits a single serialized get_memsync.
 */
extern const char *GET_MEMPOOLSYNC;
/**
 * The get_mempoolsynctx message transmits a single serialized get_memsynctx.
 */
extern const char *GET_MEMPOOLSYNCTX;

/**
 * The getaddr message requests an addr message from the receiving node,
 * preferably one with lots of IP addresses of other receiving nodes.
 * @see https://bitcoin.org/en/developer-reference#getaddr
 */
extern const char *GETADDR;
/**
 * The mempool message requests the TXIDs of transactions that the receiving
 * node has verified as valid but which have not yet appeared in a block.
 * @since protocol version 60002.
 * @see https://bitcoin.org/en/developer-reference#mempool
 */
extern const char *MEMPOOL;
/**
 * The ping message is sent periodically to help confirm that the receiving
 * peer is still connected.
 * @see https://bitcoin.org/en/developer-reference#ping
 */
extern const char *PING;
/**
 * The pong message replies to a ping message, proving to the pinging node that
 * the ponging node is still alive.
 * @since protocol version 60001 as described by BIP31.
 * @see https://bitcoin.org/en/developer-reference#pong
 */
extern const char *PONG;
/**
 * The notfound message is a reply to a getdata message which requested an
 * object the receiving node does not have available for relay.
 * @since protocol version 70001.
 * @see https://bitcoin.org/en/developer-reference#notfound
 */
extern const char *NOTFOUND;
/**
 * The filterload message tells the receiving peer to filter all relayed
 * transactions and requested merkle blocks through the provided filter.
 * @since protocol version 70001 as described by BIP37.
 *   Only available with service bit NODE_BLOOM since protocol version
 *   70011 as described by BIP111.
 * @see https://bitcoin.org/en/developer-reference#filterload
 */
extern const char *FILTERLOAD;
/**
 * The filteradd message tells the receiving peer to add a single element to a
 * previously-set bloom filter, such as a new public key.
 * @since protocol version 70001 as described by BIP37.
 *   Only available with service bit NODE_BLOOM since protocol version
 *   70011 as described by BIP111.
 * @see https://bitcoin.org/en/developer-reference#filteradd
 */
extern const char *FILTERADD;
/**
 * The filterclear message tells the receiving peer to remove a previously-set
 * bloom filter.
 * @since protocol version 70001 as described by BIP37.
 *   Only available with service bit NODE_BLOOM since protocol version
 *   70011 as described by BIP111.
 * @see https://bitcoin.org/en/developer-reference#filterclear
 */
extern const char *FILTERCLEAR;
/**
 * The filtersizexthin message tells the receiving peer the maximum xthin bloom
 * filter size that it will accept.
 */
extern const char *FILTERSIZEXTHIN;
/**
 * The reject message informs the receiving node that one of its previous
 * messages has been rejected.
 * @since protocol version 70002 as described by BIP61.
 * @see https://bitcoin.org/en/developer-reference#reject
 */
extern const char *REJECT;
/**
 * Indicates that a node prefers to receive new block announcements via a
 * "headers" message rather than an "inv".
 * @since protocol version 70012 as described by BIP130.
 * @see https://bitcoin.org/en/developer-reference#sendheaders
 */
extern const char *SENDHEADERS;

/**
 * Indicates that a node prefers to receive new block announcements
 * and transactions directly without INVs
 * @since protocol version 80000.
 */
extern const char *XPEDITEDREQUEST;

/**
 * Block or transactions sent without explicit solicitation
 * @since protocol version 80000.
 */
extern const char *XPEDITEDBLK;
/**
 * Block or transactions sent without explicit solicitation
 * @since protocol version 80000.
 */
extern const char *XPEDITEDTXN;

/**
 * Indicates that a node accepts Compact Blocks and provides version and
 * configuration information.
 * @since protocol version 70014.
 * @see https://bitcoin.org/en/developer-reference#sendcmpct
 *
 * NOTE: Compact Blocks is not currently supported by BU.  We only process this
 * message for reporting which of our peers have announced they have CB enabled
 */
extern const char *SENDCMPCT;

/**
 * Cash specific version information extending NetMsgType::VERSION
 * @since protocol version FIXME.
 */
extern const char *EXTVERSION;

extern const char *XUPDATE;

/**
 * Contains a CBlockHeaderAndShortTxIDs object - providing a header and
 * list of "short txids".
 * @since protocol version 70014 as described by BIP 152
 */
extern const char *CMPCTBLOCK;
/**
 * Contains a BlockTransactionsRequest
 * Peer should respond with "blocktxn" message.
 * @since protocol version 70014 as described by BIP 152
 */
extern const char *GETBLOCKTXN;
/**
 * Contains a BlockTransactions.
 * Sent in response to a "getblocktxn" message.
 * @since protocol version 70014 as described by BIP 152
 */
extern const char *BLOCKTXN;

/**
 * Double spend proof
 */
extern const char *DSPROOF;
};


/* Get a vector of all valid message types (see above) */
const std::vector<std::string> &getAllNetMessageTypes();

/** nServices flags */
enum
{
    // NODE_NETWORK means that the node is capable of serving the complete block chain. It is currently
    // set by all Bitcoin Unlimited nodes, and is unset by SPV clients or other peers that just want
    // network services but don't provide them.
    NODE_NETWORK = (1 << 0),
    // NODE_GETUTXO means the node is capable of responding to the getutxo protocol request.
    // Bitcoin Unlimited does not support this but a patch set called Member XT does.
    // See BIP 64 for details on how this is implemented.
    NODE_GETUTXO = (1 << 1),
    // NODE_BLOOM means the node is capable and willing to handle bloom-filtered connections.
    // Bitcoin Unlimited nodes used to support this by default, without advertising this bit,
    // but no longer do as of protocol version 70011 (= NO_BLOOM_VERSION)
    NODE_BLOOM = (1 << 2),

    // Indicates that a node can be asked for blocks and transactions including
    // witness data.
    // BU: Member Unlimitd does not support this (added to display connected node services correctly)
    NODE_WITNESS = (1 << 3),

    // NODE_XTHIN means the node supports Xtreme Thinblocks
    // If this is turned off then the node will not service xthin requests nor
    // make xthin requests
    NODE_XTHIN = (1 << 4),

    // NODE_BITCOIN_CASH means the node supports the BCH chain.  This is intended to be just
    // a temporary service bit until the fork actually happens.  Once the split between BTC
    // and BCH chain is stable it can be removed.
    NODE_BITCOIN_CASH = (1 << 5),

    // NODE_GRAPHENE means the node supports Graphene blocks
    // If this is turned off then the node will not service graphene requests nor
    // make graphene requests
    NODE_GRAPHENE = (1 << 6),

    // Bits 24-31 are reserved for temporary experiments. Just pick a bit that
    // isn't getting used, or one not being used much, and notify the
    // Bitcoin Unlimited devevelopement team. Remember that service bits are just
    // unauthenticated advertisements, so your code must be robust against
    // collisions and other cases where nodes may be advertising a service they
    // do not actually support. Other service bits should be allocated via the
    // BUIP process.

    NODE_WEAKBLOCKS = (1 << 7),

    // NODE_CF indicates the node is capable of serving compact block filters to SPV clients.
    NODE_CF = (1 << 8),

    // NODE_NETWORK_LIMITED means the same as NODE_NETWORK with the limitation
    // of only serving a small subset of the blockchain
    // See BIP159 for details on how this is implemented.
    NODE_NETWORK_LIMITED = (1 << 10),

    // indicates if node is using extversion
    NODE_EXTVERSION = (1 << 11),
};

/** A CService with information about it as peer */
class CAddress : public CService
{
public:
    CAddress();
    explicit CAddress(CService ipIn, uint64_t nServicesIn = NODE_NETWORK);

    void Init();

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        if (ser_action.ForRead())
            Init();
        int nVersion = s.GetVersion();
        if (s.GetType() & SER_DISK)
            READWRITE(nVersion);
        if ((s.GetType() & SER_DISK) || (nVersion >= CADDR_TIME_VERSION && !(s.GetType() & SER_GETHASH)))
            READWRITE(nTime);
        READWRITE(nServices);
        READWRITE(*(CService *)this);
    }

    // TODO: make private (improves encapsulation)
public:
    uint64_t nServices;

    // disk and network only
    unsigned int nTime;
};

/** inv message data */
class CInv
{
public:
    CInv();
    CInv(int typeIn, const uint256 &hashIn);
    CInv(const std::string &strType, const uint256 &hashIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action)
    {
        READWRITE(type);
        READWRITE(hash);
    }

    friend bool operator<(const CInv &a, const CInv &b);

    /// returns true if this inv is one of any of the inv types ever used.
    bool IsKnownType() const;
    const char *GetCommand() const;
    std::string ToString() const;

    // TODO: make private (improves encapsulation)
public:
    int type;
    uint256 hash;
};

enum
{
    MSG_TX = 1,
    MSG_BLOCK,
    // Nodes may always request a MSG_FILTERED_BLOCK/MSG_CMPCT_BLOCK in a getdata, however,
    // MSG_FILTERED_BLOCK/MSG_CMPCT_BLOCK should not appear in any invs except as a part of getdata.
    MSG_FILTERED_BLOCK,
    // BitcoinCore had chosen the same enum for compact blocks as thinblocks. As a result we had to
    // bump MSG_THINBLOCK to a higher value (see below).
    MSG_CMPCT_BLOCK,
    // BUIP010 Xtreme Thinblocks: an Xtreme thin block contains the first 8 bytes of all the tx hashes
    // and also provides the missing transactions that are needed at the other end to reconstruct the block
    MSG_XTHINBLOCK,
    // BUIPXXX Graphene blocks: similar to xtreme thin blocks, a graphene block contains all the transactions
    // hashes in a block and also provides the missing transaction ids that are needed at the other end to
    // reconstruct the block
    MSG_GRAPHENEBLOCK,
    // BUIP010 Xtreme Thinblocks: a thin block contains all the transactions hashes in a block
    // and also provides the missing transactions that are needed at the other end to reconstruct the block.
    //
    // With the introduction of compact block, this is being deprecated in favor of using the get_thin p2p
    // message, which solves the conflict with MSG_THINBLOCK and MSG_CMPCT_BLOCK.
    MSG_THINBLOCK = MSG_CMPCT_BLOCK,

    MSG_DOUBLESPENDPROOF = 0x94a0
};

#endif // BITCOIN_PROTOCOL_H
