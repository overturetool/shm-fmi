/*
 * FMU_TestsFixture.h
 *
 *  Created on: Oct 6, 2015
 *      Author: parallels
 */

#ifndef FMU_TESTSFIXTURE_H_
#define FMU_TESTSFIXTURE_H_

#include "gtest/gtest.h"

class myTestFixture1: public ::testing::Test {
public:
   myTestFixture1( ) {
       // initialization code here
   }

   void SetUp( ) {
       // code here will execute just before the test ensues
   }

   void TearDown( ) {
       // code here will be called just after the test completes
       // ok to through exceptions from here if need be
   }

   ~myTestFixture1( )  {
       // cleanup any pending stuff, but no exceptions allowed
   }

   // put in any custom data members that you need
};

#endif /* FMU_TESTSFIXTURE_H_ */
