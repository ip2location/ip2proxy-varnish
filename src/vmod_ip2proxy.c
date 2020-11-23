#include <stdlib.h>
#include <string.h>
#include <IP2Proxy.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#ifdef __FreeBSD__
#include <stdio.h>
#define gcvt(v, d, c) sprintf(c, "%*g", d, v);
#endif

#include "cache/cache.h"

/* Varnish < 6.2 compat */
#ifndef VPFX
#	define VPFX(a) vmod_ ## a
#	define VARGS(a) vmod_ ## a ## _arg
#	define VENUM(a) vmod_enum_ ## a
#	define VEVENT(a) a
#else
#	define VEVENT(a) VPFX(a)
#endif

#ifndef VRT_H_INCLUDED
#include "vrt.h"
#endif

/* Defined options for querying IP2Proxy data */
#define query_COUNTRY_SHORT			1
#define query_COUNTRY_LONG			2
#define query_REGION				3
#define query_CITY					4
#define query_ISP					5
#define query_DOMAIN				6
#define query_USAGETYPE				7
#define query_PROXYTYPE				8
#define query_ASN					9
#define query_AS					10
#define query_LASTSEEN				11
#define query_ISPROXY				12
#define query_THREAT				13

typedef struct vmod_ip2proxy_data {
	time_t			ip2p_db_ts;	 /* timestamp of the database file */
	IP2Proxy		*ip2p_handle;
	pthread_mutex_t	lock;
} ip2proxy_data_t;

void
ip2p_free(void *d)
{
	ip2proxy_data_t *data = d;

	if (data->ip2p_handle != NULL) {
		IP2Proxy_close(data->ip2p_handle);
	}
}

VCL_VOID
VPFX(init_db)(VRT_CTX, struct VPFX(priv) *priv, char *filename, char *memtype)
{
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv == NULL) {
		IP2Proxy *IP2ProxyObj = IP2Proxy_open((char *) filename);

		if (IP2ProxyObj == NULL) {
			printf("Not able to load IP2Proxy Database \"%s\".\n", (char *) filename);

			exit(0);
		}

		printf("IP2Proxy Database %s is loaded.\n", (char *) filename);

		priv->priv = IP2ProxyObj;

		if (strcmp(memtype, "IP2PROXY_FILE_IO") == 0) {
			IP2Proxy_set_lookup_mode(priv->priv, IP2PROXY_FILE_IO);
		} else if (strcmp(memtype, "IP2PROXY_CACHE_MEMORY") == 0) {
			IP2Proxy_set_lookup_mode(priv->priv, IP2PROXY_CACHE_MEMORY);
		} else if (strcmp(memtype, "IP2PROXY_SHARED_MEMORY") == 0) {
			IP2Proxy_set_lookup_mode(priv->priv, IP2PROXY_SHARED_MEMORY);
		}

		AN(priv->priv);
		priv->free = ip2p_free;
	}
}

// Use this function to query result, and then extract the field based on user selection
void *
query_all(VRT_CTX, struct VPFX(priv) *priv, char * ip, int option)
{
	IP2ProxyRecord *r;
	IP2Proxy *handle;
	char *result = NULL;
	
	printf("The client IP is %s.\n", (char *) ip);
	
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);

	if (priv->priv != NULL) {
		handle = priv->priv;
		r = IP2Proxy_get_all(handle, (char *) ip);

		if (r != NULL) {
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
				case query_THREAT:
					result = WS_Copy(ctx->ws, r->threat, -1);
					break;
				default:
					result = WS_Copy(ctx->ws, "-", -1);
					break;
			}

			IP2Proxy_free_record(r);

			return (result);
		}
	}

	// VMOD_LOG("ERROR: IP2Proxy database failed to load");

	return WS_Copy(ctx->ws, "????", -1);
}

VCL_STRING
VPFX(country_short)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_COUNTRY_SHORT);
	return (result);
}

VCL_STRING
VPFX(country_long)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_COUNTRY_LONG);
	return (result);
}

VCL_STRING
VPFX(region)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_REGION);
	return (result);
}

VCL_STRING
VPFX(city)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_CITY);
	return (result);
}

VCL_STRING
VPFX(isp)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ISP);
	return (result);
}

VCL_STRING
VPFX(domain)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_DOMAIN);
	return (result);
}

VCL_STRING
VPFX(usage_type)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_USAGETYPE);
	return (result);
}

VCL_STRING
VPFX(proxy_type)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_PROXYTYPE);
	return (result);
}

VCL_STRING
VPFX(asn)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ASN);
	return (result);
}

VCL_STRING
VPFX(as)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_AS);
	return (result);
}

VCL_STRING
VPFX(last_seen)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_LASTSEEN);
	return (result);
}

VCL_STRING
VPFX(is_proxy)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_ISPROXY);
	return (result);
}

VCL_STRING
VPFX(threat)(VRT_CTX, struct VPFX(priv) *priv, char * ip)
{
	const char *result = NULL;
	result = query_all(ctx, priv, ip, query_THREAT);
	return (result);
}