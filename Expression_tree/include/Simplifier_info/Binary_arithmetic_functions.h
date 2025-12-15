/*
HANDLE_OPERATION(name, c_decl, left_neutral, right_neutral, left_const_crit, left_const_res, right_const_crit, right_const_res)
*/

HANDLE_OPERATION(POW, pow, NAN, 1, 1, 1, 0, 1)
