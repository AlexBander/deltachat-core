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


#include "dc_context.h"


#define MR_LOT_MAGIC 0x00107107


dc_lot_t* dc_lot_new()
{
	dc_lot_t* ths = NULL;

	if( (ths=calloc(1, sizeof(dc_lot_t)))==NULL ) {
		exit(27); /* cannot allocate little memory, unrecoverable error */
	}

	ths->m_magic = MR_LOT_MAGIC;
	ths->m_text1_meaning  = 0;

    return ths;
}


/**
 * Frees an object containing a set of parameters.
 * If the set object contains strings, the strings are also freed with this function.
 * Set objects are created eg. by dc_chatlist_get_summary(), dc_msg_get_summary or by
 * dc_msg_get_mediainfo().
 *
 * @memberof dc_lot_t
 *
 * @param set The object to free.
 *
 * @return None
 */
void dc_lot_unref(dc_lot_t* set)
{
	if( set==NULL || set->m_magic != MR_LOT_MAGIC ) {
		return;
	}

	dc_lot_empty(set);
	set->m_magic = 0;
	free(set);
}


void dc_lot_empty(dc_lot_t* ths)
{
	if( ths == NULL || ths->m_magic != MR_LOT_MAGIC ) {
		return;
	}

	free(ths->m_text1);
	ths->m_text1 = NULL;
	ths->m_text1_meaning = 0;

	free(ths->m_text2);
	ths->m_text2 = NULL;

	free(ths->m_fingerprint);
	ths->m_fingerprint = NULL;

	free(ths->m_invitenumber);
	ths->m_invitenumber = NULL;

	free(ths->m_auth);
	ths->m_auth = NULL;

	ths->m_timestamp = 0;
	ths->m_state = 0;
	ths->m_id = 0;
}


/**
 * Get first string. The meaning of the string is defined by the creator of the object and may be roughly described by dc_lot_get_text1_meaning().
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return A string, the string may be empty and the returned value must be free()'d. NULL if there is no such string.
 */
char* dc_lot_get_text1(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return NULL;
	}
	return strdup_keep_null(lot->m_text1);
}


/**
 * Get second string. The meaning of the string is defined by the creator of the object.
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return A string, the string may be empty and the returned value must be free()'d	. NULL if there is no such string.
 */
char* dc_lot_get_text2(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return NULL;
	}
	return strdup_keep_null(lot->m_text2);
}


/**
 * Get the meaning of the first string.  Posssible meanings of the string are defined by the creator of the object and may be returned eg.
 * as DC_TEXT1_DRAFT, DC_TEXT1_USERNAME or DC_TEXT1_SELF.
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return Returns the meaning of the first string, possible meanings are defined by the creator of the object.
 *    0 if there is no concrete meaning or on errors.
 */
int dc_lot_get_text1_meaning(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return 0;
	}
	return lot->m_text1_meaning;
}


/**
 * Get the associated state. The meaning of the state is defined by the creator of the object.
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return The state as defined by the creator of the object. 0 if there is not state or on errors.
 */
int dc_lot_get_state(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return 0;
	}
	return lot->m_state;
}


/**
 * Get the associated ID. The meaning of the ID is defined by the creator of the object.
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return The state as defined by the creator of the object. 0 if there is not state or on errors.
 */
uint32_t dc_lot_get_id(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return 0;
	}
	return lot->m_id;
}


/**
 * Get the associated timestamp. The meaning of the timestamp is defined by the creator of the object.
 *
 * @memberof dc_lot_t
 *
 * @param lot The lot object.
 *
 * @return The timestamp as defined by the creator of the object. 0 if there is not timestamp or on errors.
 */
time_t dc_lot_get_timestamp(dc_lot_t* lot)
{
	if( lot == NULL || lot->m_magic != MR_LOT_MAGIC ) {
		return 0;
	}
	return lot->m_timestamp;
}


void dc_lot_fill(dc_lot_t* ths, const dc_msg_t* msg, const dc_chat_t* chat, const dc_contact_t* contact)
{
	if( ths == NULL || ths->m_magic != MR_LOT_MAGIC || msg == NULL ) {
		return;
	}

	if( msg->m_from_id == DC_CONTACT_ID_SELF )
	{
		if( dc_msg_is_info(msg) ) {
			ths->m_text1 = NULL;
			ths->m_text1_meaning = 0;
		}
		else {
			ths->m_text1 = dc_stock_str(DC_STR_SELF);
			ths->m_text1_meaning = MR_TEXT1_SELF;
		}
	}
	else if( chat == NULL )
	{
		ths->m_text1 = NULL;
		ths->m_text1_meaning = 0;
	}
	else if( DC_CHAT_TYPE_IS_MULTI(chat->m_type) )
	{
		if( dc_msg_is_info(msg) || contact==NULL ) {
			ths->m_text1 = NULL;
			ths->m_text1_meaning = 0;
		}
		else {
			ths->m_text1 = dc_contact_get_first_name(contact);
			ths->m_text1_meaning = MR_TEXT1_USERNAME;
		}
	}

	ths->m_text2     = dc_msg_get_summarytext_by_raw(msg->m_type, msg->m_text, msg->m_param, MR_SUMMARY_CHARACTERS);
	ths->m_timestamp = dc_msg_get_timestamp(msg);
	ths->m_state     = msg->m_state;
}
