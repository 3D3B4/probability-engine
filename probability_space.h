#include <map>
#include <stdexcept>
#include <cmath>
#include <set>
#include <algorithm>

// Define the accuracy required for valid probability spaces
// to add up to one
constexpr double EPSILON = 1e-9;


/**
 * @brief
 * 
 * @tparam T The type of outcomes in the probability space
 */
template <typename T>
class ProbabilitySpace {
private:
    std::map<T, double> space;
    std::set<T> events;
    bool ignoreUnknown = false;
    
    /**
     * @brief Get the keys of the given mapping.
     *
     * @param pSpace A mapping from type T to double.
     * @return A set containing the domain of the mapping.
     */
    static std::set<T> getKeys(const std::map<T, double>& pSpace) {
	std::set<T> result;
	std::transform(pSpace.begin(), pSpace.end(),
		       std::inserter(result, result.end()),
		       [](auto pair){ return pair.first; });
	return result;
    }

    void isSubset(const std::set<T>& event) const {
	    
        if (!this->ignoreUnknown && !std::includes(this->events.begin(), this->events.end(),
			   event.begin(), event.end())) {
            throw std::invalid_argument("Event contains outcome not in sample space");
	}
    }

    static void validProbabilitySpace(const std::map<T, double>& mapping) {
        double total = 0.0;
    
        for (const auto& [k, v] : mapping) {
            if( v < 0.0) throw std::invalid_argument("Probabilities must be nonnegative");
            total += v;
        }

        if (std::abs(total - 1.0) > EPSILON)
            throw std::invalid_argument("Probabilities must sum to 1");
    }

    // VERY IMPORTANT TO KEEP isSubset() here, since other methods may rely
    // on the fact that this is checked here!!!
    double probabilityCalculator(const std::set<T>& event) const {
        isSubset(event);
	double total = 0.0;

	for (const auto& outcome : event) {
	    auto it = space.find(outcome);
	    bool eventExists = (it != space.end());
	    if (!ignoreUnknown && !eventExists) {
	        throw std::invalid_argument("Event contains outcome not in sample space");
            }
	    else if (eventExists) {
	        total += it->second;
	    }
	}

	return total;
    }

    // No need to check subset, since it is handled in probabilityCalculator
    double complement(const std::set<T>& event) const {
        return 1.0 - probabilityCalculator(event);
    }

    double unionEvents(const std::set<T>& eventA, const std::set<T>& eventB) const {
        isSubset(eventA);
	isSubset(eventB);
	std::set<T> unioned;
	
	std::set_union(
            eventA.begin(), eventA.end(),
	    eventB.begin(), eventB.end(),
	    std::inserter(unioned, unioned.begin())
        );

	return probabilityCalculator(unioned);
    }

    double intersectionEvents(const std::set<T>& eventA, const std::set<T>& eventB) const {
        isSubset(eventA);
	isSubset(eventB);
	std::set<T> intersected;

	std::set_intersection(
            eventA.begin(), eventA.end(),
	    eventB.begin(), eventB.end(),
	    std::inserter(intersected, intersected.begin())
        );

	return probabilityCalculator(intersected);
    }

    // Calculate P(A|B), throw exception if P(B)=0
    double _conditionalProbability(const std::set<T>& eventA, const std::set<T>& eventB) const {
        isSubset(eventA);
        isSubset(eventB);
        double probB = probabilityCalculator(eventB);
        if (probB != 0) {
            double prob_AnB = intersectionEvents(eventA, eventB);
            return prob_AnB/probB;
	} else {
            throw std::invalid_argument("Tried to calculate conditional probability P(A|B) with B s.t. P(B)=0");
	}
        
    }

public:

    /**
     * @brief Construct a probability space from a given mapping P: W -> [0,1]. 
     *
     * @param mapping A mapping P: W -> [0,1] representing all possible singular events in the sample space W
     * and their corresponding probabilities.
     * @throws std::invalid_argumenet if the mapping contains negative probabilities or the probabilites don't
     * sum up to one.
     */
    ProbabilitySpace(std::map<T, double> mapping) {
        validProbabilitySpace(mapping);
	events = getKeys(mapping);
        space = std::move(mapping);
    }

    double probabilityOfSet(const std::set<T>& event) const {
        return probabilityCalculator(event);
    }

    double complementOfEvent(const std::set<T>& event) const {
        return complement(event);
    }

    double unionOfEvents(const std::set<T>& eventA, const std::set<T>& eventB) const {
        return unionEvents(eventA, eventB); 
    }

    double intersectionOfEvents(const std::set<T>& eventA, const std::set<T>& eventB) const {
        return intersectionEvents(eventA, eventB);
    }

    double conditionalProbability(const std::set<T>& eventA, const std::set<T>& eventB) const {
        return _conditionalProbability(eventA, eventB);
    }

    bool getCurrentMode() const {
        return ignoreUnknown;
    }

    void setIgnoreUnknown(bool mode) {
        ignoreUnknown = mode;
    }

};

