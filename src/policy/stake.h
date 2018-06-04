// Copyright (c) 2018 The XPChain Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef XPCHAIN_POLICY_STAKE_H
#define XPCHAIN_POLICY_STAKE_H

#include <primitives/block.h>
#include <primitives/transaction.h>

bool IsProofOfWork(const CBlockHeader& block);
bool IsProofOfStake(const CBlockHeader& block);

bool IsPayToYourselfTx(const CTransaction& tx);

#endif //XPCHAIN_POLICY_STAKE_H
