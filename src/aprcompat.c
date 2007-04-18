/* This file merely just provides libphp4.so or libphp5.so with apache symbols
 * so it will link
 */
#include "aprcompat.h"

const char *ap_auth_type(void) { return 0; }
const char *ap_loaded_modules(void) { return 0; }
const char *ap_log_rerror(void) { return 0; }
const char *ap_hook_post_config(void) { return 0; }
const char *apr_table_add(void) { return 0; }
const char *unixd_config(void) { return 0; }
const char *ap_get_brigade(void) { return 0; }
const char *ap_hook_handler(void) { return 0; }
const char *ap_update_mtime(void) { return 0; }
const char *apr_brigade_flatten(void) { return 0; }
const char *ap_add_cgi_vars(void) { return 0; }
const char *ap_server_root_relative(void) { return 0; }
const char *apr_table_set(void) { return 0; }
const char *ap_set_content_type(void) { return 0; }
const char *ap_server_root(void) { return 0; }
const char *ap_get_server_version(void) { return 0; }
const char *apr_pool_cleanup_register(void) { return 0; }
const char *ap_mpm_query(void) { return 0; }
const char *ap_destroy_sub_req(void) { return 0; }
const char *ap_pass_brigade(void) { return 0; }
const char *apr_pstrdup(void) { return 0; }
const char *apr_table_unset(void) { return 0; }
const char *apr_snprintf(void) { return 0; }
const char *ap_log_error(void) { return 0; }
const char *apr_table_get(void) { return 0; }
const char *ap_sub_req_lookup_uri(void) { return 0; }
const char *apr_psprintf(void) { return 0; }
const char *apr_pool_cleanup_run(void) { return 0; }
const char *ap_run_sub_req(void) { return 0; }
const char *apr_palloc(void) { return 0; }
const char *apr_brigade_cleanup(void) { return 0; }
const char *ap_hook_pre_config(void) { return 0; }
const char *ap_rwrite(void) { return 0; }
const char *apr_table_elts(void) { return 0; }
const char *ap_add_version_component(void) { return 0; }
const char *apr_bucket_eos_create(void) { return 0; }
const char *apr_pool_userdata_set(void) { return 0; }
const char *apr_brigade_create(void) { return 0; }
const char *ap_rflush(void) { return 0; }
const char *apr_pool_cleanup_null(void) { return 0; }
const char *ap_set_last_modified(void) { return 0; }
const char *ap_add_common_vars(void) { return 0; }
const char *apr_pool_userdata_get(void) { return 0; }

