<?php
/*
Copyright (C) 2008 Rehn Christoffer

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

require("config.php"); 
require("lib/geshi/geshi.php");
include("functions.php");
session_start();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>SourceHub <?php echo $title; ?></title>
<link rel="stylesheet" media="screen" type="text/css" title="Default Design" href="styles/default.css" />
<link rel="shortcut icon" href="images/favicon.png" type="image/x-icon" /> 
</head>
<body>
<table cellpadding="0" cellspacing="0"><tr>
<td><img src="images/logo.png" id="header" alt="SourceHub Logo"/>
</td><td align="right" valign="bottom"><code>v<?php echo $sb_version; ?></code></td></tr></table>
<br /><table cellpadding="0" cellspacing="0">
<tr><td class="infol" style="padding-left:1%"><?php echo '<strong>Directory:</strong>&nbsp;<code>/'.$_GET['browse'].'</code>'; ?><br /></td><td class="infor"><img src="images/refresh.gif" alt="Refresh Icon" align="absmiddle" />&nbsp;<a href="<?php echo currentURL(); ?>" id="edit" style="padding-right:2%">Refresh</a></td></tr>
</table>
<table cellpadding="0" cellspacing="0">
<tr><td class="browser">
<!-- FILE BROWSER: CONTENT -->
<?php require("browser.php"); ?>
</td></tr>
</table><br />
<!-- FILE VIEWER: CONTENT -->
<?php require("content_viewer.php"); ?>
</body>
</html>