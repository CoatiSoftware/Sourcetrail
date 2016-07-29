SHIFT.Member = function( name ) {
	SHIFT.Variable.call( this, name );

	this.visibility = SHIFT.Visibility.Public;
	this.abstraction = SHIFT.Abstraction.None;
}

SHIFT.Member.prototype = {
	__proto__ : SHIFT.Variable.prototype,

	type : SHIFT.Types.Member,

	dragable : false
}
