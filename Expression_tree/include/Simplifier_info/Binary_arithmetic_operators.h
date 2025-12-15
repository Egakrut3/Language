/*
HANDLE_OPERATION(name, c_decl, left_neutral, right_neutral, left_const_crit, left_const_res, right_const_crit, right_const_res)
*/

HANDLE_OPERATION(ADD, +, 0,   0, NAN, NAN, NAN, NAN)
HANDLE_OPERATION(SUB, -, NAN, 0, NAN, NAN, NAN, NAN)

HANDLE_OPERATION(MLT, *, 1,   1, 0,   0,   0,   0)
HANDLE_OPERATION(DIV, /, NAN, 1, 0,   0,   NAN, NAN)
