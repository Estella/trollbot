#ifndef __JS_EMBED_H__
#define __JS_EMBED_H__

struct network;

int js_eval_file(struct network *net, char *filename);
void net_init_js(struct network *net);

#endif /* __JS_EMBED_H__ */
