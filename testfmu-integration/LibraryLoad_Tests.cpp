/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */
#include "gtest/gtest.h"

#include <string>
#include <iostream>

extern "C" {
#include "fmu-loader.h"
}

TEST(LibraryLoad, loadsingle) {
  FMU fmu;
  HMODULE h;

  EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}

TEST(LibraryLoad, loaddouble) {
  FMU fmu;
  HMODULE h;

  EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
  EXPECT_EQ(true, loadDll(FMULIB, &fmu, &h));
}
