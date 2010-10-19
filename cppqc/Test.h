#ifndef CPPQC_TEST_H
#define CPPQC_TEST_H

#include "Property.h"

#include <cstddef>
#include <map>
#include <string>
#include <iostream>
#include <ostream>

namespace cppqc {

enum ResultType
{
    QC_SUCCESS, // All tests succeeded
    QC_GAVE_UP, // Failed to generate data
    QC_FAILURE, // A test failed
    QC_NO_EXPECTED_FAILURE // The property was expected to fail but didn't
};

struct Result
{
    ResultType result;
    std::size_t numTests;
    std::multimap<std::size_t, std::string> labels;

    // only used if result is QC_FAILURE
    std::size_t numShrinks;
    std::size_t usedSize;
};

namespace detail {
    struct NullOstream : std::ostream
    {
        NullOstream() : std::ios(0), std::ostream(0) {}
    };
}

template<class Property>
Result quickCheck(const Property &prop = Property(),
        std::size_t maxSuccess = 100, std::size_t maxDiscarded = 0,
        std::size_t maxSize = 0)
{
    detail::NullOstream out;
    return quickCheckOutput(prop, out, maxSuccess, maxDiscarded, maxSize);
}

namespace detail {
    inline std::multimap<std::size_t, std::string> convertLabels(
            const std::map<std::string, std::size_t> &labelsCollected)
    {
        std::multimap<std::size_t, std::string> labels;
        for (std::map<std::string, std::size_t>::const_iterator it =
                labelsCollected.begin(); it != labelsCollected.end(); ++it) {
            labels.insert(std::make_pair(it->second, it->first));
        }
        return labels;
    }

    inline void outputLabels(std::ostream &out, std::size_t numSuccess,
            const std::multimap<std::size_t, std::string> &labels)
    {
        std::size_t cnt = 20;
        for (std::map<std::size_t, std::string>::const_reverse_iterator
                it = labels.rbegin(); it != labels.rend(); --cnt, ++it) {
            if (cnt == 0) {
                out << "  ..." << std::endl;
                break;
            }

            if (it->second != "") {
                out << "  " << (100 * it->first / numSuccess) << "% "
                    << it->second << std::endl;
            }
        }
    }
}

template<class Property>
Result quickCheckOutput(const Property &prop = Property(),
        std::ostream &out = std::cout,
        std::size_t maxSuccess = 100,
        std::size_t maxDiscarded = 0, std::size_t maxSize = 0)
{
    out << "* Checking property " << prop.name() << "..." << std::endl;

    if (maxDiscarded == 0)
        maxDiscarded = maxSuccess * 5;
    if (maxSize == 0)
        maxSize = maxSuccess;

    std::map<std::string, std::size_t> labelsCollected;
    std::size_t numSuccess = 0, numDiscarded = 0, numTrivial = 0;
    RngEngine rng(time(0));
    while (numSuccess < maxSuccess) {
        try {
            std::size_t size = (numSuccess * maxSize + numDiscarded) / maxSuccess;
            typename Property::Input input = prop.generateInput(rng, size);
            bool success;
            try {
                success = prop.checkInput(input);
            } catch (...) {
                out << "Caught exception checking property...\n";
                success = false;
            }
            if (prop.isTrivialForInput(input))
                ++numTrivial;
            ++labelsCollected[prop.classifyInput(input)];

            if (success) {
                ++numSuccess;
            } else {
                if (prop.expect()) {
                    out << "*** Failed! ";
                } else {
                    out << "+++ OK, failed as expected. ";
                }

                out << "Falsifiable after " << numSuccess + 1
                    << (numSuccess == 0 ? " test" : " tests");

                std::size_t numShrinks = 0;
                try {
                    std::pair<std::size_t, typename Property::Input> shrinkRes =
                        prop.shrinkInput(input);
                    numShrinks = shrinkRes.first;
                    if (numShrinks > 0) {
                        out << " and " << numShrinks
                            << (numShrinks == 1 ? " shrink" : " shrinks");
                    }
                    out << " for input:" << std::endl;
                    prop.printInput(out, shrinkRes.second);
                } catch (...) {
                    out << " for input:" << std::endl;
                    prop.printInput(out, input);
                }

                if (prop.expect()) {
                    Result ret;
                    ret.result = QC_FAILURE;
                    ret.numTests = numSuccess + 1;
                    ret.labels = detail::convertLabels(labelsCollected);
                    ret.numShrinks = numShrinks;
                    ret.usedSize = size;
                    return ret;
                } else {
                    Result ret;
                    ret.result = QC_SUCCESS;
                    ret.numTests = numSuccess + 1;
                    ret.labels = detail::convertLabels(labelsCollected);
                    return ret;
                }
            }
        } catch (...) {
            if (++numDiscarded >= maxDiscarded) {
                out << "*** Gave up! Passed only " << numSuccess << " tests."
                    << std::endl;

                Result ret;
                ret.result = QC_GAVE_UP;
                ret.numTests = numSuccess;
                ret.labels = detail::convertLabels(labelsCollected);
                return ret;
            }
        }
    }

    std::multimap<std::size_t, std::string> labels =
        detail::convertLabels(labelsCollected);

    if (prop.expect()) {
        out << "+++ OK, passed " << numSuccess << " tests";
        if (numTrivial != 0)
            out << " (" << (100 * numTrivial / numSuccess) << "% trivial)";
        out << '.' << std::endl;
        detail::outputLabels(out, numSuccess, labels);

        Result ret;
        ret.result = QC_SUCCESS;
        ret.numTests = numSuccess;
        ret.labels = labels;
        return ret;
    } else {
        out << "*** Failed! Expected failure but passed " << numSuccess
            << " tests";
        if (numTrivial != 0)
            out << " (" << (100 * numTrivial / numSuccess) << "% trivial)";
        out << '.' << std::endl;
        detail::outputLabels(out, numSuccess, labels);

        Result ret;
        ret.result = QC_NO_EXPECTED_FAILURE;
        ret.numTests = numSuccess;
        ret.labels = labels;
        return ret;
    }
}

}

#endif
