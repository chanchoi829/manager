// A Record ontains a unique ID number, a rating, and a title
// and medium name as strings.

#ifndef RECORD_H
#define RECORD_H

#include <string>

class Record
{
public:
    // Create a Record object initialized with the supplied values.
    // The rating is set to 0.
    Record(int ID_, const std::string& medium_, const std::string& title_)
    {
        id = ID_;
        medium = medium_;
        title = title_;
        rating = 0;
    }

    // Create a record object with the given ID, medium, string and rating.
    Record(int ID_, const std::string& medium_, const std::string& title_, int rating_)
    {
        id = ID_;
        medium = medium_;
        title = title_;
        rating = rating_;
    }

    // Construct a Record object from a file stream in save format.
    // Throw Error exception if invalid data discovered in file.
    // No check made for whether the Record already exists or not.
    // Input string data is read directly into the member variables.
    // The record number will be set from the saved data.
    Record(std::ifstream& is);

    // These declarations help ensure that Record objects are unique
    Record(const Record&) = delete;  // disallow copy construction
    Record(Record&&) = delete;  // disallow move construction
    Record& operator=(const Record&) = delete;  // disallow copy assignment
    Record& operator=(Record&&) = delete;  // disallow move assignment

    // Accessors
    const std::string get_medium() const
    {
        return medium;
    }
    int get_ID() const
    {
        return id;
    }
    const std::string& get_title() const
    {
        return title;
    }
    int get_rating() const
    {
        return rating;
    }

    // Read in a new rating and set the rating to the new value
    // If an integer is not read, or if the rating is not
    // between 1 and 5 inclusive, an exception is thrown
    void read_and_set_rating();

    // Write a Record's data to a stream in save format with final endl.
    // The record ID number is saved.
    void save(std::ostream& os) const;

    // Custom << operators for a record and a record pointer
    friend std::ostream& operator<<(std::ostream& os, const Record& record);
    friend std::ostream& operator<<(std::ostream& os, const Record* record);

private:
    int id, rating;
    std::string medium, title;
};

// Print a Record's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating,
// title, separated by one space. If the rating is zero, a 'u' is
// printed instead of the rating.
std::ostream& operator<<(std::ostream& os, const Record& record);

// Print a Record pointer's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating,
// title, separated by one space. If the rating is zero, a 'u' is
// printed instead of the rating.
std::ostream& operator<<(std::ostream& os, const Record* record);

#endif