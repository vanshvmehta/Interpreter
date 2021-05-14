// This file is provided in support of the CS 146 W2021 final assessment.
// All other uses are prohibited unless permission is explicitly granted.
// Republication in any setting by unauthorized parties is prohibited.
//
// Author:  Brad Lushman
// Date:  April 9, 2021

// Place your solution to problem 3 in this file.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsefr.h"
#include "F3.h"

int main() {
    struct FRAST expr = parseFR(stdin);
    init(expr);
}

