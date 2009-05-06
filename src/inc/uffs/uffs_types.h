/*
    Copyright (C) 2005-2008  Ricky Zheng <ricky_gz_zheng@yahoo.co.nz>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef UFFS_TYPES_H
#define UFFS_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _UBASE_
#include <sys/utypes.h>
#endif

/** 
 * \file uffs_types.h
 * \brief basic types used on uffs
 * \author Ricky Zheng
 */

/* basic types */

/** \typedef i8 
 * \brief 8 bit integer
 */
typedef char i8;

/** \typedef u8 
 * \brief 8 bit unsigned integer
 */
typedef unsigned char u8;

/** \typedef i16 
 * \brief 16 bit integer
 */
typedef short int i16;


/** \typedef u16
 * \brief 16 bit unsigned integer
 */
typedef unsigned short int u16;


/** \typedef i32
 * \brief 32 bit integer
 */
typedef int i32;

/** \typedef u32 
 * \brief 32 bit unsigned integer
 */
typedef unsigned int u32;


#ifndef _UBASE_

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/* boolean type */

/** \typedef UBOOL 
 * \brief boolean type for uffs, the value would be: #U_TRUE or #U_FALSE
 */
typedef int UBOOL;

/** \def U_TRUE 
 * \brief boolean true for uffs
 */
#define U_TRUE (TRUE)


/** \def U_FALSE 
 * \brief boolean false for uffs
 */
#define U_FALSE (FALSE)


/** \typedef URET 
 * \brief return type for uffs, should be #U_FAIL or #U_SUCC
 */
typedef int URET;

/** \def U_FAIL 
 * \brief indicator of fail
 */
#define U_FAIL -1

/** \def U_SUCC 
 * \brief indicator of successful
 */
#define U_SUCC 0

/** \def IS_SUCC(ret) 
 * \brief is it successful ?
 */
#define IS_SUCC(ret) (ret >= 0 ? U_TRUE : U_FALSE)


/** \def IS_FAIL(ret) 
 * \brief is it fail ?
 */
#define IS_FAIL(ret) (ret < 0 ? U_TRUE : U_FALSE)

#ifndef NULL
/** \def NULL 
 * \brief zero for pointer
 */
#define NULL 0
#endif

#endif // _UBASE_


#ifdef __cplusplus
}
#endif


#endif
