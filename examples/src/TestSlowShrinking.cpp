/*
 * Copyright (c) 2016, Philipp Classen All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "cppqc.h"

#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

// This simulates a test that is extremely slow. For such tests,
// shrinking can become a problem, as it can take quite a while.
struct PropTestSlowFunction: cppqc::Property<std::vector<int>>
{
    mutable std::atomic<bool> shrinking{false};

    bool check(const std::vector<int> &v) const override
    {
        if (shrinking) {
            std::cout << "Sleeping..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (v.size() >= 4 && (v[3] % 5) == 1) {
            shrinking = true;
            return false;
        }
        return true;
    }
    std::string name() const override
    {
        return "Sorting should be sorted";
    }
};

int main()
{
    cppqc::quickCheckOutput(PropTestSlowFunction());
}
