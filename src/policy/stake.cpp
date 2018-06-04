// Copyright (c) 2018 The XPChain Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <policy/stake.h>

#include <crypto/sha256.h>
#include <hash.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <pubkey.h>
#include <script/interpreter.h>
#include <script/standard.h>


bool IsProofOfWork(const CBlockHeader& block)
{
    return (!block.IsNull() && block.nNonce > 0 && block.nNonce < 0xFFFFFFFF);
}

bool IsProofOfStake(const CBlockHeader& block)
{
    return (!block.IsNull() && block.nNonce == 0);
}

bool IsPayToYourselfTx(const CTransaction& tx)
{
    if (tx.IsNull() || tx.vin.size() != 1 || tx.vout.size() != 1)
        return false;

    txnouttype type;
    std::vector<std::vector<unsigned char>> solutions;

    if (!Solver(tx.vout[0].scriptPubKey, type, solutions))
        return false;

    //! TX_PUBKEY is rejected because the public key can not be calculated.
    if (solutions.size() == 0 || type == TX_PUBKEY || type == TX_NULL_DATA || type == TX_WITNESS_UNKNOWN)
        return false;

    //! TX_MULTISIG is rejected because the destination can be changed.
    if (solutions.size() > 1 || type == TX_MULTISIG)
        return false;

    //! Reject if signature type is different.
    if ((type == TX_WITNESS_V0_KEYHASH || type == TX_WITNESS_V0_SCRIPTHASH) != tx.HasWitness())
        return false;

    std::vector<unsigned char> destination;

    if (type == TX_PUBKEYHASH || type == TX_SCRIPTHASH) {
        std::vector<std::vector<unsigned char>> stack;
        if (!EvalScript(stack, tx.vin[0].scriptSig, SCRIPT_VERIFY_NONE, BaseSignatureChecker(), SIGVERSION_BASE))
            return false;
        destination = stack.back();
    } else if (type == TX_WITNESS_V0_KEYHASH || type == TX_WITNESS_V0_SCRIPTHASH) {
        destination = tx.vin[0].scriptWitness.stack.back();
    } else {
        return false;
    }

    if (destination.size() == 0)
        return false;

    if (type == TX_PUBKEYHASH || type == TX_WITNESS_V0_KEYHASH) {
        CPubKey pubkey(destination);
        if (!pubkey.IsFullyValid())
            return false;
    }

    std::vector<unsigned char> hash;

    if (type == TX_WITNESS_V0_SCRIPTHASH) {
        CSHA256().Write(destination.data(), destination.size()).Finalize(hash.data());
    } else {
        CHash160().Write(destination.data(), destination.size()).Finalize(hash.data());
    }

    return (hash == solutions[0]);
}
