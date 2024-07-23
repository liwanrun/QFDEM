#ifndef GLOBALMACROS_H
#define GLOBALMACROS_H

#define PRINT_STR_COMMENT(os, text) (os \
    << qSetFieldWidth(20) << left << (text) << reset << endl)

#define PRINT_INT_NOENDL(os, name, value) (os \
    << qSetFieldWidth(20) << left << (name) \
    << qSetFieldWidth(10) << left << (value) << reset)

#define PRINT_INT_SCALAR(os, name, value) (os \
    << qSetFieldWidth(20) << left << (name) \
    << qSetFieldWidth(10) << left << (value) << reset << endl)

#define PRINT_DBL_SCALAR(os, name, value) (os \
    << qSetFieldWidth(20) << left << (name) \
    << qSetFieldWidth(10) << left << forcepoint << (value) << reset << endl)

#define PRINT_INT_VECTOR(os, name, flag, rows, cols) (os \
    << qSetFieldWidth(20) << left << (name) \
    << qSetFieldWidth(10) << left << (flag) \
    << qSetFieldWidth(10) << left << (rows) \
    << qSetFieldWidth(10) << left << (cols) << reset << endl)

#define PRINT_STR_OPTION(os, name, opt1, opt2) (os \
    << qSetFieldWidth(20) << left << (name) \
    << qSetFieldWidth(10) << left << forcepoint << (opt1) << reset \
    << qSetFieldWidth(10) << left << forcepoint << (opt2) << reset << endl)

#endif // GLOBALMACROS_H
