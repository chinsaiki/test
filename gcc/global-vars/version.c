#include <stdio.h>

#ifndef GCC_VERSION_H
#define GCC_VERSION_H
extern const char version_string[];
extern const char pkgversion_string[];
extern const char bug_report_url[];
#endif /* ! GCC_VERSION_H */

int main() {
	printf("version_string	%s\n", version_string);
}
