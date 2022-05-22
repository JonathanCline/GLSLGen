#pragma once

/** @file */

#include <cstdlib>
#include <iostream>

#define HUBRIS_BREAK() __debugbreak()
#define HUBRIS_ABORT() { std::cout << "Aborted execution on line " << __LINE__ << " function : " << __FUNCTION__ << '\n'; ::abort(); }
#define HUBRIS_ABORT_M(what) { std::cout << "Aborted execution on line " << __LINE__ << " function : " << __FUNCTION__ << '\n' << " assertion failed : " << what<< '\n'; ::abort(); }

#define HUBRIS_ASSERT(condition) if (!(condition)) { HUBRIS_ABORT_M(#condition); }
