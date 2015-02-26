#ifndef _CONFIG_H
#define _CONFIG_H
/*
 * This file is distributed as part of the MariaDB Corporation MaxScale.  It is free
 * software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright MariaDB Corporation Ab 2013-2014
 */
#include <skygw_utils.h>

/**
 * @file config.h The configuration handling elements
 *
 * @verbatim
 * Revision History
 *
 * Date		Who			Description
 * 21/06/13	Mark Riddoch		Initial implementation
 * 07/05/14	Massimiliano Pinto	Added version_string to global configuration
 * 23/05/14	Massimiliano Pinto	Added id to global configuration
 * 17/10/14	Mark Riddoch		Added poll tuning configuration parameters
 * 26/02/15	Massimiliano Pinto	Added notification service configuration parameters
 *
 * @endverbatim
 */

#define		DEFAULT_NBPOLLS		3	/**< Default number of non block polls before we block */
#define		DEFAULT_POLLSLEEP	1000	/**< Default poll wait time (milliseconds) */
/**
 * Maximum length for configuration parameter value.
 */
enum {MAX_PARAM_LEN=256};

typedef enum {
        UNDEFINED_TYPE     = 0x00,
        STRING_TYPE        = 0x01,
        COUNT_TYPE         = 0x02,
        PERCENT_TYPE       = 0x04,
        BOOL_TYPE          = 0x08,
	SQLVAR_TARGET_TYPE = 0x10
} config_param_type_t;

typedef enum {
	TYPE_UNDEFINED = 0,
	TYPE_MASTER,
	TYPE_ALL
} target_t;

enum {MAX_RLAG_NOT_AVAILABLE=-1, MAX_RLAG_UNDEFINED=-2};

#define PARAM_IS_TYPE(p,t) ((p) & (t))

/**
 * The config parameter
 */
typedef struct config_parameter {
	char			*name;		/**< The name of the parameter */
	char                    *value;         /**< The value of the parameter */
	union {                                 /*< qualified parameter value by type */
                char*           valstr;         /*< terminated char* array */
                int             valcount;       /*< int */
                int             valpercent;     /*< int */
                bool            valbool;        /*< bool */
                target_t        valtarget;      /*< sql variable route target */
        } qfd;
        config_param_type_t     qfd_param_type; 
	struct config_parameter	*next;		/**< Next pointer in the linked list */
} CONFIG_PARAMETER;

/**
 * The config context structure, used to build the configuration
 * data during the parse process
 */
typedef struct	config_context {
	char			*object;	/**< The name of the object being configured */
	CONFIG_PARAMETER	*parameters;	/**< The list of parameter values */
	void			*element;	/**< The element created from the data */
	struct config_context	*next;		/**< Next pointer in the linked list */
} CONFIG_CONTEXT;

/**
 * The gateway global configuration data
 */
typedef struct {
	int			n_threads;		/**< Number of polling threads */
	char			*version_string;	/**< The version string of embedded database library */
	unsigned long		id;			/**< MaxScale ID */
	unsigned int		n_nbpoll;		/**< Tune number of non-blocking polls */
	unsigned int		pollsleep;		/**< Wait time in blocking polls */
} GATEWAY_CONF;

/**
 * The configuration and usage information data for feeback service
 */
typedef struct {
	int	feedback_enable;	/**< Enable/Disable Notification feedback */
	char	*feedback_url;		/**< URL to which the data is sent */
	char	*feedback_user_info;	/**< User info included in the feedback data sent */
	char	*feedback_server_uid;	/**< Installation identifier included in the feedback data sent */
	int	feedback_send_timeout;	/**< An attempt to send the data times out and fails after this many seconds */
} FEEDBACK_CONF;

extern int	    config_load(char *);
extern int	    config_reload();
extern int	    config_threadcount();
extern unsigned int config_nbpolls();
extern unsigned int config_pollsleep();
CONFIG_PARAMETER*   config_get_param(CONFIG_PARAMETER* params, const char* name);
config_param_type_t config_get_paramtype(CONFIG_PARAMETER* param);
CONFIG_PARAMETER*   config_clone_param(CONFIG_PARAMETER* param);
int          config_truth_value(char *str);
bool config_set_qualified_param(
        CONFIG_PARAMETER* param, 
        void* val, 
        config_param_type_t type);


bool config_get_valint(
	int*                val,
        CONFIG_PARAMETER*   param,
        const char*         name, /*< if NULL examine current param only */
        config_param_type_t ptype);

bool config_get_valbool(
	bool*               val,
	CONFIG_PARAMETER*   param,
	const char*         name, /*< if NULL examine current param only */
	config_param_type_t ptype);

bool config_get_valtarget(
	target_t*           val,
	CONFIG_PARAMETER*   param,
	const char*         name, /*< if NULL examine current param only */
	config_param_type_t ptype);
#endif
