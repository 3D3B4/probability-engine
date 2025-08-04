#include "probability_space.h"
#include <iostream>
#include <map>
#include <cassert>
#include <string>
#include <cmath>

constexpr int SHOULD_WORK = 0;
constexpr int SHOULD_FAIL = 1;

constexpr int NORMAL = 0;
constexpr int COMPLEMENT = 1;
constexpr int UNION = 2;
constexpr int INTERSECTION = 3;
constexpr int CONDITIONAL = 4;

const char* expectedBehavior = "Expected behavior in test: ";
const char* unexpectedBehavior = "Unexpected behavior in test: ";

template <typename T>
void testConstructor(const std::map<T, double>& mapping, int expectedOutcome, std::string testName) {
    std::string prefix = "[" + testName + "]: ";
    try {
        ProbabilitySpace<T> ps(mapping);
	if (expectedOutcome == SHOULD_WORK) {
            std::cout << "[SUCCESS]" << prefix << expectedBehavior << testName << "\n";
        } 
	else if (expectedOutcome == SHOULD_FAIL) {
            std::cerr << "[FAILED]" << prefix << unexpectedBehavior << testName << " should be raising exception\n";
	}
	else {
            std::cerr << "[TEST FAILURE]" << prefix << "Expected outcome of test was badly defined or test function is incomplete\n";
        }
    }
    catch (const std::invalid_argument& e) {
        if (expectedOutcome == SHOULD_WORK) {
            std::cerr << "[FAILED ]" << prefix << unexpectedBehavior << testName << " should be working" << std::endl;
	}
	else if (expectedOutcome == SHOULD_FAIL) {
            std::cout << "[SUCCESS]" << prefix << expectedBehavior << testName << std::endl;
	}
	else {
            std::cerr << "[TEST FAILURE]" << prefix << "Expected outcome of test was badly defined or test function is incomplete" << std::endl;
	}
        std::cerr << "\t " << prefix << "Caught exception: " << e.what() << std::endl;
    }
}

template <typename T>
void testProbability(const ProbabilitySpace<T>& ps, const std::set<T>& events, double target, int expectedOutcome, std::string testName, 
		int method, const std::set<T>& eventsB = {}) {
    std::string prefix = "[" + testName + "]: ";
    try {
	double pError = {};

	switch (method) {
            case NORMAL:
                pError = std::abs(ps.probabilityOfSet(events) - target); 
                break;
	    case COMPLEMENT:
		pError = std::abs(ps.complementOfEvent(events) - target);
	        break;
	    case UNION:
		pError = std::abs(ps.unionOfEvents(events, eventsB) - target);
                break;
	    case INTERSECTION:
                pError = std::abs(ps.intersectionOfEvents(events, eventsB) - target);
	        break;
            case CONDITIONAL:
		pError = std::abs(ps.conditionalProbability(events, eventsB) - target);
                break;
	    default:
		pError = std::abs(ps.probabilityOfSet(events) - target);
        }

	if(pError < 1e-9 && expectedOutcome == SHOULD_WORK) {
            std::cout << "[SUCCESS]" << prefix << expectedBehavior << testName << "\n";
        }
        else if (pError >= 1e-9 && expectedOutcome == SHOULD_WORK){
            std::cerr << "[FAILED ]" << prefix << "P(event) should have been " << target << " in " << testName << "\n";
        }
	else if (expectedOutcome == SHOULD_FAIL) {
            std::cerr << "[FAILED ]" << prefix << unexpectedBehavior << testName << ", should have raised an exception, pError=" << pError << "\n";
	}
	else {
            std::cerr << "[TEST FAILURE]" << prefix << "Expected outcome of test was badly defined or test function is incomplete\n";
	}
    }
    catch (const std::invalid_argument& e) {
        if (expectedOutcome == SHOULD_WORK) {
            std::cerr << "[FAILED ]" << prefix << unexpectedBehavior << testName << ", should be working" << std::endl;
        }
	else if (expectedOutcome == SHOULD_FAIL) {
            std::cout << "[SUCCESS]" << prefix << expectedBehavior << testName << std::endl;
	}
	else {
            std::cerr << "[TEST FAILURE]" << prefix << "Expected outcome of test was badly defined or test function is incomplete" << std::endl;
	}
	std::cerr << "\t " << prefix << "Caught exception: " << e.what() << std::endl;
    }
}

int main() { 
    
    std::cout << "----------<Constructor tests>----------" << std::endl;

    std::map<std::string, double> coin_toss = {
            {"heads", 0.5},
            {"tails", 0.5}
    };
    testConstructor(coin_toss, SHOULD_WORK, "fair_coin");

    std::map<int, double> die = {
            {1, 1.0/6.0},
            {2, 1.0/6.0},
	    {3, 1.0/6.0},
            {4, 1.0/6.0},
	    {5, 1.0/6.0},
	    {6, 1.0/6.0}
    };
    testConstructor(die, SHOULD_WORK, "fair_die");

    std::map<std::string, double> negative_probability = {
            {"heads", -0.1},
            {"tails", 1.1}
    };
    testConstructor(negative_probability, SHOULD_FAIL, "negative_probability");

    std::map<int, double> die_sum_neq_1 = {
            {1, 1.0/6.0},
            {2, 1.0/6.0},
	    {3, 1.1/6.0},
            {4, 1.0/6.0},
	    {5, 1.0/6.0},
	    {6, 1.1/6.0}
    };
    testConstructor(die_sum_neq_1, SHOULD_FAIL, "invalid_distribution");
 
    std::cout << "----------<Constructor tests>----------" << std::endl;

    std::cout << "\n\n\n";

    std::cout << "----------<Probability tests>----------" << std::endl;
    
    // Use the simple coin to test the probabilityOfSet method

    ProbabilitySpace<std::string> coin(coin_toss);
    std::set<std::string> empty = {};
    std::set<std::string> heads = {"heads"};
    std::set<std::string> tails = {"tails"};
    std::set<std::string> all   = {"heads", "tails"};
    std::set<std::string> wrong = {"heads", "moose"};
    std::set<std::string> moose = {"moose"};

    testProbability(coin, heads, 0.5, SHOULD_WORK, "P(heads)=0.5", NORMAL);
    testProbability(coin, empty, 0.0, SHOULD_WORK, "P({})=0.0", NORMAL);
    testProbability(coin, all, 1.0, SHOULD_WORK, "P(ALL)=1.0", NORMAL);
    testProbability(coin, wrong, 0.5, SHOULD_FAIL, "non-defined_event", NORMAL);
    testProbability(coin, empty, 1.0, SHOULD_WORK, "P({}^c)=1.0", COMPLEMENT);
    testProbability(coin, heads, 0.5, SHOULD_WORK, "P({heads}^c)=0.5", COMPLEMENT);
    testProbability(coin, all, 0.0, SHOULD_WORK, "P({ALL}^c)=0", COMPLEMENT);
    testProbability(coin, wrong, 0.5, SHOULD_FAIL, "non-defined_event", COMPLEMENT);
    testProbability(coin, heads, 1.0, SHOULD_WORK, "P(heads U tails)=1.0", UNION, tails);
    testProbability(coin, empty, 0.5, SHOULD_WORK, "P({} U tails)=0.5", UNION, tails);
    testProbability(coin, empty, 0.5, SHOULD_FAIL, "non-defined_event", UNION, wrong);
    testProbability(coin, tails, 0.5, SHOULD_WORK, "P(tails n ALL)=0.5", INTERSECTION, all);
    testProbability(coin, all, 0.0, SHOULD_WORK, "P(all n {})=0.0", INTERSECTION, empty);
    testProbability(coin, all, 0.5, SHOULD_FAIL, "P(all n wrong)_should_fail", INTERSECTION, wrong);

    coin.setIgnoreUnknown(true);
    testProbability(coin, wrong, 0.5, SHOULD_WORK, "non-defined_event_with_mode_1", COMPLEMENT);
    testProbability(coin, empty, 0.5, SHOULD_WORK, "non-defined_event_with_mode_2", UNION, wrong);
    testProbability(coin, all, 0.5, SHOULD_WORK, "P(all n wrong)_should_work_with_mode", INTERSECTION, wrong);
    testProbability(coin, all, 0.0, SHOULD_WORK, "P(all n moose)_should_work_with_mode", INTERSECTION, moose);
    coin.setIgnoreUnknown(false);

  
    ProbabilitySpace<int> noppa(die);
    std::set<int> _all = {1, 2, 3, 4, 5, 6};
    std::set<int> _1_2 = {1, 2};
    std::set<int> _3 = {3};
    std::set<int> _4_5_6 = {4, 5, 6};
    std::set<int> _4_5 = {4, 5};
    std::set<int> _empty = {};
    std::set<int> seven = {7};
    std::set<int> many = {4, 5, 6, 7};

    testProbability(noppa, _1_2, 2.0/6.0, SHOULD_WORK, "P({1,2})=1/3", NORMAL);
    testProbability(noppa, _1_2, 1.0/3.0, SHOULD_WORK, "P({1,2}|ALL)=1/3", CONDITIONAL, _all); 
    testProbability(noppa, _4_5, 2.0/3.0, SHOULD_WORK, "P({4,5}|{4,5,6})=2/3", CONDITIONAL, _4_5_6);
    testProbability(noppa, _empty, 0.0, SHOULD_WORK, "P({}|{3})=0", CONDITIONAL, _3);
    testProbability(noppa, _3, 0.0, SHOULD_WORK, "P({3}|{4,5,6})=0", CONDITIONAL, _4_5_6);
    testProbability(noppa, seven, 0.0, SHOULD_FAIL, "P({7}|{3} should fail", CONDITIONAL, _3);
    testProbability(noppa, _3, 0.0, SHOULD_FAIL, "P({3}|{}) should fail", CONDITIONAL, _empty);

    noppa.setIgnoreUnknown(true);
    testProbability(noppa, seven, 0.0, SHOULD_WORK, "p({7}|{3}) should work", CONDITIONAL, _3);
    testProbability(noppa, _4_5, 2.0/3.0, SHOULD_WORK, "p({4,5}|{4,5,6,7}) should work", CONDITIONAL, many);
    noppa.setIgnoreUnknown(false);
    testProbability(noppa, _4_5, 2.0/3.0, SHOULD_FAIL, "p({4,5}|{4,5,6,7}) should fail", CONDITIONAL, many);
    
    std::cout << "----------<Probability tests>----------" << std::endl;


    return 0;
}

