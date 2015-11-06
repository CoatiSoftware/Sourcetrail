void lambdaCaller()
{
	[](){}();
}

void lambdaSuperCaller()
{
	lambdaCaller();
}

void lambdaMetaCaller()
{
	[](){
		lambdaCaller();
	}();
}
