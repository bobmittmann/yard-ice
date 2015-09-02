/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>
#include <thinkos.h>
#include <trace.h>
#include "www.h"

const char footer_html[] = HTML_FOOTER;
const uint16_t sizeof_footer_html = sizeof(footer_html) - 1;

#if 0
<script type="text/javascript" src="modalbox.js"></script>
<script type="text/javascript" src="xmlrpc.js"></script>
<script type="text/javascript">
	function msg_show(msg) {
		var html = msg;
		document.getElementById("msg").innerHTML = html;
	}
	function upload_msg() {
		msg_show("Uploading firmware, please wait...");
	}
	function nofile_msg() {
		msg_show("You must select a file to upload...");
	}

	function restore_defaults() {
		dlg.okSelected = function () {
			dlg.hide();
			window.location.assign("restore.html");
		}
		dlg.cancelSelected = function () {
			dlg.hide();
		}
		dlg.show('defaults_dlg', 'Restore factory settings?', 400, 100);
	}
	function reset() {
		dlg.okSelected = function () {
			dlg.hide();
			window.location.assign("reset.html");
		}
		dlg.cancelSelected = function () {
			dlg.hide();
		}
		dlg.show('reset_dlg', 'Reset the camera?', 400, 100);
	}

	function conf_save() {
		xmlrpc_action('confsave', 0,
			function () { msg_show('Configuration saved.'); }
		);
	}

	function on_sysinfo_get(xml)
	{
		txt_bind(xml, 'series');
		txt_bind(xml, 'model');
		txt_bind(xml, 'version');
		txt_bind(xml, 'date');
		txt_bind(xml, 'esn');
		txt_bind(xml, 'mac');
		img_bind(xml, 'icon');
	}

	function init() {
		xml_async_get("product.xml", function (xml) {
					  document.getElementById('title').innerHTML =
					  xml_txt_get(xml, 'series') });
		dlg.init();
		xml_async_get("sysinfo.xml", on_sysinfo_get);
	}
</script>
</head>

<body onload="init()">
<div id="header">
	<ul><li><img src="ip-cam-64x64.png" alt="IP Camera"/></li>
	<li><h1 id="title">&nbsp;</h1><h2>i-LAX Electronics</h2></li><ul>
</div>
<div id="navigation">
	<ul>
		<li><a href="primary.html">Preview</a></li>
		<li><a href="video.html">Video</a></li>
		<li><a href="audio.html">Audio</a></li>
		<li><a href="alarms.html">Alarms</a></li>
		<li><a href="network.html">Network</a></li>
		<li><a href="time.html">Time</a></li>
		<li><a href="#" class="active">Tools</a></li>
	</ul>
</div>

<div id="content">

	<h1>System information:</h1>
	<div class="block">
		<div class="form">
		<div class="fbody">
		<table>
		<tr>
			<td>
				<span id="icon" style="margin-right:40px"></span>
			</td>
			<td>
				<table>
				<tr>
					<td align="right"><b>Camera series:</b></td>
					<td><span id="series">unknown</span></td>
				<tr>
					<td align="right"><b>Camera model:</b></td>
					<td><span id="model">unknown</span></td>
				</tr>
				<tr>
					<td align="right"><b>Firmware version:</b></td>
					<td><span id="version">unknown</span></td>
				</tr>
				<tr>
					<td align="right"><b>Release date:</b></td>
					<td><span id="date">unknown</span></td>
				</tr>
				<tr>
					<td align="right"><b>Serial number:</b></td>
					<td><span id="esn">unknown</span></td>
				</tr>
				<tr>
					<td align="right"><b>Mac Address:</b></td>
					<td><span id="mac">unknown</span></td>
				</tr>
				</table>
			</td>
		<tr>
		</table>
		</div>
		</div>
	</div>

	<h1>Firmware update:</h1
	#endif

const char tools_html[] = DOCTYPE_HTML "<head>\r\n"
    "<title>ThinkOS HTTP Server Demo</title>\r\n"
    META_COPY LINK_ICON LINK_CSS
    "</head>\r\n<body>\r\n"
    "<h1>ThinkOS Firmware Upgrade</h1>\r\n"
"<div class=\"form\">\r\n"
"<form method=\"post\" enctype=\"multipart/form-data\"\r\n"
"   action=\"/update.cgi\" onsubmit=\"upload_msg()\">\r\n"
"   <div class=\"fbody\">\r\n"
"       File:\r\n"
"<!--[if IE]>\r\n"
"       <input type=\"file\" id=\"browse\" class=\"text\""
"           maxlength=\"128\" size=\"40\" name=\"firmware\" />"
"<![endif]-->\r\n"
"<![if !IE]>\r\n"
"       <input type=\"text\" class=\"text\" id=\"fname\"\r\n"
"           readonly=\"readonly\" maxlength=\"128\" size=\"40\" />\r\n"
"       <input type=\"button\" class=\"button\" value=\"Browse...\"\r\n"
"           onclick=\"javascript:\r\n"
"           document.getElementById('browse').click()\" />\r\n"
"       <input type=\"file\" class=\"hidden\" id=\"browse\"\r\n"
"           name=\"firmware\" onchange=\"javascript:\r\n"
"           document.getElementById('fname').value = this.value\" />\r\n"
"<![endif]>\r\n"
"   </div>\r\n"
"   <div class=\"ffoot\">\r\n"
"       <input type=\"reset\" class=\"button\" value=\"Reset\" />\r\n"
"       <input type=\"submit\" class=\"button\" value=\"Upload...\" />\r\n"
"   </div>\r\n"
"</form>\r\n"
"</div>\r\n"
    HTML_FOOTER;

const char update_html[] = DOCTYPE_HTML "<head>\r\n"
    "<title>ThinkOS HTTP Server Demo</title>\r\n"
    META_COPY LINK_ICON LINK_CSS
    "</head>\r\n<body>\r\n"
    "<h1>ThinkOS Firmware Upgrade</h1>\r\n"
    HTML_FOOTER;

int update_cgi(struct httpctl * ctl)
{
    char s[HTML_MAX + 1];
    int n;
    int cnt = 0;

    httpd_200(ctl->tp, TEXT_PLAIN);

    while ((n = http_multipart_recv(ctl, s, 1)) > 0) {
//      (void)n;
//      DBG("http_multipart_recv=%d", n);
        cnt += n;
        http_send(ctl, s, n);
    }

    DBG("file size=%d", cnt);

    return 0;

    httpd_200(ctl->tp, TEXT_HTML);

    return http_send(ctl, update_html, sizeof(update_html) - 1);
}

#if 0
<script type="text/javascript">
	function alert(msg) {
		var html = '<img style="float:none;vertical-align:middle"' +
			'src="alert.png" alt="Alert! "/>&nbsp;'+msg;
		document.getElementById("msg").innerHTML = html;
	}
	function msg_show(msg) {
		var html = msg;
		document.getElementById("msg").innerHTML = html;
	}
	function cancel() {
		history.go(-1);
	}
</script>

<script type="text/javascript">
	function msg_show(msg) {
		var html = msg;
		document.getElementById("msg").innerHTML = html;
	}
	function upload_msg() {
		msg_show("Uploading firmware, please wait...");
	}
	function nofile_msg() {
		msg_show("You must select a file to upload...");
	}
</script>

<script type="text/javascript">
	var sysinfo_xml;

	function on_pkginfo_get(xml)
	{
		var st;
		txt_bind(xml, 'version');
		txt_bind(xml, 'date');
		st = (+xml.getElementsByTagName('status')[0].firstChild.nodeValue);
		if (st == 1)
			document.getElementById('agree').disabled = false;
		else
			alert("Package error!");
	}

	function on_disclaimer_get(txt)
	{
		document.getElementById('disclaimer').innerHTML = txt;
	}

	function on_agree_click(chk)
	{
		btn = document.getElementById('update');
		btn.disabled = chk.checked ? false : true;
	}

	function init() {
		xml_async_get("product.xml", function (xml) {
					  document.getElementById('title').innerHTML =
					  xml_txt_get(xml, 'series') });
		xml_async_get("pkginfo.xml", on_pkginfo_get);
		txt_async_get("download/disclaimer.txt", on_disclaimer_get);
	}
</script>
</head>

<body onload="init()">
<div id="header">
	<ul><li><img src="ip-cam-64x64.png" alt="IP Camera"/></li>
	<li><h1 id="title">&nbsp;</h1><h2>i-LAX Electronics</h2></li><ul>
</div>

<div id="content">
	<h1>Firmware Update</h1>
	<div class="block">
		<div class="form">
		<div class="fbody">
		<fieldset>
			<legend>Update information:</legend>
			<center>
			<table>
				<tr>
					<td align="right"><b>Firmware version:</b></td>
					<td><span id="version"></span></td>
				</tr>
				<tr>
					<td align="right"><b>Release date:</b></td>
					<td><span id="date"></span></td>
				</tr>
			</table>

			<textarea rows="10" cols="80" id="disclaimer" readonly="readonly">
			</textarea>
			</center>
		</fieldset>
		<ul style="margin:1em 2em 0em" >
		<li> If you are <b>not sure</b> about this procedure press
		'Cancel' to return.</li>
		<li>If you are sure you want to upgrade the device's firmware,
		please read carefully the information above.</li>
		<li>If you agree with the terms and conditions therein expressed,
		check the box bellow and press the 'Update...' button.</li>
		</ul>
		</div>
		</div>
		<p id="msg"></p>
		<form action="/install.html" >
		<div class="form">
			<div class="fbody">
			<center>
			<p><input type="checkbox" class="checkbox" id="agree"
			disabled="disabled"	onclick="on_agree_click(this)" />
			Yes, I agree with the above terms and conditions.</p>
			</center>
			</div>
			<div class="ffoot">
			<center>
			<input type="button" class="button" onclick="cancel()"
				value="Cancel" />
			<input type="submit" class="button" id="update"
				disabled="disabled"	value="Update..."  />
			</center>
			</div>
		</div>
		</form>
	</div>
</div>
#endif

/*---------------------------------------------------------------------------
  Initial page (index.html)
  ---------------------------------------------------------------------------*/

const char index_html[] = DOCTYPE_HTML "<head>\r\n"
	"<title>ThinkOS HTTP Server Demo</title>\r\n" 
	META_COPY LINK_ICON LINK_CSS
	"</head>\r\n<body>\r\n"
	"<h1>ThinkOS Web Server Demo</h1>\r\n"
	"<p>Welcome to the <b>ThinkOS</b> web server demo initial page.</p>\r\n"
	"<ul>\r\n"
	"<li><a href=\"ipcfg/ipcfg_form.cgi\">IP Configuration</a></li>\r\n"
	"<li><a href=\"tools.html\">Tools</a></li>\r\n"
	"</ul>\r\n"
	HTML_FOOTER;

/*---------------------------------------------------------------------------
  Cascading Style Sheet (style.css)
  ---------------------------------------------------------------------------*/

const char style_css[] = "* { border: 0; margin: 0; }\r\n"
	"body { background: #f8f8f8; color: #555; font: 0.9em Arial,Helvetica,"
		"\"bitstream vera sans\",sans-serif; margin: 10px 10px 25px 10px; }\r\n"
	"a { color: #779; text-decoration:none; }\r\n"
	"a:hover { color:#335; text-decoration:none; }\r\n"
	"p { color: #111; text-align: justify; margin: 10px 0px 20px 0px; }\r\n"
	"ul { margin: 10px 0px 20px 0px; }\r\n"
	"h1 { font-size: 1.5em; text-align:left; margin: 10px 0px 10px 0px; }\r\n"
	"h2 { font-size: 1.3em; text-align:left;  margin: 10px 0px 25px 0px; }\r\n"
	"hr { background-color:#114; color:#112; width: 100%; height: 1px; "
		"margin: 10px 0px 5px 0px; }\r\n"
	"table { border-collapse: collapse;  margin: 0; }\r\n"
	"textarea { background:#fff; margin:1px 2px 1px; border:1px "
		"solid #aaa; padding:1px 2px 1px; }\r\n"
	"form { padding:0; margin:0; border:0; display:table;}\r\n"
	"input[type=text] { background:#fff; margin:1px 2px 1px; "
		"border:1px solid #444; padding:3px 3px 2px; }\r\n"
	"input[type=text]:hover { background:#cdf; }\r\n"
	"input[type=button],input[type=reset],input[type=submit],button {"
		"background:#ccc; margin:1px 4px 1px; "
		"border:1px solid #444; padding:4px 8px 3px; }\r\n"
	"input[type=button]:hover,input[type=reset]:hover,"
	"input[type=submit]:hover,button:hover {"
		" background:#458; color:#fff; text-decoration:none; }\r\n"
	/* modal dialog box */
	"#mbox { background-color:#eee; padding:8px; border:2px solid #222; }\r\n"
	"#dlg { clear:both; }\r\n"
	"#dlg h1 { text-align:left; border-bottom:1px dashed #666; "
		"font-size:1.1em; padding:1px 0 0; margin-bottom:4px; "
		"clear:both; width:100%; }\r\n"
	"#ol { background-image: url(img/overlay.png); } \r\n"
	".dialog { display:none }\r\n"
	"* html #ol{ background-image:none; "
		"filter:progid:DXImageTransform.Microsoft.AlphaImageLoader("
		"src=\"img/overlay.png\", sizingMethod=\"scale\"); } \r\n"
	".hidden { background:#fdd; border:0; margin:0; padding:0;"
		"width: 1px; height: 1px; opacity: 0; filter: alpha(opacity=0);"
		"-ms-filter: \"alpha(opacity=0)\"; -khtml-opacity: 0; "
		"-moz-opacity: 0; } \r\n";

/*---------------------------------------------------------------------------
  root directory content
  ---------------------------------------------------------------------------*/

struct httpdobj www_root[] = {
	{ .oid = "style.css", .typ = OBJ_STATIC_CSS, .lvl = 255, 
		.len = sizeof(style_css) - 1, .ptr = style_css },
	{ .oid = "index.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(index_html) - 1, .ptr = index_html },
	{ .oid = "tools.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(tools_html) - 1, .ptr = tools_html },
    { .oid = "update.cgi", .typ = OBJ_CODE_CGI, .lvl = 100,
        .len = 0, .ptr = update_cgi },
	{ .oid = "update.html", .typ = OBJ_STATIC_HTML, .lvl = 255,
		.len = sizeof(update_html) - 1, .ptr = update_html },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

