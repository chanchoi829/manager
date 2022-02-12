/* Collections contain a name and a container of members,
represented as pointers to Records.
Collection objects manage their own Record container. 
The container of Records is not available to clients.
*/

#ifndef COLLECTION_H
#define COLLECTION_H

#include "Utility.h"
#include <set>
#include <string>

class Collection{
	public:
		// Construct a collection with the specified name and no members
		Collection(const std::string& name_) {
			name = name_;
		}

		/* Construct a Collection from an input file stream in save format,
		using the record list, restoring all the Record information.
		Record list is needed to resolve references to record members.
		No check made for whether the Collection already exists or not.
		Throw Error exception if invalid data discovered in file.
		String data input is read directly into the member variable. */
		Collection(std::ifstream& is, const Lib_ti_t& lib_ti);

		// Construct a Collection by combining Collections c1 and c2 and 
		//the given name.
		Collection(const Collection& c1, const Collection& c2, std::string name_);

		// Add the Record, throw exception if there is already a Record 
		//with the same title.
		void add_member(Record* record_ptr);

		// Return true if the record is present, false if not.
		bool is_member_present(Record* record_ptr) const;

		// Remove the specified Record, throw exception if the record was not found.
		void remove_member(Record* record_ptr);

		// Remove old_record in the member_list and insert new_record 
		// into the member_list.
		void remove_then_add_member(Record* old_record, Record* new_record);

		// discard all members
		void clear()
			{member_list.clear();}

		// Write a Collections's data to a stream in save format, with endl as 
		//specified.
		void save(std::ostream& os) const;

		// Accessors
		const std::string& get_name() const
			{return name;}
		bool empty() const
			{return member_list.empty();}
		int size() const
			{return member_list.size();}

		friend std::ostream& operator<< (std::ostream& os, 
			const Collection& collection);
		
	private:
		Lib_ti_t member_list;
		std::string name;
};

// Print the Collection data
std::ostream& operator<< (std::ostream& os, const Collection& collection);

#endif