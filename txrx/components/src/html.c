#include "html.h"

#define TAG "html"

char root_html[ROOT_HTML_SZ];

esp_err_t update_root_html(void) {

	memset(root_html,0,ROOT_HTML_SZ);
	err=sprintf(root_html,"<!DOCTYPE html>"
			"<html>"
			"<head>"
				"<title>\"MASTER\"</title>"
			"</head>"
			"<body>"
				"<h1>\"MASTER\"</h1>"
				"<p>\"textbox test\"</p>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_0\">control_0: </label>"
					"<input type=\"number\" name=\"control_0\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_1\">control_1: </label>"
					"<input type=\"number\" name=\"control_1\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_2\">control_2: </label>"
					"<input type=\"number\" name=\"control_2\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					"<label for=\"control_3\">control_3: </label>"
					"<input type=\"number\" name=\"control_3\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
			"</body>"
			"</html>",
			control_0,
			control_1,
			control_2,
			control_3);
	return err;
}
