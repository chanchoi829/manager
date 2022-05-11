#include "Utility.h"
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

// Perform lower_bound on the library to look for a Record
// Return a pair whose first member is an iterator to a member
// of the vector while second member is a bool indicating whether the
// Record was found or not.
pair<Lib_ti_iter, bool> lib_binary_search(const Lib_ti_t& lib_ti, const string& title)
{
    Title_hetero_compare func;
    Lib_ti_iter iter_found = lower_bound(lib_ti.cbegin(), lib_ti.cend(), title, func);

    // Case when the a matching Record is found
    if (iter_found != lib_ti.cend() && (*iter_found)->get_title() == title) {
        pair<Lib_ti_iter, bool> it_bool(iter_found, true);
        return it_bool;
    }
    // When the Record is not found
    pair<Lib_ti_iter, bool> it_bool(iter_found, false);
    return it_bool;
}

// Read an integer and throw an Error if it is not an integer
int read_and_check_integer()
{
    int value;
    cin >> value;

    if (!cin.good()) {
        cin.clear();
        throw Error("Could not read an integer value!");
    }

    return value;
}

// Check if stream is good and throw an Error if it is not.
void check_stream_state(ifstream& is)
{
    if (!is.good())
        throw Error("Invalid data found in file!");
}

// Check if stream is good and value is not negative. Throw an Error
// if the stream is not good or if the value is negative.
void check_stream_state_and_value(std::ifstream& is, int value)
{
    if (!is.good() || value < 0)
        throw Error("Invalid data found in file!");
}