/*                                                                            */
/* CDDL HEADER START                                                          */
/*                                                                            */
/* The contents of this file are subject to the terms of the Common           */
/* Development and Distribution License Version 1.0 (the "License").          */
/*                                                                            */
/* You can obtain a copy of the license at                                    */
/* http://www.opensource.org/licenses/CDDL-1.0.  See the License for the      */
/* specific language governing permissions and limitations under the License. */
/*                                                                            */
/* When distributing Covered Code, include this CDDL HEADER in each file and  */
/* include the License file in a prominent location with the name             */
/* LICENSE.CDDL.                                                              */
/* If applicable, add the following below this CDDL HEADER, with the fields   */
/* enclosed by brackets "[]" replaced with your own identifying information:  */
/*                                                                            */
/* Portions Copyright (c) [yyyy] [name of copyright owner].                   */
/* All rights reserved.                                                       */
/*                                                                            */
/* CDDL HEADER END                                                            */
/*                                                                            */

/*                                                                            */
/* Copyright (c) 2016--2019, Regents of the University of Minnesota.          */
/* All rights reserved.                                                       */
/*                                                                            */
/* Contributors:                                                              */
/*    Ryan S. Elliott                                                         */
/*                                                                            */

/*                                                                            */
/* Release: This file is part of the kim-api-2.1.3 package.                   */
/*                                                                            */


#ifndef KIM_COLLECTION_H_
#define KIM_COLLECTION_H_

/**
 ** \brief \copybrief KIM::Collection
 **
 ** \sa KIM::Collection, kim_collection_module::kim_collection_type
 **
 ** \since 2.1
 **/
struct KIM_Collection
{
  /**
   ** \brief \copybrief KIM::Collection::collectionID
   **
   ** \sa KIM::Collection::collectionID,
   ** kim_collection_module::kim_collection_type::collection_id
   **
   ** \since 2.1
   **/
  int collectionID;
};
#ifndef KIM_COLLECTION_DEFINED_
#define KIM_COLLECTION_DEFINED_
/**
 ** \brief Convenience typedef.
 **
 ** \since 2.1
 **/
typedef struct KIM_Collection KIM_Collection;
#endif

/**
 ** \brief \copybrief KIM::Collection::Collection(std::string const &)
 **
 ** \sa KIM::Collection::Collection(std::string const &),
 ** kim_collection_module::kim_from_string
 **
 ** \since 2.1
 **/
KIM_Collection KIM_Collection_FromString(char const * const str);

/**
 ** \brief \copybrief KIM::Collection::Known
 **
 ** \sa KIM::Collection::Known, kim_collection_module::kim_known
 **
 ** \since 2.1
 **/
int KIM_Collection_Known(KIM_Collection const collection);

/**
 ** \brief \copybrief KIM::Collection::operator==()
 **
 ** \sa KIM::Collection::operator==(), kim_collection_module::operator(.eq.)
 **
 ** \since 2.1
 **/
int KIM_Collection_Equal(KIM_Collection const lhs, KIM_Collection const rhs);

/**
 ** \brief \copybrief KIM::Collection::operator!=()
 **
 ** \sa KIM::Collection::operator!=(), kim_collection_module::operator(.ne.)
 **
 ** \since 2.1
 **/
int KIM_Collection_NotEqual(KIM_Collection const lhs, KIM_Collection const rhs);

/**
 ** \brief \copybrief KIM::Collection::ToString
 **
 ** \sa KIM::Collection::ToString, kim_collection_module::kim_to_string
 **
 ** \since 2.1
 **/
char const * KIM_Collection_ToString(KIM_Collection const collection);

/**
 ** \brief \copybrief KIM::COLLECTION::system
 **
 ** \sa KIM::COLLECTION::system,
 ** kim_collection_module::kim_collection_system
 **
 ** \since 2.1
 **/
extern KIM_Collection const KIM_COLLECTION_system;

/**
 ** \brief \copybrief KIM::COLLECTION::user
 **
 ** \sa KIM::COLLECTION::user,
 ** kim_collection_module::kim_collection_user
 **
 ** \since 2.1
 **/
extern KIM_Collection const KIM_COLLECTION_user;

/**
 ** \brief \copybrief KIM::COLLECTION::environmentVariable
 **
 ** \sa KIM::COLLECTION::environmentVariable,
 ** kim_collection_module::kim_collection_environment_variable
 **
 ** \since 2.1
 **/
extern KIM_Collection const KIM_COLLECTION_environmentVariable;

/**
 ** \brief \copybrief KIM::COLLECTION::currentWorkingDirectory
 **
 ** \sa KIM::COLLECTION::currentWorkingDirectory,
 ** kim_collection_module::kim_collection_current_working_directory
 **
 ** \since 2.1
 **/
extern KIM_Collection const KIM_COLLECTION_currentWorkingDirectory;

/**
 ** \brief \copybrief KIM::COLLECTION::GetNumberOfCollections
 **
 ** \sa KIM::COLLECTION::GetNumberOfCollections,
 ** kim_collection_module::kim_get_number_of_collections
 **
 ** \since 2.1
 **/
void KIM_COLLECTION_GetNumberOfCollections(int * const numberOfCollections);

/**
 ** \brief \brief \copybrief KIM::COLLECTION::GetCollection
 **
 ** \sa KIM::COLLECTION::GetCollection,
 ** kim_collection_module::kim_get_collection
 **
 ** \since 2.1
 **/
int KIM_COLLECTION_GetCollection(int const index,
                                 KIM_Collection * const collection);

#endif /* KIM_COLLECTION_H_ */
