#include "Record.h"
#include "Utility.h"
#include <fstream>
#include <iostream>

using namespace std;

// Construct a Record object from a file stream in save format.
// Throw Error exception if invalid data discovered in file.
// No check made for whether the Record already exists or not.
// Input string data is read directly into the member variables.
// The record number will be set from the saved data.
Record::Record(std::ifstream& is)
{
    is >> id;
    // Check if stream is good
    // and throw an Error if it is not.
    check_stream_state(is);

    is >> medium;
    check_stream_state(is);

    is >> rating;
    check_stream_state(is);

    // removing a whitespace
    is.get();
    check_stream_state(is);

    getline(is, title);
    check_stream_state(is);
}

// Read in a new rating and set the rating to the new value
// If an integer is not read, or if the rating is not
// between 1 and 5 inclusive, an exception is thrown
void Record::read_and_set_rating()
{
    int rating_ = read_and_check_integer();

    if (rating_ < 1 || rating_ > 5)
        throw Error("Rating is out of range!");

    rating = rating_;
}

// Write a Record's data to a stream in save format with final endl.
// The record ID number is saved.
void Record::save(std::ostream& os) const
{
    os << id << " " << medium << " " << rating << " " << title << endl;
}

// Print a Record's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating,
// title, separated by one space.  If the rating is zero, a 'u' is
// printed instead of the rating.
ostream& operator<<(ostream& os, const Record& record)
{
    os << record.id << ": " << record.medium << " ";

    if (record.rating == 0)
        os << "u ";
    else
        os << record.rating << " ";

    os << record.title << endl;
    return os;
}

// Print a Record pointer's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating,
// title, separated by one space. If the rating is zero, a 'u' is
// printed instead of the rating.
ostream& operator<<(ostream& os, const Record* record)
{
    os << record->id << ": " << record->medium << " ";

    if (record->rating == 0)
        os << "u ";
    else
        os << record->rating << " ";

    os << record->title << endl;
    return os;
}