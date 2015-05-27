<?php
	header('Content-type: application/json');
	$response_array['status'] = 'error';

	$email = $_POST["email"];
	$regex = "/^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$/"; 
	if (preg_match($regex, $email)) 
	{
		include("db_config.php");
		
		$link = new mysqli($servername, $username, $password, $dbname);
		
		if (mysqli_connect_errno()) 
		{
			exit();
		}
		
		$email = mysqli_real_escape_string($link, $email);

		if (mysqli_query($link, "INSERT INTO " . $tabename . " (`id`, `mail`) VALUES (0, '$email')")) 
		{
			$response_array['status'] = 'success';    
		}
		
		$link->close();
	}
	else
	{
		$response_array['status'] = 'error_invalid_email';
	}
	
	echo(json_encode($response_array));
?>