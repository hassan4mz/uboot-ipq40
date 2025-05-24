#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/string.h>
#include <linux/ctype.h>

static void urldecode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10); else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10); else b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

int web_setenv_handle(int argc, char **argv, char *resp_buf, int bufsize) {
    const char *var = NULL, *val = NULL;
    int i, len = 0;
    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "var=", 4) == 0)
            var = argv[i] + 4;
        else if (strncmp(argv[i], "val=", 4) == 0)
            val = argv[i] + 4;
    }
    char var_dec[4096] = {0}, val_dec[4096] = {0};
    if (var) urldecode(var_dec, var); else var_dec[0] = 0;
    if (val) urldecode(val_dec, val); else val_dec[0] = 0;

    len += snprintf(resp_buf + len, bufsize - len,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n");

    // 查询所有变量
    if (!var_dec[0] || strcmp(var_dec, "all") == 0) {
        int idx = 0;
        const unsigned char *env = env_get_addr(0);
        int truncated = 0;
        while (env[idx]) {
            int n = strlen((const char *)&env[idx]);
            if (len + n + 2 >= bufsize) {
                truncated = 1;
                break;
            }
            len += snprintf(resp_buf + len, bufsize - len, "%s\n", &env[idx]);
            idx += n + 1;
        }
        if (truncated)
            len += snprintf(resp_buf + len, bufsize - len, "[...output truncated...]\n");
        resp_buf[len] = '\0';
        return len;
    }

    // 查询单个变量
    if (val == NULL) {
        char *env_val = getenv(var_dec);
        if (env_val)
            len += snprintf(resp_buf + len, bufsize - len, "Value: %s=%s\n", var_dec, env_val);
        else
            len += snprintf(resp_buf + len, bufsize - len, "Error: variable not found\n");
        resp_buf[len] = '\0';
        return len;
    }

    // 清空变量（unsetenv）
    if (val && val_dec[0] == 0) {
        if (setenv(var_dec, NULL) == 0 && saveenv() == 0)
            len += snprintf(resp_buf + len, bufsize - len, "Success: %s unset\n", var_dec);
        else
            len += snprintf(resp_buf + len, bufsize - len, "Error: unsetenv failed\n");
        resp_buf[len] = '\0';
        return len;
    }

    // 设置变量
    if (setenv(var_dec, val_dec) == 0 && saveenv() == 0)
        len += snprintf(resp_buf + len, bufsize - len, "Success: %s=%s\n", var_dec, val_dec);
    else
        len += snprintf(resp_buf + len, bufsize - len, "Error: setenv failed\n");
    resp_buf[len] = '\0';
    return len;
}
