#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked-list.h"
#include "grades.h"

//list and elements define
struct courses_element {
	char *course_name;
	int grade;
};

struct student_element {
	char *student_name;
	int id;
	struct list *course_list;
};

struct grades {
	struct list *student;
};

//typedef
typedef struct grades grades_t;
typedef struct student_element student_t;
typedef struct courses_element courses_t; 


//user functions
/**
 * @brief clone “element” to “output”
 * @return 0 on success
 * @note Failes if "element" is invalid
 */
int courses_clone (void *element ,void **output) {
	if (!element) {
		return 1;
	}
	courses_t *arg = (courses_t*)element;
	courses_t *out = (courses_t*)malloc(sizeof(courses_t));
	if (!out) {
		return 1;
	}
	out->course_name =(char*)malloc(sizeof(char)*(strlen(arg->course_name)+1));
	if (!(out->course_name)) {
		free(out);
		return 1;
	}
	strcpy(out->course_name ,arg->course_name);
	out->grade = arg->grade;
	*output = (void*)out;
	return 0;
}
/**
 * @brief Destroy "element” ,free allocated memory ,always succeed
 */
void courses_destroy (void *element) {
	if (!element) {
		return;
	}
	courses_t *arg = (courses_t*)element;
	if (!arg) {
		return;
	}
	free(arg->course_name);
	free (arg);
}

/**
 * @brief clone “element” to “output”
 * @return 0 on success
 * @note Failes if "element" is invalid
 */
int student_clone(void *element, void **output) {
	if (!element) {
		return 1;
	}
	student_t *arg = (student_t*)element;
	student_t *out = (student_t*)malloc(sizeof(student_t));
	if (!out) {
		return 1;
	}
	out->student_name =(char*)malloc(strlen(arg->student_name)+1);
	if (!(out->student_name)) {
		free (out);
		return 1;
	}
	strcpy(out->student_name ,arg->student_name);
	/* cloning the student's courses list */
	out->course_list = list_init(&courses_clone,&courses_destroy);
	struct node *it = list_begin(arg->course_list);
	while (it!=NULL) {
		list_push_back(out->course_list,list_get(it));
		it = list_next(it);
	}
	out->id = arg->id;
	*output = (void*)out;
	return 0;
}

/**
 * @brief Destroy "element” ,free allocated memory ,always succeed
 */
void student_destroy (void *element) {
	if (!element) {
		return;
	}
	student_t *arg = (student_t*)element;
	if (!arg) {
		return;
	}
	/* destroy the student's course list */
	list_destroy(arg->course_list);
	free(arg->student_name);
	free (arg);
}

/**
 * @brief Initializes the "grades" data-structure.
 * @returns A pointer to the data-structure, or NULL in case of an error
 */
struct grades* grades_init() {
	grades_t *arg = (grades_t*)malloc(sizeof(grades_t));
	arg->student = list_init(&student_clone, &student_destroy);		  		 
	return arg;
}

/**
 * @brief Destroys "grades", de-allocate all memory!
 */
void grades_destroy(grades_t *grades) {
	if (!grades) {
		return;
	}
	list_destroy(grades->student);
	free(grades);
}

/**
 * @brief check if the id of a certain student is found in the list
 * if so , the element that contains the given id is pointed by out
 * @return We return 0 if the id is in the linked list and 1 otherwise
*/
static int list_search_id (struct grades *grades, 
						   int id,
						   struct student_element **out) {
	if (grades==NULL) {
		return 1;
	}
	struct node *it = list_begin(grades->student);
	while (it!=NULL) {
		student_t *elm = (student_t*)list_get(it);
		if (elm->id == id) {
			*out = elm;
			return 0;
		}
		it = list_next(it);
	}
	return 1;
}
/**
 * @brief check if a course is found in the course list of a given student
 * @return 0 if the course is in the course list and 1 otherwise 
*/
static int list_search_course (student_t *student , const char* course) {
	if (student==NULL) {
		return 1;
	}
	struct node *it = list_begin(student->course_list);
	while (it!=NULL) {
		courses_t *elm = (courses_t*)list_get(it);
		if (!strcmp(elm->course_name ,course)) {
			return 0;
		}
		it = list_next(it);
	}
	return 1;
}

/**
 * @brief Adds a student with "name" and "id" to "grades"
 * @returns 0 on success
 * @note Failes if "grades" is invalid, or a student with 
 * the same "id" already exists in "grades"
 */
int grades_add_student (grades_t *grades, const char *name, int id) {
	student_t *helper_pointer = NULL;
	student_t **out =&helper_pointer;
	/* check data validation */
	if (!list_search_id (grades ,id ,out)||!grades) {
		return 1;
	}
	student_t *new_student =(student_t*)malloc(sizeof(student_t));
	if (!new_student) {
		return 1;
	}
	new_student->id = id;
	new_student->student_name = (char*)malloc(strlen(name)+1);
	if (!(new_student->student_name)) {
		free(new_student);
		return 1;
	}
	strcpy(new_student->student_name , name);
	new_student->course_list = NULL;
	int res = list_push_back(grades->student,(void*)new_student);
	free(new_student->student_name);
	free(new_student);
	return res;
}
/**
 * @brief Adds a course with "name" and "grade" to the student with "id"
 * @return 0 on success
 * @note Failes if "grades" is invalid, if a student with "id" does not exist
 * in "grades", if the student already has a course with "name", or if "grade"
 * is not between 0 to 100.
 */
int grades_add_grade (grades_t *grades,
                      const char *name,
                      int id,
                      int grade) {
	student_t *helper_pointer = NULL;
	student_t **out = &helper_pointer;
	/* check data validation */
	if (list_search_id(grades,id ,out)||!grades) {
		return 1;
	}
	if (!list_search_course(*out,name)|| grade <0 || grade > 100) {
		return 1;
	}
	courses_t *new_course =(courses_t*)malloc(sizeof(courses_t));
	if (!new_course) {
		return 1;
	}
	new_course ->grade = grade;
	new_course ->course_name =(char*)malloc(strlen(name)+1);
	if (!(new_course->course_name)) {
		free (new_course);
		return 1;
	}
	strcpy(new_course->course_name,name);
	int res=list_push_back((*out)->course_list ,(void*)new_course);
	free(new_course->course_name);
	free(new_course);
	return res;
}
/**
 * @brief Calcs the average of the student with "id" in "grades".
 * @param[out] out This method sets the variable pointed by "out" to the
 * student's name.
 * @returns The average, or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note If the student has no courses, the average is 0.
 * @note On error, sets "out" to NULL.
 */

float grades_calc_avg (struct grades *grades, int id, char **out) {
	/* check data validation */
	if(!grades) {
		return -1;
	}
	student_t *helper_pointer = NULL;
	student_t **elm =&helper_pointer;
	if (list_search_id(grades , id, elm)) {
		return -1;
	}
	char *tmp_name = (char*)malloc(strlen((*elm)->student_name)+1);
	if (!tmp_name) {
		return -1;
	}
	strcpy(tmp_name , (*elm)->student_name);
	*out = tmp_name;
	/* if the student has no courses the avg is 0 */
	if ((*elm)->course_list == NULL||list_begin((*elm)->course_list)== NULL) {
		return 0;
	}
	float avg;
	int sum=0;
	int courses_num =0;
	/* calculating the student's avg */
	struct node *it = list_begin((*elm)->course_list);
	while (it!=NULL) {
		courses_t *val=(courses_t*)list_get(it);
		sum += val->grade;
		courses_num++;
		it = list_next(it);
	}
	avg = (float)sum / (float)courses_num;
	return avg;
}
/**
 * @brief Prints the courses of the student with "id" 
 * @returns 0 on success or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 */
int grades_print_student (struct grades *grades, int id) {
	/* check data validation */
	if(!grades) {
		return -1;
	}
	student_t *test = NULL;
	student_t **el =&test;
	if (list_search_id(grades , id, el)) {
		return -1;
	}
	/* prints the student's data */
	printf("%s %d:",(*el)->student_name ,id);
	struct node *it =list_begin((*el)->course_list);
	/* print the studen't courses and grades */
	while (it!=NULL) {
		courses_t *val = (courses_t*)list_get(it);
		printf(" %s %d",val->course_name, val->grade);
		it = list_next(it);
		if (it!=NULL) {
			printf(",");
		}
	}
	printf("\n");
	return 0 ;
}

/**
 * @brief Prints all students in "grade"
 * @returns 0 on success or -1 on error
 * @note Fails if "grades" is invalid
 */
int grades_print_all (struct grades *grades) {
	if(!grades) {
		return -1;
	}
	struct node *it =list_begin(grades->student);
	while (it!=NULL) {
		grades_print_student(grades ,
							((student_t*)list_get(it))->id);
		it = list_next(it);
	}
	return 0 ;
}


