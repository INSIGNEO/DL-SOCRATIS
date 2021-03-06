/*============================================================================
  KWSys - Kitware System Library
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef kwssys_System_h
#define kwssys_System_h

#include <kwssys/Configure.h>

/* Redefine all public interface symbol names to be in the proper
   namespace.  These macros are used internally to kwsys only, and are
   not visible to user code.  Use kwsysHeaderDump.pl to reproduce
   these macros after making changes to the interface.  */
#if !defined(KWSYS_NAMESPACE)
# define kwsys_ns(x) kwssys##x
# define kwsysEXPORT kwssys_EXPORT
#endif
#if !kwssys_NAME_IS_KWSYS
# define kwsysSystem_Parse_CommandForUnix             kwsys_ns(System_Parse_CommandForUnix)
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * Parse a unix-style command line string into separate arguments.
 *
 * On success, returns a pointer to an array of pointers to individual
 * argument strings.  Each string is null-terminated and the last
 * entry in the array is a NULL pointer (just like argv).  It is the
 * caller's responsibility to free() the strings and the array of
 * pointers to them.
 *
 * On failure, returns NULL.  Failure occurs only on invalid flags or
 * when memory cannot be allocated; never due to content of the input
 * string.  Missing close-quotes are treated as if the necessary
 * closing quote appears.
 *
 * By default single- and double-quoted arguments are supported, and
 * any character may be escaped by a backslash.  The flags argument is
 * reserved for future use, and must be zero (or the call will fail).
 */
kwsysEXPORT char** kwsysSystem_Parse_CommandForUnix(const char* command,
                                                    int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif

/* If we are building a kwsys .c or .cxx file, let it use these macros.
   Otherwise, undefine them to keep the namespace clean.  */
#if !defined(KWSYS_NAMESPACE)
# undef kwsys_ns
# undef kwsysEXPORT
# if !defined(KWSYS_NAMESPACE) && !kwssys_NAME_IS_KWSYS
#  undef kwsysSystem_Parse_CommandForUnix
# endif
#endif

#endif
