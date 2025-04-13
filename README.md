# TOV File Manager Documentation

## Table of Contents
1. [Introduction](#introduction)
2. [Project Structure](#project-structure)
3. [Core Concepts](#core-concepts)
4. [Data Structures](#data-structures)
5. [Key Functions](#key-functions)
6. [UI Components](#ui-components)
7. [Building and Running](#building-and-running)
8. [Implementation Details](#implementation-details)
9. [Potential Improvements](#potential-improvements)

## Introduction

The TOV File Manager is a C application that implements a file management system using a TOV (Table of Overflow) structure. The system provides a graphical user interface built using GTK for creating, managing, and searching through records in a custom file format.

This application demonstrates:
- Custom file format implementation
- Hash table for efficient record lookup
- GTK-based GUI for user interaction
- Basic CRUD operations (Create, Read, Update, Delete)
- Data validation and error handling

## Project Structure

```
TOV-File-Manager/
├── algorithms.c        # Core algorithms for TOV file management
├── tov.c               # Main application logic and GTK UI handlers
├── tov.h               # Header defining data structures and function prototypes
├── design.glade        # GTK interface design file
├── .vscode/            # VSCode configuration files
└── .gitignore          # Git ignore file
```

## Core Concepts

### TOV File Format

The TOV (Table of Overflow) is a custom file format designed to store records efficiently. Each record contains:
- A unique ID
- Three data fields (data1, data2, data3)

Records are stored in a physical file (`monFichierTOV.tov`) with a simple delimiter-based format: `ID|data1|data2|data3`.

### Hash Table

A hash table is implemented to provide fast record lookup by ID. The hash function simply uses the modulo operation to map IDs to table indices.

### Buffer Transmission

A buffer system is used to safely handle data transfer between components, preventing buffer overflow issues.

## Data Structures

### EnregistrementPhysique (Physical Record)
```c
typedef struct {
    EnteteEnregistrement entete;
    char data1[TAILLE_MAX_ENREGISTREMENT];
    char data2[TAILLE_MAX_ENREGISTREMENT];
    char data3[TAILLE_MAX_ENREGISTREMENT];
} EnregistrementPhysique;
```
Represents a single record stored in the TOV file, with an ID and three data fields.

### HashTable
```c
typedef struct {
    int *table;
    int taille;
} HashTable;
```
An array-based hash table to store record IDs for fast lookup.

### FichierTOV (TOV File)
```c
typedef struct {
    EnteteFichierTOV entete;
    EnregistrementPhysique *enregistrements;
} FichierTOV;
```
The main data structure representing the TOV file, containing a header and an array of records.

### BufferTransmission
```c
typedef struct {
    char data[TAILLE_BUFFER];
    int taille;
} BufferTransmission;
```
Buffer for safely transferring data between components.

## Key Functions

### File Management

- `initialiserFichierTOV(FichierTOV *fichier, int capaciteMax)`: Initializes a TOV file with specified capacity.
- `libererFichierTOV(FichierTOV *fichier)`: Frees resources associated with a TOV file.
- `ajouterEnregistrement(FichierTOV *fichier, HashTable *hashTable, EnregistrementPhysique *enregistrement, BufferTransmission *buffer)`: Adds a new record to the TOV file.
- `supprimerEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id, BufferTransmission *buffer)`: Removes a record from the TOV file.
- `rechercherEnregistrement(FichierTOV *fichier, HashTable *hashTable, int id)`: Searches for a record by ID.
- `afficherFichierTOV(const FichierTOV *fichier)`: Displays all records in the TOV file.

### Hash Table Functions

- `initialiserHashTable(HashTable *hashTable, int taille)`: Initializes a hash table with specified size.
- `hashFunction(int id, int tailleTable)`: Maps an ID to a hash table index.

### Buffer Functions

- `remplirBuffer(BufferTransmission *buffer, const char *data)`: Fills the buffer with data.
- `viderBuffer(BufferTransmission *buffer)`: Empties the buffer.

## UI Components

The user interface is built using GTK and defined in `design.glade`. It includes:

### Main Window
- Create button: Creates a new TOV file
- Add button: Adds a new record to the file
- Delete button: Removes a record from the file
- Show Content button: Displays all records in the file
- Search button: Searches for a record by ID
- Quit button: Exits the application

### Dialogs
- **Add Item Modal**: Dialog for adding a new record with fields for first name, second name, and ID
- **Delete Modal**: Dialog for specifying which record to delete by ID
- **Search Modal**: Dialog for entering an ID to search for
- **Information Modal**: Dialog for setting the TOV file capacity

## Building and Running

### Prerequisites
- GCC compiler
- GTK3 development libraries
- MSYS2/MinGW (for Windows)

### Building
The project appears to be configured for building with GCC on Windows using MSYS2/MinGW. You can build it with:

```bash
gcc -o tov_manager tov.c algorithms.c -I/mingw64/include/gtk-3.0 -I/mingw64/include/glib-2.0 `pkg-config --cflags --libs gtk+-3.0`
```

### Running
Execute the compiled binary:

```bash
./tov_manager
```

## Implementation Details

### TOV File Operations

1. **Creating a TOV File**:
   - User enters the file capacity
   - System initializes an empty TOV file structure and hash table

2. **Adding Records**:
   - User enters first name, second name, and ID
   - System validates input
   - Record is added to the in-memory structure and written to the physical file
   - Hash table is updated

3. **Deleting Records**:
   - User specifies the ID to delete
   - System removes the record from the in-memory structure and physical file
   - Hash table is updated

4. **Searching Records**:
   - User provides an ID
   - System uses the hash table to quickly locate the record
   - Record details are displayed if found

5. **Displaying All Records**:
   - System reads all records from the physical file
   - Records are displayed in the main window

### Error Handling

The application includes validation for:
- Empty input fields
- Non-numeric input for ID fields
- File operation failures

### Memory Management

The application allocates memory for:
- Hash table array
- TOV file records array

Memory is freed when:
- The application exits
- The TOV file is closed

