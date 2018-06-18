// Copyright (c) 2018 The XPChain Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <reward.h>

#include <pow.h>

CAmount GetBlockReward(int nHeight, const Consensus::Params& consensusParams)
{
    return GetProofOfWorkReward(nHeight, consensusParams);
}
