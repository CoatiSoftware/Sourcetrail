KIT.Model.Program = function() {
	KIT.Model.Graph.call( this );
};

KIT.Model.Program.prototype = {
	__proto__ : KIT.Model.Graph.prototype,

	type : KIT.Types.Program,

	get name() {
		return '';
	},

	getClass : function( name ) {
		var names = name.split( '::' );
		if ( names.length === 2 ) {
			var classNode = this.getOrCreateNode( KIT.Model.Class, names[0] );
			return classNode.getOrCreateNode( KIT.Model.Class, names[1] );
		}

		var node = this.findNode( name );
		if ( !node ) {
			var classes = this.findAllByKeyInArray( 'type', KIT.Types.Class, this.nodes_ );
			for ( var i = 0; i < classes.length; i++ ) {
				node = classes[i].findNode( name );
				if ( node ) {
					return node;
				}
			}
			return this.getOrCreateNode( KIT.Model.Class, name );
		}
		return node;
	},

	createClass : function( name, location ) {
		return this.createNode( KIT.Model.Class, name, location );
	},

	getVariable : function( name ) {
		return this.getOrCreateNode( KIT.Model.Variable, name );
	},

	createVariable : function( name, location ) {
		return this.createNode( KIT.Model.Variable, name, location );
	},

	getFunction : function( name ) {
		return this.getOrCreateNode( KIT.Model.Function, name );
	},

	createFunction : function( name, location ) {
		return this.createNode( KIT.Model.Function, name, location );
	},

	createCall : function( from, to, location ) {
		var edge = this.createEdge( KIT.Model.Call, from, to, location );

		if ( to.abstraction === KIT.Abstraction.PureVirtual ) {
			for ( var i = 0; i < to.virtuals.length; i++ ) {
				var e = this.createEdge( KIT.Model.Call, to, to.virtuals[i], location );
				e.abstraction = KIT.Abstraction.PureVirtual;
			}
		}

		return edge;
	},

	createInheritance : function( from, to, location ) {
		return this.createEdge( KIT.Model.Inheritance, from, to, location );
	},

	createInstantiation : function( from, to, location ) {
		return this.createEdge( KIT.Model.Instantiation, from, to, location );
	},

	createUsage : function( from, to, location ) {
		return this.createEdge( KIT.Model.Usage, from, to, location );
	},

	searchData : function() {
		data = {
			classes : [],
			variables : [],
			functions : [],
			members : [],
			methods : []
		};

		this.addSearchData( data );

		function dataObject( name ) {
			return {
				name : name,
				local : data[name],
				header : '<p class="tt-header">' + name + '</p>',
				limit : 30
			}
		}

		return [
			dataObject( 'classes' ),
			dataObject( 'variables' ),
			dataObject( 'functions' ),
			dataObject( 'members' ),
			dataObject( 'methods' )
		];
	}
};
