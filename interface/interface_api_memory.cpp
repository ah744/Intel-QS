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
#include <iostream>
#include <stdexcept>

#include "qureg/qureg.hpp"

using namespace std;


// Global variables related to Psi-function .malloc/.free routines.
using Type = ComplexDP;
extern QbitRegister<Type> *psi1;
extern bool fPsiAllocated;


unsigned long qumalloc(string args) {

    int num_qubits = 0;

    // Check for an attempt at a memory leak.
    if (fPsiAllocated) {
        cerr << ".malloc called before .free - memory leak detected."<<endl;
        return 1;
    }

    // Convert the argument to an integer number of qubits.
    try
    {
        num_qubits = stoi(args,nullptr,10);

    } catch(const invalid_argument& inv_arg) {
        cerr << ".malloc (size) - size parameter was not an integer."<<endl;
    } catch(const out_of_range& oor_arg) {
        cerr << ".malloc (size) - size parameter was too large to fit in an integer."<<endl;
    }

    // Ensure wavefunction register is in the allowed range.
    if ((num_qubits > 0) && (num_qubits <= 43)) {
        psi1 = new QbitRegister<Type>(num_qubits);

        if (psi1) {
            (*psi1)[0] = 1;
            fPsiAllocated = true;
            cout << "Allocated ["<<num_qubits<<"] qubits."<<endl;
            return 0;
        } 
    }
    else 
    {
        cerr << ".malloc (size) - allocations in the range from 1..43 only."<<endl;
    }
 

    return 1;
}


unsigned long qufree(string args) {

    if (fPsiAllocated) {
        delete psi1;
        psi1 = nullptr;
        fPsiAllocated = false;

        cout << "Qubits successfully free."<<endl;
        return 0;
    }

    cerr << "Harmless attempt to .free before .malloc. Nothing done."<<endl;

    return 1;
}
