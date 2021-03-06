#ifndef __APRCOMPAT_H__
#define __APRCOMPAT_H__

const char *ap_auth_type(void);
const char *ap_loaded_modules(void);
const char *ap_log_rerror(void);
const char *ap_hook_post_config(void);
const char *apr_table_add(void);
const char *unixd_config(void);
const char *ap_get_brigade(void);
const char *ap_hook_handler(void);
const char *ap_update_mtime(void);
const char *apr_brigade_flatten(void);
const char *ap_add_cgi_vars(void);
const char *ap_server_root_relative(void);
const char *apr_table_set(void);
const char *ap_set_content_type(void);
const char *ap_server_root(void);
const char *ap_get_server_version(void);
const char *apr_pool_cleanup_register(void);
const char *ap_mpm_query(void);
const char *ap_destroy_sub_req(void);
const char *ap_pass_brigade(void);
const char *apr_pstrdup(void);
const char *apr_table_unset(void);
const char *apr_snprintf(void);
const char *ap_log_error(void);
const char *apr_table_get(void);
const char *ap_sub_req_lookup_uri(void);
const char *apr_psprintf(void);
const char *apr_pool_cleanup_run(void);
const char *ap_run_sub_req(void);
const char *apr_palloc(void);
const char *apr_brigade_cleanup(void);
const char *ap_hook_pre_config(void);
const char *ap_rwrite(void);
const char *apr_table_elts(void);
const char *ap_add_version_component(void);
const char *apr_bucket_eos_create(void);
const char *apr_pool_userdata_set(void);
const char *apr_brigade_create(void);
const char *ap_rflush(void);
const char *apr_pool_cleanup_null(void);
const char *ap_set_last_modified(void);
const char *ap_add_common_vars(void);
const char *apr_pool_userdata_get(void);
const char *ap_hook_child_init(void);

#endif /* __APRCOMPAT_H__ */
