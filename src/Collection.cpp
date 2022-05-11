#include "Collection.h"
#include "Utility.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <utility>

using namespace std;

/* Construct a Collection from an input file stream in save format,
using the record list, restoring all the Record information.
Record list is needed to resolve references to record members.
No check made for whether the Collection already exists or not.
Throw Error exception if invalid data discovered in file.
String data input is read directly into the member variable. */
Collection::Collection(ifstream& is, const Lib_ti_t& lib_ti)
{
    string name_;
    is >> name_;

    check_stream_state(is);

    name = move(name_);

    int num_member;
    is >> num_member;

    check_stream_state_and_value(is, num_member);

    if (num_member != 0) {
        // removing a whitespace
        is.get();

        // Read in members
        for (int i = 0; i < num_member; ++i) {
            check_stream_state(is);

            string title;
            getline(is, title);

            check_stream_state(is);

            // .first contains the iterator and .second contains a boolean
            // indicating whether the item was found or not.
            pair<Lib_ti_iter, bool> iter_bool = lib_binary_search(lib_ti, title);

            if (!iter_bool.second)
                throw Error("Invalid data found in file!");

            member_list.insert(*iter_bool.first);
        }
    }
}

// Construct a Collection by combining Collections c1 and c2 and the
// given name.
Collection::Collection(const Collection& c1, const Collection& c2, string name_)
{
    name = move(name_);

    // Insert c1 and c2's members into the member_list
    for_each(c1.member_list.cbegin(), c1.member_list.cend(), [&](Record* record) { member_list.insert(record); });
    for_each(c2.member_list.cbegin(), c2.member_list.cend(), [&](Record* record) { member_list.insert(record); });
}

// Add the Record, throw exception if there is already a Record
// with the same title.
void Collection::add_member(Record* record_ptr)
{
    if (!member_list.insert(record_ptr).second)
        throw Error("Record is already a member in the collection!");
}

// Return true if the record is present, false if not.
bool Collection::is_member_present(Record* record_ptr) const
{
    if (member_list.find(record_ptr) != member_list.cend())
        return true;
    return false;
}

// Remove the specified Record, throw exception if the record was not found.
void Collection::remove_member(Record* record_ptr)
{
    Lib_ti_iter it = member_list.find(record_ptr);
    if (it == member_list.cend())
        throw Error("Record is not a member in the collection!");
    member_list.erase(it);
}

// Remove old_record in the member_list and insert new_record
// into the member_list. If old_record does not exist in member_list.
// do not do anything and return
void Collection::remove_then_add_member(Record* old_record, Record* new_record)
{
    Lib_ti_iter it = member_list.find(old_record);
    if (it == member_list.cend())
        return;
    member_list.erase(it);
    member_list.insert(new_record);
}

// Write a Collections's data to a stream in save format, with
// endl as specified.
void Collection::save(std::ostream& os) const
{
    os << name << " " << member_list.size() << endl;

    for_each(
        member_list.cbegin(), member_list.cend(), [&](const Record* record) { os << record->get_title() << endl; });
}

// Print the Collection data
std::ostream& operator<<(std::ostream& os, const Collection& collection)
{
    os << "Collection " << collection.name << " contains:";

    if (collection.member_list.empty()) {
        os << " None" << endl;
        return os;
    }

    os << endl;

    // Print each member's/record's information
    ostream_iterator<Record*> out_it(cout);
    copy(collection.member_list.cbegin(), collection.member_list.cend(), out_it);
    return os;
}