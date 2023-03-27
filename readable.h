/* Licensed under the MIT License
 * Copyright (c) 2023 Lars Erik Wik
 */

#ifndef _READABLE_H
#define _READABLE_H

enum {
  READABLE_SUCCESS = 0, // Successfully read file.
  READABLE_FAILURE,     // Failed to read file.
  READABLE_ERROR,       // Unexpected error occured.
};

/**
 * @brief Determine if a file is readable.
 * @param[in] filename path to file.
 * @param[in] timeout number of milliseconds to wait.
 * @return READABLE_SUCCESS on successful read, READABLE_FAILUIRE on
 *         unsuccessful read, and READABLE_ERROR in case of unexpected error.
 */
int Readable(const char *const filename, const long timeout);

#endif // _READABLE_H
