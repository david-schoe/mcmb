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
					//  changing control0 name
					"<label for=\"output_ip1\">output_ip1: </label>"
					"<input type=\"number\" name=\"output_ip1\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					// changing control1 name
					"<label for=\"output_ip2\">output_ip2: </label>"
					"<input type=\"number\" name=\"output_ip2\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					// changing control2 name
					"<label for=\"output_ip3\">output_ip3: </label>"
					"<input type=\"number\" name=\"output_ip3\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
				"<form action=\"/\" method=\"POST\">"
					// changing control3 name
					"<label for=\"output_ip4\">output_ip4: </label>"
					"<input type=\"number\" name=\"output_ip4\" placeholder=%s>"
					"<input type=\"submit\" value=\"submit\"/>"
				"</form>"
			"</body>"
			"</html>",
			output_ip1,
			output_ip2,
			output_ip3,
			output_ip4);
	return err;
}
