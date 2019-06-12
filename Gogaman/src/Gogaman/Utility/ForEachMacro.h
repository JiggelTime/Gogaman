#pragma once

#define GM_FOR_EACH_SELECT_MACRO(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, x, ...) x
#define GM_FOR_EACH_ITERATION_1(macro, x, ...) macro(x)
#define GM_FOR_EACH_ITERATION_2(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_1(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_3(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_2(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_4(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_3(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_5(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_4(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_6(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_5(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_7(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_6(macro, __VA_ARGS__);
#define GM_FOR_EACH_ITERATION_8(macro, x, ...)\
macro(x)\
GM_FOR_EACH_ITERATION_7(macro, __VA_ARGS__);

#define GM_FOR_EACH(macro, ...)\
GM_FOR_EACH_SELECT_MACRO(__VA_ARGS__, GM_FOR_EACH_ITERATION_8, GM_FOR_EACH_ITERATION_7, GM_FOR_EACH_ITERATION_6, GM_FOR_EACH_ITERATION_5, GM_FOR_EACH_ITERATION_4, GM_FOR_EACH_ITERATION_3, GM_FOR_EACH_ITERATION_2, GM_FOR_EACH_ITERATION_1)(macro, __VA_ARGS__)