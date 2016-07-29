KIT.Model.Node = function( name ) {
	this.edges_ = [];
	this.name_ = name;

	this.parent_ = null;
	this.location_ = null;
};

KIT.Model.Node.prototype = {

	get name() {
		var parentName = this.parent.name;
		if ( parentName.length ) {
			return parentName + '::' + this.name_;
		}
		return this.name_;
	},

	get shortName() {
		return this.name_;
	},

	get parent() {
		return this.parent_;
	},

	set parent( parent ) {
		this.parent_ = parent;
	},

	get location() {
		return this.location_;
	},

	set location( location ) {
		if ( this.location_ ) {
			console.error( 'Location already set!');
		}
		this.location_ = location;
	},

	addEdge : function( edge ) {
		if ( edge.from === this ) {
			this.edges_.unshift( edge );
		} else if ( edge.to === this ) {
			this.edges_.push( edge );
		} else {
			console.error( 'This node is not associated with the edge' );
		}
	},

	lastParent : function() {
		if ( this.parent.type !== KIT.Types.Program ) {
			return this.parent.lastParent();
		}
		return this;
	},

	getProgram : function() {
		if ( this.parent.type === KIT.Types.Program ) {
			return this.parent;
		}
		return this.parent.getProgram();
	},

	addSearchData : function( data ) {
		var values;
		switch ( this.type ) {
			case KIT.Types.Class :    values = data.classes;   break;
			case KIT.Types.Variable : values = data.variables; break;
			case KIT.Types.Function : values = data.functions; break;
			case KIT.Types.Member :   values = data.members;   break;
			case KIT.Types.Method :   values = data.methods;   break;
			default: return;
		}

		var tokens = [];
		var name = this.name;
		for ( var i = 0; i < name.length; i++ ) {
			tokens.push( name.substring( i ) );
		}

		values.push(
			{
				value : name,
				tokens : tokens,
				model : this
			}
		);
	},

	highlightLocation : function() {
		if ( this.location ) {
			this.location.highlight();
		}
	}
};

// Derived models.

KIT.Model.Function = function( name ) {
	KIT.Model.Node.call( this, name );

	this.returnValue_ = null;
	this.params_ = null;
};

KIT.Model.Function.prototype = {
	__proto__ : KIT.Model.Node.prototype,

	type : KIT.Types.Function,

	get returnValue() {
		return this.returnValue_;
	},

	set returnValue( returnValue ) {
		this.returnValue_ = returnValue;
	},

	get params() {
		return this.params_;
	},

	set params( params ) {
		this.params_ = params;
	}
};


KIT.Model.Method = function( name ) {
	KIT.Model.Function.call( this, name );

	this.visibility_ = KIT.Visibility.Public;
	this.abstraction_ = KIT.Abstraction.None;

	this.virtuals = [];
};

KIT.Model.Method.prototype = {
	__proto__ : KIT.Model.Function.prototype,

	type : KIT.Types.Method,

	get visibility() {
		return this.visibility_;
	},

	set visibility( visibility ) {
		this.visibility_ = visibility;
	},

	get abstraction() {
		return this.abstraction_;
	},

	set abstraction( abstraction ) {
		this.abstraction_ = abstraction;

		if ( abstraction === KIT.Abstraction.PureVirtual ) {
			this.parent.abstraction = KIT.Abstraction.PureVirtual;
		} else if ( abstraction === KIT.Abstraction.Virtual ) {
			var nodes = this.parent.baseClass.nodes;
			for ( var i = 0; i < nodes.length; i++ ) {
				if ( nodes[i].shortName === this.shortName ) {
					nodes[i].virtuals.push( this );
				}
			}
		}
	}
};


KIT.Model.Variable = function( name ) {
	KIT.Model.Node.call( this, name );
};

KIT.Model.Variable.prototype = {
	__proto__ : KIT.Model.Node.prototype,

	type : KIT.Types.Variable
};


KIT.Model.Member = function( name ) {
	KIT.Model.Variable.call( this, name );

	this.visibility_ = KIT.Visibility.Public;
	this.abstraction_ = KIT.Abstraction.None;

	this.typeName_ = null;
};

KIT.Model.Member.prototype = {
	__proto__ : KIT.Model.Variable.prototype,

	type : KIT.Types.Member,

	get visibility() {
		return this.visibility_;
	},

	set visibility( visibility ) {
		this.visibility_ = visibility;
	},

	get abstraction() {
		return this.abstraction_;
	},

	set abstraction( abstraction ) {
		this.abstraction_ = abstraction;
	},

	get typeName() {
		return this.typeName_;
	}, 

	set typeName( typeName ) {
		this.typeName_ = typeName;
	}
};
