SHIFT.Use = function( name, user, used ) {
	SHIFT.Connection.call( this, name, user, used );
}

SHIFT.Use.prototype = {
	__proto__ : SHIFT.Connection.prototype,

	type : SHIFT.Types.Use,

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		if ( sameClass ) {
			return paper.bow( fromNode, toNode, 1 ).line( this.color ).thin();
		} else {
			return paper.connection( fromNode, toNode ).line( this.color ).thin();
		}
	}
}
