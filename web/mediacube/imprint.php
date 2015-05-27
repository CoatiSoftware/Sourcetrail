<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">

<?php
	include("admin/head.php");
?>	
	<body>
		<div class="row">
			<div class="medium-12 columns">
				<div class="left">
					<a href="index.php?lan=<?php echo($lan) ?>"><img src="./img/logo_coati.png" alt="coati logo" style="width: 100px;" /></a>
				</div>
				<div id="language_selection" class="right">
<?php
	echo("<a href='imprint.php?lan=de'");

	if ($lan == "de")
		echo(" class='selected'");

	echo(">deutsch</a> | <a href='imprint.php?lan=en'");

	if ($lan == "en")
		echo(" class='selected'");
		
	echo(">english</a>");	
?>			
				</div>
			</div>
		</div>
		<div class="row">
			<div class="medium-8 medium-offset-2 columns">
				<div class="row" style="height: 80px;"></div>
				<div class="row">
					<div class="medium-12 columns" style="text-align:center;">
						<h1>
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
						</h1>
					</div>
				</div>
				<div class="row" style="height: 80px;"></div>
				<div class="row">
					<div class="medium-6 columns">
						<p>
<?php
	if ($lan == "de")
	{
		echo("Coati wurde von Studentinnen und Studenten im Rahmen eines Masterstudiums an der FH Salzburg konzipiert und umgesetzt.");
	}
	else
	{
		echo("Coati has been developed by master-degree students of the university of applied sciences Salzburg.");
	}
?>
						</p>
						<p>
							<span style="display: block; margin-bottom: 0.6rem;">
<?php
	if ($lan == "de")
	{
		echo("Unser Team besteht aus:");
	}
	else
	{
		echo("We are:");
	}
?>
							</span>
							Eberhard Gräther<br />
							Manuel Dobusch<br />
							Malte Langkabel<br />
							Andreas Stallinger<br />
							Viktoria Pfausler<br />
						</p>
					</div>
					<div class="medium-6 columns">
						<p>
<?php
	if ($lan == "de")
	{
		echo("Für sämtliche Inhalte dieser Seite ist das Team von Coati verantwortlich.");
	}
	else
	{
		echo("The team of Coati is responsible for all contents of this page.");
	}
?>
						</p>
						<span style="display: block; margin-bottom: 0.6rem;">
<?php
	if ($lan == "de")
	{
		echo("Kontaktdaten:");
	}
	else
	{
		echo("Contact:");
	}
?>
						</span>
						<p>
						Eberhard Gräther<br />
						Fachhochschule Salzburg<br />
						Urstein Süd 1<br />
						Raum 362<br />
						A-5412 Puch/Salzburg<br />
						</p>
						<p>
						egraether.mmt-m2013@fh-salzburg.ac.at
						</p>
					</div>
				</div>
				<div class="row">
					<div class="medium-12 columns">
						<img src="./img/pixel_characters.png" alt="team" style="width: 100%; margin: 80px 0px" />
					</div>
				</div>
			</div>
		</div>
	</body>
</html>