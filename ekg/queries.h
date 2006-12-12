#ifndef __EKG_QUERIES
#define __EKG_QUERIES

#define QUERY_ARGS_MAX 12

enum query_arg_type {
	QUERY_ARG_END = 0,	/* MUSTBE LAST ELEMENT OF `query_arg_type` */

	QUERY_ARG_CHARP,	/* char * 	*/
	QUERY_ARG_CHARPP,	/* char ** 	*/
	QUERY_ARG_INT, 		/* int */
	QUERY_ARG_UINT,		/* unsgined int */		/* -> time_t, uint32_t */
	QUERY_ARG_WINDOW = 100, /* window_t	*/
	QUERY_ARG_FSTRING, 	/* fstring_t	*/
};

struct query {
	int id;
	char *name;
	enum query_arg_type params[QUERY_ARGS_MAX];	/* scripts will use it */
};

/* uniq id of known queries... keep it sorted.. please */
enum queries_id {
	IRC_TOPIC = 0,
	MAIL_COUNT,

	BINDING_COMMAND, BINDING_DEFAULT, BINDING_SET, 
	CONFERENCE_RENAMED,

	CONFIG_POSTINIT,
	DAY_CHANGED,

	EVENT_AWAY, EVENT_AVAIL, EVENT_DESCR, EVENT_ONLINE, EVENT_NA,

	METACONTACT_ADDED, METACONTACT_ITEM_ADDED, METACONTACT_ITEM_REMOVED, METACONTACT_REMOVED,

	PLUGIN_PRINT_VERSION,

	PROTOCOL_CONNECTED, PROTOCOL_DISCONNECTED, PROTOCOL_IGNORE, PROTOCOL_MESSAGE, PROTOCOL_MESSAGE_ACK, PROTOCOL_STATUS, PROTOCOL_VALIDATE_UID, PROTOCOL_XSTATE,

	SESSION_ADDED, SESSION_CHANGED, SESSION_REMOVED, SESSION_RENAMED, SESSION_STATUS,

	SET_VARS_DEFAULT,
	STATUS_SHOW,

	UI_BEEP, UI_IS_INITIALIZED, UI_KEYPRESS, UI_LOOP, UI_WINDOW_ACT_CHANGED,
	UI_WINDOW_CLEAR, UI_WINDOW_KILL, UI_WINDOW_NEW, UI_WINDOW_PRINT, UI_WINDOW_REFRESH,
	UI_WINDOW_SWITCH, UI_WINDOW_TARGET_CHANGED,

	USERLIST_ADDED, USERLIST_CHANGED, USERLIST_REMOVED, USERLIST_RENAMED,

	VARIABLE_CHANGED,

	QUERY_EXTERNAL,
};

#ifdef __DECLARE_QUERIES_STUFF
#undef __DECLARE_QUERIES_STUFF

/* list of known queries. keep it sorted too. */

struct query query_list[] = {
	{ IRC_TOPIC, "irc-topic", {
		QUERY_ARG_CHARP,		/* if CHANNEL -> topic; 	if USER -> ident@host */
		QUERY_ARG_CHARP,		/* if CHANNEL -> topicby;	if USER -> realname */
		QUERY_ARG_CHARP,		/* if CHANNEL -> chanmodes;	if USER -> undefined */
		QUERY_ARG_END } },

	{ MAIL_COUNT, "mail-count", {
		QUERY_ARG_INT,			/* mail count */
		QUERY_ARG_END } },

	{ BINDING_COMMAND, "binding-command", {
		/* XXX */
		QUERY_ARG_END } },

	{ BINDING_DEFAULT, "binding-default", {
		/* XXX */
		QUERY_ARG_END } },

	{ BINDING_SET, "binding-set", {
		/* XXX */
		QUERY_ARG_END } },

	{ CONFERENCE_RENAMED, "conference-renamed", {
		/* XXX */
		QUERY_ARG_END } },

	{ CONFIG_POSTINIT, "config-postinit", {
		QUERY_ARG_END } },		/* no params */

	{ DAY_CHANGED, "day-changed", {		/* XXX: struct tm *, struct tm * 	*/
		QUERY_ARG_END } },

	{ EVENT_AWAY, "event_away", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_END } },

	{ EVENT_AVAIL, "event_avail", {
		/* XXX */
		QUERY_ARG_END } },

	{ EVENT_DESCR, "event_descr", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_CHARP,		/* descr */
		QUERY_ARG_END } },

	{ EVENT_ONLINE, "event_online", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_END } },

	{ EVENT_NA, "event_na", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_END } },

	{ METACONTACT_ADDED, "metacontact-added", {
		QUERY_ARG_CHARP,		/* metacontact name */
		QUERY_ARG_END } },

	{ METACONTACT_ITEM_ADDED, "metacontact-item-added", {
		/* XXX */
		QUERY_ARG_END } },
	
	{ METACONTACT_ITEM_REMOVED, "metacontact-item-removed", {
		/* XXX */
		QUERY_ARG_END } },

	{ METACONTACT_REMOVED, "metacontact-removed", {
		QUERY_ARG_CHARP,		/* metacontact name */
		QUERY_ARG_END } },

	{ PLUGIN_PRINT_VERSION, "plugin-print-version", {
		QUERY_ARG_END } },		/* no params */

	{ PROTOCOL_CONNECTED, "protocol-connected", {
		QUERY_ARG_CHARP,		/* session */
		QUERY_ARG_END } }, 

	{ PROTOCOL_DISCONNECTED, "protocol-disconnected", {
		QUERY_ARG_CHARP,		/* session */
		QUERY_ARG_CHARP,		/* reason */
		QUERY_ARG_INT,			/* type */
		QUERY_ARG_END } }, 

	{ PROTOCOL_IGNORE, "protocol-ignore", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_INT,			/* oldlevel */
		QUERY_ARG_INT,			/* newlevel */
		QUERY_ARG_END } },

	{ PROTOCOL_MESSAGE, "protocol-message", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_CHARPP,		/* rcpts */
		QUERY_ARG_CHARP,		/* text */
		QUERY_ARG_UINT,	/* uint32 */	/* format */
		QUERY_ARG_UINT,	/* time_t */	/* sent */
		QUERY_ARG_INT,			/* class */
		QUERY_ARG_CHARP,		/* seq */
		QUERY_ARG_INT,			/* dobeep */
		QUERY_ARG_INT,			/* secure */
		QUERY_ARG_END } },

	{ PROTOCOL_MESSAGE_ACK, "protocol-message-ack", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_CHARP,		/* seq */
		QUERY_ARG_CHARP,		/* status */
		QUERY_ARG_END } },

	{ PROTOCOL_STATUS, "protocol-status", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_CHARP,		/* status */
		QUERY_ARG_CHARP,		/* descr */
		QUERY_ARG_CHARP,		/* host */
		QUERY_ARG_INT,			/* port */
		QUERY_ARG_UINT, /* time_t */	/* when */
		QUERY_ARG_END } }, 

	{ PROTOCOL_VALIDATE_UID, "protocol-validate-uid", {
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_INT,			/* valid */
		QUERY_ARG_END } },

	{ PROTOCOL_XSTATE, "protocol-xstate", {
		QUERY_ARG_CHARP,		/* session */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_INT,			/* state 	- bits on */
		QUERY_ARG_INT,			/* offstate 	- bits off */
		QUERY_ARG_END } },
	
	{ SESSION_ADDED, "session-added", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_END } },

	{ SESSION_CHANGED, "session-changed", {
		QUERY_ARG_END } },		/* no params */

	{ SESSION_REMOVED, "session-removed", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_END } },

	{ SESSION_RENAMED, "session-renamed", {
		QUERY_ARG_CHARP,		/* new session alias */
		QUERY_ARG_END } },

	{ SESSION_STATUS, "session-status", {
		/* XXX */
		QUERY_ARG_END } },

	{ SET_VARS_DEFAULT, "set-vars-default", {
		QUERY_ARG_END } },		/* no params */

	{ STATUS_SHOW, "status-show", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_END } },

	{ UI_BEEP, "ui-beep", {
		QUERY_ARG_END } }, 		/* no params */

	{ UI_IS_INITIALIZED, "ui-is-initialized", {
		QUERY_ARG_INT,			/* is_ui */
		QUERY_ARG_END } }, 

	{ UI_KEYPRESS, "ui-keypress", {
		QUERY_ARG_UINT,	 		/* key */
		QUERY_ARG_END } },

	{ UI_LOOP, "ui-loop", {
		QUERY_ARG_END } }, 		/* no params */

	{ UI_WINDOW_ACT_CHANGED, "ui-window-act-changed", {
		QUERY_ARG_END } },		/* no params */

	{ UI_WINDOW_CLEAR, "ui-window-clear", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_END } },

	{ UI_WINDOW_KILL, "ui-window-kill", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_END } },

	{ UI_WINDOW_NEW, "ui-window-new", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_END } }, 

	{ UI_WINDOW_PRINT, "ui-window-print", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_FSTRING,		/* fstring_t */
		QUERY_ARG_END } }, 

	{ UI_WINDOW_REFRESH, "ui-window-refresh", {
		QUERY_ARG_END } },		/* no params */

	{ UI_WINDOW_SWITCH, "ui-window-switch", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_END } },

	{ UI_WINDOW_TARGET_CHANGED , "ui-window-target-changed", {
		QUERY_ARG_WINDOW,		/* window */
		QUERY_ARG_END } },

	{ USERLIST_ADDED, "userlist-added", {
		/* XXX, we need here a session->uid too (?) */

		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_CHARP,		/* nickname */
		QUERY_ARG_INT,			/* quiet */
		QUERY_ARG_END } },

	{ USERLIST_CHANGED, "userlist-changed", {
		QUERY_ARG_CHARP,		/* session uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_END } },

	{ USERLIST_REMOVED, "userlist-removed", {
		/* XXX, we need here a session->uid too (?) */

		QUERY_ARG_CHARP,		/* nickname or uid */
		QUERY_ARG_CHARP,		/* uid */
		QUERY_ARG_END } },

	{ USERLIST_RENAMED, "userlist-renamed", {
		/* XXX */
		QUERY_ARG_END } },

	{ VARIABLE_CHANGED, "variable-changed", {
		QUERY_ARG_CHARP,		/* variable */
		QUERY_ARG_END } },
};

/* other, not listed above here queries, for example plugin which use internally his own query, 
 * and if devel of that plugin doesn't want share with us info about that plugin..
 * can use query_connect() query_emit() and it will work... however, binding that query from scripts/events (/on) won't work.. untill devel fill query_arg_type...
 */

static list_t queries_external;
static int queries_count = QUERY_EXTERNAL;	/* list_count(queries_other)+QUERY_EXTERNAL */
#else

extern struct query query_list[];		/* for: events.h scripts.h */

#endif

#endif

