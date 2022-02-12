#ifndef UTILITY_H
#define UTILITY_H

#include "Record.h"
#include <utility>
#include <set>
#include <string>

// Utility functions, constants, and classes used by 
// more than one other modules 

// Functor used for ordering records in an alphabetical order.
struct Title_compare {
	bool operator()(const Record* r1, const Record* r2) const {
		return r1->get_title() < r2->get_title();
	}
};

using Lib_ti_t = std::set<Record*, Title_compare>;
using Lib_ti_iter = std::set<Record*, Title_compare>::iterator;

// a simple class for error exceptions - msg points to a 
// C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
	{}
	const char* const msg;
};

// Functor for heterogeneous lookups for Record pointers.
struct Title_hetero_compare {
	bool operator()(const Record* r1, const std::string& title) const {
		return r1->get_title() < title;
	}
};

// Perform lower_bound on the library to look for a Record
// Return a pair whose first member is an iterator to a member
// of the vector while second member is a bool indicating whether the
// Record was found or not.
std::pair<Lib_ti_iter, bool> lib_binary_search(
	const Lib_ti_t& lib_ti, const std::string& title);

// Read an integer and throw an Error if it is not an integer.
int read_and_check_integer();

// Check if stream is good and throw an Error if it is not.
void check_stream_state(std::ifstream& is);

// Check if stream is good and value is not negative. Throw an Error
// if the stream is not good or if the value is negative.
void check_stream_state_and_value(std::ifstream& is, int value);

#endif