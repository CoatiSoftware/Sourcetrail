KIT.Model.Class = function( name ) {
	KIT.Model.Node.call( this, name );
	KIT.Model.Graph.call( this );

	this.baseClass_ = null;

	this.visibility_ = KIT.Visibility.Public;
	this.abstraction = KIT.Abstraction.None;
};

KIT.Model.Graph.prototype.__proto__ = KIT.Model.Node.prototype;

KIT.Model.Class.prototype = {
	__proto__ : KIT.Model.Graph.prototype,

	type : KIT.Types.Class,

	get baseClass() {
		return this.baseClass_;
	},

	set baseClass( baseClass ) {
		if ( this.baseClass_ ) {
			console.error( 'base class already set!');
		}
		this.baseClass_ = baseClass;
	},

	createNode : function( type, name, location, visibility, abstraction ) {
		var node = KIT.Model.Graph.prototype.createNode.call( this, type, name, location );
		if ( visibility ) {
			node.visibility = visibility;
		}
		if ( abstraction ) {
			node.abstraction = abstraction;
		}
		return node;
	},

	getMember : function( name ) {
		return this.getOrCreateNode( KIT.Model.Member, name );
	},

	createMember : function( name, location, visibility, abstraction ) {
		return this.createNode( KIT.Model.Member, name, location, visibility, abstraction );
	},

	getMethod : function( name ) {
		return this.getOrCreateNode( KIT.Model.Method, name );
	},

	createMethod : function( name, location, visibility, abstraction ) {
		return this.createNode( KIT.Model.Method, name, location, visibility, abstraction );
	},

	createClass : function( name, location, visibility, abstraction ) {
		return this.createNode( KIT.Model.Class, name, location, visibility, abstraction );
	},

	highlightLocation : function() {
		KIT.Model.Node.prototype.highlightLocation.call( this );
		KIT.Model.Graph.prototype.highlightLocation.call( this );
	}
};
