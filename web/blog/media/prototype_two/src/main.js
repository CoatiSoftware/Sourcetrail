function main( data ) {

	var idx;

	var width = $( '#paper' ).width() - $('#wrapper').width() - 2;
	var height = $( '#paper' ).height();
	var paper = Raphael( 'paper', width, height );

	if ( getURLParams().sample ) {
		data = [data.shift()];
		idx = 5;
	} else {
		data.shift();
		idx = 15;
	}

	var programModel = KIT.Interpreter.createProgramModel( data );
	var programView = new KIT.View.Program( programModel );
	programView.initView( paper, data );

	setTimeout( function() {
		programView.activeView = programView.nodeViews[idx];
	}, 100);

}
