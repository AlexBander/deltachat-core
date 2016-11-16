/*******************************************************************************
 *
 *                             Messenger Backend
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
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
 *******************************************************************************
 *
 * File:    mrchatlist.c
 * Authors: Björn Petersen
 * Purpose: See header
 *
 ******************************************************************************/


#include <stdlib.h>
#include "mrmailbox.h"


/*******************************************************************************
 * Tools
 ******************************************************************************/


int mrchatlist_load_from_db__(mrchatlist_t* ths)
{
	int           success = 0;
	sqlite3_stmt* stmt = NULL;
	mrchat_t*     chat = NULL;
	int           row_offset;
	int           show_strangers;

	if( ths == NULL || ths->m_mailbox == NULL ) {
		goto cleanup;
	}

	mrchatlist_empty(ths);

	show_strangers = mrsqlite3_get_config_int__(ths->m_mailbox->m_sql, "show_strangers", 0);

	/* select example with left join and minimum: http://stackoverflow.com/questions/7588142/mysql-left-join-min */
	stmt = mrsqlite3_predefine__(ths->m_mailbox->m_sql, SELECT_itndd_ircftttstpb_FROM_chats_LEFT_JOIN_msgs,
		"SELECT " MR_CHAT_FIELDS "," MR_MSG_FIELDS " FROM chats c "
			" LEFT JOIN msgs m ON (c.id=m.chat_id AND m.timestamp=(SELECT MAX(timestamp) FROM msgs WHERE chat_id=c.id)) "
			" WHERE c.id>? OR c.id=?"
			" GROUP BY c.id " /* GROUP BY is needed as there may be several messages with the same timestamp */
			" ORDER BY MAX(c.draft_timestamp, m.timestamp) DESC,m.id DESC;" /* the list starts with the newest chats */
			);
	if( stmt==NULL ) {
		goto cleanup;
	}
	sqlite3_bind_int(stmt, 1, MR_CHAT_ID_LAST_SPECIAL);
	sqlite3_bind_int(stmt, 2, show_strangers? MR_CHAT_ID_STRANGERS : 0);

    while( sqlite3_step(stmt) == SQLITE_ROW )
    {
		chat = mrchat_new(ths->m_mailbox);
		row_offset = mrchat_set_from_stmt__(chat, stmt);

		chat->m_last_msg_ = mrmsg_new();
		mrmsg_set_from_stmt__(chat->m_last_msg_, stmt, row_offset);

		carray_add(ths->m_chats, (void*)chat, NULL);
    }

	/* success */
	success = 1;

	/* cleanup */
cleanup:
	return success;
}


/*******************************************************************************
 * Main interface
 ******************************************************************************/


mrchatlist_t* mrchatlist_new(mrmailbox_t* mailbox)
{
	mrchatlist_t* ths = NULL;

	if( (ths=calloc(1, sizeof(mrchatlist_t)))==NULL ) {
		exit(20); /* cannot allocate little memory, unrecoverable error */
	}

	ths->m_mailbox = mailbox;
	ths->m_chats = carray_new(128);

	return ths;
}


void mrchatlist_unref(mrchatlist_t* ths)
{
	if( ths==NULL ) {
		return;
	}

	mrchatlist_empty(ths);
	if( ths->m_chats ) {
		carray_free(ths->m_chats);
	}
	free(ths);
}


void mrchatlist_empty(mrchatlist_t* ths)
{
	if( ths && ths->m_chats )
	{
		size_t i, cnt = (size_t)carray_count(ths->m_chats);
		for( i = 0; i < cnt; i++ )
		{
			mrchat_t* chat = (mrchat_t*)carray_get(ths->m_chats, i);
			mrchat_unref(chat);
		}

		carray_set_size(ths->m_chats, 0);
	}
}


size_t mrchatlist_get_cnt(mrchatlist_t* ths)
{
	if( ths == NULL || ths->m_chats == NULL ) {
		return 0;
	}

	return (size_t)carray_count(ths->m_chats);
}


mrchat_t* mrchatlist_get_chat_by_index(mrchatlist_t* ths, size_t index)
{
	if( ths == NULL || ths->m_chats == NULL || index >= (size_t)carray_count(ths->m_chats) ) {
		return 0;
	}

	return mrchat_ref((mrchat_t*)carray_get(ths->m_chats, index));
}



