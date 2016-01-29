/*
**  mod_osrm.c -- Apache sample osrm module
**  [Autogenerated via ``apxs -n osrm -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory
**  by running:
**
**    $ apxs -c -i mod_osrm.c
**
**  Then activate it in Apache's apache2.conf file for instance
**  for the URL /osrm in as follows:
**
**    #   apache2.conf
**    LoadModule osrm_module modules/mod_osrm.so
**    <Location /osrm>
**    SetHandler osrm
**    </Location>
**
**  Then after restarting Apache via
**
**    $ apachectl restart
**
**  you immediately can request the URL /osrm and watch for the
**  output of this module. This can be achieved for instance via:
**
**    $ lynx -mime_header http://localhost/osrm
**
**  The output should be similar to the following one:
**
**    HTTP/1.1 200 OK
**    Date: Tue, 31 Mar 1998 14:42:22 GMT
**    Server: Apache/1.3.4 (Unix)
**    Connection: close
**    Content-Type: text/html
**
**    The sample page from mod_osrm.c
*/

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_provider.h"
#include "ap_config.h"
#include "socket/unix-server.h"


/*
    Configuration prototype and declaration;
*/
typedef struct
{
    const char *osrmd_socket_path;
} mod_osrm_config;

static mod_osrm_config config;

/*
    Handler for OSRMdSocketName
*/
const char *mod_osrm_set_osrmd_socket_path(cmd_parms *cmd, void *cfg, const char *arg)
{
    config.osrmd_socket_path = arg;
    return NULL;
}

/*
    The directive structure for name tag
*/
static const command_rec mod_osrm_directives[] =
{
    AP_INIT_TAKE1("OSRMdSocketName", mod_osrm_set_osrmd_socket_path,NULL,RSRC_CONF,"The name of the unix socket for OSRMd"),
    {NULL}
};


/*
    Content handler
*/
static int mod_osrm_handler(request_rec *r)
{
    if (strcmp(r->handler, "osrm")) {
        return DECLINED;
    }
    r->content_type = "text/html";

    if (!r->header_only)
        ap_rputs("The sample page from mod_osrm.c <br/>", r);
        ap_rputs("You requested: ",r);
        ap_rputs(r->uri,r);

        if(r->args)
        {
            ap_rputs("?",r);
            ap_rputs(r->args,r);
        }

        ap_rputs("<br/>",r);
        ap_rputs("Socket name: ",r);
        ap_rputs(config.osrmd_socket_path,r);


    return OK;
}

static void osrm_register_hooks(apr_pool_t *p)
{
    config.osrmd_socket_path = "/tmp/unix-socket";
    ap_hook_handler(mod_osrm_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA osrm_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    mod_osrm_directives,   /* table of config file commands       */
    osrm_register_hooks    /* register hooks                      */
};
