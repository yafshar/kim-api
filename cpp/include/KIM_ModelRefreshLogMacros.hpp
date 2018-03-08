//
// CDDL HEADER START
//
// The contents of this file are subject to the terms of the Common Development
// and Distribution License Version 1.0 (the "License").
//
// You can obtain a copy of the license at
// http://www.opensource.org/licenses/CDDL-1.0.  See the License for the
// specific language governing permissions and limitations under the License.
//
// When distributing Covered Code, include this CDDL HEADER in each file and
// include the License file in a prominent location with the name LICENSE.CDDL.
// If applicable, add the following below this CDDL HEADER, with the fields
// enclosed by brackets "[]" replaced with your own identifying information:
//
// Portions Copyright (c) [yyyy] [name of copyright owner]. All rights reserved.
//
// CDDL HEADER END
//

//
// Copyright (c) 2016--2018, Regents of the University of Minnesota.
// All rights reserved.
//
// Contributors:
//    Ryan S. Elliott
//

//
// Release: This file is part of the kim-api.git repository.
//


#undef FATAL_VERBOSITY
#define FATAL_VERBOSITY (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_FATAL_)
#ifdef LOG_FATAL
#undef LOG_FATAL
#endif
#if (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_FATAL_)
#define LOG_FATAL(message)                                          \
  modelRefresh->LogEntry(KIM_LOG_VERBOSITY_fatal, message,          \
                         __LINE__, __FILE__)
#else
#define LOG_FATAL(message)
#endif

#undef ERROR_VERBOSITY
#define ERROR_VERBOSITY (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_ERROR_)
#ifdef LOG_ERROR
#undef LOG_ERROR
#endif
#if (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_ERROR_)
#define LOG_ERROR(message)                                          \
  modelRefresh->LogEntry(KIM_LOG_VERBOSITY_error, message,          \
                         __LINE__, __FILE__)
#else
#define LOG_ERROR(message)
#endif

#undef WARNING_VERBOSITY
#define WARNING_VERBOSITY (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_WARNING_)
#ifdef LOG_WARNING
#undef LOG_WARNING
#endif
#if (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_WARNING_)
#define LOG_WARNING(message)                                        \
  modelRefresh->LogEntry(KIM_LOG_VERBOSITY_warning, message,        \
                         __LINE__, __FILE__)
#else
#define LOG_WARNING(message)
#endif

#undef INFORMATION_VERBOSITY
#define INFORMATION_VERBOSITY (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_INFORMATION_)
#ifdef LOG_INFORMATION
#undef LOG_INFORMATION
#endif
#if (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_INFORMATION_)
#define LOG_INFORMATION(message)                                    \
  modelRefresh->LogEntry(KIM_LOG_VERBOSITY_information, message,    \
                         __LINE__, __FILE__)
#else
#define LOG_INFORMATION(message)
#endif

#undef DEBUG_VERBOSITY
#define DEBUG_VERBOSITY (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_DEBUG_)
#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#if (KIM_LOG_MAXIMUM_LEVEL >= KIM_LOG_VERBOSITY_DEBUG_)
#define LOG_DEBUG(message)                                          \
  modelRefresh->LogEntry(KIM_LOG_VERBOSITY_debug, message,          \
                         __LINE__, __FILE__)
#else
#define LOG_DEBUG(message)
#endif
