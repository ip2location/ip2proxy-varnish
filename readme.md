# IP2Proxy Varnish Module

| | |
|-|-|
| Author:         | IP2Location |
| Date:           | 2020-11-24 |
| Version:        | 1.2.0    |
| Manual section: | 3           |

An Varnish module that enables the user to find the country, region, city, ISP and proxy information by IP address. The module reads the proxy information from **IP2Proxy BIN data** file. 

This module currently only support Varnish version 6.2.

Required [IP2Proxy C Library](https://github.com/ip2location/ip2proxy-c) to work.

# Installation

Before install and use this module, you have to install:

- IP2Proxy C Library. You can get IP2Proxy C Library from <https://github.com/ip2location/ip2proxy-c> .
- varnishapi (varnish-dev) package. install varnishapi package by running this command:

```bash
apt-get install varnish-dev
```

- autoconf, libtool and make. Those packages will be needed during compilation and installation process.
- And of course, the **Varnish**.

Then, clone this repo into your local, and run following commands to install:

```bash
./autogen.sh
./configure
make
make install
```

# Usage

First, import this module like this:

```c
import ip2proxy;
```

After that, initialize and load the database you downloaded from IP2Location like this:

```c
ip2proxy.init_db("path_to_database", "IP2PROXY_FILE_IO");
```

Finally, called our module functions with IP address. For example:

```c
set req.http.X-Country-Code = ip2proxy.country_short("client.ip");
set req.http.X-Region = ip2proxy.region("client.ip");
```

# Functions

The table below listed down the functionality of available functions. For more information on returned data from IP2Location BIN database file, visit here: <https://www.ip2location.com/database/px8-ip-proxytype-country-region-city-isp-domain-usagetype-asn-lastseen>

### init\_db

Initialize and load database. The first argument indicates the path of the database, and the second argument indicates the mode used to open the database. Modes available are: IP2PROXY_FILE_IO, IP2PROXY_SHARED_MEMORY and IP2PROXY_CACHE_MEMORY.

#### Example

```c
ip2proxy.init_db("path_to_database", "IP2LOCATION_FILE_IO");
```

### is\_proxy

Check wether if an IP address was a proxy. Returned value:<ul><li>-1 : errors</li><li>0 : not a proxy</li><li>1 : a proxy</li><li>2 : a data center IP address</li></ul> 

#### Example

```c
set req.http.X-Is-Proxy = ip2proxy.is_proxy("client.ip");
```

### proxy\_type

Return the proxy type. Please visit <a href="https://www.ip2location.com/databases/px4-ip-proxytype-country-region-city-isp" target="_blank">IP2Location</a> for the list of proxy types supported.

#### Example

```c
set req.http.X-Proxy-Type = ip2proxy.proxy_type("client.ip");
```

### country\_short

Returns two-letter country code based on ISO 3166.

#### Example

```c
set req.http.X-Country-Code = ip2proxy.country_short("client.ip");
```

### country\_long

Returns country name based on ISO 3166.

#### Example

```c
set req.http.X-Country-Name = ip2proxy.country_long("client.ip");
```

### region 

Returns region or state name.

#### Example

```c
set req.http.X-Region = ip2proxy.region("client.ip");
```

### city 

Returns city name.

#### Example

```c
set req.http.X-City = ip2proxy.city("client.ip");
```

### isp 

Returns Internet Service Provider or company's name of the IP Address.

#### Example

```c
set req.http.X-ISP = ip2proxy.isp("client.ip");
```

### domain 

 Returns Internet domain name associated with IP address range.

#### Example

```c
set req.http.X-Domain = ip2proxy.domain("client.ip");
```

### usage\_type 

 Returns Usage type classification of ISP or company.

#### Example

```c
set req.http.X-Usagetype = ip2proxy.usagetype("client.ip");
```

### asn

 Return autonomous system number (ASN).

#### Example

```c
set req.http.X-Usagetype = ip2proxy.asn("client.ip");
```

### as 

 Return autonomous system (AS) name.     

#### Example

```c
set req.http.X-Usagetype = ip2proxy.as("client.ip");
```

### last\_seen 

 Returns proxy last seen in days.

#### Example

```c
set req.http.X-Usagetype = ip2proxy.last_seen("client.ip");
```



### threat

Return security threat reported for this IP.

#### Example

````
set req.http.X-Threat = ip2proxy.threat("client.ip");
````



# FAQ 

1.  "Where can I get the database to use?" 

   Answer: You can get free IP2Proxy LITE databases from [https://lite.ip2location.com](https://lite.ip2location.com/), or purchase an IP2Proxy commercial database from <https://www.ip2location.com/database/ip2proxy>.

2.  "I can't install the module. Please help me."

   Answer: Once again, before you install the package, please make sure that you have installed autoconf, libtool and make packages first. Those packages are necessary to compile and install this module.

3.  "Why am I getting an error message said that Package varnishapi was not found?"

   Answer: In order to use Varnish vmod, you have to install varnishapi package before hand. Please refer to the [Installation](#installation) section first before install this module. 
   
 4. "I am getting error message said that You need rst2man installed to make dist. What should I do?"

​       Answer: If you encounter such message, you can install rst2man by installing python-docutils package than provide rst2man:

```bash
apt-get install python-docutils
```

 5. "I am getting Permission denied when running autogen.sh.. What should I do?"

​       Answer: You can try to edit the permission for autogen.sh by:

```bash
chmod +x autogen.sh
```

# Support

Email: [support@ip2location.com](mailto:support@ip2location.com).
URL: [https://www.ip2location.com](https://www.ip2location.com/)
