<?php
if($_GET['browse'] == NULL) { $current_dir = ""; } else { $current_dir = $_GET['browse']; }
if($current_file == NULL) { $current_file = $_GET['file']; }

echo "<img src='images/home.gif' />&nbsp;<a href=".'index.php?browse=&amp;file='.str_replace(' ','%20',$current_file)." id='edit'>Home</a><br />";

if(!is_dir(getcwd().$sources_dir.$current_dir)){
	echo '<br /><code><strong>ERROR:</strong> /'.$_GET['browse'].' doesn\'t exist!</code><br />';
} else {

	if($handle = opendir(getcwd().$sources_dir.$current_dir)) {
    	while($file = readdir($handle)) {
			if($file == NULL){$files_there = false;}
			elseif($file == "."){$files_there = false;}
			elseif($file == ".."){$files_there = false;}
			elseif(is_dir(getcwd().$sources_dir.$current_dir.$file)) {
				$files_there = true;
				$file = str_replace(' ','%20',$file);
				echo "<img src='images/dir.png' />&nbsp;<a href=".'index.php?browse='.str_replace(' ','%20',$current_dir.$file).'/&amp;file='.str_replace(' ','%20',$current_file)." id='edit'>".str_replace('%20',' ',$file)."/</a><br />";
			}
			else {;}
		}
	closedir($handle);
	}

	/* Init. variables */
	$handle = NULL;
	$file = NULL;
	$current_file = NULL;
	$current_dir = NULL;

	if($_GET['browse'] == NULL) { $current_dir = ""; } else { $current_dir = $_GET['browse']; }
	if($current_file == NULL) { $current_file = $_GET['file']; }

	if($handle = opendir(getcwd().$sources_dir.$current_dir)) {
    	while($file = readdir($handle)) {
			if($file == NULL){$files_there = false;}
			if($file == "..") {$files_there = false;}
			if($file == ".") {$files_there = false;}
			else {
				if(is_file(getcwd().$sources_dir.$current_dir.$file)) {
					$files_there = true;
					$file = str_replace(' ','%20',$file);
					$type = explode('.',$file);
					$type = end($type);
					if($type == "s"){$type = "asm";}
					if($type == "htm"){$type = "html";}
				
					if($type == "c"){echo "<img src='images/c.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "cpp"){echo "<img src='images/c.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "h"){echo "<img src='images/h.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "txt"){echo "<img src='images/txt.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "asm"){echo "<img src='images/asm.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "php"){echo "<img src='images/php.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "html"){echo "<img src='images/html.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "rar"){echo "<img src='images/rar.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					elseif($type == "exe"){echo "<img src='images/exe.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}
					else {echo "<img src='images/file.png' />&nbsp;<a href=".'index.php?file='.str_replace(' ','%20',$current_dir.$file).'&amp;browse='.str_replace(' ','%20',$current_dir)." id='edit'>".str_replace('%20',' ',$file)."</a><br />";}		
				} else {;}
			}
		}
	}
	closedir($handle);
	if($files_there == false){echo 'This directory doesn\'t contain any files!<br />';}
}
?>