// Copyright (c) 2018 The XPChain Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <test/test_bitcoin.h>

#include <amount.h>
#include <chainparams.h>
#include <pow.h>
#include <reward.h>

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(reward_tests, BasicTestingSetup)

static struct {
    int height;
    CAmount coin;
} checklist[] = {
    {     0, 5000000000}, {     1, 5000000000}, {     2, 5000000000}, {     3, 5000000000}, {     4, 5000000000},
    {209998, 5000000000}, {209999, 5000000000}, {210000, 2500000000}, {210001, 2500000000}, {210002, 2500000000},
    {419998, 2500000000}, {419999, 2500000000}, {420000, 1250000000}, {420001, 1250000000}, {420002, 1250000000},
    {629998, 1250000000}, {629999, 1250000000}, {630000,  625000000}, {630001,  625000000}, {630002,  625000000},
};

BOOST_AUTO_TEST_CASE(pow_reward)
{
    const auto params = CreateChainParams(CBaseChainParams::MAIN)->GetConsensus();

    for (unsigned int i = 0; i < sizeof(checklist)/sizeof(*checklist); ++i)
    {
        BOOST_CHECK_EQUAL(GetBlockReward(checklist[i].height, params), checklist[i].coin);
        BOOST_CHECK_EQUAL(GetBlockReward(checklist[i].height, params), GetProofOfWorkReward(checklist[i].height, params));
    }
}

BOOST_AUTO_TEST_SUITE_END()
