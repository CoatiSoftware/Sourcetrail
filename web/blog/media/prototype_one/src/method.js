SHIFT.Method = function( name ) {
	SHIFT.Function.call( this, name );

	this.visibility = SHIFT.Visibility.Public;
	this.abstraction = SHIFT.Abstraction.None;
}

SHIFT.Method.prototype = {
	__proto__ : SHIFT.Function.prototype,

	type : SHIFT.Types.Method,

	dragable : false,

	CreateNode : function( paper, fromNode, toNode ) {
		var node = SHIFT.Function.prototype.CreateNode.call( this, paper, fromNode, toNode );
		if ( this.abstraction === SHIFT.Abstraction.PureVirtual ) {
			node[0].dash();
		}
		return node;
	},

	SetAbstraction : function( abstraction ) {
		this.abstraction = abstraction;
		if ( abstraction === SHIFT.Abstraction.Virtual || abstraction === SHIFT.Abstraction.PureVirtual ) {
			this.parent.CheckVirtualCall( this );
		}
		if ( abstraction === SHIFT.Abstraction.PureVirtual ) {
			this.parent.abstraction = SHIFT.Abstraction.PureVirtual;
		}
	}
}