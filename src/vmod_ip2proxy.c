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

/* Defined options for querying IP2Location data */
#define query_COUNTRY_SHORT       1
#define query_COUNTRY_LONG        2
#define query_REGION              3
#define query_CITY                4
#define query_ISP                 5
#define query_DOMAIN              6
#define query_USAGETYPE           7
#define query_PROXYTYPE           8
#define query_ASN                 9
#define query_AS                 10
#define query_LASTSEEN           11
#define query_ISPROXY            12

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

// Use this function to query result, and then extract the field based on user selection
void *
query_all(VRT_CTX, struct VPFX(priv) *priv, char * ip, int option)
{
    IP2ProxyRecord *r;
    IP2Proxy *handle;
    char *result = NULL;
	
	printf("The IP address accepted is %s.\n", ip);
	
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv == NULL)
		return ("-");

	handle = priv->priv;
	r = IP2Proxy_get_all(handle, ip);

	if (r == NULL)
		return ("-");

	switch (option) {
		case query_COUNTRY_SHORT:
			result = WS_Copy(ctx->ws, r->country_short, -1);
			break;
		case query_COUNTRY_LONG:
			result = WS_Copy(ctx->ws, r->country_long, -1);
			break;
		case query_REGION:
			result = WS_Copy(ctx->ws, r->region, -1);
			break;
		case query_CITY:
			result = WS_Copy(ctx->ws, r->city, -1);
			break;
		case query_ISP:
			result = WS_Copy(ctx->ws, r->isp, -1);
			break;
		case query_DOMAIN:
			result = WS_Copy(ctx->ws, r->domain, -1);
			break;
		case query_USAGETYPE:
			result = WS_Copy(ctx->ws, r->usage_type, -1);
			break;
		case query_PROXYTYPE:
			result = WS_Copy(ctx->ws, r->proxy_type, -1);
			break;
		case query_ASN:
			result = WS_Copy(ctx->ws, r->asn, -1);
			break;
		case query_AS:
			result = WS_Copy(ctx->ws, r->as_, -1);
			break;
		case query_LASTSEEN:
			result = WS_Copy(ctx->ws, r->last_seen, -1);
			break;
		case query_ISPROXY:
			result = WS_Copy(ctx->ws, r->is_proxy, -1);
			break;
		default:
			result = "-";
			break;
	}
	IP2Proxy_free_record(r);

	return (result);
}

#define FUNC(lc, uc)						\
VCL_STRING							\
vmod_ ## lc(VRT_CTX, struct vmod_priv *priv, char * ip)		\
{								\
	return (query_all(ctx, priv, ip, query_ ## uc));	\
}

FUNC(country_short, COUNTRY_SHORT)
FUNC(country_long, COUNTRY_LONG)
FUNC(region, REGION)
FUNC(city, CITY)
FUNC(isp, ISP)
FUNC(domain, DOMAIN)
FUNC(usage_type, USAGETYPE)
FUNC(proxy_type, PROXYTYPE)
FUNC(asn, ASN)
FUNC(as, AS)
FUNC(last_seen, LASTSEEN)
FUNC(is_proxy, ISPROXY)
