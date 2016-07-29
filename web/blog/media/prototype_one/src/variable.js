SHIFT.Variable = function( name ) {
	SHIFT.Object.call( this, name );

	this.typename = '';
}

SHIFT.Variable.prototype = {
	__proto__ : SHIFT.Object.prototype,

	type : SHIFT.Types.Variable,

	CreateNode : function( paper, x, y ) {
		y += 11;
		var name = this.typename.length ? this.typename + ' ' + this.name : this.name;
		var text = paper.text( x, y + 1, name ).code( 14 );
		var bb = text.getBBox();
		var rect = paper.rect( x - bb.width / 2 - 8, y - bb.height / 2 - 5, bb.width + 16, bb.height + 8 ).box();
		text.toFront();
		return paper.set().push( rect, text );
	}
}
