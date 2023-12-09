#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <stdio.h>

/* One of the params were NULL. */
#define FIO_NULL_ERR 'n'

/* A memory error, such as a failed malloc. */
#define FIO_MEM_ERR 'm'

/* The only character present is the EOF character. */
#define FIO_EOF 'e'

/* The operation was successful. */
#define FIO_SUCCESS 's'


/**
 * Functions in the fileio library use
 * this type to report errors.
 */
typedef char fio_Status;


/**
 * A structure to facilitate returning multiple
 * files from the read functions. Always check the
 * status field before using any other field.
 */
typedef struct {
    /* The array of strings holding the lines read. */
    char **lines; 

    /* An array of the length of each string in lines. */
    int *str_lens;

    /* The length of the lines and str_lens array. */
    int length;

    /* The status of the operation. */
    fio_Status status;
} fio_DataRead;


/**
 * Read a line from the file stream.
 * Returns a malloc-ed fio_DataRead with the length
 * field set to 1 (since we read a single line).
 *
 * @param stream the FILE stream to read from.
 *
 * @return a pointer to the fio_DataRead struct.
 */
fio_DataRead* fio_read_line(FILE *stream);

/**
 * Read all lines in file till End-of-file.
 * Returns a malloc-ed fio_DataRead with all the
 * fields populated.
 *
 * @param stream the FILE stream to read from.
 *
 * @return a pointer to the fio_DataRead struct.
 */
fio_DataRead* fio_read_lines(FILE *stream);

/**
 * Read till End-of-file from a file.
 * Returns a malloc-ed fio_DataRead with the
 * lines array containing a single line which
 * holds the entire file.
 *
 * @param stream the FILE stream to read from.
 *
 * @return a pointer to the fio_DataRead struct.
 */
fio_DataRead* fio_read_file(FILE *stream);

/**
 * A function to free the fio_DataRead struct.
 *
 * @param fio_data_read a pointer to the struct to be freed.
 */
void fio_free_DataRead(fio_DataRead *fio_data_read);

/**
 * Write a line to a file.
 * The newline character isn't added automatically.
 * @param stream a FILE stream to write data to.
 * @param string the string that is to be written.
 * @return The status of the operation. 
 */
fio_Status fio_write_line(FILE *stream, char *string);

/**
 * Write an array of string to a file. The delimiter
 * string is appended between all pairs of strings.
 * @param stream a FILE stream to write data to.
 * @param str_arr the array of string to be written.
 * @param arr_len the length of the string array.
 * @param delimiter the delimiter to add between 2 items in the array.
 *        If NULL, delimiter will be set to the empty string.
 * @return The status of the operation. 
 */
fio_Status fio_write_lines(FILE *stream, char **str_arr, int arr_len, char* delimiter);

/**
 * Write a string to a file.
 * The newline character isn't added automatically.
 * @param stream a FILE stream to write data to.
 * @param string the string that is to be written.
 * @return The status of the operation. 
 */
fio_Status fio_write_file(FILE *stream, char *string);

#endif
