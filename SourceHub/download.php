<?php
include("config.php");
$file = getcwd().$sources_dir.$_GET['file'];

/* HEADERS */
header("Pragma: public");
header("Expires: 0");
header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
header("Content-Type: application/force-download");
header("Content-Type: application/octet-stream");
header("Content-Type: application/download");
header("Content-Disposition: attachment; filename=".str_replace(' ','_',basename($file)).";");
header("Content-Transfer-Encoding: binary");
/* HEADERS END */
@readfile($file);
exit();
?>