// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017 PM-Tech
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"
#include <cmath>

static const int64_t nDiffChangeTarget = 56000; // Patch effective @ block 56000

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    int nHeight = pindexLast->nHeight + 1;
    bool fNewDifficultyProtocol = (nHeight >= nDiffChangeTarget);

    if (fNewDifficultyProtocol || params.fPowAllowMinDifficultyBlocks) {
        return DigiShield(pindexLast, pblock, params);
    }
    else {

        static const int64_t	            	 BlocksTargetSpacing                          = 60; // 1 minute
        unsigned int                             TimeDaySeconds                               = 60 * 60 * 24;
        int64_t                                  PastSecondsMin                               = TimeDaySeconds * 0.25;
        int64_t                                  PastSecondsMax                               = TimeDaySeconds * 7;
        uint64_t                                 PastBlocksMin                                = PastSecondsMin / BlocksTargetSpacing;
        uint64_t                                 PastBlocksMax                                = PastSecondsMax / BlocksTargetSpacing;
        return KimotoGravityWell(pindexLast, pblock, BlocksTargetSpacing, PastBlocksMin, PastBlocksMax, params);
    }
}

unsigned int DigiShield(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
         }
        return pindexLast->nBits;
    }

    // Litecoin: This fixes an issue where a 51% attack can change difficulty at will.
    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
        blockstogoback = params.DifficultyAdjustmentInterval();

    // Go back by what we want to be 14 days worth of blocks
    const CBlockIndex* pindexFirst = pindexLast;
    for (int i = 0; pindexFirst && i < blockstogoback; i++)
        pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);

	return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}	
	
unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)	
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;

    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);

//DigiShield implementation - thanks to RealSolid & WDC for this code
//amplitude filter - thanks to daft27 for this code
        nActualTimespan = params.nPowTargetTimespan + (nActualTimespan - params.nPowTargetTimespan)/8;
        if (nActualTimespan < (params.nPowTargetTimespan - (params.nPowTargetTimespan/4)) ) nActualTimespan = (params.nPowTargetTimespan - (params.nPowTargetTimespan/4));
        if (nActualTimespan > (params.nPowTargetTimespan + (params.nPowTargetTimespan/2)) ) nActualTimespan = (params.nPowTargetTimespan + (params.nPowTargetTimespan/2));
    // Retarget

        bnNew *= nActualTimespan;
        bnNew /= params.nPowTargetTimespan;

        const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
        if (bnNew > bnPowLimit)
            bnNew = bnPowLimit;

    /// debug print
    LogPrintf("GetNextWorkRequired: DIGISHIELD RETARGET\n");
    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}

unsigned int KimotoGravityWell(const CBlockIndex* pindexLast, const CBlockHeader *pblock, uint64_t TargetBlocksSpacingSeconds, uint64_t PastBlocksMin, uint64_t PastBlocksMax, const Consensus::Params& params)
{
        /* current difficulty formula - kimoto gravity well */
        const CBlockIndex *BlockLastSolved                                         = pindexLast;
        const CBlockIndex *BlockReading                                            = pindexLast;

        uint64_t                              PastBlocksMass                       = 0;
        int64_t                               PastRateActualSeconds                = 0;
        int64_t                               PastRateTargetSeconds                = 0;
        double                                PastRateAdjustmentRatio              = double(1);
        arith_uint256                         PastDifficultyAverage;
        arith_uint256                         PastDifficultyAveragePrev;
        double                                EventHorizonDeviation;
        double                                EventHorizonDeviationFast;
        double                                EventHorizonDeviationSlow;
        const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);

        if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return bnPowLimit.GetCompact(); }

        for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++) {
                if (PastBlocksMax > 0 && i > PastBlocksMax) { break; }
                PastBlocksMass++;

                if (i == 1)        { PastDifficultyAverage.SetCompact(BlockReading->nBits); }
                else             //Testcoin: workaround were to overcome the overflow issue when changing from CBigNum to arith_uint256
                                    if (arith_uint256().SetCompact(BlockReading->nBits) >= PastDifficultyAveragePrev)
                                    PastDifficultyAverage = ((arith_uint256().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev;
                                    else
                                    PastDifficultyAverage = PastDifficultyAveragePrev - ((PastDifficultyAveragePrev - arith_uint256().SetCompact(BlockReading->nBits)) / i);

                PastDifficultyAveragePrev = PastDifficultyAverage;

                PastRateActualSeconds                        = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
                PastRateTargetSeconds                        = TargetBlocksSpacingSeconds * PastBlocksMass;
                PastRateAdjustmentRatio                        = double(1);
                if (PastRateActualSeconds < 0) { PastRateActualSeconds = 0; }
                if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
                PastRateAdjustmentRatio                        = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
                }
                EventHorizonDeviation                        = 1 + (0.7084 * pow((double(PastBlocksMass)/double(144)), -1.228));
                EventHorizonDeviationFast                = EventHorizonDeviation;
                EventHorizonDeviationSlow                = 1 / EventHorizonDeviation;

                if (PastBlocksMass >= PastBlocksMin) {
                        if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast)) { assert(BlockReading); break; }
                }
                if (BlockReading->pprev == NULL) { assert(BlockReading); break; }
                BlockReading = BlockReading->pprev;
        }

        arith_uint256 bnNew(PastDifficultyAverage);

        if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0) {
            LogPrintf("Difficulty Retarget - Kimoto Gravity Well\n");
            bnNew *= PastRateActualSeconds;
            bnNew /= PastRateTargetSeconds;
       }
        if (bnNew > bnPowLimit)
            bnNew = bnPowLimit;

/* debug print (commented out due to spamming logs when the loop above breaks)
   printf("Difficulty Retarget - Kimoto Gravity Well\n");
   printf("PastRateAdjustmentRatio = %g\n", PastRateAdjustmentRatio);
   printf("Before: %08x %s\n", BlockLastSolved->nBits, arith_uint256().SetCompact(BlockLastSolved->nBits).ToString().c_str());
   printf("After: %08x %s\n", bnNew.GetCompact(), bnNew.ToString().c_str());
*/

        return bnNew.GetCompact();
}

