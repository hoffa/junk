<?php
if($_GET['file'] != NULL) {
	$url = rawurldecode(htmlspecialchars($_GET['file']));
	if(!is_file(getcwd().$sources_dir.$_GET['file'])) {
		echo '<code><strong>ERROR:</strong> /'.$_GET['file'].' doesn\'t exist!</code><br />';
	}
	$lines = file(getcwd().$sources_dir.$url);
	$type = explode('.',$_GET['file']);
	$type = end($type);
	$type = str_replace(' ','%20',$type);
	if($show_file_type == true) {
		if($type == "c"){$language = "C/C++ Source";}
		elseif($type == "cpp"){$language = "C/C++ Source";}
		elseif($type == "h"){$language = "C/C++ Source";}
		elseif($type == "asm" or $type == "s"){$language = "Assembler";}
		elseif($type == "php"){$language = "PHP";}
		elseif($type == "htm" or $type == "html"){$language = "(x)HTML";}
		elseif($type == "rar"){$language = "RAR archive";}
		elseif($type == "zip"){$language = "ZIP archive";}
		elseif($type == "7z"){$language = "7z archive";}
		elseif($type == "ocx"){$language = "OCX file";}
		elseif($type == "txt"){$language = "Text file";}
		elseif($type == "exe"){$language = "Windows Executable";}
		elseif($type == "dll"){$language = "Dynamic Link Library";}
		elseif($type == "png"){$language = "PNG image"; $image = true;}
		elseif($type == "gif"){$language = "GIF image"; $image = true;}
		elseif($type == "jpg" or $type == "jpeg"){$language = "JPG image"; $image = true;}
		elseif($type == "bmp"){$language = "BMP image"; $image = true;}
		else{$language = "Unknown";}
	}
	
	if($type == "h"){$type = "c";}
	if($type == "s"){$type = "asm";}
	
if($show_number_of_lines == true) {
	$lines_file = -1;
	foreach($lines as $line_num => $line){ $lines_file++; }
}
?> 
<table cellpadding="0" cellspacing="0">
<tbody>
<tr><td class="infol" style="padding-left:1%;"><strong>File: </strong><code>/<?php echo $_GET['file']; ?></code><?php if($show_file_size == true){echo '&nbsp;&nbsp;<strong>Size: </strong><code>'.format_size(filesize(getcwd().$sources_dir.$_GET['file'])).'</code>';} if($show_number_of_lines == true){echo '&nbsp;&nbsp;<strong>Lines: </strong><code>'.$lines_file.'</code>';}if($show_file_type == true){echo '&nbsp;&nbsp;<strong>Type: </strong><code>'.$language.'</code>';}?></td><td class="infor" style="padding-right:1%;"><img src="images/close.gif" align="absmiddle" alt="Close Icon" />&nbsp;<a href="<?php echo 'index.php?file=&amp;browse='.$current_dir; ?>" id="edit">Close file</a>&nbsp;&nbsp;&nbsp;&nbsp;<img src="images/download.gif" align="absmiddle" alt="Download Icon" />&nbsp;<a href="download.php?file=<?php echo $current_file; ?>" id="edit">Download</a></td></tr>
</table>
<table cellpadding="0" cellspacing="0"><tr><td valign="baseline" class="lines" align="right" nowrap="nowrap"><pre>
<?php
if($type == "rar" or $type == "exe" or $type == "dll" or $type == "ocx" or $image == true){
	echo '<code><strong><div align="left" style="padding-left:20px;">ERROR:</strong> Cannot read binary data! Please <a href="download.php?file='.$current_file.'" id="edit">download</a> the file.</div></code><br />';
} else {
	foreach($lines as $line_num => $line){ echo $line_num.'<br />'; }
	?>
</pre></td>
<td valign="baseline" class="content" nowrap="nowrap"><tt>
	<?php
	foreach($lines as $line_num => $line){ $source .= $line; }
	$language = $type;
	$geshi =& new GeSHi($source, $language);
	echo $geshi->parse_code();
}
?>
</tt></td>
</tr>
</tbody>
</table>
<?php
}
elseif($default_file != NULL) {
	if($_SESSION['default_file_viewed'] == true){;}
	else{
		$default_file_path = getcwd().$default_file;
		if(is_file($default_file_path)) {
			$url = rawurldecode(htmlspecialchars($default_file_path));
		if(!is_file($default_file_path)) {
			echo '<code><strong>ERROR:</strong> '.$default_file_path.' doesn\'t exist!</code><br />';
		}
		$lines = file($url);
		?>
<table cellpadding="0" cellspacing="0">
<tr>
<td class="default_content" nowrap="nowrap"><tt>
		<?php
		echo '<pre>';
		foreach($lines as $line_num => $line){ echo $line; }
		echo '</pre>';
		?>
</tt>
</td>
</tr>
</table>
    	<?php
		$_SESSION['default_file_viewed'] = true;
		}
	}
}
else{;}
?>