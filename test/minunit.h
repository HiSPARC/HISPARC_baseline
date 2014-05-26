/*
 * ============================================================================
 *
 *       Filename:  minunit.h
 *
 *    Description:  This file contains a very minimal unit testing framework
 *
 *        Version:  1.0
 *        Created:  21-05-2014 14:31:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Source:  http://www.jera.com/techinfo/jtns/jtn002.html#Source_Code
 *   Organization:  
 *
 * ============================================================================
 */

 #define mu_assert(message, test) do { if (!(test)) return message; } while (0)
 #define mu_run_test(test) do { char *message = test(); tests_run++; \
                                     if (message) return message; } while (0)
 extern int tests_run;
