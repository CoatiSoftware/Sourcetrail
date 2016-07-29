KIT.Model.Edge = function( name, from, to ) {
	this.name_ = name;

	this.from_ = from;
	this.to_ = to;

	this.locations_ = [];

	from.addEdge( this );
	to.addEdge( this );
};

KIT.Model.Edge.prototype = {

	get name() {
		return this.name_;
	},

	get shortName() {
		return this.name_;
	},

	get from() {
		return this.from_;
	},

	get to() {
		return this.to_;
	},

	addLocation : function( location ) {
		this.locations_.push( location );
	},

	get locations() {
		return this.locations_;
	},

	highlightLocation : function() {
		for ( var i = 0; i < this.locations_.length; i++ ) {
			this.locations_[i].highlight();
		}
	}
};

// Derived models.

KIT.Model.Usage = function( name, usingNode, usedNode ) {
	KIT.Model.Edge.call( this, name, usingNode, usedNode );
};

KIT.Model.Usage.prototype = {
	__proto__ : KIT.Model.Edge.prototype,

	type : KIT.Types.Usage
};


KIT.Model.Call = function( name, caller, callee ) {
	KIT.Model.Edge.call( this, name, caller, callee );

	this.abstraction = KIT.Abstraction.None;
};

KIT.Model.Call.prototype = {
	__proto__ : KIT.Model.Edge.prototype,

	type : KIT.Types.Call
};


KIT.Model.Inheritance = function( name, baseClass, derivedClass ) {
	KIT.Model.Edge.call( this, name, baseClass, derivedClass );

	derivedClass.baseClass = baseClass;
};

KIT.Model.Inheritance.prototype = {
	__proto__ : KIT.Model.Edge.prototype,

	type : KIT.Types.Inheritance
};


KIT.Model.Instantiation = function( name, variable, classNode ) {
	KIT.Model.Edge.call( this, name, classNode, variable );
};

KIT.Model.Instantiation.prototype = {
	__proto__ : KIT.Model.Edge.prototype,

	type : KIT.Types.Instantiation
};
