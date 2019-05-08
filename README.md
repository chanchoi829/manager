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
The first letter is an action letter, and the second letter is an object word. 

Action Letters:
```
f - find (for records only)
p - print
m - modify (for rating only)
a - add
d - delete
c - clear
s - save
r - restore
```

Object Letters:
```
**r** - an individual record
c - an individual collection
m - member for the add and delete commands
L - the Library - the set of all individual records
C - the Catalog - the set of all individual collections
A - all data - both the Library and the Catalog - for the clear, save and restore commands
a - allocations in the print command (memory information)
```

Possible Parameters:
```
<title> - a title string which is entered with whitespace before, after, and internally, but is always terminated by a newline character. Case sensitive
<ID> - a record number which must be an integer value
<name> - a collection name which consists of any non-whitespace characters and terminates with a whitespace character. Case sensitive
<medium> - a medium name which consists of any non-whitespace characters with no embedded whitespace characters and terminates with a whitespace character. Case sensitive
```
