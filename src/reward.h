// Copyright (c) 2018 The XPChain Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef XPCHAIN_REWARD_H
#define XPCHAIN_REWARD_H

#include <amount.h>
#include <consensus/params.h>

CAmount GetBlockReward(int nHeight, const Consensus::Params& consensusParams);

#endif //XPCHAIN_REWARD_H
