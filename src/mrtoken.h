/*******************************************************************************
 *
 *                              Delta Chat Core
 *                      Copyright (C) 2017 Björn Petersen
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 ******************************************************************************/


#ifndef __MRTOKEN_H__
#define __MRTOKEN_H__
#ifdef __cplusplus
extern "C" {
#endif


typedef struct _mrmailbox mrmailbox_t;


// Token namespaces
typedef enum {
	MRT_INVITENUMBER = 100,
	MRT_AUTH = 110
} mrtokennamespc_t;


// Functions to read/write token from/to the database. A token is any string associated with a key.
void     mrtoken_save__                 (mrmailbox_t*, mrtokennamespc_t, uint32_t foreign_id, const char* token);
char*    mrtoken_lookup__               (mrmailbox_t*, mrtokennamespc_t, uint32_t foreign_id);
int      mrtoken_exists__               (mrmailbox_t*, mrtokennamespc_t, const char* token);


#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRTOKEN_H__ */

