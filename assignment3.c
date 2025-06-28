#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAME_SIZE 20
#define TABLE_SIZE 101
#define MAX_SCOPES 101
#define EMPTY_SLOT NULL
#define MAX_PROBES 101

// Structure for variable entry in symbol table
typedef struct SymbolTableEntry
{
    char name[NAME_SIZE];
    int value;
    int isOccupied;
} Entry;

// Structure for the symbol table for each scope
typedef struct SymbolTable
{
    Entry *entryTable[TABLE_SIZE];
    struct SymbolTable *parent; // Pointer to the parent scope
    int scopeLevel;
} SymbolTable;

SymbolTable *scopes[MAX_SCOPES];
int scopeCount = 0; // Number of scopes

// Hash function
unsigned int hash(char *name)
{
    unsigned int hash = 0;
    while (*name)
    {
        hash = (hash * 31) + *name++;
    }
    return hash % TABLE_SIZE;
}

// Quadratic probing function toavoid collisions
// Returns the index of the variable in the symbol table
int quadratic_probe(SymbolTable *table, char *name, int insertMode)
{
    unsigned int index = hash(name);
    int i = 0;
    int retVal = -1;
    while (i < MAX_PROBES)
    {
        int newIndex = (index + i * i) % TABLE_SIZE;
        if (table->entryTable[newIndex] == EMPTY_SLOT || (insertMode && table->entryTable[newIndex]->isOccupied == 0) || (table->entryTable[newIndex] && strcmp(table->entryTable[newIndex]->name, name) == 0))
        {
            retVal = newIndex;
        }
        i++;
    }
    return retVal;
}

// Create a new scope
SymbolTable *createTable(SymbolTable *parent)
{
    SymbolTable *newTable = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!newTable)
    {
        printf("Memory allocation failed\n");
    }
    else
    {
        newTable->parent = parent;
        if (parent)
        {
            newTable->scopeLevel = parent->scopeLevel + 1;
        }
        else
        {
            newTable->scopeLevel = 0;
        }
        for (int i = 0; i < TABLE_SIZE; i++)//initializing entry table
        {
            newTable->entryTable[i] = EMPTY_SLOT;
        }
        scopes[scopeCount++] = newTable;
        for (int i = 0; i < newTable->scopeLevel; i++)
        {
            printf("   "); // Indentation for nested visualization
        }
        if (newTable->scopeLevel > 0)
            printf("[Scope Level %d] New scope created.\n", newTable->scopeLevel);
    }
    return newTable;
}

// Insert a variable in symbol table
void assign(SymbolTable *currScope, char *name, int value)
{
    int index = quadratic_probe(currScope, name, 1);
    if (index == -1)
    {
        printf("Symbol table full!\n");
    }
    else
    {
        if (currScope->entryTable[index] == EMPTY_SLOT)
        {
            currScope->entryTable[index] = (Entry *)malloc(sizeof(Entry));
        }
        strcpy(currScope->entryTable[index]->name, name);
        currScope->entryTable[index]->value = value;
        currScope->entryTable[index]->isOccupied = 1;
        for (int i = 0; i < currScope->scopeLevel; i++)
        {
            printf("   ");
        }
        printf("[Scope Level %d] Assigned %s = %d\n", currScope->scopeLevel, name, value);
    }
}

// Lookup a variable
void lookup(SymbolTable *currScope, char *name, int *found)
{
    *found = 0;
    SymbolTable *startScope = currScope;

    while (currScope != NULL && *found == 0)
    {
        int index = quadratic_probe(currScope, name, 0);
        if (index != -1 && currScope->entryTable[index] != EMPTY_SLOT && currScope->entryTable[index]->isOccupied && strcmp(currScope->entryTable[index]->name, name) == 0)
        {
            for (int i = 0; i < startScope->scopeLevel; i++)
            {
                printf("   ");
            }
            printf("[Scope Level %d] Looked up %s from [Scope %d] -- Found %s = %d\n", startScope->scopeLevel, name, currScope->scopeLevel, name, currScope->entryTable[index]->value);
            *found = 1;    
        }
        currScope = currScope->parent;
    }
    if( *found == 0 ) // Variable not found in any scope
    {
        for (int i = 0; i < startScope->scopeLevel; i++)
        {
            printf("   ");
        }
        printf("[Scope Level %d] Looked up %s -- Variable not found\n", startScope->scopeLevel, name);
    }
}

void display_all_scopes()
{
    printf("\n\n========================= SYMBOL TABLES =========================");

    for (int i = 0; i < scopeCount; i++)
    {
        printf("\n\n  Scope Level: %d   (Parent Scope ID: %d)   [%s Scope]\n", 
               scopes[i]->scopeLevel, 
               scopes[i]->parent ? scopes[i]->parent->scopeLevel : -1,
               scopes[i]->scopeLevel == 0 ? "Global" : "Local");
        
        printf("  |___________________________________________________________________|\n");
        printf("  | Variable | Address               | Value      | Scope Type        |\n");
        printf("  |___________________________________________________________________|\n");

        int hasEntries = 0;
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            if (scopes[i]->entryTable[j] != EMPTY_SLOT && scopes[i]->entryTable[j]->isOccupied)
            {
                printf("  | %-9s | %-20p | %-10d | %-17s |\n",
                       scopes[i]->entryTable[j]->name,
                       (void*)scopes[i]->entryTable[j], 
                       scopes[i]->entryTable[j]->value,
                       scopes[i]->scopeLevel == 0 ? "Global" : "Local");
                hasEntries = 1;
            }
        }

        if (!hasEntries)
        {
            printf("  |   ---     |         ---          |     ---    |     ---           |\n");
        }

        printf("  |___________________________________________________________________|\n");
    }
    
    printf("\n\n=================================================================\n");
}

// Free all memory
void free_all_scopes()
{
    for (int i = 0; i < scopeCount; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            if (scopes[i]->entryTable[j] != EMPTY_SLOT)
            {
                free(scopes[i]->entryTable[j]);
            }
        }
        free(scopes[i]);
    }
}

// Process commands from file
void process_file()
{
    FILE *file = fopen("read.txt", "r");
    if (!file)
    {
        printf("Error opening file\n");
    }
    else
    {
        SymbolTable *currentScope = createTable(NULL);
        char command[50], var[50];
        int value;
        while (fscanf(file, "%s", command) != EOF)
        {
            if (strcmp(command, "begin") == 0)
            {
                currentScope = createTable(currentScope);
            }
            else if (strcmp(command, "end") == 0)
            {
                if (currentScope->parent)
                {
                    SymbolTable *oldScope = currentScope;
                    currentScope = currentScope->parent;
                    for (int i = 0; i < oldScope->scopeLevel; i++)
                    {
                        printf("   ");
                    }
                    printf("[Scope Level %d] Exited scope.\n", oldScope->scopeLevel);
                }
                else
                {
                    printf("Cannot exit global scope.\n");
                }
            }
            else if (strcmp(command, "assign") == 0)
            {
                fscanf(file, "%s %d", var, &value);
                assign(currentScope, var, value);
            }
            else if (strcmp(command, "print") == 0)
            {
                fscanf(file, "%s", var);
                int found;
                lookup(currentScope, var, &found);
            }
        }
        fclose(file);
        display_all_scopes();
        free_all_scopes();
    }
}

int main()
{
    process_file();
    return 0;
}
