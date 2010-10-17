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
    std::map<std::string, std::size_t> labels;

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

template<class Property>
Result quickCheckOutput(const Property &prop = Property(),
        std::ostream &out = std::cout,
        std::size_t maxSuccess = 100,
        std::size_t maxDiscarded = 0, std::size_t maxSize = 0)
{
    out << "* Checking property " << prop.name() << " " << std::flush;

    if (maxDiscarded == 0)
        maxDiscarded = maxSuccess * 5;
    if (maxSize == 0)
        maxSize = maxSuccess;

    std::map<std::string, std::size_t> labels; // fixme
    std::size_t numSuccess = 0, numDiscarded = 0;
    RngEngine rng;
    while (numSuccess < maxSuccess) {
        try {
            std::size_t size = (numSuccess * maxSize + numDiscarded) / maxSuccess;
            bool success = prop.test(rng, size);
            if (success) {
                ++numSuccess;
                out << '.' << std::flush;
                // fixme - labels
            } else {
                if (prop.expect()) {
                    out << "\n*** Failed! ";
                } else {
                    out << "\n+++ OK, failed as expected. ";
                }

                out << "Falsifiable (after test " << numSuccess + 1
                    << ")..." << std::endl;

                // fixme - do shrinking
                // fixme - output the generated args

                if (prop.expect()) {
                    Result ret;
                    ret.result = QC_FAILURE;
                    ret.numTests = numSuccess + 1;
                    ret.labels = labels;
                    ret.numShrinks = 0;
                    ret.usedSize = size;
                    return ret;
                } else {
                    Result ret;
                    ret.result = QC_SUCCESS;
                    ret.numTests = numSuccess + 1;
                    ret.labels = labels;
                    return ret;
                }
            }
        } catch (...) {
            out << 'x' << std::flush;
            if (++numDiscarded >= maxDiscarded) {
                out << "\n*** Gave up! Passed only " << numSuccess << " tests"
                    << std::endl;

                Result ret;
                ret.result = QC_GAVE_UP;
                ret.numTests = numSuccess;
                ret.labels = labels;
                return ret;
            }
        }
    }

    if (prop.expect()) {
        out << "\n+++ OK, passed " << numSuccess << " tests" << std::endl;

        Result ret;
        ret.result = QC_SUCCESS;
        ret.numTests = numSuccess;
        ret.labels = labels;
        return ret;
    } else {
        out << "\n*** Failed! Passed " << numSuccess
            << " tests (expected failure)" << std::endl;

        Result ret;
        ret.result = QC_NO_EXPECTED_FAILURE;
        ret.numTests = numSuccess;
        ret.labels = labels;
        return ret;
    }
}

}

#endif
