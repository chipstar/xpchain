// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util.h>

//pos: find last block index up to pindex
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, const Consensus::Params& params, bool fProofOfStake)
{
    // TODO: FIXME
    while (pindex && pindex->pprev && !pindex->IsProofOfStake())
    {
        pindex = pindex->pprev;
    }
    return pindex;
}

uint32_t GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params, bool fProofOfStake)
{
    assert(pindexLast != nullptr);
    return fProofOfStake ?
                GetNextWorkRequiredForPos(pindexLast, pblock, params) :
                GetNextWorkRequiredForPow(pindexLast, pblock, params);
}

uint32_t GetNextWorkRequiredForPow(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    int nHeight = pindexLast->nHeight+1;

    const CBlockIndex* pindexFirst = pindexLast;
    arith_uint256 bnTotal {0};
    int i = 0;
    while (pindexFirst && i < params.nPowAveragingWindow) {
        if (!pindexFirst->IsProofOfStake()) {
            arith_uint256 bnTmp;
            bnTmp.SetCompact(pindexFirst->nBits);
            bnTotal += bnTmp;
            ++i;
        }

        pindexFirst = pindexFirst->pprev;
    }

    if (pindexFirst == nullptr) {
        return UintToArith256(params.powLimit).GetCompact();
    }

    arith_uint256 bnAvg {bnTotal / params.nPowAveragingWindow};

    return CalculateNextWorkRequired(bnAvg, pindexLast->GetBlockTime(), pindexFirst->GetBlockTime(), params);
}

uint32_t CalculateNextWorkRequired(arith_uint256 bnAvg, int64_t nLastBlockTime, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    // Limit adjustment step
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    nActualTimespan = params.AveragingWindowTimespan() + (nActualTimespan - params.AveragingWindowTimespan())/4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew {bnAvg};
    bnNew /= params.AveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

uint32_t CalculateNextWorkRequiredOld(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

uint32_t GetNextWorkRequiredForPos(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, params, true);
    if (pindexPrev == nullptr)
        return UintToArith256(params.nInitialHashTargetPoS).GetCompact();

    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, params, true);
    if (pindexPrevPrev == nullptr || pindexPrevPrev->pprev == nullptr)
        return UintToArith256(params.nInitialHashTargetPoS).GetCompact();

    const int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();
    const int64_t nInterval = params.DifficultyAdjustmentIntervalPos();
    arith_uint256 nProofOfWorkLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    bnNew *= ((nInterval - 1) * params.nPosTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * params.nPosTargetSpacing);

    if (bnNew > nProofOfWorkLimit)
        bnNew = nProofOfWorkLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow)
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
