#pragma once
#include <string.h>
#include <assert.h>

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

// Enumeration for data types

typedef enum {
	UINT8,
	UINT32,
	INT32,
	CHAR,
	OBJ_PTR,
	VOID_PTR,
	FLOAT,
	DOUBLE,
	OBJ_STRUCT
} data_type_t;

typedef enum {
	MLD_FALSE,
	MLD_TRUE,
} mld_boolean_t;

#define OFFSETOF(struct_name, fld_name) (unsigned long)&(((struct_name*)0)->fld_name)

#define FIELD_SIZE(struct_name, fld_name) sizeof(((struct_name*)0)->fld_name)

typedef struct _struct_db_rec_ struct_db_rec_t;

// Structure to store the information of one field of a C structure

typedef struct _field_info {
	char fname[MAX_FIELD_NAME_SIZE];	// Name of the field
	data_type_t dtype;					// Data type of the field
	unsigned int size;					// Size of the field
	unsigned int offset;				// Offset of the field
	char nested_str_name[MAX_STRUCTURE_NAME_SIZE];	// if dtype = OBJ_PTR or OBJ_STRUCT
} field_info_t;

// Structure to store the information of one C structure which could have 'n_fields' fields

struct _struct_db_rec_ {
	struct_db_rec_t* next;				// Pointer to the next structure in the linked list
	char struct_name[MAX_STRUCTURE_NAME_SIZE];	// key
	unsigned int ds_size;				// Size of the structure
	unsigned int n_fields;				// No of fields in the structure
	field_info_t* fields;				// Pointer to the array of fields
};

// The head of the linked list representing the structure database

typedef struct _struct_db_ {
	struct_db_rec_t *head;
	unsigned int count;
} struct_db_t; 

// Printing functions

void print_structure_rec(struct_db_rec_t* struct_rec);	// print all the details that are present in this db record
void print_structure_db(struct_db_t* struct_db);		// print the entire structure database (all the records)
int add_structure_to_struct_db(struct_db_t* struct_db, struct_db_rec_t* struct_rec);	// add the structure record in a structure db

#define FIELD_INFO(struct_name, fld_name, dtype, nested_struct_name)    \
   {#fld_name, dtype, FIELD_SIZE(struct_name, fld_name),                \
        OFFSETOF(struct_name, fld_name), #nested_struct_name} 

#define REG_STRUCT(struct_db, st_name, fields_arr)                    \
    do{                                                               \
        struct_db_rec_t *rec = (struct_db_rec_t*)calloc(1, sizeof(struct_db_rec_t));    \
        strncpy(rec->struct_name, #st_name, MAX_STRUCTURE_NAME_SIZE); \
        rec->ds_size = sizeof(st_name);                              \
        rec->n_fields = sizeof(fields_arr)/sizeof(field_info_t);     \
        rec->fields = fields_arr;                                    \
        if(add_structure_to_struct_db(struct_db, rec)){              \
            assert(0);                                               \
        }                                                            \
    }while(0);

typedef struct _object_db_rec_ object_db_rec_t;

struct _object_db_rec_ {
	object_db_rec_t* next;			// pointer to a next object in a linked list
	void* ptr;						// pointer to the object that app just malloc
	unsigned int units;				// # of units of the object that was malloc
	struct_db_rec_t* struct_rec;	// pointer to a structure from structures linked list
	mld_boolean_t is_visited;		// used for graph traversal
	mld_boolean_t is_root;			// is this object a root or not
};

typedef struct _object_db_ {
	struct_db_t* struct_db;			//
	object_db_rec_t* head;			// pointer to the first object record in the object linked list
	unsigned int count;				// # of object present in obj_db
} object_db_t;

void print_object_rec(object_db_rec_t* obj_rec, int i);
void print_object_db(object_db_t* object_db);
void* xcalloc(object_db_t* object_db, char* struct_name, int units);
static struct_db_rec_t * struct_db_look_up(struct_db_t *struct_db, char *struct_name);
static object_db_rec_t *object_db_look_up(object_db_t *object_db, void *ptr);
void add_object_to_object_db(object_db_t* object_db, void* ptr, int units, struct_db_rec_t* struct_rec, mld_boolean_t is_root);
void mld_register_global_object_as_root(object_db_t *object_db, void *objptr, char *struct_name, unsigned int units);
void mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr);
static void init_mld_algorithm(object_db_t* object_db);
static object_db_rec_t* get_next_root_object(object_db_t* object_db, object_db_rec_t* starting_from_here);
void run_mld_algorithm(object_db_t* object_db);
static void mld_explore_objects_recursively(object_db_t* object_db, object_db_rec_t* parent_obj_rec);
void report_leaked_objects(object_db_t* object_db);
void mld_init_primitive_data_types_support(struct_db_t* struct_db);
