#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <malloc.h>
struct env_entry
{
	char *name;
	char *value;
};
static void urldecode(char *dst, const char *src)
{
	char a, b;
	while (*src)
	{
		if ((*src == '%') &&
			((a = src[1]) && (b = src[2])) &&
			(isxdigit(a) && isxdigit(b)))
		{
			if (a >= 'a')
				a -= 'a' - 'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a' - 'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = 16 * a + b;
			src += 3;
		}
		else if (*src == '+')
		{
			*dst++ = ' ';
			src++;
		}
		else
		{
			*dst++ = *src++;
		}
	}
	*dst = '\0';
}
static int env_get_items(struct env_entry **entries, int *count)
{
	char *env, *nxt;
	int env_size = 0;
	/* 计算有效变量数量 */
	for (env = env_get_addr(0); *env; env = nxt + 1)
	{
		nxt = env + strlen(env);
		if (strchr(env, '='))
			env_size++;
	}
	/* 分配内存 */
	struct env_entry *env_list = malloc(env_size * sizeof(struct env_entry));
	if (!env_list)
		return -1;
	/* 填充条目 */
	int i = 0;
	for (env = env_get_addr(0); *env; env = nxt + 1)
	{
		nxt = env + strlen(env);
		char *eq = strchr(env, '=');
		if (eq)
		{
			*eq = '\0';								 // 临时分隔名称
			env_list[i].name = strdup(env);			 // 复制名称
			env_list[i].value = strdup(getenv(env)); // 获取当前值并复制
			*eq = '=';								 // 恢复原始字符串
			i++;
		}
	}
	*entries = env_list;
	*count = env_size;
	return 0;
}
int web_setenv_handle(int argc, char **argv, char *resp_buf, int bufsize)
{
	const char *var = NULL, *val = NULL;
	int i, len = 0;
	/* 解析参数 */
	for (i = 0; i < argc; i++)
	{
		if (strncmp(argv[i], "var=", 4) == 0)
			var = argv[i] + 4;
		else if (strncmp(argv[i], "val=", 4) == 0)
			val = argv[i] + 4;
	}
	/* URL解码 */
	char var_dec[4096] = {0}, val_dec[4096] = {0};
	if (var)
		urldecode(var_dec, var);
	if (val)
		urldecode(val_dec, val);
	/* 准备HTTP响应头 */
	len += snprintf(resp_buf + len, bufsize - len,
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Connection: close\r\n\r\n");

	/* 查询所有变量 */
	if (!var_dec[0] || strcmp(var_dec, "all") == 0)
	{
		struct env_entry *entries = NULL;
		int count = 0;
		int truncated = 0;
		int i;
		if (env_get_items(&entries, &count) == 0)
		{
			for (i = 0; i < count; i++)
			{
				if (entries[i].value)
				{
					int n = strlen(entries[i].name) + strlen(entries[i].value) + 3;
					if (len + n >= bufsize)
					{
						truncated = 1;
						break;
					}
					len += snprintf(resp_buf + len, bufsize - len,
									"%s=%s<br>", entries[i].name, entries[i].value);
				}
				free(entries[i].name);
				free(entries[i].value);
			}
			free(entries);
		}
		if (truncated)
			len += snprintf(resp_buf + len, bufsize - len,
							"[...output truncated...]<br>");

		resp_buf[len] = '\0';
		return len;
	}
	/* 恢复默认环境变量 */
	if (strcmp(var_dec, "default") == 0)
	{
		set_default_env("Resetting to default environment");
		if (saveenv() == 0)
			len += snprintf(resp_buf + len, bufsize - len,
							"Success: Environment restored to default\n");
		else
			len += snprintf(resp_buf + len, bufsize - len,
							"Error: Failed to restore default environment\n");
		resp_buf[len] = '\0';
		return len;
	}
	/* 查询单个变量 */
	if (val == NULL)
	{
		char *env_val = getenv(var_dec);
		if (env_val)
			len += snprintf(resp_buf + len, bufsize - len,
							"Value: %s=%s\n", var_dec, env_val);
		else
			len += snprintf(resp_buf + len, bufsize - len,
							"Error: variable not found\n");
		resp_buf[len] = '\0';
		return len;
	}
	/* 清空变量 */
	if (val && val_dec[0] == 0)
	{
		if (setenv(var_dec, NULL) == 0 && saveenv() == 0)
			len += snprintf(resp_buf + len, bufsize - len,
							"Success: %s unset\n", var_dec);
		else
			len += snprintf(resp_buf + len, bufsize - len,
							"Error: unsetenv failed\n");
		resp_buf[len] = '\0';
		return len;
	}
	/* 设置变量 */
	if (setenv(var_dec, val_dec) == 0 && saveenv() == 0)
		len += snprintf(resp_buf + len, bufsize - len,
						"Success: %s=%s\n", var_dec, val_dec);
	else
		len += snprintf(resp_buf + len, bufsize - len,
						"Error: setenv failed\n");
	resp_buf[len] = '\0';
	return len;
}