SHIFT.Inheritance = function( name, base, derived ) {
	SHIFT.Connection.call( this, name, base, derived );

	derived.base = base;
}

SHIFT.Inheritance.prototype = {
	__proto__ : SHIFT.Connection.prototype,

	type : SHIFT.Types.Inheritance,

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		if ( sameClass ) {
			return paper.bow( fromNode, toNode, 5 ).line( this.color );
		} else {
			return paper.arrow2( fromNode, toNode ).line( this.color );
		}
	}
}
