<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
	
<?php
	include("admin/head.php");
?>	
	<body>
		<div class="row">
			<div class="medium-12 columns">
<?php
	echo("<div id='language_selection' class='right'><a href='index.php?lan=de'");

	if ($lan == "de")
		echo(" class='selected'");

	echo(">deutsch</a> | <a href='index.php?lan=en'");

	if ($lan == "en")
		echo(" class='selected'");
		
	echo(">english</a></div>");	
?>			
			</div>
		</div>
		<div class="row">
			<div class="medium-8 medium-centered columns">
				<div class="row">
					<div id="logo_coati">
						<img src="./img/logo_coati.png" alt="coati logo" />
					</div>
				</div>
				<div class="row" style="height: 80px;"></div>
				<div class="row">
					<div class="medium-6 columns">
						<p>
<?php
	if ($lan == "de")
	{
		echo(
			"An einem normalen Arbeitstag produziert ein Programmierer im Durchschnitt 12 Zeilen Code. Unterstützt 
			wird er dabei von mächtiger Software, die ihm das Schreiben auf alle erdenkliche Arten erleichtert. Den 
			Großteil seiner Zeit verbringt der Programmierer jedoch nicht damit, neuen Code zu schreiben, sondern 
			damit, bestehenden Code zu lesen."
		);
	}
	else
	{
		echo(
			"The average programmer spends his ordinary working day writing around 12 lines of code. He is often supported 
			by powerful software that simplifies this task. The larger part of the day, however he does not use to write 
			new code but to read existing code."
		);
	}
	?>
							</p>
						</div>
						<div class="medium-6 columns">
							<p>
<?php
	if ($lan == "de")
	{
		echo(
			"Für diese Aufgabe gibt es bisher vergleichsweise wenig unterstützende Tools.
							</p>
							<p>
			Coati schließt diese Lücke und bietet ein Interface, mit dem die Navigation in der Codebase 
			beschleunigt und das Verstehen von Zusammenhängen erleichtert wird."
		);
	}
	else
	{
		echo(
			"Currently there are only few tools that support this task. 
							</p>
							<p>
			Coati is developed to close this gap and provide an interface that accelerates navigating the codebase and support 
			the comprehension of relations."
		);
	}
?>
						</p>
					</div>
				</div>
				<div class="row" style="height: 40px;"></div>
				<div class="row">
					<div class="medium-12 columns">
						<form id="subscribe_form">
							<fieldset>
								<legend style="text-align: center; background:transparent;">
									<img src="./img/letter_icon.png" alt="Newsletter" style="padding: 0px 15px; width: 80px;"/>
								</legend>
								
								<div class="row">
									<div class="medium-12 columns">
										<p>
<?php
	if ($lan == "de")
	{
		echo(
			"Coati tritt demnächst in die erste Phase des Betatests ein. Aktuell unterstützen wir die Programmiersprache C++. 
			Wenn die Beta beginnt, benachrichtigen wir dich gerne per Newsletter."
		);
	}
	else
	{
		echo(
			"Coati will enter the first phase of a private beta soon. At the moment Coati supports the programming language 
			C++. When the beta starts we will be glad to inform you via newsletter."
		);
	}
?>

										</p>
									</div>
									<div id="submission_form">
									
										<div class="medium-8 columns">
											<div class="rounded">
												<input type="text" placeholder=
<?php
	if ($lan == "de")
	{
		echo("'für Newsletter anmelden'");
	}
	else
	{
		echo("'subscribe for Newsletter'");
	}
?>
												name="email" />
											</div>
										</div>
										<div class="medium-4 columns end">
											<div id="button_wrapper">
												<button type="button" class="button" id="submit" value="Submit" name="submit" onclick="onFormSubmit();">SUBMIT</button>
											</div>
										</div>
									</div>
									<div id="submission_message" class="medium-12 columns">
									</div>
								</div>
							</fieldset>
						</form>
					</div>
				</div>
				<div class="row">
					<div id="footer">
						<div class="medium-6 columns">
							<div class="footer_container_left" style="width: 20%">
								<img src="./img/logo_mma.png" alt="mma logo" id="logo_mma" />
							</div>
							<div class="footer_spacer"style="width: 5%"></div>
							<div class="footer_container_left" style="width: 20%">
								<img src="./img/logo_mmt.png" alt="mmt logo" id="logo_mmt" />
							</div>
							<div class="footer_spacer"style="width: 5%"></div>
							<div class="footer_container_left" style="width: 50%">
								<img src="./img/logo_fhs.png" alt="fhs logo" id="logo_fhs" /> 
							</div>
						</div>
						<div class="medium-6 columns">
							<div class="footer_container_right">
								<a href="imprint.php?lan=<?php echo($lan) ?>" id="imprint_link">
<?php
	if ($lan == "de")
	{
		echo("IMPRESSUM");
	}
	else
	{
		echo("IMPRINT");
	}
?>
								</a>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</body>
	
	<script src="js/jquery-2.1.4.min.js"></script>
	<script>
	function onFormSubmit(){
		var email = $('input[name=email]').val();
	
		$.ajax({
			type: "POST",
			url: "admin/submit.php",
			data: {email: email},
			dataType: "json",
			success: function(data) {
				if(data.status == 'success'){
					$("#submission_form").fadeTo( 
						200, 0.0, "linear", function(){
							$("#submission_form").css("display", "none");
							$("#submission_message").text(
<?php
	if ($lan == "de")
	{
		echo("'Danke für die Anmeldung.'");
	}
	else
	{
		echo("'Thank you for subscribing.'");
	}
?>
							);
						}
					);
				}else if(data.status == 'error_invalid_email'){
					$("#submission_message").text(
<?php
	if ($lan == "de")
	{
		echo("'Deine E-Mail-Adresse scheint ungültig zu sein.'");
	}
	else
	{
		echo("'Your email address seems to be invalid.'");
	}
?>
					);
				}else{
					$("#submission_message").text(
<?php
	if ($lan == "de")
	{
		echo("'Ein Fehler ist aufgetreten. Bitte versuch es später nochmal.'");
	}
	else
	{
		echo("'An error has occurred. Please try again later.'");
	}
?>
					);
				}
			},
			error: function(data) {
				alert( "error:" + data);
			}
		});
			
		return true;
	}
	
	</script>

</html>