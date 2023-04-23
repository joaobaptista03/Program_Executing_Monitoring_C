#include <ctype.h>
#include <string.h>

char *strtrim(char *str) {
    char *end;
    while (isspace(*str)) { // skip leading whitespace
        str++;
    }
    if (*str == '\0') { // empty string
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) { // skip trailing whitespace
        end--;
    }
    *(end + 1) = '\0'; // terminate the trimmed string
    return str;
}