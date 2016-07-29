SHIFT.Function = function( name ) {
	SHIFT.Object.call( this, name );

	this.return = null;
	this.params = null;
}

SHIFT.Function.prototype = {
	__proto__ : SHIFT.Object.prototype,

	type : SHIFT.Types.Function,

	CreateNode : function( paper, x, y ) {
		y += 12;
		var t = this.name;
		// if ( this.return ) {
		// 	t = this.return + ' ' + t;
		// }
		// if ( this.params ) {
		// 	t += '( ' + this.params.join( ', ' ) + ' )';
		// }

		var text = paper.text( x, y + 2, t ).code( 14 );
		var bb = text.getBBox();
		var ellipse = paper.ellipse( x, y, bb.width / 2 + 10, bb.height / 2 + 4 ).box();
		text.toFront();
		return paper.set().push( ellipse, text );
	}
}
