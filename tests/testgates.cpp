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

#include "qureg/qureg.hpp"



int main(int argc, char **argv)
{
  openqu::mpi::Environment env(argc, argv);
  if (env.is_usefull_rank() == false) return 0;
  int myid = env.rank();


  int nqbits = 3;
  std::size_t tmpSize = 0;
  if(argc != 2)
  {
     fprintf(stderr, "usage: %s <nqbits> \n", argv[0]);
     exit(1);
  }
  else
  {
     nqbits = atoi(argv[1]);
  }

  
  TM2x2<ComplexDP> G;
  G(0, 0) = {0.592056606032915, 0.459533060553574}; 
  G(0, 1) = {-0.314948020757856, -0.582328159830658};
  G(1, 0) = {0.658235557641767, 0.070882241549507}; 
  G(1, 1) = {0.649564427121402, 0.373855203932477};


  // generate random pairs for control qubits
  std::vector<std::pair<unsigned, unsigned>> qpair;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> qubit1(0, nqbits - 1);
  std::uniform_int_distribution<int> qubit2(0, nqbits - 1);
  unsigned i = 0;
  while (i < 20) {
     unsigned q1 = qubit1(generator);
     unsigned q2 = qubit2(generator);
     if (q1 != q2)
     {
        qpair.push_back(std::make_pair(q1, q2));
        i++;
     }
  }



  QbitRegister<ComplexDP> psi1(nqbits, "rand", -1);
  QbitRegister<ComplexDP> psi2(nqbits, "rand", -1);
  QbitRegister<ComplexDP> psi3(nqbits, "rand", -1);
  {
    // no specialization
    psi1.EnbStat();
    for (auto &p : qpair) {
       psi1.applyControlled1QubitGate(p.first, p.second, G);
    }
    
    for(int pos = 0; pos < nqbits; pos++) {
       psi1.apply1QubitGate(pos, G);
    }
    psi1.GetStat();
  }

  {
    // with specialization
    psi2.specializeon();
    psi2.EnbStat();
    for (auto &p : qpair) {
       psi2.applyControlled1QubitGate(p.first, p.second, G);
    }

    for(int pos = 0; pos < nqbits; pos++) {
       psi2.apply1QubitGate(pos, G);
    }
    psi2.GetStat();
  }

  double e = psi2.maxabsdiff(psi1);
  if (myid == 0) printf("e = %lf\n", e);

}
