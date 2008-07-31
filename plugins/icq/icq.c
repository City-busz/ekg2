/*
 *  (C) Copyright 2006-2008 Jakub Zawadzki <darkjames@darkjames.ath.cx>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ekg/debug.h>
#include <ekg/plugins.h>
#include <ekg/queries.h>
#include <ekg/protocol.h>
#include <ekg/themes.h>
#include <ekg/windows.h>	// XXX
#include <ekg/stuff.h>
#include <ekg/vars.h>
#include <ekg/xmalloc.h>

#include "icq.h"
#include "misc.h"

#include "icq_flap_handlers.h"
#include "icq_snac_handlers.h"

#define ICQ_HUB_SERVER "login.icq.com"
#define ICQ_HUB_PORT	5190

int icq_send_pkt(session_t *s, string_t buf) {
	icq_private_t *j;
	watch_t *w;
	int fd;
	list_t l;

	if (!s || !(j = s->priv) || !buf)
		return -1;

	fd = j->fd;

	debug_io("icq_send_pkt(%s) fd: %d len: %d\n", s->uid, fd, buf->len);
	icq_hexdump(DEBUG_IO, (unsigned char *) buf->str, buf->len);

	for (l = watches; l; l = l->next) {
		watch_t *w = l->data;

		if (w && w->is_session && w->data == s && w->type == WATCH_WRITE_LINE && w->fd == fd) {
			string_append_raw(w->buf, buf->str, buf->len);
			string_free(buf, 1);
			return 1;
		}
	}

	w = watch_add_session(s, fd, WATCH_WRITE_LINE, NULL);

	string_append_raw(w->buf, buf->str, buf->len);
	string_free(buf, 1);

	watch_handle_write(w);
	return 0;
}

static TIMER_SESSION(icq_ping) {
	icq_private_t *j;

	if (type)
		return 0;

	if (!s || !(j = s->priv) || !s->connected)
		return -1;

/* 	write_flap(&packet, ICQ_PING_CHAN); */

	return 0;
}

void icq_session_connected(session_t *s) {
	string_t pkt;

	{
		#define m_bAvatarsEnabled 0
		#define m_bUtfEnabled 0
		#define m_bAimEnabled 0

		string_t pkt, tlv_5;

		tlv_5 = string_init(NULL);

#ifdef DBG_CAPMTN
		icq_pack_append(tlv_5, "I", (uint32_t) 0x563FC809);		/* CAP_TYPING */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x0B6F41BD);
		icq_pack_append(tlv_5, "I", (uint32_t) 0x9F794226);
		icq_pack_append(tlv_5, "I", (uint32_t) 0x09DFA2F3);
#endif

		icq_pack_append(tlv_5, "P", (uint32_t) 0x1349);			/* AIM_CAPS_ICQSERVERRELAY */
	
		/* Broadcasts the capability to receive UTF8 encoded messages */
		if (m_bUtfEnabled) 
			icq_pack_append(tlv_5, "I", (uint32_t) 0x134E);		/* CAP_UTF8MSGS */
#ifdef DBG_NEWCAPS			/* Tells server we understand to new format of caps */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x0000);			/* CAP_NEWCAPS */
#endif

#ifdef DBG_CAPXTRAZ
		icq_pack_append(tlv_5, "I", (uint32_t) 0x1a093c6c);		/* CAP_XTRAZ */
		icq_pack_append(tlv_5, "I", (uint32_t) 0xd7fd4ec5);		/* Broadcasts the capability to handle */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x9d51a647);		/* Xtraz */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x4e34f5a0);
#endif
		if (m_bAvatarsEnabled)
			icq_pack_append(tlv_5, "I", (uint32_t) 0x134C);		/* CAP_DEVILS */

#ifdef DBG_OSCARFT
		/* Broadcasts the capability to receive Oscar File Transfers */
		icq_pack_append(tlv_5, "P", (uint32_t) 0x1343); 		/* CAP_AIM_FILE */
#endif

		if (m_bAimEnabled)
			icq_pack_append(tlv_5, "I", (uint32_t) 0x134D);	/* Tells the server we can speak to AIM */
#ifdef DBG_AIMCONTACTSEND
		icq_pack_append(tlv_5, "P", (uint32_t) 0x134B);		/* CAP_AIM_SENDBUDDYLIST */
#endif
#if 0
		BYTE bXStatus = getContactXStatus(NULL);
		if (bXStatus)
		{
			packBuffer(tlv_5, capXStatus[bXStatus-1], 0x10);
		}
#endif
		icq_pack_append(tlv_5, "P", (uint32_t) 0x1344);		/* AIM_CAPS_ICQDIRECT */

		/*packDWord(&packet, 0x178c2d9b); // Unknown cap
		  packDWord(&packet, 0xdaa545bb);
		  packDWord(&packet, 0x8ddbf3bd);
		  packDWord(&packet, 0xbd53a10a);*/

#ifdef DBG_CAPHTML
		icq_pack_append(tlv_5, "I", (uint32_t) 0x0138ca7b);	/* CAP_HTMLMSGS */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x769a4915);	/* Broadcasts the capability to receive */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x88f213fc);	/* HTML messages */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x00979ea8);
#endif
		icq_pack_append(tlv_5, "I", (uint32_t) 0x4D697261);   /* Miranda Signature */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x6E64614D);
		icq_pack_append(tlv_5, "I", (uint32_t) 0x12345678);	/* XXX, MIRANDA_VERSION */
		icq_pack_append(tlv_5, "I", (uint32_t) 0x80050003);

		pkt = icq_pack("T", icq_pack_tlv(0x05, tlv_5->str, tlv_5->len));
		
		icq_makesnac(s, pkt, 0x01, 0x04, 0, 0);
		icq_send_pkt(s, pkt);

		string_free(tlv_5, 1);
	}

	/* SNAC 3,4: Tell server who's on our list */
	if (s->userlist) {
		pkt = string_init(NULL);
		/* XXX, dla kazdego kontaktu... */
		icq_makesnac(s, pkt, 0x03, 0x04, 0, 0);
		icq_send_pkt(s, pkt); pkt = NULL;
	}

	if (s->status == EKG_STATUS_INVISIBLE) {
		/* Tell server who's on our visible list */
#if MIRANDA
		if (!j->ssi)
			sendEntireListServ(ICQ_BOS_FAMILY, ICQ_CLI_ADDVISIBLE, BUL_VISIBLE);
		else
			updateServVisibilityCode(3);
#endif
	}

	if (s->status != EKG_STATUS_INVISIBLE)
	{
		/* Tell server who's on our invisible list */
#if MIRANDA
		if (!j->ssi)
			sendEntireListServ(ICQ_BOS_FAMILY, ICQ_CLI_ADDINVISIBLE, BUL_INVISIBLE);
		else
			updateServVisibilityCode(4);
#endif
		;
	}

	/* SNAC 1,1E: Set status */
	{
#if MIRANDA
		DWORD dwDirectCookie = rand() ^ (rand() << 16);
		BYTE bXStatus = getContactXStatus(NULL);
		char szMoodId[32];
		WORD cbMoodId = 0;
		WORD cbMoodData = 0;

		if (bXStatus && moodXStatus[bXStatus-1] != -1)
		{ // prepare mood id
			null_snprintf(szMoodId, SIZEOF(szMoodId), "icqmood%d", moodXStatus[bXStatus-1]);
			cbMoodId = strlennull(szMoodId);
			cbMoodData = 8;
		}
#endif
		string_t pkt;
		string_t tlv_c;
		uint16_t status;

		status = icq_status(s->status);

		pkt = string_init(NULL);

		icq_pack_append(pkt, "tI", icq_pack_tlv_dword(0x06, (0x00 << 8 | status)));	/* TLV 6: Status mode and security flags */
		icq_pack_append(pkt, "tW", icq_pack_tlv_word(0x08, 0x00));			/* TLV 8: Error code */

	/* TLV C: Direct connection info */
		tlv_c = string_init(NULL);
		icq_pack_append(tlv_c, "I", (uint32_t) 0x00000000);	/* XXX, getSettingDword(NULL, "RealIP", 0) */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x00000000);	/* XXX, nPort */
		icq_pack_append(tlv_c, "C", (uint32_t) 0x04);		/* Normal direct connection (without proxy/firewall) */
		icq_pack_append(tlv_c, "W", (uint32_t) 0x08);		/* Protocol version */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x00);		/* XXX, DC Cookie */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x50);		/* WEBFRONTPORT */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x03);		/* CLIENTFEATURES */
		icq_pack_append(tlv_c, "I", (uint32_t) 0xffffffff);	/* gbUnicodeCore ? 0x7fffffff : 0xffffffff */ /* Abused timestamp */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x80050003);	/* Abused timestamp */
		icq_pack_append(tlv_c, "I", (uint32_t) 0x00000000);	/* XXX, Timestamp */
		icq_pack_append(tlv_c, "W", (uint32_t) 0x0000);		/* Unknown */

		icq_pack_append(pkt, "T", icq_pack_tlv(0x0C, tlv_c->str, tlv_c->len));
		/* XXX, assert(tlv_c->len == 0x25) */
		string_free(tlv_c, 1);

		icq_pack_append(pkt, "tW", icq_pack_tlv_word(0x1F, 0x00));

#if MIRANDA
		if (cbMoodId)
		{ // Pack mood data
			packWord(&packet, 0x1D);              // TLV 1D
			packWord(&packet, (WORD)(cbMoodId + 4)); // TLV length
			packWord(&packet, 0x0E);              // Item Type
			packWord(&packet, cbMoodId);          // Flags + Item Length
			packBuffer(&packet, (LPBYTE)szMoodId, cbMoodId); // Mood
		}
#endif
		icq_makesnac(s, pkt, 0x01, 0x1E, 0, 0);
		icq_send_pkt(s, pkt); pkt = NULL;
	}
	/* SNAC 1,11 */
	pkt = icq_pack("I", (uint32_t) 0x00000000);
	icq_makesnac(s, pkt, 0x01, 0x11, 0, 0);
	icq_send_pkt(s, pkt);

	/* j->idleAllow = 0; */

	/* Finish Login sequence */

#if 1	/* XXX, wtf? */
	pkt = string_init(NULL);

	icq_pack_append(pkt, "WWI", (uint32_t) 0x22, (uint32_t) 0x01, (uint32_t) 0x0110161b);	/* imitate ICQ 6 behaviour */
	icq_pack_append(pkt, "WWI", (uint32_t) 0x01, (uint32_t) 0x04, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x13, (uint32_t) 0x04, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x02, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x03, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x15, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x04, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x06, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x09, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x0a, (uint32_t) 0x01, (uint32_t) 0x0110161b);
	icq_pack_append(pkt, "WWI", (uint32_t) 0x0b, (uint32_t) 0x01, (uint32_t) 0x0110161b);

	icq_makesnac(s, pkt, 0x01, 0x02, 0, 0);
	icq_send_pkt(s, pkt); pkt = NULL;
#endif

	debug_white(" *** Yeehah, login sequence complete\n");

#if 0
	info->bLoggedIn = 1;
#endif
	/* login sequence is complete enter logged-in mode */

	if (!s->connected) {
		/* Get Offline Messages Reqeust */
		/* XXX, cookie */
		pkt = string_init(NULL);
		icq_makesnac(s, pkt, 0x4, 0x10, 0, 0);
		icq_send_pkt(s, pkt);

		/* Update our information from the server */
		pkt = icq_pack("i", atoi(s->uid+4));
		/* XXX, cookie, in-quiet-mode */
		icq_makemetasnac(s, pkt, 2000, 0x04D0, 0);
		icq_send_pkt(s, pkt);

		/* Start sending Keep-Alive packets */
		timer_remove_session(s, "ping");
		timer_add_session(s, "ping", 60, 1, icq_ping);
#if 0
		if (m_bAvatarsEnabled)
		{ // Send SNAC 1,4 - request avatar family 0x10 connection
			icq_requestnewfamily(ICQ_AVATAR_FAMILY, &CIcqProto::StartAvatarThread);

			m_pendingAvatarsStart = 1;
			NetLog_Server("Requesting Avatar family entry point.");
		}
#endif
	}
	protocol_connected_emit(s);

#if MIRANDA
	if (m_bAimEnabled)
	{
		char **szMsg = MirandaStatusToAwayMsg(m_iStatus);

		if (szMsg)
			icq_sendSetAimAwayMsgServ(*szMsg);
	}
#endif
}

static int icq_theme_init();
PLUGIN_DEFINE(icq, PLUGIN_PROTOCOL, icq_theme_init);

static QUERY(icq_session_init) {
	char		*session = *(va_arg(ap, char**));
	session_t	*s = session_find(session);
	icq_private_t *j;

	if (!s || s->priv || s->plugin != &icq_plugin)
		return 1;

#if OLD_ICQ
	userlist_free(s);
	userlist_read(s);
#endif

	j = xmalloc(sizeof(icq_private_t));
	j->fd = -1;
	j->fd2= -1;

	s->priv = j;
	return 0;
}

static QUERY(icq_session_deinit) {
	char		*session = *(va_arg(ap, char**));
	session_t	*s = session_find(session);
	icq_private_t *j;

	if (!s || !(j = s->priv) || s->plugin != &icq_plugin)
		return 1;
#if OLD_ICQ
	userlist_write(s);
#endif
	s->priv = NULL;

	string_free(j->cookie, 1);

	xfree(j);
	return 0;
}

static QUERY(icq_validate_uid) {
	char	*uid 	= *(va_arg(ap, char **));
	int	*valid 	= va_arg(ap, int *);

	if (!uid)
		return 0;

	if (!xstrncmp(uid, "icq:", 4) && uid[4]) {
		uid+=4;
		/* now let's check if after icq: we have only digits */
		for (; *uid; uid++)
			if (!isdigit(*uid))
				return 0;

		(*valid)++;
		return -1;
	}
	return 0;
}

static QUERY(icq_print_version) {
	return 0;
}

void icq_handle_disconnect(session_t *s, const char *reason, int type) {
	icq_private_t *j;

	if (!s || !(j = s->priv))
		return;

	if (!s->connected && !j->connecting)
		return;

	j->connecting = 0;
	timer_remove_session(s, "ping");
	protocol_disconnected_emit(s, reason, type);

	/* XXX, watch_free() */

	if (j->fd != -1) {
		close(j->fd);
		j->fd = -1;
	}

	if (j->fd2 != -1) {
		close(j->fd2);
		j->fd2 = -1;
	}
}

static WATCHER_SESSION(icq_handle_stream);

static WATCHER_SESSION(icq_handle_connect) {
	icq_private_t *j = NULL;
	int res = 0; 
	socklen_t res_size = sizeof(res);

	if (type) 
		return 0;

	if (type == 1) {
		debug ("[icq] handle_connect(): type %d\n", type);
		return 0;
	}

	if (!s || !(j = s->priv)) { 
		debug_error("icq_handle_connect() s: 0x%x j: 0x%x\n", s, j);
		return -1;
	}

	debug("[icq] handle_connect(%d)\n", j->connecting);

	if (type || getsockopt(fd, SOL_SOCKET, SO_ERROR, &res, &res_size) || res) {
		if (type) 
			debug_error("[icq] handle_connect(): SO_ERROR %s\n", strerror(res));

		icq_handle_disconnect(s, (type == 2) ? "Connection timed out" : strerror(res), EKG_DISCONNECT_FAILURE);
	}

	watch_add_session(s, fd, WATCH_READ, icq_handle_stream);

	return -1;
}

static WATCHER_SESSION(icq_handle_stream) {
	icq_private_t *j = NULL;

	char buf[8192];
	int len, ret;

	if (!s || !(j = s->priv)) { 
		debug_error("icq_handle_stream() s: 0x%x j: 0x%x\n", s, j);
		return -1;
	}

	if (type)
		return 0;

	len = read(fd, buf, sizeof(buf));

	debug("icq_handle_stream(%d) rcv: %d\n", j->connecting, len);

	if (len < 1) {
		icq_handle_disconnect(s, strerror(errno), EKG_DISCONNECT_NETWORK);
		return -1;
	}

	icq_hexdump(DEBUG_IORECV, (unsigned char *) buf, len);

	/* XXX,
	 * 	- add buf to string_t
	 * 	- icq_flap_handler(s, buffer);
	 */

	ret = icq_flap_handler(s, fd, buf, len);

	switch (ret) {		/* XXX, magic values */
		case 0:
			/* OK */
			break;

		case -1:
			/* XXX, fragmentacja */
			debug_white("icq_flap_loop() NEED MORE DATA\n");
			break;

		case -2:
			debug_error("icq_flap_loop() DISCONNECT\n");
			return -1;

		default:
			debug_error("icq_flap_loop() == %d ???\n", ret);
			break;
	}

	if (j->fd2 != -1) {	/* fd changed */
		close(j->fd);
		j->fd = j->fd2;
		j->fd2 = -1;

		if (j->connecting == 2) {
			watch_add_session(s, j->fd, WATCH_WRITE, icq_handle_connect);
		} else {
			debug_error("unknown j->connecting: %d\n", j->connecting);
		}
		return -1;
	}

	return 0;
}

static WATCHER(icq_handle_hubresolver) {
	session_t *s = session_find((char *) data);
	icq_private_t *j;

	struct sockaddr_in sin;
	struct in_addr a;
	int one = 1;
	int hubport;
	int res;

	if (type) {
		xfree(data);
		close(fd);
		return 0;
	}

	if (!s || !(j = s->priv))
		return -1;

	if (!j->connecting)	/* user makes /disconnect before resolver finished */
		return -1;

	res = read(fd, &a, sizeof(a));

	if ((res != sizeof(a)) || (res && a.s_addr == INADDR_NONE /* INADDR_NONE kiedy NXDOMAIN */)) {
		if (res == -1)
			debug_error("[icq] unable to read data from resolver: %s\n", strerror(errno));
		else
			debug_error("[icq] read %d bytes from resolver. not good\n", res);

		/* no point in reconnecting by icq_handle_disconnect() XXX? */

		print("conn_failed", format_find("conn_failed_resolving"), session_name(s));
		j->connecting = 0;
		return -1;
	}

        debug_function("[icq] resolved to %s\n", inet_ntoa(a));

/* port */
	hubport = session_int_get(s, "hubport");
	if (hubport < 1 || hubport > 65535)
		hubport = ICQ_HUB_PORT;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		debug("[icq] socket() failed: %s\n", strerror(errno));
		icq_handle_disconnect(s, strerror(errno), EKG_DISCONNECT_FAILURE); 
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = ntohs(hubport);
        sin.sin_addr.s_addr = a.s_addr;

        if (ioctl(fd, FIONBIO, &one) == -1) 
		debug_error("[icq] ioctl() FIONBIO failed: %s\n", strerror(errno));
        if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one)) == -1) 
		debug_error("[icq] setsockopt() SO_KEEPALIVE failed: %s\n", strerror(errno));

	res = connect(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)); 

	if (res == -1 && errno != EINPROGRESS) {
		int err = errno;

                debug_error("[icq] connect() failed: %s (errno=%d)\n", strerror(err), err);
		icq_handle_disconnect(s, strerror(err), EKG_DISCONNECT_FAILURE);
		return -1;
	}

	j->fd = fd;

	watch_add_session(s, fd, WATCH_WRITE, icq_handle_connect);
	return -1;
}

static COMMAND(icq_command_away) {
	const char *format;

	if (!xstrcmp(name, ("_autoback"))) {
		format = "auto_back";
		session_status_set(session, EKG_STATUS_AUTOBACK);
		session_unidle(session);
	} else if (!xstrcmp(name, ("back"))) {
		format = "back";
		session_status_set(session, EKG_STATUS_AVAIL);
		session_unidle(session);
	} else if (!xstrcmp(name, ("_autoaway"))) {
		format = "auto_away";
		session_status_set(session, EKG_STATUS_AUTOAWAY);
	} else if (!xstrcmp(name, ("_autoxa"))) {
		format = "auto_xa";
		session_status_set(session, EKG_STATUS_AUTOXA);
	} else if (!xstrcmp(name, ("away"))) {
		format = "away"; 
		session_status_set(session, EKG_STATUS_AWAY);
		session_unidle(session);
	} else if (!xstrcmp(name, ("dnd"))) {
		format = "dnd";
		session_status_set(session, EKG_STATUS_DND);
		session_unidle(session);
	} else if (!xstrcmp(name, ("ffc"))) {
	        format = "ffc";
	        session_status_set(session, EKG_STATUS_FFC);
                session_unidle(session);
        } else if (!xstrcmp(name, ("xa"))) {
		format = "xa";
		session_status_set(session, EKG_STATUS_XA);
		session_unidle(session);
	} else if (!xstrcmp(name, ("invisible"))) {
		format = "invisible";
		session_status_set(session, EKG_STATUS_INVISIBLE);
		session_unidle(session);
	} else
		return -1;

	ekg_update_status(session);
	
	printq(format, session_name(session));

/* XXX
	if (session->connected)
		icq_write_status(session);
 */	
	return 0;
}

static COMMAND(icq_command_connect) {
	icq_private_t *j = session->priv;
	const char *hubserver;

	if (j->connecting) {
		printq("during_connect", session_name(session));
		return -1;
	}

	if (session->connected) {
		printq("already_connected", session_name(session));
		return -1;
	}

/* proxy */
	if (session_int_get(session, "proxy") == 1) {
		/* XXX, implement proxy connection */
		debug_error("icq_command_connect() proxy?\n");
		return -1;
	}

/* XXX, jak user podal adres serwera, to lacz sie z serwerem */
	if ((session_get(session, "server"))) {
		/* XXX */
	}

/* hubserver */
	if (!(hubserver = session_get(session, "hubserver"))) 
		hubserver = ICQ_HUB_SERVER;

	j->connecting = 1;
	j->ssi = 1;	/* XXX */

	if (ekg_resolver2(&icq_plugin, hubserver, icq_handle_hubresolver, xstrdup(session->uid)) == NULL) {
		print("generic_error", strerror(errno));
		j->connecting = 0;
		return -1;
	}

	return 0;
}

static COMMAND(icq_command_disconnect) {
	icq_private_t *j = session->priv;

	if (timer_remove_session(session, "reconnect") == 0) {
		printq("auto_reconnect_removed", session_name(session));
		return 0;
	}

	if (!j->connecting && !session->connected) {
		printq("not_connected", session_name(session));
		return -1;
	}

	if (j->connecting)
		icq_handle_disconnect(session, NULL, EKG_DISCONNECT_STOPPED);
	else    
		icq_handle_disconnect(session, NULL, EKG_DISCONNECT_USER);

	return 0;
}

static COMMAND(icq_command_reconnect) {
	icq_private_t   *j = session->priv;

	if (j->connecting || session->connected)
		icq_command_disconnect(name, params, session, target, quiet);

	return icq_command_connect(name, params, session, target, quiet);
}

static COMMAND(icq_command_userinfo) {
	const char *uid;

	string_t pkt;
	int number;
	int minimal_req = 0;	/* XXX */

	if (!(uid = get_uid(session, target)))
		uid = target;

	if (!xstrncmp(uid, "icq:", 4))
		uid += 4;

	/* XXX */

	number = atoi(uid);

	if (number <= 0) {
		printq("invalid_params", name);
		return -1;
	}

	/* XXX */

	pkt = icq_pack("i", number);
	icq_makemetasnac(session, pkt, 2000, (minimal_req == 0) ? 1202 : 1210, 0);
	icq_send_pkt(session, pkt);
	return 0;
}

static COMMAND(icq_command_register) {
	printq("generic_error", "Create a new ICQ account on http://lite.icq.com/register");
	return 0;
}

static int icq_theme_init() {
#ifndef NO_DEFAULT_THEME
	format_add("icq_userinfo_start",	"%g,+=%G----- %g%3%G info for: %Ticq:%2%n", 1);

	format_add("icq_userinfo_affilations",	"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_basic",	"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_email",	"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_short",	"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_more",		"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_work",		"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_interests",	"%g|| %n  %T%3:%n %4", 1);
	format_add("icq_userinfo_notes",	"%g|| %n  %T%3:%n %4", 1);

	format_add("icq_userinfo_end",		"%g`+=%G----- End%n", 1);

#endif
	return 0;
}

static plugins_params_t icq_plugin_vars[] = {
	PLUGIN_VAR_ADD("alias", 		VAR_STR, NULL, 0, NULL), 
	PLUGIN_VAR_ADD("auto_connect", 		VAR_BOOL, "0", 0, NULL),
	PLUGIN_VAR_ADD("auto_reconnect", 	VAR_INT,  "0", 0, NULL),
	PLUGIN_VAR_ADD("log_formats", 		VAR_STR, "xml,simple", 0, NULL),
	PLUGIN_VAR_ADD("password", 		VAR_STR, NULL, 1, NULL),
	PLUGIN_VAR_ADD("plaintext_passwd",	VAR_BOOL, "0", 0, NULL),
	PLUGIN_VAR_END()
};

EXPORT int icq_plugin_init(int prio) {
#define ICQ_ONLY 		SESSION_MUSTBELONG | SESSION_MUSTHASPRIVATE 
#define ICQ_FLAGS 		ICQ_ONLY | SESSION_MUSTBECONNECTED
#define ICQ_FLAGS_TARGET	ICQ_FLAGS | COMMAND_ENABLEREQPARAMS | COMMAND_PARAMASTARGET

	PLUGIN_CHECK_VER("icq");

	icq_plugin.params = icq_plugin_vars;

	plugin_register(&icq_plugin, prio);

	query_connect_id(&icq_plugin, PROTOCOL_VALIDATE_UID, icq_validate_uid, NULL);
	query_connect_id(&icq_plugin, PLUGIN_PRINT_VERSION, icq_print_version, NULL);
	query_connect_id(&icq_plugin, SESSION_ADDED, icq_session_init, NULL);
	query_connect_id(&icq_plugin, SESSION_REMOVED, icq_session_deinit, NULL);

	command_add(&icq_plugin, "icq:away", NULL, icq_command_away, ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:back", NULL, icq_command_away, ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:dnd",  NULL, icq_command_away, ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:ffc",  NULL, icq_command_away, ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:invisible", NULL, icq_command_away, ICQ_ONLY, NULL);

	command_add(&icq_plugin, "icq:userinfo", "!u",	icq_command_userinfo,	ICQ_FLAGS_TARGET, NULL);
	command_add(&icq_plugin, "icq:register", NULL,	icq_command_register,	0, NULL);

	command_add(&icq_plugin, "icq:connect", NULL,	icq_command_connect, 	ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:disconnect", NULL,icq_command_disconnect,	ICQ_ONLY, NULL);
	command_add(&icq_plugin, "icq:reconnect", NULL,	icq_command_reconnect,	ICQ_ONLY, NULL);

	return 0;
}

static int icq_plugin_destroy() {
	plugin_unregister(&icq_plugin);
	return 0;
}