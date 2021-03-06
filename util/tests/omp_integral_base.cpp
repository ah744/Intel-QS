//------------------------------------------------------------------------------
// INTEL CONFIDENTIAL
// Copyright 2017 Intel Corporation
//
// The source code contained or described herein and all documents related to
// the source code ("Material") are owned by Intel Corporation or its suppliers
// or licensors. Title to the Material remains with Intel Corporation or its
// suppliers and licensors. The Material contains trade secrets and proprietary
// and confidential information of Intel or its suppliers and licensors.
// The Material is protected by worldwide copyright and trade secret laws and
// treaty provisions. No part of the Material may be used, copied, reproduced,
// modified, published, uploaded, posted, transmitted, distributed, or disclosed
// in any way without Intel's prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or delivery
// of the Materials, either expressly, by implication, inducement, estoppel or
// otherwise. Any license under such intellectual property rights must be
// express and approved by Intel in writing.
//------------------------------------------------------------------------------

/**
 * @file omp_integral_base.cpp
 *
 * This file tests the implementation of OMP wrapper by computing pi using a
 * Reimann sum approach.
 *
 */
#include "openmp_affinity_noomp.hpp"
#include "openmp_affinity_corei7.hpp"
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <stdexcept>
#include <chrono>

#define GET_PI_DEVIATION(v) (M_PI-v)

#ifndef _OPENMP
qhipster::openmp::AffinityNoOmp affinity;
#else
qhipster::openmp::AffinityCoreI7 affinity;
#endif

/**
 */
int main(int argc, char **argv) {

    double integral_start = 0.0,
           integral_end   = 1.0;
    unsigned long N = 4000000000; // number of steps.

    // Compute delta_x of the integral.
    double delta_x = (integral_end - integral_start) / N;
    double pi = 0.0;
    
    const int global_num_threads = 12;

    // Parameter set for the specific architecture.
    //omp_set_num_threads(global_num_threads);
    affinity.set_thread_affinity(global_num_threads);

    // Start performance check.
    auto start = std::chrono::steady_clock::now();
    double sum = 0.0;

#pragma omp parallel
    {
        int threadID = omp_get_thread_num();

        printf("Thread %d\n", threadID);

        // Compute the Riemann sum for the approximate integral.
        #pragma omp for reduction(+:sum)
        for(unsigned long i=0;i<N;i++) {
            double x = (i+0.5) * delta_x;
            sum = sum + (4.0 / (1.0+x*x)); 
        }
    }

    pi = sum * delta_x;


    // End performance check.
    auto end = std::chrono::steady_clock::now();
    auto diff = end-start;

    printf("Result: %.16f w/ error=%.16f Exec Time=(%lu) ms\n",\
               pi,\
               GET_PI_DEVIATION(pi),\
               std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());

    return 1;
}
