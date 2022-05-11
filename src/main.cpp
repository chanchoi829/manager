#include "Collection.h"
#include "Record.h"
#include "Utility.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace std::placeholders;

using Cat_t = vector<Collection>;
using Cat_citer = vector<Collection>::const_iterator;

// Functor used for ordering Records in an ascending ID.
struct ID_compare
{
    bool operator()(const Record* r1, const Record* r2) const
    {
        return r1->get_ID() < r2->get_ID();
    }
};

using Lib_id_t = set<Record*, ID_compare>;
using Lib_id_iter = set<Record*, ID_compare>::iterator;

// Find commands
void fr_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&);
void fs_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&);

// Print commands
void pr_command(const Lib_ti_t&, const Lib_id_t& lib_id, const Cat_t&, const int&);
void pc_command(const Lib_ti_t&, const Lib_id_t&, const Cat_t& cat, const int&);
void pL_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&);
void pC_command(const Lib_ti_t&, const Lib_id_t&, const Cat_t& cat, const int&);
void pa_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&);

// List command
void lr_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&);

// Collection stats & combine commands
void cs_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&);
void cc_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&);

// Add commands
void ar_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t&, int& record_id);
void ac_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&);
void am_command(const Lib_ti_t&, const Lib_id_t& lib_id, Cat_t& cat, const int&);

// Modify command
void mr_command(const Lib_ti_t&, const Lib_id_t& lib_id, const Cat_t&, const int&);
void mt_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, const int&);

// Delete commands
void dr_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, const int&);
void dc_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&);
void dm_command(const Lib_ti_t&, const Lib_id_t& lib_id, Cat_t& cat, const int&);

// Function wrappers for cL and cC commands
void cL_command_wrapper(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, int& record_id);
void cC_command_wrapper(const Lib_ti_t& lib_ti, const Lib_id_t& lib_id, Cat_t& cat, const int& record_id);

// Clear commands
void cL_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, int& record_id);
void cC_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&);
void cA_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id);

// Save & restore commands
void sA_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&);
void rA_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id);

// Quit command
void qq_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id);

// Helper functions used for main
void skip_rest_of_line(const char* error_msg);
void print_and_clear_data(const char* error_msg, Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id);

// Helper functions for Collection commands
Collection& find_collection_ref(Cat_t& cat);
const Collection& find_const_collection(const Cat_t& cat);
Cat_citer find_collection_iter(const Cat_t& cat);
Cat_citer find_and_check_if_already_present(const Cat_t& cat, const string& name);
pair<Cat_citer, bool> cat_binary_search(const Cat_t& cat, const string& name);

// Helper functions for Record commands
Lib_id_iter find_record_iter(const Lib_id_t& lib_id);
Lib_ti_iter find_record_iter(const Lib_ti_t& lib_ti);
int read_record_id();
string read_title();

// Title error struct to indicate that there is no need to skip line
struct Title_error
{
    Title_error(const char* msg_ = "")
        : msg(msg_)
    { }
    const char* const msg;
};

int main()
{
    // Map of command function pointers
    const map<string, function<void(Lib_ti_t&, Lib_id_t&, Cat_t&, int& record_id)>> command_map = {{"fr", fr_command},
        {"fs", fs_command},
        {"pr", pr_command},
        {"pc", pc_command},
        {"pL", pL_command},
        {"pC", pC_command},
        {"pa", pa_command},
        {"lr", lr_command},
        {"cs", cs_command},
        {"cc", cc_command},
        {"ar", ar_command},
        {"ac", ac_command},
        {"am", am_command},
        {"mr", mr_command},
        {"mt", mt_command},
        {"dr", dr_command},
        {"dc", dc_command},
        {"dm", dm_command},
        {"cL", cL_command_wrapper},
        {"cC", cC_command_wrapper},
        {"cA", cA_command},
        {"sA", sA_command},
        {"rA", rA_command},
        {"qq", qq_command}};

    // std::set of Record pointers arranged
    // by an alphabetical order
    Lib_ti_t lib_ti;

    // std::set of Record pointers arranged
    // by an ascending order of ID
    Lib_id_t lib_id;

    // std::vector of Collections
    Cat_t cat;

    char first_char, second_char;
    int record_id = 1;

    while (true) {
        cout << "\nEnter command: ";
        cin >> first_char >> second_char;

        // Construct a command from the two chars
        string command;
        command.push_back(first_char);
        command.push_back(second_char);

        try {
            command_map.at(command)(lib_ti, lib_id, cat, record_id);
            if (command == "qq")
                return 0;
        }
        // Skip rest of the line for Errors
        catch (Error& e) {
            skip_rest_of_line(e.msg);
        }
        // When the command does not exist
        catch (out_of_range&) {
            skip_rest_of_line("Unrecognized command!");
        }
        // Do not skip line for title errors
        catch (Title_error& e) {
            cout << e.msg << endl;
        }
        // Clear data and exit for other exceptions
        catch (bad_alloc&) {
            print_and_clear_data("Memory allocation failure!", lib_ti, lib_id, cat, record_id);
            return 0;
        } catch (...) {
            print_and_clear_data("Unknown exception caught!", lib_ti, lib_id, cat, record_id);
            return 0;
        }
    }
}

// Command functions

// Find a Record in the library by reading in the title. When the read-in title
// is invalid or not found, throw a Title_error
void fr_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&)
{
    cout << *find_record_iter(lib_ti);
}

// Custom function object class for fs_command's for_each
class Find_string
{
public:
    Find_string()
        : at_least_one(false)
    { }
    void operator()(const Record* record, const string& str_to_find)
    {
        // Create a copy of the record's title and use transform with
        // tolower to turn it into all lower case
        string temp = record->get_title();
        transform(temp.cbegin(), temp.cend(), temp.begin(), ::tolower);

        // If str_to_find is found within the copy, print the Record's
        // information and set at_least_one to true to indicate that
        // there is at least one matching Record.
        if (temp.find(str_to_find) != string::npos) {
            cout << *record;
            at_least_one = true;
        }
    }
    // Getter function for at_least_one
    bool is_at_least_one()
    {
        return at_least_one;
    }

private:
    bool at_least_one;
};

// Find and print a set of Records that contain a certain string
// The match is case-insensitive. Throw an Error if there is no
// matching Record
void fs_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&)
{
    // Read in a string and turn it into all lower case
    string str_to_find;
    cin >> str_to_find;

    transform(str_to_find.cbegin(), str_to_find.cend(), str_to_find.begin(), ::tolower);

    // Find_string functor finds Records that contain str_to_find
    // It sets is_at_least_one boolean to true if there is at least
    // one matching Record
    Find_string func;
    for_each(lib_ti.cbegin(), lib_ti.cend(), bind(ref(func), _1, str_to_find));

    // No matching record existss
    if (!func.is_at_least_one())
        throw Error("No records contain that string!");
}

// Print a Record's information after reading in a Record's
// ID. When the ID is invalid or not found in the library,
// throw an Error
void pr_command(const Lib_ti_t&, const Lib_id_t& lib_id, const Cat_t&, const int&)
{
    cout << *find_record_iter(lib_id);
}

// Find a Collection by reading in its name and print its
// information. When the read-in Collection is not found in
// the catalog, throw an Error
void pc_command(const Lib_ti_t&, const Lib_id_t&, const Cat_t& cat, const int&)
{
    cout << find_const_collection(cat);
}

// Print the library's entire set of Records
void pL_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&)
{
    if (lib_ti.empty()) {
        cout << "Library is empty" << endl;
        return;
    }

    cout << "Library contains " << lib_ti.size() << " records:" << endl;

    // Print each Record's information
    ostream_iterator<Record*> out_it(cout);
    copy(lib_ti.cbegin(), lib_ti.cend(), out_it);
}

// Print the catalog's entire set of Collections and their members
void pC_command(const Lib_ti_t&, const Lib_id_t&, const Cat_t& cat, const int&)
{
    if (cat.empty()) {
        cout << "Catalog is empty" << endl;
        return;
    }

    cout << "Catalog contains " << cat.size() << " collections:" << endl;

    // Print each Collection's information
    ostream_iterator<Collection> out_it(cout);
    copy(cat.cbegin(), cat.cend(), out_it);
}

// Print the number of Records and Collections
void pa_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&)
{
    cout << "Memory allocations:" << endl;
    cout << "Records: " << lib_ti.size() << endl;
    cout << "Collections: " << cat.size() << endl;
}

// Functor used for lr_command. It returns the order of rating
// in a descending order. When the ratings are equal return the
// order of titles in an alphabetical order
struct Rating_compare
{
    bool operator()(const Record* r1, const Record* r2)
    {
        if (r1->get_rating() == r2->get_rating())
            return r1->get_title() < r2->get_title();
        return r1->get_rating() > r2->get_rating();
    }
};

// Output the contents of the library in a descending order of rating.
// Records with the same rating appear in an alphabetical order by title.
// If the library is empty, simply print a message indicating it is empty.
void lr_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t&, const int&)
{
    if (lib_ti.empty()) {
        cout << "Library is empty" << endl;
        return;
    }

    // Create a temporary set that orders Records in a descending order
    // of Records; when titles are equal they are ordered in an
    // alphabetical order. Then copy the library's Records to the temp set.
    set<Record*, Rating_compare> temp;
    for_each(lib_ti.cbegin(), lib_ti.cend(), [&](Record* record) { temp.insert(record); });

    // Print temp set's Records
    ostream_iterator<Record*> out_it(cout);
    copy(temp.cbegin(), temp.cend(), out_it);
}

// Custom function object class used for cs_command's for_each.
class Collection_stats
{
public:
    Collection_stats()
        : first_time(true)
        , total_member(0)
        , total_at_least_one(0)
        , total_more_than_one(0)
    { }

    // Loop through every Collection in catalog to find the
    // number of Collections in which the Record exists.
    void operator()(Record* record, const Cat_t& cat)
    {
        // exists_in_collection keeps track of the number
        // of Collections in which the Record exists
        int exists_in_collection = 0;

        // the one range for
        for (const Collection& col : cat) {
            // first_time ensures that total_member
            // only counts the total members in the first
            // set of iterations of this for loop
            if (first_time)
                total_member += col.size();

            if (col.is_member_present(record))
                ++exists_in_collection;
        }
        first_time = false;
        if (exists_in_collection > 0)
            ++total_at_least_one;

        if (exists_in_collection > 1)
            ++total_more_than_one;
    }
    // Getter functions for private members
    int get_total_member()
    {
        return total_member;
    }
    int get_total_at_least_one()
    {
        return total_at_least_one;
    }
    int get_total_more_than_one()
    {
        return total_more_than_one;
    }

private:
    bool first_time;
    int total_member;
    int total_at_least_one;
    int total_more_than_one;
};

// Iterate through all Collections to see how many Records exist in
// at least one Collection and in more than one Collection. Also count
// the total number of members in all Collections.
void cs_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&)
{
    Collection_stats func;
    for_each(lib_ti.cbegin(), lib_ti.cend(), bind(ref(func), _1, cat));

    cout << func.get_total_at_least_one() << " out of " << lib_ti.size() << " Records appear in at least one Collection"
         << endl;

    cout << func.get_total_more_than_one() << " out of " << lib_ti.size()
         << " Records appear in more than one Collection" << endl;

    cout << "Collections contain a total of " << func.get_total_member() << " Records" << endl;
}

// Find two Collections from the catalog and combine them to
// create a new Collection. Throw an Error if any of the two
// Collection is not found, and if the new Collection's name
// already exists in the catalog.
void cc_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&)
{
    const Collection& col_first = find_collection_ref(cat);
    const Collection& col_second = find_collection_ref(cat);

    string name;
    cin >> name;

    Cat_citer insert_here = find_and_check_if_already_present(cat, name);

    cout << "Collections " << col_first.get_name() << " and " << col_second.get_name()
         << " combined into new collection " << name << endl;

    // Create a new Collection from the two Collections and add it to the catalog
    cat.emplace(insert_here, Collection(col_first, col_second, name));
}

// Create a Record by reading in its medium and title. When the title
// is invalid, or the library has the Record with the same title already,
// throw a Title_error
void ar_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t&, int& record_id)
{
    string medium;
    cin >> medium;

    string title = read_title();

    // Create a Record with the given medium and string but throw an Error
    // if the Record already exists in the library.
    // Initialize record_ptr here to be able to delete the pointer
    // when an exception is thrown.
    // Set record_ptr to nullptr initially to avoid compiler errors.
    Record* new_record = nullptr;
    try {
        new_record = new Record(record_id, medium, title);
    } catch (...) {
        delete new_record;
        throw;
    }

    if (!lib_ti.insert(new_record).second) {
        delete new_record;
        throw Title_error("Library already has a record with this title!");
    }
    lib_id.insert(new_record);

    cout << "Record " << record_id++ << " added" << endl;
}

// Add a Collection by reading in a name. When the catalog already
// has a Collection with the same name, throw an Error
void ac_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&)
{
    string name;
    cin >> name;

    // Search the catalog and throw an Error if the Collection
    // already exists.
    Cat_citer insert_here = find_and_check_if_already_present(cat, name);

    // Create a new Collection with the given name and
    // add it to the catalog.
    cat.emplace(insert_here, Collection(name));
    cout << "Collection " << name << " added" << endl;
}

// Add a member to a Collection. When the read-in Collection does not
// exist, the read-in Record's ID does not exist, or the Record is already
// a member of the Collection, throw an Error
void am_command(const Lib_ti_t&, const Lib_id_t& lib_id, Cat_t& cat, const int&)
{
    Collection& col = find_collection_ref(cat);

    Record* record_ptr = *find_record_iter(lib_id);
    col.add_member(record_ptr);

    cout << "Member " << record_ptr->get_ID() << " " << record_ptr->get_title() << " added" << endl;
}

// Modify a Record's rating by reading in an ID and the desired rating
// When the ID or rating is invalid, or ID does not exist, throw an Error
void mr_command(const Lib_ti_t&, const Lib_id_t& lib_id, const Cat_t&, const int&)
{
    Record* record_ptr = *find_record_iter(lib_id);

    // read_and_set_rating() reads a rating and sets the record's rating
    // to a new value.
    record_ptr->read_and_set_rating();
    cout << "Rating for record " << record_ptr->get_ID() << " changed to " << record_ptr->get_rating() << endl;
}

// Modify a Record's title. Throw an Error if an integer is not read,
// could not read a title, or there is already a Record with the title.
void mt_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, const int&)
{
    Lib_id_iter record_iter_found = find_record_iter(lib_id);
    string title = read_title();

    // .first contains the iterator and .second contains a boolean
    // indicating whether the item was found or not.
    pair<Lib_ti_iter, bool> iter_bool = lib_binary_search(lib_ti, title);

    // If the title already exists, throw a Title_error
    if (iter_bool.second)
        throw Title_error("Library already has a record with this title!");

    // If the title does not exist, delete the Record in the library(set)
    // and create a new Record with the same property except for the
    // new title and then insert it into the library.
    else {
        // Create a new Record with the new title and insert.
        // Initialize record_ptr here to be able to delete the pointer
        // when an exception is thrown.
        // Set record_ptr to nullptr initially to avoid compiler errors.
        Record* new_record = nullptr;

        try {
            new_record = new Record((*record_iter_found)->get_ID(),
                (*record_iter_found)->get_medium(),
                title,
                (*record_iter_found)->get_rating());
        } catch (...) {
            delete new_record;
            throw;
        }

        // For Collections which have the changed Record's pointer,
        // remove the outdated Record and insert a new one with
        // the new title.
        for_each(cat.begin(), cat.end(), [&](Collection& col) {
            col.remove_then_add_member(*record_iter_found, new_record);
        });

        cout << "Title for record " << (*record_iter_found)->get_ID() << " changed to " << title << endl;

        // Save the pointer temporarily and erase the pointer
        // in the libraries. Then free its memory.
        Record* temp = *record_iter_found;

        lib_ti.erase(lib_ti.find(*record_iter_found));
        lib_id.erase(record_iter_found);
        delete temp;

        lib_ti.insert(iter_bool.first, new_record);
        lib_id.insert(new_record);
    }
}

// Delete a Record in the library by reading in a title and finding it in
// the library When the title is invalid, the title does not exist, or
// the Record is a member of a Collection, throw a Title_error
void dr_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, const int&)
{
    Lib_ti_iter record_iter = find_record_iter(lib_ti);

    // If any Collection is not empty, throw a Title_error
    if (any_of(cat.cbegin(), cat.cend(), [&](const Collection& col) { return col.is_member_present(*record_iter); }))
        throw Title_error("Cannot delete a record that is a member of a collection!");

    cout << "Record " << (*record_iter)->get_ID() << " " << (*record_iter)->get_title() << " deleted" << endl;

    Record* temp = *record_iter;
    lib_id.erase(lib_id.find(*record_iter));
    lib_ti.erase(record_iter);
    delete temp;
}

// Delete a Collection in the catalog by reading in a name. When
// the Collection does not exist, throw an Error
void dc_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&)
{
    Cat_citer col_iter = find_collection_iter(cat);
    cout << "Collection " << col_iter->get_name() << " deleted" << endl;
    cat.erase(col_iter);
}

// Delete a member of a Collection by reading in a name and a Record's ID.
// When the read-in Collection does not exist, or the Record is not a member
// of the Collection, throw an Error
void dm_command(const Lib_ti_t&, const Lib_id_t& lib_id, Cat_t& cat, const int&)
{
    Collection& col = find_collection_ref(cat);
    Record* record_ptr = *find_record_iter(lib_id);

    col.remove_member(record_ptr);
    cout << "Member " << record_ptr->get_ID() << " " << record_ptr->get_title() << " deleted" << endl;
}

// Function wrappers for cL and cC commands
void cL_command_wrapper(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, int& record_id)
{
    cL_command(lib_ti, lib_id, cat, record_id);
    cout << "All records deleted" << endl;
}

void cC_command_wrapper(const Lib_ti_t& lib_ti, const Lib_id_t& lib_id, Cat_t& cat, const int& record_id)
{
    cC_command(lib_ti, lib_id, cat, record_id);
    cout << "All collections deleted" << endl;
}

// Remove all Records from the library. When at least one Record is
// present in the catalog, throw an Error
void cL_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, const Cat_t& cat, int& record_id)
{
    // If any Collection is not empty, throw an Error
    if (any_of(cat.cbegin(), cat.cend(), [](Collection col) { return !col.empty(); }))
        throw Error("Cannot clear all records unless all collections are empty!");

    // Delete all Records
    for_each(lib_id.cbegin(), lib_id.cend(), [](Record* record) { delete record; });
    lib_ti.clear();
    lib_id.clear();
    record_id = 1;
}

// Remove all Collections from the catalog
void cC_command(const Lib_ti_t&, const Lib_id_t&, Cat_t& cat, const int&)
{
    cat.clear();
}

// Remove all Collections from the catalog and all Records from the library
void cA_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id)
{
    cC_command(lib_ti, lib_id, cat, record_id);
    cL_command(lib_ti, lib_id, cat, record_id);
    cout << "All data deleted" << endl;
}

// Save the current library and catalog to a file. When the file cannot be
// opened for writing, throw an Error
void sA_command(const Lib_ti_t& lib_ti, const Lib_id_t&, const Cat_t& cat, const int&)
{
    ofstream myfile;
    string file_name;
    cin >> file_name;

    myfile.open(file_name);
    if (myfile.is_open()) {

        myfile << lib_ti.size() << endl;

        // Save each Record to the specified file first
        for_each(lib_ti.cbegin(), lib_ti.cend(), [&](Record* record) { record->save(myfile); });

        myfile << cat.size() << endl;

        // Save each Collection to the file
        for_each(cat.cbegin(), cat.cend(), [&](Collection collection) { collection.save(myfile); });

        myfile.close();
        cout << "Data saved" << endl;
    } else
        throw Error("Could not open file!");
}

// Load a set of Records and Collections and their members, and set
// the Record ID to the highest ID + 1 of the load file. When the
// file cannot be opened, throw an Error, but the current library and
// catalog do not lose their data. When the file ends early, or has
// a negative or invalid number, throw an error and roll back the
// library and the catalog to the original state so that they do not
// lose any data.
void rA_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id)
{
    string file_name;
    cin >> file_name;

    ifstream myfile(file_name);
    if (myfile.is_open()) {
        // Create backup containers
        Cat_t cat_backup(move(cat));
        Lib_id_t lib_id_backup(move(lib_id));
        Lib_ti_t lib_ti_backup(move(lib_ti));

        int record_id_backup = record_id;
        int biggest_id = 0;

        // Initialize record_ptr here to be able to delete the pointer
        // when an exception is thrown.
        // Set record_ptr to nullptr initially to avoid compiler errors.
        Record* record_ptr = nullptr;
        try {
            int num_record;
            myfile >> num_record;

            check_stream_state_and_value(myfile, num_record);

            // Load records from the file
            for (int i = 0; i < num_record; ++i) {
                record_ptr = new Record(myfile);
                lib_id.insert(record_ptr);
                lib_ti.insert(record_ptr);
                if (record_ptr->get_ID() > biggest_id)
                    biggest_id = record_ptr->get_ID();
            }

            int num_collection;
            myfile >> num_collection;

            check_stream_state_and_value(myfile, num_collection);

            // Load collections from the file
            for (int j = 0; j < num_collection; ++j) {
                Collection new_collection(myfile, lib_ti);
                // Binary search to see where to insert the new Collection
                pair<Cat_citer, bool> iter_bool = cat_binary_search(cat, new_collection.get_name());

                // First element in the pair indicates where to insert
                cat.emplace(iter_bool.first, new_collection);
            }

            // Delete backup Records
            for_each(lib_id_backup.cbegin(), lib_id_backup.cend(), [](Record* record) { delete record; });

            record_id = ++biggest_id;
            myfile.close();
            cout << "Data loaded" << endl;
        } catch (Error& e) {
            // Delete Records read from the file
            for_each(lib_id.cbegin(), lib_id.cend(), [](Record* record) { delete record; });

            // Rollback to the backups
            cat.swap(cat_backup);
            lib_id.swap(lib_id_backup);
            lib_ti.swap(lib_ti_backup);
            record_id = record_id_backup;
            myfile.close();
            throw e;
        } catch (...) {
            delete record_ptr;
            myfile.close();
            throw;
        }
    } else {
        myfile.close();
        throw Error("Could not open file!");
    }
}

// Clear the catalog and library
void qq_command(Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id)
{
    cC_command(lib_ti, lib_id, cat, record_id);
    cL_command(lib_ti, lib_id, cat, record_id);
    cout << "All data deleted\n";
    cout << "Done";
}

// Helper functions used for main

// Print error_msg to cout and skip rest of the line until \n character
void skip_rest_of_line(const char* error_msg)
{
    cout << error_msg << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Print error_msg and clear all data
void print_and_clear_data(const char* error_msg, Lib_ti_t& lib_ti, Lib_id_t& lib_id, Cat_t& cat, int& record_id)
{
    cout << error_msg << endl;
    cA_command(lib_ti, lib_id, cat, record_id);
}

// Helper functions for Collection commands

// Read in a name and attempt to find the name in the given catalog.
// Return an index of the found item but throw an Error if no item is found.
Collection& find_collection_ref(Cat_t& cat)
{
    return cat[find_collection_iter(cat) - cat.cbegin()];
}

// Read in a name and attempt to find the name in the given catalog.
// Return a const reference of the found item but throw an Error if
// no item is found.
const Collection& find_const_collection(const Cat_t& cat)
{
    return cat[find_collection_iter(cat) - cat.cbegin()];
}

// Read in a name and attempt to find the name in the given catalog.
// Return a const_iterator to the found item but throw an Error if
// no item is found.
Cat_citer find_collection_iter(const Cat_t& cat)
{
    string name;
    cin >> name;

    pair<Cat_citer, bool> iter_bool = cat_binary_search(cat, name);
    if (!iter_bool.second)
        throw Error("No collection with that name!");

    return iter_bool.first;
}

// Read in a name, use lower_bound to find a Collection
// and throw an Error if it already exists. Return an iterator
// where a new Collection with the name should be inserted.
Cat_citer find_and_check_if_already_present(const Cat_t& cat, const string& name)
{
    // Search the catalog and throw an Error if the Collection
    // already exists.
    pair<Cat_citer, bool> iter_bool = cat_binary_search(cat, name);

    if (iter_bool.second)
        throw Error("Catalog already has a collection with this name!");

    return iter_bool.first;
}

// Perform lower_bound on the catalog to look for a Collection
// Return a pair whose first member is a const_iterator to a member
// of the vector while second member is a bool indicating whether the
// Collection was found or not.
pair<Cat_citer, bool> cat_binary_search(const Cat_t& cat, const string& name)
{
    Cat_citer iter_found = lower_bound(
        cat.cbegin(), cat.cend(), name, [&](const Collection& c1, const string&) { return c1.get_name() < name; });

    // Case when the a matching Collection is found
    if (iter_found != cat.cend() && iter_found->get_name() == name) {
        pair<Cat_citer, bool> it_bool(iter_found, true);
        return it_bool;
    }
    // When the Collection is not found
    pair<Cat_citer, bool> it_bool(iter_found, false);
    return it_bool;
}

// Helper functions for Record commands

// Read in a Record ID, attempt to find a matching Record in
// the given library, and return an iterator to the matching
// Record. Throw an Error if an integer could not be read, or
// if there is no matching Record in the library.
Lib_id_iter find_record_iter(const Lib_id_t& lib_id)
{
    int id = read_record_id();

    // Use lower_bound to search for a matching record via ID
    // Throw an Error if no matching item is found.
    Lib_id_iter record_iter = lower_bound(
        lib_id.cbegin(), lib_id.cend(), id, [&](const Record* r1, const int) { return r1->get_ID() < id; });

    if (record_iter == lib_id.cend() || (*record_iter)->get_ID() != id)
        throw Error("No record with that ID!");

    return record_iter;
}

// Read in a title, attempt to find a matching Record in the given
// library, and return the matching Record's pointer. Throw
// an Error if a title could not be read, or if there is no
// matching Record in the library.
Lib_ti_iter find_record_iter(const Lib_ti_t& lib_ti)
{
    string title = read_title();

    // Use lower_bound to search for a matching record via title
    // .first contains the iterator and .second contains a boolean
    // indicating whether the item was found or not.
    pair<Lib_ti_iter, bool> iter_bool = lib_binary_search(lib_ti, title);

    // Throw an Error if no matching item is found.
    if (!iter_bool.second)
        throw Title_error("No record with that title!");

    return iter_bool.first;
}

// Read in a Record's ID and throw an Error if an
// integer was not read, or if there is no Record with
// the given ID (it is less than 1).
int read_record_id()
{
    int id_ = read_and_check_integer();

    if (id_ < 1)
        throw Error("No record with that ID!");

    return id_;
}

// Custom functor class for read_title()
class Remove_unneeded_white
{
public:
    Remove_unneeded_white()
        : initial_white(true)
        , add_white(false)
        , white_at_the_end(false)
        , index(0)
    { }
    // Returns a boolean depending on whether we should add
    // a character to a string or not
    bool operator()(const char c)
    {
        bool is_space = (isspace(c));
        // Return true if the char is not a white space
        if (!is_space) {
            // Done traversing through initial white spaces
            initial_white = false;

            // Allowed to add one white space
            add_white = true;

            // There is currently no white at the very end
            white_at_the_end = false;
            ++index;
            return true;
        }
        // Add one white space if it is not an initial white space
        // and if it is allowed to add one white space.
        else if (!initial_white && is_space && add_white) {
            // Not allowed to add more white spaces because
            // one white space is already added
            add_white = false;

            // There is currently a white space at the end
            white_at_the_end = true;

            ++index;
            return true;
        }
        // Don't add this white space.
        else
            return false;
    }
    // Getter functions
    bool only_white()
    {
        return initial_white;
    }
    bool is_end_white()
    {
        return white_at_the_end;
    }
    int get_white_index()
    {
        return index - 1;
    }

private:
    // initial_white indicates if we are reading the
    // initial white spaces.
    bool initial_white;

    // add_white indicates if we should add the white space
    // to the output string
    bool add_white;

    // white_at_the_end indicates if there is a white space
    // at the the end of the output string
    bool white_at_the_end;

    // index keeps track of the last index in the output string
    int index;
};

// Read in a line from the input stream by using the getline function
// and remove unnecessary white spaces from the line. Throw an Error
// if there is only whitespace in the line that was read.
string read_title()
{
    string title_;
    string title_out;

    getline(cin, title_);

    // Remove unnecessary white spaces in title_ and put the trimmed string
    // into title_out
    Remove_unneeded_white func;
    copy_if(title_.cbegin(), title_.cend(), back_inserter(title_out), ref(func));

    // Throw a Title_error if there is only white space
    if (func.only_white())
        throw Title_error("Could not read a title!");

    // If there is a whitespace at the end, erase the white space.
    if (func.is_end_white())
        title_out.erase(title_out.cbegin() + func.get_white_index());

    return title_out;
}