<?php
//	include('admin/config.php');
	function getValueFromUrl($valueName)
	{
		if (isset($_GET[$valueName]) && !empty($_GET[$valueName]))
			return $_GET[$valueName];
		else
			return "";
	}
?>

<?php
	$lan = getValueFromUrl("lan");
	if ($lan != "de")
		$lan = "en";
?>	

	<!-- paulirish.com/2008/conditional-stylesheets-vs-css-hacks-answer-neither/ -->
	<!--[if lt IE 7]> <html class="no-js lt-ie9 lt-ie8 lt-ie7" lang="en"> <![endif]-->
	<!--[if IE 7]>    <html class="no-js lt-ie9 lt-ie8" lang="en"> <![endif]-->
	<!--[if IE 8]>    <html class="no-js lt-ie9" lang="en"> <![endif]-->
	<!--[if gt IE 8]><!--> <html class="no-js" lang="en"> <!--<![endif]-->

	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

		<!-- Set the viewport width to device width for mobile -->
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />

		<title>Coati - Smart Code Exploration.</title>
		
		<meta name="keywords" content="software development, tool, sourcecode, source code, exploration" />
		
		<link rel="icon" href="favicon.ico" type="image/x-icon" />
		<link rel="shortcut icon" href="favicon.ico" type="image/x-icon" />
		
		<!-- Included CSS Files -->
		<link rel="stylesheet" href="css/foundation.css">
		<link rel="stylesheet" href="css/app.css">
		<script src="js/vendor/modernizr.js"></script>

		<!-- IE Fix for HTML5 Tags -->
		<!--[if lt IE 9]>
			<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>
		<![endif]-->

	</head>
	