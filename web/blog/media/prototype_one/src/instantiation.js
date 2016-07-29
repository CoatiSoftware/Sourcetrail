SHIFT.Instantiation = function( name, variable, classModel ) {
	SHIFT.Inheritance.call( this, name, classModel, variable );
}

SHIFT.Instantiation.prototype = {
	__proto__ : SHIFT.Inheritance.prototype,

	type : SHIFT.Types.Instantiation,

	CreateNode : function( paper, fromNode, toNode, sameClass ) {
		return SHIFT.Inheritance.prototype.CreateNode.call( this, paper, fromNode, toNode, sameClass ).dash();
	}
}
