#ifndef LIST_ERROR_H
#define LIST_ERROR_H

void err(const char* format, ...) __attribute__((analyzer_noreturn));
void perr(const char* format, ...) __attribute__((analyzer_noreturn));

#endif
