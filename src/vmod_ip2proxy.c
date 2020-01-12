/*
 * IP2Proxy Varnish library is distributed under LGPL version 3
 * Copyright (c) 2013-2019 IP2Proxy.com. support at ip2location dot com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <IP2Proxy.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "cache/cache.h"

/* Varnish < 6.2 compat */
#ifndef VPFX
#  define VPFX(a) vmod_ ## a
#  define VARGS(a) vmod_ ## a ## _arg
#  define VENUM(a) vmod_enum_ ## a
#  define VEVENT(a) a
#else
#  define VEVENT(a) VPFX(a)
#endif

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

typedef struct vmod_ip2proxy_data {
  time_t		ip2proxy_db_ts;     /* timestamp of the database file */
  IP2Proxy		*ip2proxy_handle;
  pthread_mutex_t	lock;
} ip2proxy_data_t;

void
ip2proxy_free(void *d)
{
  ip2proxy_data_t *data = d;

    if (data->ip2proxy_handle != NULL) {
        IP2Proxy_close(data->ip2proxy_handle);
    }
}

VCL_VOID
VPFX(init_db)(VRT_CTX, struct VPFX(priv) *priv, char *filename, char *memtype)
{

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	printf("The filename accepted is %s.\n", (char *) filename);	
	if (priv->priv != NULL) {
		IP2Proxy_close(priv->priv);
	}
	IP2Proxy *IP2ProxyObj = IP2Proxy_open( (char *) filename);
	if (strcmp(memtype, "IP2PROXY_FILE_IO") == 0) {
		IP2Proxy_open_mem(priv->priv, IP2PROXY_FILE_IO);
	} else if (strcmp(memtype, "IP2PROXY_SHARED_MEMORY") == 0) {
		IP2Proxy_open_mem(priv->priv, IP2PROXY_SHARED_MEMORY);
	} else if (strcmp(memtype, "IP2PROXY_CACHE_MEMORY") == 0) {
		IP2Proxy_open_mem(priv->priv, IP2PROXY_CACHE_MEMORY);
	}
	priv->priv = IP2ProxyObj;
	AN(priv->priv);
	priv->free = ip2proxy_free;
}

#define FUNC(lower, field)					\
VCL_STRING							\
vmod_ ## lower(VRT_CTX, struct vmod_priv *priv, char * ip)	\
{								\
	char *result;						\
	IP2ProxyRecord *r;					\
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);			\
	AN(priv);						\
	if (!ip || !priv->priv)					\
		return ("-");					\
	r = IP2Proxy_get_all((IP2Proxy *)priv->priv, ip);	\
	if (!r)							\
		return ("-");					\
	result = WS_Copy(ctx->ws, r->field, -1);		\
	IP2Proxy_free_record(r);				\
	return (result);					\
}

FUNC(country_short,  country_short)
FUNC(country_long,   country_long)
FUNC(region,         region)
FUNC(city,           city)
FUNC(isp,            isp)
FUNC(domain,         domain)
FUNC(usage_type,     usage_type)
FUNC(proxy_type,     proxy_type)
FUNC(asn,            asn)
FUNC(as,             as_)
FUNC(last_seen,      last_seen)
FUNC(is_proxy,       is_proxy)
