# Simple Media Manager
Simple Media Manager is a simple C++ program which lets you organize collections of records.
### How to Build and Run
Git clone:
```bash
$ git clone https://github.com/chanchoi829/manager.git
$ cd manager
```

Make and Run:
```bash
$ make
$ ./manager
```

### How to Use Simple Media Manager
When you run the program, it will ask for a two-letter command.
You can enter many two-letter commands at once.
When an error occurs, the program skips rest of the input.
The first letter is an action letter, and the second letter is an object word. 

Action Letters:
```
f - find (for records only)
p - print
m - modify (for rating only)
l - list
a - add
d - delete
c - clear, collection or combine
s - save
r - restore
```

Object Letters:
```
r - an individual record or rating
c - an individual collection
m - member for the add and delete commands
s - string or statistics
t - title
L - the Library - the set of all individual records
C - the Catalog - the set of all individual collections
A - all data - both the Library and the Catalog - for the clear, save and restore commands
a - allocations in the print command (memory information)
```

Possible Parameters:
```
<title> - a title string which is entered with whitespace before, after, and internally, but
is always terminated by a newline character. Case sensitive

<ID> - a record number which must be an integer value

<name> - a collection name which consists of any non-whitespace characters and terminates with
a whitespace character. Case sensitive

<medium> - a medium name which consists of any non-whitespace characters with no embedded 
whitespace characters and terminates with a whitespace character. Case sensitive

<rating> - a rating value which must be an integer value in the range 1 through 5 inclusive.
Initially, ratings are zero which means they are unrated.

<filename> - a file name for which the program's data is to be written to or saved from. No embedded
whitespace characters, entered as a whitespace-delimited string.
```

Possible Commands:
```
fr <title> - find and print the record with the matching title. Case sensitive 
Errors: title could not be read; no record with the title.

fs <string> - find with string. Output all records in the Library whose title contains the string. Case insensitive.
Errors: No records contain the string.

pr <ID> - print the specified record with the matching ID number. 
Errors: can't read an integer, no record with that ID.

pc <name> - print collection - print each record in the collection with the specified name.
Errors: no collection with that name

pL - print all the records in the Library.
Errors: none.

pC - print the Catalog - print all the collections in the Catalog.
Errors: none.

pa - print memory allocations - print the number of records and the number of collections.
Errors: none.

lr - list ratings. Ouput the Library in a descending order of rating.
Errors: None.

cs - collection statistics. Show how many Records appear in at least one Collection, 
how many appear in more than one Collection, and the total of the number of Records 
appearing in all Collections.
Errors: None.

cc <name1> <name2> <new name> - combine collections. Merge collections with names 
<name1> and <name2> to create a new collection with <new name>
Errors: No collections with names <name1> or <name2>; a collection's name is already <new name>

ar <medium> <title> - add a record to the Library.
Errors: Title could not be read; a record with that title is already in the Library.

ac <name> - add a collection with the specified name.
Errors: A collection with that name already exists.

am <name> <ID> - add a record to a specified collection.
Errors: No collection of that name; unable to read an integer; no record with that ID number,
record is already a member of that collection.

mr <ID> <rating> - modify the rating of the specified record with the matching ID.
Errors: unable to read an integer for the ID; unable to read an integer for the rating; rating out of range

mt <ID> <title> - modify the title of a record whose ID number is <ID>
Errors: Unable to read an integer; no record with that ID; could not read a title; 
there is already a record with that title.

dr <title> - delete the specified collection from the Catalog.
Errors: A collection with that name does not exist in the Catalog.

dc <name> - delete the specified collection from the Catalog.
Errors: A collection with that name does not exist in the Catalog.

dm <name> <ID> - delete the specified record from a collection with the given name from the Catalog
Errors: No collection with that name; unable to read an integer; no record with that ID number;
record is not a member of the collection.

cL - clear the Library; destroy all of the records in the Library only if the Catalog is empty.
Errors: There are collections with members

cC - clear the Catalog; destroy all of the collections in the Catalog, and clear the Catalog.
Errors: None.

cA - clear all data: first clear the Catalog like cC and clear the Library like cL.
Errors: None.

sA <filename> - save all data: write the Library and Catalog data to the named file.
Errors: the file cannot be opened for output.

rA <filename> - restore all data - restore the Library and Catalog data from the file.
Errors: the file cannot be opened for input; invalid data is found in the file. If an error occurs
while parsing the file, the Library and the Catalog revert back to the state before rA was called.

qq - clear all data like cA and then terminate.
Errors: none.
```

### Example Usage
```
Enter command: ar DVD Star Wars
Record 1 added

Enter command: ar VHS Pink Flamingos
Record 2 added

Enter command: ar DVD Alien
Record 3 added

Enter command: ar DVD The House
Record 4 added

Enter command: ar DVD It
Record 5 added

Enter command: ac Favorites
Collection Favorites added

Enter command: ac Dirty
Collection Dirty added

Enter command: mr 2 5
Rating for record 2 changed to 5

Enter command: am Favorites 3
Member 3 Alien added

Enter command: am Favorites 1
Member 1 Star Wars added

Enter command: am Dirty 2
Member 2 Pink Flamingos added

Enter command: am Dirty 4
Member 4 The House added

Enter command: fr Pink Flamingos
2: VHS 5 Pink Flamingos

Enter command: pr 1
1: DVD u Star Wars

Enter command: pc Dirty
Collection Dirty contains:
2: VHS 5 Pink Flamingos
4: DVD u The House

Enter command: pL
Library contains 5 records:
3: DVD u Alien
5: DVD u It
2: VHS 5 Pink Flamingos
1: DVD u Star Wars
4: DVD u The House

Enter command: pC
Catalog contains 2 collections:
Collection Dirty contains:
2: VHS 5 Pink Flamingos
4: DVD u The House
Collection Favorites contains:
3: DVD u Alien
1: DVD u Star Wars

Enter command: palrcs
Memory allocations:
Records: 5
Collections: 2

Enter command: 2: VHS 5 Pink Flamingos
3: DVD u Alien
5: DVD u It
1: DVD u Star Wars
4: DVD u The House

Enter command: 4 out of 5 Records appear in at least one Collection
0 out of 5 Records appear in more than one Collection
Collections contain a total of 4 Records

Enter command: mt 4 House
Title for record 4 changed to House

Enter command: cc Favorites Dirty Fun
Collections Favorites and Dirty combined into new collection Fun

Enter command: plpLpC
Unrecognized command!

Enter command: pLpC
Library contains 5 records:
3: DVD u Alien
4: DVD u House
5: DVD u It
2: VHS 5 Pink Flamingos
1: DVD u Star Wars

Enter command: Catalog contains 3 collections:
Collection Dirty contains:
4: DVD u House
2: VHS 5 Pink Flamingos
Collection Favorites contains:
3: DVD u Alien
1: DVD u Star Wars
Collection Fun contains:
3: DVD u Alien
4: DVD u House
2: VHS 5 Pink Flamingos
1: DVD u Star Wars

Enter command: sA save.txt
Data saved

Enter command: cApLpC
All data deleted

Enter command: Library is empty

Enter command: Catalog is empty

Enter command: rA save.txt
Data loaded

Enter command: pLpC
Library contains 5 records:
3: DVD u Alien
4: DVD u House
5: DVD u It
2: VHS 5 Pink Flamingos
1: DVD u Star Wars

Enter command: Catalog contains 3 collections:
Collection Dirty contains:
4: DVD u House
2: VHS 5 Pink Flamingos
Collection Favorites contains:
3: DVD u Alien
1: DVD u Star Wars
Collection Fun contains:
3: DVD u Alien
4: DVD u House
2: VHS 5 Pink Flamingos
1: DVD u Star Wars

Enter command: qq
All data deleted
```