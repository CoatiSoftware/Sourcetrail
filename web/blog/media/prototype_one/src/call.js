SHIFT.Call = function( name, caller, callee ) {
	SHIFT.Connection.call( this, name, caller, callee );

	this.abstraction = SHIFT.Abstraction.None;
}

SHIFT.Call.prototype = {
	__proto__ : SHIFT.Connection.prototype,

	type : SHIFT.Types.Call,

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		var set = paper.set();
		var line;
		if ( sameClass ) {
			line = paper.bow( fromNode, toNode, 3 );
		} else {
			line = paper.wave( fromNode, toNode );
		}

		line.line( this.color );

		if ( this.abstraction === SHIFT.Abstraction.Virtual ) {
			line.dash();
		}

		var end = line.points[1];
		var arrow = paper.arrow( end.x, end.y, end.i ).line( this.color );
		return set.push( line, arrow );
	}
}
