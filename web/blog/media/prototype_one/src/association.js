SHIFT.Association = function( name, from, to ) {
	SHIFT.Connection.call( this, name, from, to );

	this.count = 0;
}

SHIFT.Association.prototype = {
	__proto__ : SHIFT.Connection.prototype,

	type : SHIFT.Types.Association,

	color : 'lightblue',
	highlight : '#3EC3EE',

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		var fromClass = this.from.type === SHIFT.Types.Class;
		var toClass = this.to.type === SHIFT.Types.Class;
		var node;

		if ( sameClass ) {
			node = paper.bow( fromNode, toNode, 0.3 );
		} else if ( fromClass && toClass ) {
			node = paper.shortest( fromNode, toNode );
		} else if ( fromClass ) {
			node = paper.shortest2( fromNode, toNode );
		} else if ( toClass ) {
			node = paper.shortest2( toNode, fromNode );
		} else {
			node = paper.connection( fromNode, toNode );
		}
		return node.line( this.color, 1 + this.count * 0.5 );
	},

	Increment : function() {
		this.count++;
	},

	Decrement : function() {
		this.count--;
		if ( this.count === 0 ) {
			this.Remove();
		}
	}
}
