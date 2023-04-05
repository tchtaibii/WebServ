<?php
	if (isset ($_POST['name']))
		echo $_POST['name'];
	else
	{
		?>
		<form action="test2.php" method="POST">
			<input type="text" name="name">
			<input type="text" name="sname">
			<input type="submit" value="send">
		</form>
		<?php
	}
?>
